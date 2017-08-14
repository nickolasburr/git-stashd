# git-stashd(1)

`git-stashd` is an autostashing daemon for Git.

## How it works

`git-stashd` brings autosave functionality to Git. By using the builtin stashing functionality in Git, `git-stashd` stores the modified contents of a dirty worktree in a stash.

## Options

+ `--daemon`, `-D`: Run daemon in detached process.
+ `--interval`, `-I`: Interval (in seconds) to check for changes. Defaults to `600`.
+ `--repository-path`, `-P`: Path to Git repository. Defaults to `cwd`.
