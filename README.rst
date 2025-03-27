git-stashd(1)
=============

.. contents:: :local:

Description
-----------

``git-stashd`` is an autostashing daemon for Git. By utilizing the stashing functionality built into Git,
``git-stashd`` stores the modified contents of a dirty worktree in a stash, which you can checkout, commit,
or discard at a later point.

Installation
------------

There are two installation methods, Homebrew and manual.

Homebrew
^^^^^^^^

.. code-block:: sh

    brew tap nickolasburr/pfa
    brew install git-stashd

Manual
^^^^^^

By default, ``git-stashd`` is installed to ``/usr/local/bin``. You can set the ``prefix`` variable when running ``make`` to specify an alternate path.

.. code-block:: sh

    make
    make install

Options
-------

.. raw:: html

    <blockquote>
        <table frame="void" rules="none">
            <tbody valign="top">
                <tr>
                    <td>
                        <kbd>
                            <span>-F, --foreground</span>
                        </kbd>
                    </td>
                    <td>Run daemon in foreground. Helpful for debugging.</td>
                </tr>
                <tr>
                    <td>
                        <kbd>
                            <span>-I, --interval [NUM]</span>
                        </kbd>
                    </td>
                    <td>Interval (in seconds) to check for changes. Defaults to <code>600</code> (10 minutes).</td>
                </tr>
                <tr>
                    <td>
                        <kbd>
                            <span>-L, --log-file [PATH]</span>
                        </kbd>
                    </td>
                    <td>Path to alternate log file. Default location is <code>$HOME/git-stashd.log</code>.</td>
                </tr>
                <tr>
                    <td>
                        <kbd>
                            <span>-M, --max-entries [NUM]</span>
                        </kbd>
                    </td>
                    <td>Maximum number of entries a stash should hold. Once the threshold has been met, the daemon will exit.</td>
                </tr>
                <tr>
                    <td>
                        <kbd>
                            <span>-P, --path</span>
                        </kbd>
                    </td>
                    <td>Path to Git repository. Defaults to <code>cwd</code>.</td>
                </tr>
                <tr>
                    <td>
                        <kbd>
                            <span>-h, --help</span>
                        </kbd>
                    </td>
                    <td>Show help information and usage examples.</td>
                </tr>
                <tr>
                    <td>
                        <kbd>
                            <span>-v, --version</span>
                        </kbd>
                    </td>
                    <td>Show version information.</td>
                </tr>
            </tbody>
        </table>
    </blockquote>

Examples
--------

Start daemon for repository ``~/projects/example``

.. code-block:: sh

   # You can omit --path ~/projects/example, if currently in the ~/projects/example directory.
   git stashd --path ~/projects/example


Start daemon for repository ``~/projects/example`` with an interval of 30 seconds

.. code-block:: sh

   git stashd --path ~/projects/example --interval 30

Start daemon in foreground for repository ``~/projects/example`` with an interval of 5 minutes

.. code-block:: sh

   git stashd --foreground --path ~/projects/example --interval 300
