# git-stashd(1)

`git-stashd` is an autostashing daemon for Git.

## How it works

`git-stashd` brings autosave functionality to Git. By using the stashing functionality built into Git, `git-stashd` stores the modified contents of a dirty worktree in a stash, which you can checkout, commit, or discard at a later point.

## Options

+ `--foreground`, `-F`: Run daemon in foreground. Helpful for debugging.
+ `--interval`, `-I`: Interval (in seconds) to check for changes. Defaults to `600` (10 minutes).
+ `--log-file`, `-L`: Path to alternate log file. Defaults to `/var/log/git-stashd.log`.
+ `--path`, `-P`: Path to Git repository. Defaults to `cwd`.
