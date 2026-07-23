
# Setup

The site uses Hextra v0.12.3 as a pinned Hugo Module. Hugo downloads it automatically on the
first build. Hugo Extended, Git, and Go are required.

No Git submodules are used.

# Update theme

    $ cd hugo
    $ hugo mod get github.com/imfing/hextra@<version>

# Serve site locally

    $ cd hugo
    $ ./serve_local.sh

Open `http://localhost:1313/jctest/`.

The preview uses a fixed port and exits with an error if port 1313 is already occupied. Stop the
existing preview before starting another one.

# Build site

    $ cd hugo
    $ ./publish.sh

The new doc files are in `jctest/docs`, ready to be checked in.

# Build and test the Dash docset

The docset build is separate from `publish.sh` and does not modify or stage the web documentation
in `docs`. It requires Python 3 and Hugo.

From the repository root, build a development archive with:

    $ python3 scripts/build_docset.py --version dev

The command validates every indexed page and anchor and creates:

    build/docset/jctest.docset
    build/docset/jctest-dev.docset.tgz

Inspect the search index from the command line with:

    $ sqlite3 build/docset/jctest.docset/Contents/Resources/docSet.dsidx \
        'SELECT type, name, path FROM searchIndex ORDER BY type, name;'

Install the local build in Dash on macOS with:

    $ open -a Dash build/docset/jctest.docset

You can also double-click the `.docset` bundle in Finder. Search for entries such as `ASSERT_EQ`,
`TEST_F`, `jc_test_base_class`, and `--test-filter`, and check that pages retain their styling and
navigation while offline. To test a rebuilt bundle, remove the previous `jctest` docset in Dash's
Settings > Docsets and open the new bundle again.

# Release the Dash docset

Pushing a version tag matching `v*` runs `.github/workflows/docset-release.yml`. The workflow builds
and validates the docset, creates a GitHub release if the tag does not already have one, and uploads
an archive named `jctest-<tag>.docset.tgz`. If the release already exists, the archive is attached to
that release.

For example:

    $ git tag -a v0.15 -m "jctest v0.15"
    $ git push origin v0.15
