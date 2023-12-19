#!/usr/bin/env python3

import argparse
import os
from pathlib import Path
import subprocess
import tempfile


TEST_TARGETS = {
    "ownership_characterization": [
        "test_ownership_characterization.cpp", "levenshtein.cpp"
    ],
    "ownership_edge_cases": ["test_ownership_edge_cases.cpp", "levenshtein.cpp"],
    "exclusive_handle": ["test_exclusive_handle.cpp"],
    "chatbot_integration": ["test_chatbot_integration.cpp"],
    "graphnode_move": ["test_graphnode_move.cpp"],
    "graphnode_move_edge_cases": ["test_graphnode_move_edge_cases.cpp"],
    "graphnode_move_comparison": ["test_graphnode_move_comparison.cpp"],
}


def parse_args():
    parser = argparse.ArgumentParser(description="Build and run the offline C++ tests.")
    parser.add_argument(
        "--sanitizers", action="store_true",
        help="Enable address and undefined-behavior sanitizers",
    )
    return parser.parse_args()


def main():
    args = parse_args()
    tests_dir = Path(__file__).resolve().parent
    repository = tests_dir.parent
    compiler = os.environ.get("CXX", "g++")
    flags = [
        "-std=c++17", "-Wall", "-Wextra", "-Wpedantic", "-Werror", "-UNDEBUG"
    ]
    if args.sanitizers:
        flags.extend(
            ["-fsanitize=address,undefined", "-fno-omit-frame-pointer"]
        )

    with tempfile.TemporaryDirectory(prefix="membot-tests-") as build_dir:
        build_path = Path(build_dir)
        for name, sources in TEST_TARGETS.items():
            executable = build_path / (name + (".exe" if os.name == "nt" else ""))
            command = [
                compiler,
                *flags,
                "-I{}".format(repository / "src"),
                *(str(tests_dir / source) for source in sources),
                "-o",
                str(executable),
            ]
            print("BUILD {}".format(name), flush=True)
            subprocess.run(command, check=True)
            print("RUN   {}".format(name), flush=True)
            subprocess.run([str(executable)], check=True)

    print("All {} offline test binaries passed.".format(len(TEST_TARGETS)))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
