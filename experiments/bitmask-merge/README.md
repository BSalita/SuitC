# Experiment: a parallel bit-mask merge

This directory holds an experiment, not part of SuitC: a stand-alone
re-implementation of the step where SuitC spends almost all of its time on
large suit combinations. It shows that this step can give **identical
results 5–14 times faster** on 32 threads. SuitC itself is unchanged; nothing
here is built by the main `CMakeLists.txt`.

## Where SuitC spends its time

On hard combinations nearly all the time goes into building the QUIRK tree
(phase 3 in [How SuitC works](../../docs/algorithm.md)), and within that into
a few **East-West merges**: `merge_quirk` combining the strategies of two
East-West plays, and `check_quirk` removing the dominated ones. Measured with
an instrumented copy of SuitC (Rev 2.022, MSVC Release build):

| Combination | Total | `merge_quirk` + `add_quirk` | `check_quirk` | Rest |
|---|---|---|---|---|
| `-Ls AQT3 74 KJ98652` | 5 s | 57 % | 42 % | 1 % |
| `-Ls AQ93 75 KJT8642` | 324 s | 50 % | 49 % | 1 % |

In the second case, 12 merges at depth 2 (West's play to the first trick) take
98.5 % of the time; the slowest takes 61 s. Such a merge forms every pair of
strategies, 68,280 × 530 = 36 million, and keeps only 66,993 (0.2 %).
`-Ls AQT3 75 KJ98642` (the case in [usage.md](../../docs/usage.md)) takes
32 minutes on the same machine, with 500,520 strategies at the end.

## The idea

A strategy is the number of tricks North-South take on each East-West
distribution (board) that is still possible. In these merges that is 60–85
boards with 0 to 3 tricks each. `bitmask_merge` stores a strategy bit-sliced:
`mask[v]` is the set of boards on which North-South take more than `v`
tricks. Then:

- the merge of two strategies, the minimum on each board, is a bitwise AND;
- "x is dominated by y" is: every mask of x is a subset of the mask of y.

Boards that only one side has get all bits set on the other side, so the AND
keeps the value of the side that has the board, as `merge_quirk` does.

The dominated strategies are removed in three steps:

1. **Group filter.** The pairs that share one strategy of the smaller side
   form a group. Each group is sorted by its total number of tricks and
   filtered on its own, in parallel. This cuts 36 million pairs to about
   200,000 candidates.
2. **Sort.** A strategy can only be dominated by one with more tricks in
   total, or by an identical one, so after sorting each candidate only has
   to be checked against the earlier ones.
3. **Chunked final filter.** The sorted list is cut into 256 chunks; each
   chunk is filtered on its own, then each survivor is checked against the
   survivors of the earlier chunks. Both passes run in parallel.

## Results

AMD Ryzen Threadripper PRO 5995WX, Windows 11, MSVC 18 (Visual Studio 2026),
OpenMP. SuitC's times are for `merge_quirk` alone, measured inside SuitC.
In every case `bitmask_merge` keeps exactly the same set of strategies as
SuitC.

| Merge (strategies) | Pairs | SuitC | `bitmask_merge`, 1 thread | 32 threads | Speed-up at 32 threads |
|---|---|---|---|---|---|
| 1,703 × 799 | 1.4 M | 1.41 s | 1.71 s | 0.12 s | 12× |
| 10,682 × 132 | 1.4 M | 1.84 s | 2.56 s | 0.15 s | 12× |
| 68,280 × 530 | 36 M | 60.8 s | — | 4.2 s | 14× |
| 66,993 × 470 | 31 M | 56.4 s | — | 10.1 s | 5.6× |

What this shows, and what it does not:

- On **one thread** `bitmask_merge` is slightly *slower* than SuitC. SuitC's
  QUIRK tree, which shares the common start of strategies, is already an
  efficient structure. The gain comes from the bit masks *together with*
  the group filter and the threads, which SuitC's tree cannot use easily.
- The **final filter** grows roughly with the square of the number of
  survivors. It takes three quarters of the time in the last merge, and is
  the next thing to improve (for example by indexing the kept strategies).
- These are times for **single merges**, not for a complete SuitC run.

## Before this could go into SuitC

- **Order.** SuitC keeps the strategies in the order of its QUIRK tree, and
  that order can decide which of several equally good plays is printed. A
  real implementation has to reproduce the order, or the regression tests
  in [tests/](../../tests) can change.
- **More levels.** These merges have at most 3 tricks per board. Suits with
  up to 13 tricks need up to 13 masks per strategy, so each operation costs
  more.
- **Threads.** All of SuitC's state is global (`global.h`), so it cannot run
  threads inside an analysis yet. The merge could be swapped in first,
  converting the QUIRK trees to bit masks before the merge and back after it.

## Building and running

```sh
cmake -S experiments/bitmask-merge -B build-bitmask
cmake --build build-bitmask --config Release
```

The code is portable C with optional OpenMP, but so far it has only been built
and tested with MSVC on Windows. This builds two programs (in
`build-bitmask/Release/` with Visual Studio):

- `suitc_dump`: SuitC with one change: `convert_tree_to_quirk` calls
  [dump_merge](dump_merge.c) instead of `merge_quirk`. For each large merge it
  writes the two inputs and SuitC's result as `dump<n>_a.txt`, `dump<n>_b.txt`
  and `dump<n>_out.txt` in the current directory, one strategy per line as
  `board:tricks board:tricks ...`. Its analysis output is the same as
  SuitC's. `SUITC_DUMP_MIN_PAIRS` (default 1,000,000) sets the size of the
  merges that are written; `SUITC_DUMP_MAX` (default 3) sets how many.
- `bitmask_merge`: the merge itself. `bitmask_merge a.txt b.txt [out.txt]`
  merges the two lists, and with `out.txt` compares the result with SuitC's.
  `OMP_NUM_THREADS` sets the number of threads. It exits with 1 if the
  results differ.

To reproduce the first two rows (about 5 seconds):

```sh
mkdir dumps && cd dumps
../build-bitmask/Release/suitc_dump -Ls AQT3 74 KJ98652
../build-bitmask/Release/bitmask_merge dump0_a.txt dump0_b.txt dump0_out.txt
../build-bitmask/Release/bitmask_merge dump1_a.txt dump1_b.txt dump1_out.txt
```

For the last two rows run `SUITC_DUMP_MIN_PAIRS=30000000 suitc_dump -Ls AQ93
75 KJT8642` instead (about 5 minutes, about 100 MB of dump files).
