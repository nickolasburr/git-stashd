# git-stashd(1)

`git-stashd` is an autostashing daemon for Git.

## Table of Contents

- [Description](#description)
- [Installation](#installation)
  + [Homebrew](#homebrew)
  + [Manual](#manual)
- [Options](#options)
- [Examples](#examples)

## Description

By utilizing the stashing functionality built into Git, `git-stashd` stores the modified contents of a dirty worktree in a stash, which you can checkout, commit, or discard at a later point.

## Installation

You can install `git-stashd` via Homebrew or manually.

### Homebrew

```
brew tap nickolasburr/pfa
brew install git-stashd
```

### Manual

By default, `git-stashd` is installed to /usr/local/bin. You can set the `prefix` variable when running `make` to specify an alternate install location.

```
make
make install
```

## Options

+ `--foreground`, `-F`: Run daemon in foreground. Helpful for debugging.
+ `--interval`, `-I`: Interval (in seconds) to check for changes. Defaults to `600` (10 minutes).
+ `--log-file`, `-L`: Path to alternate log file. Default location is `$HOME/git-stashd.log`.
+ `--max-entries`, `-M`: Maximum number of entries a stash should hold. Once the threshold has been met, the daemon will exit.
+ `--path`, `-P`: Path to Git repository. Defaults to `cwd`.

## Examples

**Start daemon for repository `~/projects/example`**

```
# You can omit --path ~/projects/example, if currently in the ~/projects/example directory.
git stashd --path ~/projects/example
```

**Start daemon for repository `~/projects/example` with an interval of 30 seconds**

```
git stashd --path ~/projects/example --interval 30
```

**Start daemon in foreground for repository `~/projects/example` with an interval of 5 minutes**

```
git stashd --foreground --path ~/projects/example --interval 300
```
