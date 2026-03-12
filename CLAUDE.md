# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

`git-stashd` is a C daemon that periodically stashes dirty Git worktrees using libgit2 (v1.9.2, statically linked). It forks into the background, watches a repository path, and auto-stashes changes on a configurable interval.

## Build Commands

```sh
# First-time: build libgit2 dependency (clones and compiles libgit2 v1.9.2)
make build

# Compile the executable
make

# Relink executable without rebuilding libgit2
make exec

# Install to /usr/local/bin
make install

# Clean all build artifacts
make clean
```

`make build` must be run before `make` on a fresh checkout — it clones libgit2 into `lib/libgit2/` and compiles a static archive at `lib/libgit2/build/libgit2.a`.

If `configure.ac` is modified, regenerate the Makefile with:
```sh
cd tools && ./bootstrap.sh
```

## Architecture

### Entry Point: `src/main.c`
Parses CLI options (`getopt_long`), opens the git repository via libgit2, checks for an existing `stashd.lock` in `.git/`, writes the lock file, optionally daemonizes, then enters the main loop: check if worktree is dirty → if dirty and no matching stash entry exists → create stash entry → sleep.

### Core Components

- **`src/git.c` / `include/git.h`**: All git operations. Mix of libgit2 API calls and `system()`/`popen()` shell invocations (`git stash create`, `git stash store`, `git diff --quiet`, etc.). The key data structures are:
  - `git_stashd_repository` — holds path and pointer to stash
  - `git_stashd_stash` — array of up to 100 `git_stashd_entry` pointers
  - `git_stashd_entry` — index, SHA hash, and message for one stash entry

- **`src/daemon.c` / `include/daemon.h`**: Classic double-fork daemonization. `flog()` writes `[PID] message` to stdout (which is redirected to the log file). `ftouch()` creates the lock file.

- **`src/signals.c` / `include/signals.h`**: SIGHUP/SIGINT handler removes the lock file and exits. `nap()` implements interruptible sleep via SIGALRM + `sigsuspend()`.

- **`src/except.c` / `include/except.h`**: Dreisbach-style C exception framework using `setjmp`/`longjmp`. Used throughout via `TRY`/`EXCEPT`/`RAISE` macros.

- **`src/mem.c` / `include/mem.h`**: Memory allocation wrappers that `RAISE` `Mem_Failed` on allocation failure instead of returning NULL.

### Lock File Behavior
One daemon per repository is enforced via `stashd.lock` in the `.git/` directory. On startup, if the lock file exists, the daemon exits. On SIGHUP/SIGINT, the signal handler removes the lock file before exiting.

### Stash Deduplication
Before creating a stash, `has_match_entry()` in `git.c` compares the current worktree diff against each existing stash entry using `git diff --quiet`. A new stash is only created if no equivalent entry exists.

## Key Files

| File | Purpose |
|------|---------|
| `src/main.c` | Entry point, option parsing, main loop |
| `src/git.c` | Git operations (dirty check, stash create/store, dedup) |
| `src/daemon.c` | Daemonization, lock file, logging |
| `src/signals.c` | Signal handlers, interruptible sleep |
| `include/daemon.h` | Default constants (interval=600s, max_entries=100, log path) |
| `include/git.h` | Core data structures and function declarations |
| `tools/build.sh` | Builds libgit2 static archive |

## Platform Notes

On macOS (Darwin), the linker adds `-liconv -framework CoreFoundation -framework Security`. The cmake archive flags in `tools/build.sh` are also macOS-specific (`-no_warning_for_no_symbols`).
