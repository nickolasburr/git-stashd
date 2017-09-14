# git-stashd(1)

`git-stashd` is an autostashing daemon for Git.

## How it works

`git-stashd` brings autosave functionality to Git. By using the stashing functionality built into Git, `git-stashd` stores the modified contents of a dirty worktree in a stash, which you can checkout, commit, or discard at a later point.

## Installation

You can install `git-stashd` via Homebrew or manually. See the [tap repository](https://github.com/nickolasburr/homebrew-git-stashd) for tap-specific information.

### Homebrew

```shell
brew tap nickolasburr/git-stashd
brew install git-stashd
```

### Manual

By default, `git-stashd` is installed to /usr/local/bin. You can set the `prefix` variable when running `make` to specify an alternate install location.

```shell
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

```shell
# You can omit --path ~/projects/example, if currently in the ~/projects/example directory.
git stashd --path ~/projects/example
```

**Start daemon for repository `~/projects/example` with an interval of 30 seconds**

```shell
git stashd --path ~/projects/example --interval 30
```

**Start daemon in foreground for repository `~/projects/example` with an interval of 5 minutes**

```shell
git stashd --foreground --path ~/projects/example --interval 300
```
