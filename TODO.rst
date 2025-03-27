TODO
====

.. contents:: :local:

Features
--------

1. Add ``--include-untracked`` option for specifying how to handle untracked files in a dirty worktree.
2. Add ``--skip-index`` option for skipping the index when creating a stash commit.

Improvements
------------

1. Utilize PID lock file, set maximum daemons for a single repository to ``1``.
2. Get log directory from ``--log-file`` argument via ``dirname``.
