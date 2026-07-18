SIMPLE LOOM GIT WORKSHOP

Quinn Yockey
Last modified 2026-07-10
Version 0.1


About
-----

This document describes my ideas as I develop an interactive lesson for the
incoming Loom team members covering advanced features of git.


Motivation
----------

The Loom team uses git substantially for the development of its software
product.  Some students are exposed to git through class or personal use, but
often only cover basics such as adding, committing, and pushing.  By teaching
advanced features of git along with essential best practices, students will be
better able to utilize the power of git, resulting in a cleaner codebase. 


Command Line
------------

Yes, this lesson uses the command line.  Yes, I know tools like VSCode have
fancy buttons.  Feel free to switch back to VSCode's buttons later, but I think
it is important to understand how the tool itself works.  VSCode is just a
wrapper around git, sacrificing power for ease of use.  Learn the tool, not the
wrapper.


Pre-Lesson Homework
-------------------

Complete lessons on learngitbranching.js.org 
  - Commit
  - Branch
  - Merge
  - Rebase
  - Cherry-pick
  - Revert
  - Fetch
  - Remote

Install git and environment:
  - Linux: git from package manager, use default terminal
  - Mac: git from git-scm.org, use default terminal
  - Windows: Git Bash


Lesson
------

1.  Recap homework, questions, confusions.

2.  Check git installation.  Ensure user.name and user.email are set.

2.  Introduce Loom--.  This is a simplified version of Loom with all but the
    essentials cut out.  A simple version of Loom is desirable for this exercise
    so that code is more easily understood and modified.

3.  Append author name to Loom_Random author list.  Observe that the file was
    modified with `git status`.  See specific changes with `git diff`.  Add the
    change with `git add <file>` and `git commit -m <descriptive message>`.

4.  Add Loom_Random sensor implementation on new branch
    `feat/Loom_Random_[Name]`.  This "sensor" will spit out a random number
    instead of a measured value.  Look to TSL2591 and other sensors for
    guidance.  Create the following tasks and have students complete all of them
    on their own.  Add descriptive comments.  Start from skeleton files.
    Emphasize atomic nature of commits: only change one thing at a time.

      - Constructor
      - initialize()
      - display_data()
      - power_up()
      - power_down()
      - measure()

5.  Git diff defaults to showing the difference of unstaged changes.  How can we
    view a diff of staged changes?  Use `git diff --help`.

6.  Add a feature then revert.
    TODO determine feature.

7.  Use checkout to browse code of other students to see different ways the task
    was completed.

8.  Have each student merge all branches together, selecting snippets they deem
    most appropriate.  Ensure that at bare minimum the final product includes
    names of all authors.

9.  Instructor will add a conflicting change.  Use rebase instead of merge to
    create a linear history.  This keeps the log clean and organized.

10. Add separate branch that fails to compile.  Cherry pick single good commit
    from that branch.
    TODO determine branch, compilation error.

11. Show .gitconfig for those interested.


Miscellaneous
-------------

- "There is nothing more permanent than a temporary fix"
- How on earth is this going to compile?
- File rename with git mv
- Emphasize importance of status and log, readability
