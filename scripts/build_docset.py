#!/usr/bin/env python3

import argparse
import os
import plistlib
import re
import shutil
import sqlite3
import subprocess
import sys
import tarfile
from html.parser import HTMLParser
from pathlib import Path
from urllib.parse import urlsplit, urlunsplit


DOCSET_NAME = "jctest"
FALLBACK_URL = "https://jcash.github.io/jctest/"
ROOT_URL_RE = re.compile(r'(?P<prefix>\b(?:href|src)\s*=\s*["\'])/(?P<path>[^/][^"\']*)')
HREF_RE = re.compile(
    r'(?P<prefix>\bhref\s*=\s*(?P<quote>["\']))(?P<url>.*?)(?P=quote)'
)


class IdParser(HTMLParser):
    def __init__(self):
        HTMLParser.__init__(self)
        self.ids = set()

    def handle_starttag(self, tag, attrs):
        del tag
        for name, value in attrs:
            if name == "id" and value:
                self.ids.add(value)


class ReferenceParser(HTMLParser):
    def __init__(self):
        HTMLParser.__init__(self)
        self.references = []

    def handle_starttag(self, tag, attrs):
        del tag
        for name, value in attrs:
            if name in ("href", "src") and value:
                self.references.append(value)


def parse_args():
    parser = argparse.ArgumentParser(description="Build the jctest Dash docset")
    parser.add_argument("--version", default="dev", help="version used in the archive name")
    parser.add_argument(
        "--output-dir",
        type=Path,
        help="output directory (default: build/docset)",
    )
    return parser.parse_args()


def run_hugo(repo_root, documents):
    hugo = shutil.which("hugo")
    if not hugo:
        raise RuntimeError("Hugo was not found in PATH")

    subprocess.run(
        [
            hugo,
            "--source",
            str(repo_root / "hugo"),
            "--config",
            "config.toml,docset.toml",
            "--destination",
            str(documents),
            "--cleanDestinationDir",
        ],
        check=True,
    )


def make_root_urls_relative(documents):
    for html_path in documents.rglob("*.html"):
        text = html_path.read_text(encoding="utf-8")

        def replace(match):
            url = match.group("path")
            path, marker, suffix = url.partition("#")
            path, query_marker, query = path.partition("?")
            target = documents / path
            relative = os.path.relpath(str(target), str(html_path.parent)).replace(os.sep, "/")
            if path.endswith("/") and not relative.endswith("/"):
                relative += "/"
            if query_marker:
                relative += "?" + query
            if marker:
                relative += "#" + suffix
            return match.group("prefix") + relative

        updated = ROOT_URL_RE.sub(replace, text)
        if updated != text:
            html_path.write_text(updated, encoding="utf-8")


def make_directory_urls_explicit(documents):
    for html_path in documents.rglob("*.html"):
        text = html_path.read_text(encoding="utf-8")

        def replace(match):
            parsed = urlsplit(match.group("url"))
            if parsed.scheme or parsed.netloc or not parsed.path.endswith("/"):
                return match.group(0)
            explicit = parsed.path + "index.html"
            url = urlunsplit(("", "", explicit, parsed.query, parsed.fragment))
            return match.group("prefix") + url + match.group("quote")

        updated = HREF_RE.sub(replace, text)
        if updated != text:
            html_path.write_text(updated, encoding="utf-8")


def write_info_plist(contents, version):
    info = {
        "CFBundleIdentifier": "org.jcash.jctest",
        "CFBundleName": DOCSET_NAME,
        "CFBundleShortVersionString": version,
        "CFBundleVersion": version,
        "DocSetPlatformFamily": DOCSET_NAME,
        "isDashDocset": True,
        "dashIndexFilePath": "index.html",
        "DashDocSetFallbackURL": FALLBACK_URL,
        "DashDocSetDefaultFTSEnabled": True,
        "isJavaScriptEnabled": True,
    }
    with (contents / "Info.plist").open("wb") as stream:
        plistlib.dump(info, stream, sort_keys=True)


def read_entries(index_path):
    entries = []
    for line_number, raw_line in enumerate(index_path.read_text(encoding="utf-8").splitlines(), 1):
        line = raw_line.strip()
        if not line or line.startswith("#"):
            continue
        fields = line.split("|")
        if len(fields) != 3 or not all(fields):
            raise RuntimeError("Invalid index entry at %s:%d" % (index_path, line_number))
        entries.append(tuple(fields))
    return entries


def validate_entry(documents, entry):
    name, entry_type, target = entry
    relative_path, marker, anchor = target.partition("#")
    html_path = documents / relative_path
    if not html_path.is_file():
        raise RuntimeError("Missing target for %s %s: %s" % (entry_type, name, relative_path))
    if not marker:
        return

    parser = IdParser()
    parser.feed(html_path.read_text(encoding="utf-8"))
    if anchor not in parser.ids:
        raise RuntimeError("Missing anchor for %s %s: %s" % (entry_type, name, target))


def write_search_index(resources, entries):
    database_path = resources / "docSet.dsidx"
    connection = sqlite3.connect(str(database_path))
    try:
        connection.execute(
            "CREATE TABLE searchIndex(id INTEGER PRIMARY KEY, name TEXT, type TEXT, path TEXT)"
        )
        connection.execute(
            "CREATE UNIQUE INDEX anchor ON searchIndex (name, type, path)"
        )
        connection.executemany(
            "INSERT OR IGNORE INTO searchIndex(name, type, path) VALUES (?, ?, ?)",
            entries,
        )
        connection.commit()
    finally:
        connection.close()


def validate_offline_urls(documents):
    root_relative = []
    directory_links = []
    missing = []
    for html_path in documents.rglob("*.html"):
        text = html_path.read_text(encoding="utf-8")
        if ROOT_URL_RE.search(text):
            root_relative.append(str(html_path.relative_to(documents)))

        parser = ReferenceParser()
        parser.feed(text)
        for reference in parser.references:
            parsed = urlsplit(reference)
            if parsed.scheme or parsed.netloc or not parsed.path:
                continue
            if parsed.path.endswith("/"):
                directory_links.append(
                    "%s -> %s" % (html_path.relative_to(documents), reference)
                )
            target = html_path.parent / parsed.path
            if not target.exists():
                missing.append("%s -> %s" % (html_path.relative_to(documents), reference))

    if root_relative:
        raise RuntimeError("Root-relative URLs remain in: %s" % ", ".join(root_relative))
    if directory_links:
        raise RuntimeError("Directory links remain:\n%s" % "\n".join(directory_links))
    if missing:
        raise RuntimeError("Missing local resources:\n%s" % "\n".join(missing))


def copy_icons(documents, docset):
    shutil.copy2(documents / "favicon-16x16.png", docset / "icon.png")
    shutil.copy2(documents / "favicon-32x32.png", docset / "icon@2x.png")


def create_archive(output_dir, docset, version):
    safe_version = re.sub(r"[^A-Za-z0-9._-]", "-", version)
    archive = output_dir / ("%s-%s.docset.tgz" % (DOCSET_NAME, safe_version))
    with tarfile.open(str(archive), "w:gz") as tar:
        tar.add(str(docset), arcname=docset.name, filter=exclude_metadata)
    return archive


def exclude_metadata(info):
    if Path(info.name).name == ".DS_Store":
        return None
    return info


def main():
    args = parse_args()
    repo_root = Path(__file__).resolve().parent.parent
    output_dir = (args.output_dir or repo_root / "build/docset").resolve()
    docset = output_dir / (DOCSET_NAME + ".docset")
    contents = docset / "Contents"
    resources = contents / "Resources"
    documents = resources / "Documents"

    if docset.exists():
        shutil.rmtree(str(docset))
    output_dir.mkdir(parents=True, exist_ok=True)
    documents.mkdir(parents=True)

    run_hugo(repo_root, documents)
    make_directory_urls_explicit(documents)
    make_root_urls_relative(documents)
    validate_offline_urls(documents)
    write_info_plist(contents, args.version)
    copy_icons(documents, docset)

    entries = read_entries(repo_root / "hugo/data/docset-index.txt")
    for entry in entries:
        validate_entry(documents, entry)
    write_search_index(resources, entries)
    archive = create_archive(output_dir, docset, args.version)

    print("Built %s with %d search entries" % (docset, len(entries)))
    print("Archive: %s" % archive)
    return 0


if __name__ == "__main__":
    try:
        sys.exit(main())
    except (OSError, RuntimeError, subprocess.CalledProcessError) as error:
        print("error: %s" % error, file=sys.stderr)
        sys.exit(1)
