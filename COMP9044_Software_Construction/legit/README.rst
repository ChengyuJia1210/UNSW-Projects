`legit -- A simulation of a part of git.`
-----------------------------------------------------

This tinny project is simulating the git, which store different versions of the files in the same directory.
It has 7 functions, legit-init, legit-add, legit-commit, legit-show, legit-log, legit-status and legit-rm.


Usage 1:
    $ ./legit-init
        | Initialized empty legit repository in .legit
        | echo line 1 > a
        | echo hello world >b
    $ ./legit-add a b
    $ ./legit-commit -m 'first commit'
        | Committed as commit 0
        | echo  line 2 >>a
    $ ./legit-add a
    $ ./legit-commit -m 'second commit'
        | Committed as commit 1
    $ ./legit-log
        | 1 second commit
        | 0 first commit
        | echo line 3 >>a
    $ ./legit-add a
        | echo line 4 >>a
    $ ./legit-show 0:a
        | line 1
    $ ./legit-show 1:a
        | line 1
        | line 2
    $ ./legit-show :a
        | line 1
        | line 2
        | line 3
        | cat a
        | line 1
        | line 2
        | line 3
        | line 4
    $./legit-show 0:b
        | hello world
    $./legit-show 1:b
        | hello world

Usage 2:
    $ ./legit-init
        Initialized empty legit repository in .legit
        touch a b c d e f g h
    $ ./legit-add a b c d e f
    $ ./legit-commit -m 'first commit'
        | Committed as commit 0
        | echo hello >a
        | echo hello >b
        | echo hello >c
    $ ./legit-add a b
        | echo world >a
        | rm d
    $ ./legit-rm e
    $ ./legit-add g
    $ ./legit-status
        | a - file changed, different changes staged for commit    
        | b - file changed, changes staged for commit
        | c - file changed, changes not staged for commit
        | d - file deleted
        | e - deleted
        | f - same as repo
        | g - added to index
        | h - untracked
        | legit-add - untracked
        | legit-commit - untracked
        | legit-init - untracked
        | legit-rm - untracked
        | legit-status - untracked
