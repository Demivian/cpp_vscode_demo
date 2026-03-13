# MyProject Demo Notes

## Overview

This document is the detailed companion to the repository root `README.md`.
It records the current project structure, build targets, and development entry points.

The repository is a C++ demo project organized around a small executable, a reusable core library,
and a small test set.
It uses CMake to manage a core library, a main executable, and multiple test executables.

It focuses on project layout and development entry points rather than algorithm details.

## Directory Layout

```text
project/
  app/
  include/
  src/
  tests/
  data/
  examples/
  docs/
  build/
  CMakeLists.txt
  CMakePresets.json
```

## Directory Responsibilities

- `app/`
  Main program entry. This is where the demo executable starts.

- `include/`
  Public headers used by the project targets. This directory is intended to hold reusable interfaces.

- `src/`
  Core implementation. The `core` library target is defined from sources in this directory.

- `tests/`
  Test code and test target definitions.

- `data/`
  Static input data bundled with the demo.

- `examples/`
  Example case inputs and outputs. `examples/default_case/` is the current working example case.

- `docs/`
  Project documentation. This file is intended as the structural guide for the demo.

- `build/`
  Generated build trees and binaries created by CMake presets. This directory is not part of the
  source layout.

## Build Targets

- `core`
  The internal library target that contains the reusable implementation code.

- `main`
  The main demo executable.

- `map3d_synthetic_test`
  A test executable.

- `map3d_hwr010_smoke_test`
  A test executable.

## Targets

- `main`
  Runs the current demo entry flow.

- `map3d_synthetic_test`
  Runs a test flow.

- `map3d_hwr010_smoke_test`
  Runs a test flow.

## Running and Debugging

This project is configured for CMake Tools in VS Code.

- Use `CMake: Select Launch Target` to switch between the available executable and test targets.
- The current VS Code launch configurations use `${command:cmake.launchTargetPath}` so the selected
  CMake launch target determines what is run.
- The working directory is configured around `examples/default_case`.

## Running Tests

The project exposes CTest tests for the current test targets:

- `map3d_synthetic_test`
- `map3d_hwr010_smoke_test`

CTest decides pass/fail from the test process result:

- exit code `0` means pass
- non-zero exit, crash, or timeout means fail

The current tests use assertion helpers that terminate with a non-zero exit code on failure.
If all checks succeed, the test program exits normally and CTest marks it as passed.

Common commands:

```powershell
ctest --test-dir build/gcc-debug --output-on-failure
ctest --test-dir build/gcc-debug -V
ctest --test-dir build/gcc-debug -R map3d_synthetic_test --output-on-failure
ctest --test-dir build/msvc-ninja-multi -C Debug --output-on-failure
```

Notes about output:

- By default, CTest usually does not print stdout for passing tests.
- Use `-V` or `-VV` if you want to see test program output even when tests pass.
- `CMake: Run Tests` in VS Code also runs tests through CTest.

## Where To Start

- Start from `app/main.cpp` if you want to inspect the demo program flow.
- Start from `include/` and `src/` if you want to inspect the current core interfaces and implementation.
- Start from `tests/` if you want to inspect the current test layout.

## Notes

- This README is intentionally structure-oriented.
- It does not describe the numerical model in detail.
- It should stay aligned with the current directory layout, target names, and CMake workflow.
