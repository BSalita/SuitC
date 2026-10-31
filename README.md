# SuitC

**SuitC** analyses bridge suit combinations. Given the cards North and South
hold in one suit, it finds the best way to play the suit, assuming North-South
know only their own cards and the probability of each East-West distribution.
It does this for several objectives:

- the maximum expected number of tricks,
- safety plays: the best chance of making at least *n* tricks,
- the best play at **matchpoints** and at **IMPs**, including mixed strategies.

SuitC was written by **Gerrit A. van der Velde** (Leiden, the Netherlands). The
project started in the late 1990s, with his earlier program ExpertPlay. Gerrit's own
revision history from 2008 onwards is preserved in this repository; see
[HISTORY.md](HISTORY.md). SuitC is used by the bridge programs
[BEN](https://github.com/lorserker/ben) and Lia, and by Lars Kappel Hansen's
[SuitC web application](https://bridge.esmarkkappel.dk/suitc/main.html), through the
[SuitCWinLib](https://github.com/codescrut/SuitCWinLib) library.

```text
$ SuitC AQ2 543 KJT9876

The optimum play for: MAX N2
============================

	2 tricks:  50.4845
	1 tricks:  49.5155
	Result  :   1.5048

                                          - J J J J J J K K K K K K K
                                          J   J J J J J   J J J J J J
                                          ...
N:A  KJ-  3  KJ-    S:5  K   2  J-     ns=2 2 2 2 2 2 - - - - - - - -
                         J-  Q  KJ-    ns=2 2 2 2 2 2 2 2 1 1 1 1 1 1
```

With AQ2 opposite 543, the best line is: cash the ace, then lead towards the
queen. It wins two tricks 50.48 % of the time, and the expected number of tricks
is 1.5048. [docs/output.md](docs/output.md) explains how to read the game tree.

## Contents

| Directory | What it is |
|---|---|
| [src/](src) | SuitC, the suit combination analyser |
| [dds/](dds) | dds, a double-dummy solver that compares plays over several layouts of a deal |
| [playc/](playc) | playc, which enumerates the distributions of a suit (used to build the database) |
| [scripts/](scripts) | Gerrit's scripts to build a database of all suit combinations and report on it |
| [examples/](examples) | Example input and output for dds and for the playing-strength studies |
| [docs/](docs) | Documentation |
| [tests/](tests) | Regression tests |
| [experiments/](experiments) | Experiments that are not part of the SuitC build, such as a [faster parallel merge](experiments/bitmask-merge) |

## Getting SuitC

Pre-built programs for Linux (x64, arm64), macOS (arm64, x64) and Windows
(x64) are attached to each [release](../../releases).

To build from source you need a C compiler and CMake 3.21 or newer:

```sh
cmake -S . -B build
cmake --build build --config Release
ctest --test-dir build -C Release      # optional: run the regression tests
```

The programs are written to `build/` (`build/Release/` with Visual Studio).
Gerrit's original Makefiles in `src/`, `dds/` and `playc/` still work on Linux,
macOS, Cygwin and WSL. See [docs/building.md](docs/building.md) for details.

## Documentation

- [Usage](docs/usage.md): command-line options and examples
- [Reading the output](docs/output.md)
- [How SuitC works](docs/algorithm.md)
- [Building](docs/building.md): compile options and memory limits
- [The suit-combination database and scripts](docs/database.md), and
  [how the database is built](docs/building-the-database.md) and
  [how to use it](docs/using-the-database.md) by Gerrit A. van der Velde
- [dds and playc](docs/dds-and-playc.md)
- [Hand Valuation and Computers](docs/hand-valuation.md), an essay by Gerrit A. van der Velde
- [History](HISTORY.md) and [changelog](CHANGELOG.md)

## Credits

SuitC, dds, playc and the scripts are the work of **Gerrit A. van der Velde**.
[AUTHORS.md](AUTHORS.md) lists everyone who contributed ideas, ports and fixes.

## Licence

SuitC is released under the [MIT licence](LICENSE).
