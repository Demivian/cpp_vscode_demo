# MyProject Demo

A C++ project scaffold built with CMake.

## Quick Start

Configure and build with a preset:

```powershell
cmake --preset gcc-debug
cmake --build --preset gcc-debug
```

Run tests:

```powershell
ctest --test-dir build/gcc-debug --output-on-failure
```

## Project Layout

- `app/`: demo program entry point
- `include/`: public headers
- `src/`: core implementation
- `tests/`: test executables
- `data/`: bundled data
- `examples/`: example inputs and outputs
- `docs/`: project notes and structure guide

## Entry Points

- Main program: `app/main.cpp`
- Core headers: `include/`
- Core implementation: `src/`
- Structure notes: `docs/README.md`

See `docs/README.md` for the fuller project-structure notes.
