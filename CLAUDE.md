# CLAUDE.md

Guidance for Claude Code (claude.ai/code) when working in this repository.

## Project

SuitC is a bridge suit combination analyser by Gerrit A. van der Velde
(Leiden). It is plain C with no dependencies beyond the standard library.
The repository also holds dds (a multi-layout double-dummy solver), playc (a
suit combination enumerator), Gerrit's database scripts (bash/Perl), examples
and documentation. See README.md and docs/. `experiments/` holds work that is
not part of the SuitC build (each with its own CMakeLists.txt and README).

The git history up to Rev 2.022 was reconstructed from Gerrit's RCS archives.
Those commits are authored by him and carry an `RCS-History-Id` trailer. Do
not rewrite them.

## Build and test

```sh
cmake -S . -B build && cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

- CMake works on Linux, macOS and Windows (MSVC). Gerrit's own Makefiles in
  `src/`, `dds/` and `playc/` must keep working too (`mkdir -p bin; cd src; make`).
- `extern.h` is generated from `global.h` (CMake: in the build dir; Makefiles:
  next to the sources). Never edit or commit `extern.h`; edit `global.h`.
- `tests/cases.txt` lists the regression cases; `tests/expected/*.out` is
  the output of Rev 2.022, identical on gcc, clang and MSVC. Any change not
  meant to alter the analysis must keep every test passing. Regenerate the
  expected output with `cmake -DSUITC=<binary> -P tests/update_expected.cmake`
  only for intended changes of results.

## Code conventions

- One C function per `.c` file, named after the function.
- All state is global and declared in `global.h`. `header.h` holds the
  types, macros and pool sizes (`NNODES`, `NINDICES`, `NQUIRKS`).
- `COLOR` is an `unsigned short` card bit mask; `addcard(c)` sets a bit and
  `low_c(c)` isolates the lowest one. Players: `west=0, north=1, east=2, south=3`.
- Memory comes from pre-allocated pools with bump allocation, not
  malloc/free per item.
- `SUITCAPP` (compile-time, `YES`/`NO`) selects the reduced output used when
  SuitC is embedded in other applications.
- Match Gerrit's style: 4-space indentation, `//` comments, spaces inside
  parentheses.

## Related project

SuitCWinLib (github.com/codescrut/SuitCWinLib) wraps this code as a shared
library with a JSON interface for BEN, Lia and Lars's SuitC web application.
