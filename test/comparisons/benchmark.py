#!/usr/bin/env python3
"""Benchmark jc_test C/C++ against sheredom/utest.h.

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


def elapsed(command, warmups, runs):
    for _ in range(warmups):
        run(command, stdout=subprocess.DEVNULL, check=False)
    samples = []
    for _ in range(runs):
        start = time.perf_counter()
        run(command, stdout=subprocess.DEVNULL, check=False)
        samples.append(time.perf_counter() - start)
    return statistics.median(samples)


def generated_tests(framework, count, passing):
    assertion = "ASSERT_TRUE(1);" if passing else "ASSERT_TRUE(0);"
    if framework == "jctest_c":
        lines = ["#define JC_TEST_IMPLEMENTATION", '#include "jc_test.h"']
        test_macro = "TEST"
        main = ["int main(int argc, char** argv) {",
                "    jc_test_init(&argc, argv);",
                "    return jc_test_run_all();", "}"]
    elif framework == "jctest_cpp":
        lines = ["#define JC_TEST_IMPLEMENTATION", '#include "jc_test.hpp"']
        test_macro = "TEST"
        main = ["int main(int argc, char** argv) {",
                "    jc_test_init(&argc, argv);",
                "    return jc_test_run_all();", "}"]
    else:
        lines = ['#include "utest.h"']
        test_macro = "UTEST"
        main = ["UTEST_MAIN();"]
    for index in range(count):
        lines.append(f"{test_macro}(Suite, Test{index:05d}) {{ {assertion} }}")
    lines.extend(main)
    return "\n".join(lines) + "\n"


def compiler_version(compiler):
    result = run([compiler, "--version"])
    return result.stdout.splitlines()[0]


def git_revision(path):
    result = run(["git", "-C", str(path), "rev-parse", "HEAD"], check=False)
    if result.returncode == 0:
        return result.stdout.strip()
    return "unknown"


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--utest-dir", type=Path, required=True,
                        help="checkout containing utest.h")
    parser.add_argument("--tests", type=int, default=5000)
    parser.add_argument("--runs", type=int, default=10)
    parser.add_argument("--warmups", type=int, default=2)
    parser.add_argument("--cc", default=os.environ.get("CC", "clang"))
    parser.add_argument("--cxx", default=os.environ.get("CXX", "clang++"))
    parser.add_argument("--build-dir", type=Path,
                        default=Path("build/comparisons"))
    parser.add_argument("--output", type=Path)
    parser.add_argument("--framework", action="append",
                        choices=("jctest_c", "jctest_cpp", "utest"),
                        help="benchmark only the selected framework (repeatable)")
    args = parser.parse_args()

    root = Path(__file__).resolve().parents[2]
    source_dir = root / "src"
    args.build_dir.mkdir(parents=True, exist_ok=True)
    if not (args.utest_dir / "utest.h").is_file():
        parser.error("--utest-dir must contain utest.h")

    configs = {
        "jc_test C": ("jctest_c", args.cc, "c11", [source_dir]),
        "jc_test C++": ("jctest_cpp", args.cxx, "c++11", [source_dir]),
        "utest C": ("utest", args.cc, "c11", [args.utest_dir]),
    }
    if args.framework:
        selected = set(args.framework)
        configs = {label: config for label, config in configs.items()
                   if config[0] in selected}
    rows = []
    for label, (key, compiler, standard, includes) in configs.items():
        pass_source = args.build_dir / f"{key}_pass.c"
        fail_source = args.build_dir / f"{key}_fail.c"
        if key == "jctest_cpp":
            pass_source = pass_source.with_suffix(".cpp")
            fail_source = fail_source.with_suffix(".cpp")
        pass_source.write_text(generated_tests(key, args.tests, True))
        fail_source.write_text(generated_tests(key, args.tests, False))

        include_flags = [flag for directory in includes
                         for flag in ("-I", str(directory))]
        common = [compiler, "-O2", f"-std={standard}", *include_flags]
        pass_object = args.build_dir / f"{key}_pass.o"
        pass_executable = args.build_dir / f"{key}_pass"
        fail_executable = args.build_dir / f"{key}_fail"
        compile_command = [*common, "-c", str(pass_source), "-o", str(pass_object)]

        compile_seconds = elapsed(compile_command, args.warmups, args.runs)
        run(compile_command)
        run([compiler, str(pass_object), "-o", str(pass_executable)])
        run([*common, str(fail_source), "-o", str(fail_executable)])
        preprocessed = run([*common, "-E", str(pass_source)]).stdout

        filter_name = f"Suite.Test{args.tests - 1:05d}"
        filter_argument = f"--filter={filter_name}"
        rows.append({
            "framework": label,
            "preprocessed_lines": len(preprocessed.splitlines()),
            "compile_seconds": compile_seconds,
            "object_bytes": pass_object.stat().st_size,
            "executable_bytes": pass_executable.stat().st_size,
            "single_filter_seconds": elapsed(
                [str(pass_executable), filter_argument], args.warmups, args.runs),
            "all_pass_seconds": elapsed(
                [str(pass_executable)], args.warmups, args.runs),
            "all_fail_seconds": elapsed(
                [str(fail_executable)], args.warmups, args.runs),
        })

    result = {
        "platform": platform.platform(),
        "tests": args.tests,
        "warmups": args.warmups,
        "runs": args.runs,
        "cc": compiler_version(args.cc),
        "cxx": compiler_version(args.cxx),
        "utest_revision": git_revision(args.utest_dir),
        "results": rows,
    }
    lines = [
        f"Platform: {result['platform']}",
        f"C compiler: {result['cc']}",
        f"C++ compiler: {result['cxx']}",
        f"utest revision: {result['utest_revision']}",
        f"Workload: {args.tests} tests, {args.warmups} warmups, "
        f"median of {args.runs} runs",
        "",
        "| Framework | Preprocessed lines | Compile (s) | Object bytes | "
        "Executable bytes | Single filter (s) | All pass (s) | All fail (s) |",
        "|---|---:|---:|---:|---:|---:|---:|---:|",
    ]
    for row in rows:
        lines.append(
            f"| {row['framework']} | {row['preprocessed_lines']} | "
            f"{row['compile_seconds']:.6f} | {row['object_bytes']} | "
            f"{row['executable_bytes']} | {row['single_filter_seconds']:.6f} | "
            f"{row['all_pass_seconds']:.6f} | {row['all_fail_seconds']:.6f} |")
    report = "\n".join(lines) + "\n"
    print(report, end="")
    if args.output:
        args.output.write_text(report)
        args.output.with_suffix(".json").write_text(json.dumps(result, indent=2) + "\n")


if __name__ == "__main__":
    main()
