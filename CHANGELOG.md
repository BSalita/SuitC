# Changelog

SuitC, dds and playc are versioned with the revision numbers of their author,
Gerrit A. van der Velde. Each revision below is a commit in the git history
(see `git log`). The tags `rev-1.037`, `rev-1.038`, `rev-1.039`, `rev-2.010`,
`rev-2.011`, `rev-2.020` and `rev-2.022` mark the revisions Gerrit distributed; published
releases, with binaries for all platforms, are tagged `v<revision>`. The
entries are Gerrit's own comments from his RCS history files, which are kept
unchanged in [docs/history](docs/history).

## Unreleased

- [experiments/bitmask-merge](experiments/bitmask-merge): a profile of where
  SuitC spends its time on large combinations (the East-West merges), and a
  stand-alone parallel bit-mask version of that merge that keeps identical
  strategies 5–14 times faster on 32 threads. SuitC itself is unchanged.

## v2.022

Gerrit's latest revision. The results are identical to Rev 2.011 for all
regression cases; only the presentation of the game trees differs slightly.

- New options for presenting the game tree: `-F8` (select the North-South
  plays, then fold the East-West plays), `-Fs<n>` (do not fold the first `<n>`
  plies) and `-m{U|US|D|DS}` (the order in which the tree is folded).
- Fixes for IMP scoring. `-I` without a score is now rejected; it read an
  uninitialised variable.
- Folding the East-West plays is back to the Rev 2.011 code, which also
  handles `-D`, where both West and East can lead.
- Bug fixes and a check of the cards entered (Rev 2.012 – 2.015).

## v2.011: open-source release

The first public release of SuitC, at Gerrit's revision 2.011. The analysis
code is unchanged from Rev 2.011.

- MIT licence, documentation, and the git history rebuilt from Gerrit's RCS
  archives.
- CMake build for Linux, macOS and Windows (no WSL or Cygwin needed), with
  pre-built programs for Linux x64/arm64, macOS arm64/x64 and Windows x64.
- Regression tests and continuous integration.
- The Makefiles for dds and playc no longer report an error after a
  successful build outside Cygwin.
- The suit-combination database is published as a release asset.

## SuitC Rev 2 (2025 – 2026)

- **Rev 2.022** (2026-10-18) — A problem with incomplete specification of the -I parameter is handled.
- **Rev 2.021** (2026-10-17) — The code to handle throwins has been deleted again.
- **Rev 2.020** (2026-10-13) — Two minor bug fixes in handling IMP scores.
- **Rev 2.019** (2026-07-03) — We re-introduced throwin plays.
- **Rev 2.018** (2026-04-22) — We have added the facility to skip folding the first n plies of the tree with the argument -Fs<n>. This produces trees where it is clear how east-west play at the first n plies.
- **Rev 2.017** (2026-02-17) — The fold mode of the final tree is entered with parameter -m{U|US|D|DS}. The default is mode DOWN. *(The default is UP; confirmed by Gerrit.)*
- **Rev 2.016** (2025-10-15) — We added alternatives for the order in which the game tree is folded. UP start at the last branch and work upwards; UPSTRIDE start at the last branch and work upwards with equal strides; DOWN start at the first branch and work downwards; DOWNSTRIDE start at the first branch and work downwards with equal strides.
- **Rev 2.015** (2025-09-17) — Bug fix.
- **Rev 2.014** (2025-09-11) — Bug fix.
- **Rev 2.013** (2025-07-14) — Check the cards.
- **Rev 2.012** (2025-06-16) — Minor correction.
- **Rev 2.011** (2025-02-20) — Some minor adjustments.
- **Rev 2.010** (2025-02-09) — The code to maximize entries in the suit itself is moved from determine_trick_count to use_suit_entries; A number of variables have been initialized in the declaration.
- **Rev 2.009** (2025-02-06) — A new option has been added for the selection of the north-south plays in the final game tree. The option '-v' selects the play in which north-south win the tricks as soon as possible and where the use of outside entries is minimized. *(Renamed to `-u` in Rev 2.010.)*
- **Rev 2.008** (2025-02-05) — Routine print_tree has been brushed up.
- **Rev 2.007** (2025-01-31) — Option '-nC' has been deleted.
- **Rev 2.006** (2025-01-28) — The print out of results has been adapted. The new code restores the version that was used before Rev1.046.
- **Rev 2.005** (2025-01-28) — This is some simplification of the code. 'struct STACK' and global variables ns and w have been eliminated. Their functionality is now part of 'struct NODE'.
- **Rev 2.004** (2025-01-26) — The facility to sort the plays in decreasing number of leaves before building the QUIRK tree has been deleted.
- **Rev 2.003** (2025-01-21) — An obsolete option is deleted; THe criteria to lump finals plays together have been sharpended.
- **Rev 2.002** (2025-01-16) — Change in the name of a variable.
- **Rev 2.001** (2025-01-14) — Original version

Revisions 1.040 to 1.050 (August 2024 to January 2025) were not distributed;
Rev 2.001 restarted from Rev 1.050.

## SuitC Rev 1 (2020 – 2024)

- **Rev 1.039** (2024-07-28) — The switch to use the app version for Lars has been moved from header.h to Makefile; A couple of minor cleanups has been inserted.
- **Rev 1.038** (2024-06-16) — A bug fix in print_all_results and some cleanup.
- **Rev 1.037** (2024-02-09) — Cleanup.
- **Rev 1.036** (2024-02-08) — The code for the allocation was incorrect.
- **Rev 1.035** (2024-01-02) — The header files are now correct. The global variables are contained in file global.h. File header.h includes file extern.h. File extern.h defines all global variables as external.
- **Rev 1.034** (2024-01-02) — Some minor corrections.
- **Rev 1.033** (2023-06-26) — The probabilities are printed with additional accuracy; Reordering of the code and removal of some obsolete information in process_args.
- **Rev 1.032** (2023-05-28) — Improved handling of the -l option; Minor changes in comments, probability output and Copyright statement.
- **Rev 1.031** (2023-02-27) — In evaluate_mp_results.c the matchpoint flag in the SCORE list is cleared if only one MP play is found and the list contains a MAX play; The maximum number of INDEX exntries is increased; Some cleanup and corrections of comments
- **Rev 1.030** (2022-10-11) — Minor tidying of the code.
- **Rev 1.029** (2022-10-02) — 1. The output of the board header is modified. We now use the symbol of the highest card in a sequence to represent all cards in the sequence; 2. Each card of a card sequence in the north-south hands is represented only once in the game tree; 3. The options -pS and -pH switch back to the old conventions.
- **Rev 1.028** (2022-09-23) — Routine merge_nodes is also called for north-south plays; The default order of the east-west distributions is the same as that of Jeroen Warmerdam.
- **Rev 1.027** (2022-09-05) — Bug fix to handle the case that neither player can lead to another trick.
- **Rev 1.026** (2022-08-29) — The output for card sequences in the north-south hands has been modified. Instead of printing e.g. twice A for the card sequence AK we now print A and K if twice a card is played form this sequence.
- **Rev 1.025** (2022-08-07) — Some minor corrections in the output.
- **Rev 1.024** (2022-07-26) — The MP or IMP result is printed for all plays unless it is switched off explicitly.
- **Rev 1.023** (2022-07-25) — The handling of optimal IMP plays is installed.
- **Rev 1.022** (2022-07-22) — Print the MP score for all printed plays.
- **Rev 1.021** (2022-04-06) — Renaming of a variable and the correction of a typo.
- **Rev 1.020** (2021-07-26) — Code has been added to print for each distribution the minimum and maximum number of tricks.
- **Rev 1.019** (2021-06-30) — Constant SUITCAPP is added to the header file. With SUITCAPP=YES code is generated for the app in which cutoff is set to CUTOFF and NPLAYS to a value adapted for the app environment. Also some output information is affected.
- **Rev 1.018** (2021-06-30) — For the app we will print at most 10 plays. For this purpose we added constant SUITCAPP in header.h and we added code to evaluate_best_results.c and print_all_results.c to restrict the number of plays that is reported.
- **Rev 1.017** (2021-06-07) — Corrective code for the handling of ruffs.
- **Rev 1.016** (2021-06-01) — Some information is printed only if the -T1 option is used; Some minor changes in error messages.
- **Rev 1.015** (2021-05-07) — A bug fix and a few typographical changes in the output.
- **Rev 1.014** (2021-04-24) — r1.3 of print_tree.c introduced a bug; this has been corrected. Error checks are introduced for the malloc statements. A default value CUTOFF is introduced for the valur of the cutoff variable. The code for handling fixed cards has been improved.
- **Rev 1.013** (2021-04-23) — Work around for a Cygwin error.
- **Rev 1.013x** (2021-03-16) — Remarks on portability issues are applied.
- **Rev 1.012** (2020-11-14) — Minor bug fix.
- **Rev 1.011** (2020-10-21) — The logic to specify 3rd hand strategies has again be implemented.
- **Rev 1.010** (2020-10-21) — The code for throwins has been adapted. It is now possible to specify that an opponent is thrown in if he wins a trick, but this option contains a serious flaw. The program cannot handle the situation that an opponent is thrown in at the moment that he has no cards left. We added the possibility that a player is thrown in at trick 1. To do this we specify -Lw[<card>] or -Le<[card>] on the command line to force a lead at trick 1 rom west or east.
- **Rev 1.009** (2020-10-18) — We switch back to compact trees as default.
- **Rev 1.008** (2020-10-14) — Corrective code for some minor bugs.
- **Rev 1.007** (2020-10-04) — routine select_ns_plays has been rewritten; the default for mergeselect is that north-south win their tricks as soon as possible.
- **Rev 1.006** (2020-09-20) — routine select_ew_plays has been rewritten.
- **Rev 1.005** (2020-08-30) — A new option has been introduced to select north-south plays the wins tricks as soon as possible, This option is activated by the command the command line option '-nC', the command line option '-nC'. The '-k' and '-K' option have been replaced by '-nk' and '-nK'.
- **Rev 1.004** (2020-05-01) — Merging of North-south branches is standard enabled. North-south branches are merge only if both plays have no effect on who wins in the current trick; Some reordering of the code in fold_ew_tree; The leaves are no longer merged explicitly in merge tree. Some cleanupleanup.
- **Rev 1.003** (2020-04-25) — Thge first step in the development of mergeing north-south branches in the NODE tree. routine fold_tree is split in 2 routines.
- **Rev 1.002** (2020-04-25) — All code to handle QUIRK trees has been deleted. A number of routines have been renamed to reflect new naming conventions and a number of routines have been deleted as they are no longer used. The buiild of the SCARD sequences has been recoded. We now use only SCARD sequences restricted to the current hoilding of each player. The bump_*routines are replaced by cpp define's. add_node has been rewritten. A new '-s' option allows sorting of the plays before convert_tree_to_quirk is called.
- **Rev 1.001** (2020-04-25) — This version of the code is identical to ExpertPlay-Rev6.102

## dds

- **dds 1.030** (2024-04-01) — Renaming of variables.
- **dds 1.029** (2024-04-01) — The SEQC information is stored in lists in stead of arrays; The memory management is moved to a separate include file.
- **dds 1.028** (2024-02-19) — Multiple cards enabled for the play.
- **dds 1.027** (2024-01-02) — The header files are now correct. The global variables are contained in file global.h. File header.h includes file extern.h. File extern.h defines all global variables as external.
- **dds 1.026** (2024-01-02) — Some decarations are re-arranged.
- **dds 1.025** (2023-11-30) — Introducing symbolic names for constants.
- **dds 1.024** (2023-11-15) — The name of the NODE tree has been changed from lnode into wnode.
- **dds 1.023** (2023-09-07) — An additional option has been added to specify the starting player in the BOARD description.
- **dds 1.022** (2023-08-28) — The calling sequence of a number of routines has been changed.
- **dds 1.021** (2023-08-19) — Chnge in variable names.
- **dds 1.020** (2023-08-17) — In print_all_boards the boards are now printed five at a time to improve the layout.
- **dds 1.019** (2023-08-16) — The results of all hands are combined in a single report.
- **dds 1.018** (2023-08-15) — This code implements the handling of multiple hands.
- **dds 1.017** (2023-08-14) — The code has been adapted to handle multiple hands. This is an intermediate version that requires additional fine tunung.
- **dds 1.016** (2023-08-11) — Some cosmetic changes; The statistics can be printed selectively.
- **dds 1.015** (2022-05-20) — Renaming of struct member.
- **dds 1.014** (2022-04-17) — Elimination of superfluous variable.
- **dds 1.013** (2022-04-17) — The code to keep track of winning plays has been modified; The order of assignments in dds has been changed.
- **dds 1.012** (2022-04-17) — Copyright statements have been added; member qt has been replaced by member rnk in 'struct NODE'.
- **dds 1.011** (2022-04-17) — Routine build_trick renamed as build_ply. EOF
- **dds 1.010** (2021-08-15) — Renaming of the executable.
- **dds 1.009** (2021-07-23) — We switch to shifted card holdings, i.e. we always use card holdings that have the ace, king, etc. without holes. This idea has been copied fron Bo Haglund.
- **dds 1.008** (2021-07-23) — A. We use now 2 routines to build a list of plays for a ply. Routine build_ply is called from play_board, routine build_trick is called from build_tree and hashbackup. The purpose is to modify build_trick to reduce the number of plays; B. The card sequences are deleted from 'struct SCARD'. The holding of each player is now standardised by shifting the cards to use the highest cards. As a consequence we can use the holding of the players to locate existing SCARD slots.
- **dds 1.007** (2021-07-23) — Routine build_tree has been rewritten as a recursive routine.
- **dds 1.006** (2021-07-23) — The '#define' statements in header.h are inlined.
- **dds 1.005** (2021-07-23) — Routine build_trick has been renamed to build_ply; routine print_scard has been rewritten.
- **dds 1.004** (2021-07-23) — 'struct SCARD' has been split into 'struct SCARD' and 'struct SEQC' to keep track of card sequences; The code has been revised to keep track of cards that are still in play.
- **dds 1.003** (2021-07-23) — The code for handling backup information is simplified. The depth in the tree is changed by one in hashbackup and join_scard. Routine join_scard is rewritten.
- **dds 1.002** (2021-07-23) — This is a temporary work around for a compiler problem. The compiler cannot handle the situation that a header file is included in each source, so we commented out the #include statements in the source files and included all sources in the new file dda.c.
- **dds 1.001** (2021-07-23) — This version is the same as ../Backups/dds.20201125153554.Rev1.016.tgz

## playc

- **playc 1.003** (2012-09-19) — Cleanup.
- **playc 1.002** (2012-09-19) — Expand all tabs.
- **playc 1.1** (2008-08-03) — Original version
