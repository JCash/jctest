#!/usr/bin/env python3
"""Benchmark the C and C++ jc_test implementations against their peers.

Generated sources and binaries are written below the selected build directory.
The benchmark is informational and deliberately has no pass/fail threshold.
"""

import argparse
import json
import os
from pathlib import Path
import platform
import statistics
import subprocess
import time


def run(command, *, stdout=None, check=True):
    destination = subprocess.PIPE if stdout is None else stdout
    return subprocess.run(command, stdout=destination, stderr=subprocess.PIPE,
                          check=check, text=destination is not subprocess.DEVNULL)


def elapsed(command, warmups, runs, check=True):
    for _ in range(warmups):
        run(command, stdout=subprocess.DEVNULL, check=check)
    samples = []
    for _ in range(runs):
        start = time.perf_counter()
        run(command, stdout=subprocess.DEVNULL, check=check)
        samples.append(time.perf_counter() - start)
    return statistics.median(samples)


def generated_tests(framework, count, passing):
    condition = "1" if passing else "0"
    if framework == "jctest_c":
        lines = ["#define JC_TEST_IMPLEMENTATION", '#include "jc_test.h"']
        for index in range(count):
            lines.append(
                f"TEST(Suite, Test{index:05d}) {{ ASSERT_TRUE({condition}); }}")
        lines.extend(["int main(int argc, char** argv) {",
                      "    jc_test_init(&argc, argv);",
                      "    return jc_test_run_all();", "}"])
    elif framework == "utest":
        lines = ['#include "utest.h"']
        for index in range(count):
            lines.append(
                f"UTEST(Suite, Test{index:05d}) {{ ASSERT_TRUE({condition}); }}")
        lines.append("UTEST_MAIN();")
    elif framework == "greatest":
        lines = ['#include "greatest.h"', "GREATEST_MAIN_DEFS();"]
        for index in range(count):
            lines.append(
                f"TEST test_{index:05d}(void) {{ ASSERT({condition}); PASS(); }}")
        lines.append("SUITE(suite) {")
        for index in range(count):
            lines.append(f"    RUN_TEST(test_{index:05d});")
        lines.extend(["}", "int main(int argc, char** argv) {",
                      "    GREATEST_MAIN_BEGIN();", "    RUN_SUITE(suite);",
                      "    GREATEST_MAIN_END();", "}"])
    elif framework == "unity":
        lines = ['#include "unity.h"', "#include <string.h>",
                 "void setUp(void) {}", "void tearDown(void) {}"]
        for index in range(count):
            lines.append(
                f"void test_{index:05d}(void) {{ "
                f"TEST_ASSERT_TRUE({condition}); }}")
        lines.extend(["int main(int argc, char** argv) {",
                      "    const char* filter = argc > 1 ? argv[1] : 0;",
                      "    UNITY_BEGIN();"])
        for index in range(count):
            name = f"Suite.Test{index:05d}"
            lines.append(
                f'    if (!filter || strcmp(filter, "{name}") == 0) '
                f"RUN_TEST(test_{index:05d});")
        lines.extend(["    return UNITY_END();", "}"])
    elif framework == "acutest":
        lines = ['#include "acutest.h"']
        for index in range(count):
            lines.append(
                f"void test_{index:05d}(void) {{ TEST_CHECK({condition}); }}")
        lines.append("TEST_LIST = {")
        for index in range(count):
            lines.append(
                f'    {{ "Suite.Test{index:05d}", test_{index:05d} }},')
        lines.extend(["    { 0, 0 }", "};"])
    elif framework == "jctest_cpp":
        lines = ["#define JC_TEST_IMPLEMENTATION", '#include "jc_test.hpp"']
        for index in range(count):
            lines.append(
                f"TEST(Suite, Test{index:05d}) {{ ASSERT_TRUE({condition}); }}")
        lines.extend(["int main(int argc, char** argv) {",
                      "    jc_test_init(&argc, argv);",
                      "    return jc_test_run_all();", "}"])
    elif framework == "gtest":
        lines = ["#include <gtest/gtest.h>"]
        for index in range(count):
            lines.append(
                f"TEST(Suite, Test{index:05d}) {{ ASSERT_TRUE({condition}); }}")
    elif framework == "doctest":
        lines = ["#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN",
                 '#include "doctest/doctest.h"']
        for index in range(count):
            lines.append(
                f'TEST_CASE("Suite.Test{index:05d}") {{ CHECK({condition}); }}')
    elif framework == "catch2":
        lines = ["#define CATCH_CONFIG_MAIN", '#include "catch2/catch.hpp"']
        for index in range(count):
            lines.append(
                f'TEST_CASE("Suite.Test{index:05d}") {{ REQUIRE({condition}); }}')
    else:
        raise ValueError(f"unknown framework: {framework}")
    return "\n".join(lines) + "\n"


def filter_arguments(framework, count):
    full_name = f"Suite.Test{count - 1:05d}"
    if framework in ("jctest_c", "utest", "jctest_cpp"):
        return [f"--filter={full_name}"]
    if framework == "greatest":
        return ["-t", f"test_{count - 1:05d}"]
    if framework in ("unity", "acutest", "catch2"):
        return [full_name]
    if framework == "gtest":
        return [f"--gtest_filter={full_name}"]
    if framework == "doctest":
        return [f"--test-case={full_name}"]
    raise ValueError(f"unknown framework: {framework}")


def runtime_arguments(framework):
    if framework == "acutest":
        return ["--no-exec"]
    return []


def compiler_version(compiler):
    result = run([compiler, "--version"])
    return result.stdout.splitlines()[0]


def git_revision(path):
    if path is None:
        return "not benchmarked"
    result = run(["git", "-C", str(path), "rev-parse", "HEAD"], check=False)
    if result.returncode == 0:
        return result.stdout.strip()
    return "unknown"


def require_file(parser, configs, key, path, relative_path, option):
    if any(config["key"] == key for config in configs.values()) and \
            (path is None or not (path / relative_path).is_file()):
        parser.error(f"{option} must contain {relative_path}")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--utest-dir", type=Path,
                        help="checkout containing utest.h")
    parser.add_argument("--greatest-dir", type=Path,
                        help="checkout containing greatest.h")
    parser.add_argument("--unity-dir", type=Path,
                        help="checkout containing src/unity.c")
    parser.add_argument("--acutest-dir", type=Path,
                        help="checkout containing include/acutest.h")
    parser.add_argument("--gtest-dir", type=Path,
                        help="GoogleTest checkout")
    parser.add_argument("--doctest-dir", type=Path,
                        help="checkout containing doctest/doctest.h")
    parser.add_argument("--catch2-dir", type=Path,
                        help="Catch2 v2 checkout containing catch2/catch.hpp")
    parser.add_argument("--tests", type=int, default=5000)
    parser.add_argument("--runs", type=int, default=10,
                        help="measured runtime runs")
    parser.add_argument("--warmups", type=int, default=2,
                        help="runtime warmup runs")
    parser.add_argument("--compile-runs", type=int, default=3)
    parser.add_argument("--compile-warmups", type=int, default=1)
    parser.add_argument("--cc", default=os.environ.get("CC", "clang"))
    parser.add_argument("--cxx", default=os.environ.get("CXX", "clang++"))
    parser.add_argument("--build-dir", type=Path,
                        default=Path("build/comparisons"))
    parser.add_argument("--output", type=Path)
    parser.add_argument("--framework", action="append",
                        choices=("jctest_c", "utest", "greatest", "unity",
                                 "acutest", "jctest_cpp", "gtest",
                                 "doctest", "catch2"),
                        help="benchmark only the selected framework (repeatable)")
    args = parser.parse_args()

    root = Path(__file__).resolve().parents[2]
    source_dir = root / "src"
    args.build_dir.mkdir(parents=True, exist_ok=True)

    configs = {
        "jc_test C": {
            "language": "C", "key": "jctest_c", "compiler": args.cc,
            "standard": "c11", "includes": [source_dir], "support": [],
        },
        "utest.h": {
            "language": "C", "key": "utest", "compiler": args.cc,
            "standard": "c11", "includes": [args.utest_dir], "support": [],
            "dependency": ("utest.h", args.utest_dir),
        },
        "greatest": {
            "language": "C", "key": "greatest", "compiler": args.cc,
            "standard": "c11", "includes": [args.greatest_dir], "support": [],
            "dependency": ("greatest", args.greatest_dir),
        },
        "Unity": {
            "language": "C", "key": "unity", "compiler": args.cc,
            "standard": "c11", "includes": [args.unity_dir / "src"
                                               if args.unity_dir else None],
            "support": [args.unity_dir / "src" / "unity.c"
                        if args.unity_dir else None],
            "dependency": ("Unity", args.unity_dir),
        },
        "Acutest": {
            "language": "C", "key": "acutest", "compiler": args.cc,
            "standard": "c11", "includes": [args.acutest_dir / "include"
                                               if args.acutest_dir else None],
            "support": [], "dependency": ("Acutest", args.acutest_dir),
        },
        "jc_test C++": {
            "language": "C++", "key": "jctest_cpp", "compiler": args.cxx,
            "standard": "c++11", "includes": [source_dir], "support": [],
        },
        "GoogleTest": {
            "language": "C++", "key": "gtest", "compiler": args.cxx,
            "standard": "c++11",
            "includes": [args.gtest_dir / "googletest" / "include"
                         if args.gtest_dir else None,
                         args.gtest_dir / "googletest"
                         if args.gtest_dir else None],
            "support": [args.gtest_dir / "googletest" / "src" / "gtest-all.cc",
                        args.gtest_dir / "googletest" / "src" / "gtest_main.cc"]
            if args.gtest_dir else [None],
            "dependency": ("GoogleTest", args.gtest_dir),
        },
        "doctest": {
            "language": "C++", "key": "doctest", "compiler": args.cxx,
            "standard": "c++11", "includes": [args.doctest_dir], "support": [],
            "dependency": ("doctest", args.doctest_dir),
        },
        "Catch2 v2": {
            "language": "C++", "key": "catch2", "compiler": args.cxx,
            "standard": "c++11",
            "includes": [args.catch2_dir / "single_include"
                         if args.catch2_dir else None],
            "support": [], "dependency": ("Catch2 v2", args.catch2_dir),
        },
    }
    if args.framework:
        selected = set(args.framework)
        configs = {label: config for label, config in configs.items()
                   if config["key"] in selected}

    require_file(parser, configs, "utest", args.utest_dir, Path("utest.h"),
                 "--utest-dir")
    require_file(parser, configs, "greatest", args.greatest_dir,
                 Path("greatest.h"), "--greatest-dir")
    require_file(parser, configs, "unity", args.unity_dir,
                 Path("src/unity.c"), "--unity-dir")
    require_file(parser, configs, "acutest", args.acutest_dir,
                 Path("include/acutest.h"), "--acutest-dir")
    require_file(parser, configs, "gtest", args.gtest_dir,
                 Path("googletest/src/gtest-all.cc"), "--gtest-dir")
    require_file(parser, configs, "doctest", args.doctest_dir,
                 Path("doctest/doctest.h"), "--doctest-dir")
    require_file(parser, configs, "catch2", args.catch2_dir,
                 Path("single_include/catch2/catch.hpp"), "--catch2-dir")

    rows = []
    for label, config in configs.items():
        language = config["language"]
        key = config["key"]
        compiler = config["compiler"]
        extension = ".cpp" if language == "C++" else ".c"
        pass_source = args.build_dir / f"{key}_pass{extension}"
        fail_source = args.build_dir / f"{key}_fail{extension}"
        pass_source.write_text(generated_tests(key, args.tests, True))
        fail_source.write_text(generated_tests(key, args.tests, False))

        include_flags = [flag for directory in config["includes"]
                         for flag in ("-I", str(directory))]
        common = [compiler, "-O2", f"-std={config['standard']}",
                  *include_flags]
        support_objects = []
        for index, support_source in enumerate(config["support"]):
            support_object = args.build_dir / f"{key}_support_{index}.o"
            run([*common, "-c", str(support_source), "-o", str(support_object)])
            support_objects.append(support_object)

        pass_object = args.build_dir / f"{key}_pass.o"
        fail_object = args.build_dir / f"{key}_fail.o"
        pass_executable = args.build_dir / f"{key}_pass"
        fail_executable = args.build_dir / f"{key}_fail"
        compile_command = [*common, "-c", str(pass_source),
                           "-o", str(pass_object)]
        compile_seconds = elapsed(compile_command, args.compile_warmups,
                                  args.compile_runs)
        run(compile_command)
        run([*common, "-c", str(fail_source), "-o", str(fail_object)])
        run([compiler, str(pass_object), *map(str, support_objects),
             "-o", str(pass_executable)])
        run([compiler, str(fail_object), *map(str, support_objects),
             "-o", str(fail_executable)])
        preprocessed = run([*common, "-E", str(pass_source)]).stdout

        runtime_args = runtime_arguments(key)
        filter_command = [str(pass_executable), *runtime_args,
                          *filter_arguments(key, args.tests)]
        rows.append({
            "framework": label,
            "language": language,
            "preprocessed_lines": len(preprocessed.splitlines()),
            "compile_seconds": compile_seconds,
            "object_bytes": pass_object.stat().st_size + sum(
                item.stat().st_size for item in support_objects),
            "executable_bytes": pass_executable.stat().st_size,
            "single_filter_seconds": elapsed(
                filter_command, args.warmups, args.runs),
            "all_pass_seconds": elapsed(
                [str(pass_executable), *runtime_args],
                args.warmups, args.runs),
            "all_fail_seconds": elapsed(
                [str(fail_executable), *runtime_args],
                args.warmups, args.runs, check=False),
        })

    dependencies = {}
    for config in configs.values():
        if "dependency" in config:
            name, path = config["dependency"]
            dependencies[name] = git_revision(path)
    result = {
        "platform": platform.platform(),
        "tests": args.tests,
        "warmups": args.warmups,
        "runs": args.runs,
        "compile_warmups": args.compile_warmups,
        "compile_runs": args.compile_runs,
        "cc": compiler_version(args.cc),
        "cxx": compiler_version(args.cxx),
        "dependencies": dependencies,
        "results": rows,
    }
    lines = [
        f"Platform: {result['platform']}",
        f"C compiler: {result['cc']}",
        f"C++ compiler: {result['cxx']}",
    ]
    for name, revision in dependencies.items():
        lines.append(f"{name} revision: {revision}")
    lines.extend([
        f"Workload: {args.tests} tests",
        f"Compile timing: {args.compile_warmups} warmups, median of "
        f"{args.compile_runs} runs",
        f"Runtime timing: {args.warmups} warmups, median of {args.runs} runs",
    ])
    for language in ("C", "C++"):
        language_rows = [row for row in rows if row["language"] == language]
        if not language_rows:
            continue
        lines.extend([
            "",
            f"## {language} frameworks",
            "",
            "| Framework | Preprocessed lines | Compile (s) | Object bytes | "
            "Executable bytes | Single filter (s) | All pass (s) | All fail (s) |",
            "|---|---:|---:|---:|---:|---:|---:|---:|",
        ])
        for row in language_rows:
            lines.append(
                f"| {row['framework']} | {row['preprocessed_lines']} | "
                f"{row['compile_seconds']:.6f} | {row['object_bytes']} | "
                f"{row['executable_bytes']} | "
                f"{row['single_filter_seconds']:.6f} | "
                f"{row['all_pass_seconds']:.6f} | "
                f"{row['all_fail_seconds']:.6f} |")
    report = "\n".join(lines) + "\n"
    print(report, end="")
    if args.output:
        args.output.write_text(report)
        args.output.with_suffix(".json").write_text(
            json.dumps(result, indent=2) + "\n")


if __name__ == "__main__":
    main()
