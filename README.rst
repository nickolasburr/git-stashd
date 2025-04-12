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
            <thead>
                <tr>
                    <th scope="col">Syntax</th>
                    <th scope="col">Description</th>
                    <th scope="col">Default</th>
                </tr>
            </thead>
            <tbody valign="top">
                <tr>
                    <td>
                        <kbd>
                            <span>-F, --foreground</span>
                        </kbd>
                    </td>
                    <td>Run daemon in foreground.</td>
                    <td>&mdash;</td>
                </tr>
                <tr>
                    <td>
                        <kbd>
                            <span>-i &lt;SEC&gt;, --interval=&lt;SEC&gt;</span>
                        </kbd>
                    </td>
                    <td>Interval in seconds to check for changes.</td>
                    <td>600</td>
                </tr>
                <tr>
                    <td>
                        <kbd>
                            <span>-L &lt;PATH&gt;, --log-file=&lt;PATH&gt;</span>
                        </kbd>
                    </td>
                    <td>Path to alternate log file.</td>
                    <td>
                        <code>$HOME/git-stashd.log</code>
                    </td>
                </tr>
                <tr>
                    <td>
                        <kbd>
                            <span>-M &lt;COUNT&gt;, --max-entries=&lt;COUNT&gt;</span>
                        </kbd>
                    </td>
                    <td>Maximum number of stash entries.<sup>&#8224;</sup></td>
                    <td>100</td>
                </tr>
                <tr>
                    <td>
                        <kbd>
                            <span>-p &lt;PATH&gt;, --path=&lt;PATH&gt;</span>
                        </kbd>
                    </td>
                    <td>Path to Git repository.</td>
                    <td>
                        <code>$PWD</code>
                    </td>
                </tr>
                <tr>
                    <td>
                        <kbd>
                            <span>-h, --help</span>
                        </kbd>
                    </td>
                    <td>Show help information.</td>
                    <td>&mdash;</td>
                </tr>
                <tr>
                    <td>
                        <kbd>
                            <span>-v, --version</span>
                        </kbd>
                    </td>
                    <td>Show version information.</td>
                    <td>&mdash;</td>
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

Notes
---------

.. raw:: html

    <div>
        <span>&#8224;&nbsp;When <code>--max-entries</code> threshold is met, the daemon will exit.</span>
    </div>
