# Todo List

### Known Bugs

None reported.

### Upcoming Features

+ Add `--include-untracked` option for specifying how to handle untracked files in a dirty worktree.
+ Add `--max-entries` option for specifying the maximum amount of entries a stash should contain.

### Potential Features
+ Add `--skip-index` option for skipping the index when creating a stash commit.

### Improvements

+ Utilize PID lock file, set maximum daemons for a single repository to 1.
+ Get log directory from --log-file argument via `dirname`.
