#require serve

# This test file is in the unified test format of Mercurial and can be run using
# the run-tests.py script from Mercurial, e.g.:
#   python run-tests.py test-repositories.t
# Before running, make sure to set your environment variable, REPOSITORIESPM,
# to the fully qualified directory that Repositories.pm resides in, e.g.
# /home/user/src/Tools/Build
#
# In order to run on Windows, you also need an sh shell, and
# http://win-bash.sourceforge.net/
# is the easiest one that stays most compatible with Mercurial
# To run on Windows, use:
#   python run-tests.py -l --shell "c:/path/shell.w32/sh.exe" test-repositories.t
# You will need a working Python 2.x and shell.w32 must be before any Cygwin
# binaries in your PATH or everything will fail.
#
# On Linux, the tests can be run from the unity repo root with:
#   REPOSITORIESPM=$PWD/Tools/Build ~/hg/tests/run-tests.py -l Tools/Build/tests/test-repositories.t


  $ write_repo() {
  >   echo "# Tracked repositories" > unity/Repositories.ini
  >   echo "" >> unity/Repositories.ini
  >   echo "[Repositories/gfxtest]" >> unity/Repositories.ini
  >   echo "source = http://localhost:$HGPORT1/" >> unity/Repositories.ini
  >   echo "revision = $1 # on trunk" >> unity/Repositories.ini
  > }

  $ fetch() {
  >   perl -MRepositories -E "Repositories::ReposFetch(['*'])"
  > }


Create gfxtest repository with single commit on trunk
  $ hg init gfxtest
  $ echo a > gfxtest/a
  $ hg -q -R gfxtest branch trunk
  $ hg -q -R gfxtest ci -m0 -A
  $ hg -q -R gfxtest serve -d -p $HGPORT1 --pid-file=hg.pid -a localhost --config web.push_ssl=False --config web.allow_push=*
  $ cat hg.pid >> "$DAEMON_PIDS"

Create the trunk revision pointing to it
  $ hg init unity
  $ hg -q -R unity branch trunk
  $ mkdir "unity/Repositories"
  $ write_repo '6134e2dc9c794ccfa6e9a2e93e8e0c7b97fa0460'
  $ hg -q -R unity ci -m0 -A

Fetch tracked repositories on same branch
  $ export PERL5LIB=$REPOSITORIESPM
  $ cd unity
  $ fetch
  Cloning Repositories/gfxtest from http://localhost:$HGPORT1/
  streaming all changes
  3 files to transfer, 307 bytes of data
  transferred * (glob)
  searching for changes
  no changes found
  Checking Repositories/gfxtest
   - pinned revision 6134e2dc9c79 is already available locally
   - already on the pinned branch trunk - not pulling
  1 files updated, 0 files merged, 0 files removed, 0 files unresolved
   - updated from 000000000000 to 6134e2dc9c79
  
  $ hg -R Repositories/gfxtest par --template "{node}\n"
  6134e2dc9c794ccfa6e9a2e93e8e0c7b97fa0460
  $ cd ..

Create dev branch in outer repository only
  $ hg -q -R unity branch dev
  $ hg -q -R unity ci -mdev
  $ rm -rf unity/Repositories/gfxtest
  $ cd unity
  $ fetch
  Cloning Repositories/gfxtest from http://localhost:$HGPORT1/
  streaming all changes
  3 files to transfer, 307 bytes of data
  transferred * (glob)
  searching for changes
  no changes found
  Checking Repositories/gfxtest
   - pinned revision 6134e2dc9c79 is already available locally
   - trying to pull branch dev - expect failure if it doesn't exist (if it hangs you need to enter your username)
  abort: unknown revision 'dev'!
   - branch dev not found, will use pinned revision 6134e2dc9c79
  1 files updated, 0 files merged, 0 files removed, 0 files unresolved
   - updated from 000000000000 to 6134e2dc9c79
  
  $ cd ..

Create dev branch in gfxtest
  $ hg -q -R gfxtest branch dev
  $ hg -q -R gfxtest ci -mdev
  $ rm -rf unity/Repositories/gfxtest
  $ cd unity
  $ fetch
  Cloning Repositories/gfxtest from http://localhost:$HGPORT1/
  streaming all changes
  3 files to transfer, * bytes of data (glob)
  transferred * (glob)
  searching for changes
  no changes found
  Checking Repositories/gfxtest
   - pinned revision 6134e2dc9c79 is already available locally
   - trying to pull branch dev - expect failure if it doesn't exist (if it hangs you need to enter your username)
   - using dev revision c4192987f9ca which is a descendant of pinned revision 6134e2dc9c79
   - to merge it back to trunk, as janitor:
      hg -R Repositories/gfxtest up -c -r "branch('trunk') & 6134e2dc9c79::"
      hg -R Repositories/gfxtest merge -r "branch('dev') & c4192987f9ca"
      hg -R Repositories/gfxtest ci -m "Merge dev to trunk"
      hg -R Repositories/gfxtest push -r ". & branch('trunk')"
  1 files updated, 0 files merged, 0 files removed, 0 files unresolved
   - updated from 000000000000 to c4192987f9ca
  
  $ cd ..

Ignoring stuff if we are on a derived branch with changesets other than what is pinned
  $ echo foo > gfxtest/a
  $ hg -q -R gfxtest ci -mfoo -A
  $ echo bar > gfxtest/b
  $ hg -q -R gfxtest branch dev2
  $ hg -q -R gfxtest ci -mbar -A
  $ write_repo c4192987f9caeb63e513da003bd13bb150181f4b
  $ hg -q -R unity ci -mdevcset
  $ hg -q -R unity branch dev2
  $ hg -q -R unity ci -mdev2
  $ cd unity
  $ fetch
  Checking Repositories/gfxtest
   - pinned revision c4192987f9ca is already available locally
   - trying to pull branch dev2 - expect failure if it doesn't exist (if it hangs you need to enter your username)
  pulling from http://localhost:$HGPORT1/
  searching for changes
  adding changesets
  adding manifests
  adding file changes
  added 2 changesets with 2 changes to 2 files
  (run 'hg update' to get a working copy)
   - ignoring branch dev2, it has more than c4192987f9ca on the pinned branch dev (such as 0cda93374ea1) and will thus probably match a later revision where 0cda93374ea1 has been pinned
   - already at revision c4192987f9ca, not updating
  
  $ cd ..

Test for issue https://trello.com/c/Eck0Yvmz
  $ hg -q -R gfxtest up trunk
  $ hg -q -R unity up trunk
  $ hg -q -R unity branch foo
  $ hg -q -R unity ci -mfoo
  $ hg -q clone unity unity-A -U
  $ hg -q clone unity unity-B -U
  $ hg -q -R unity-A up foo
  $ hg -q -R unity-B up foo
Make sure we have the same base setup
  $ cd unity-A
  $ mkdir Repositories
  $ fetch > /dev/null 2>&1
  $ cd ..
  $ cd unity-B
  $ mkdir Repositories
  $ fetch > /dev/null 2>&1
  $ cd ..
Start work on branch (#2)
  $ cd unity-A
  $ echo bad > Repositories/gfxtest/bad
  $ hg -q -R Repositories/gfxtest branch foo
  $ hg -q -R Repositories/gfxtest ci -m x1
  $ hg -q -R Repositories/gfxtest push --new-branch
  $ cd ..
Fetch x1 as B (#3)
  $ cd unity-B
  $ fetch
  Checking Repositories/gfxtest
   - pinned revision 6134e2dc9c79 is already available locally
   - trying to pull branch foo - expect failure if it doesn't exist (if it hangs you need to enter your username)
  pulling from http://localhost:$HGPORT1/
  searching for changes
  adding changesets
  adding manifests
  adding file changes
  added 1 changesets with 0 changes to 0 files (+1 heads)
  (run 'hg heads' to see heads)
   - using foo revision f555d02af473 which is a descendant of pinned revision 6134e2dc9c79
   - to merge it back to trunk, as janitor:
      hg -R Repositories/gfxtest up -c -r "branch('trunk') & 6134e2dc9c79::"
      hg -R Repositories/gfxtest merge -r "branch('foo') & f555d02af473"
      hg -R Repositories/gfxtest ci -m "Merge foo to trunk"
      hg -R Repositories/gfxtest push -r ". & branch('trunk')"
  0 files updated, 0 files merged, 0 files removed, 0 files unresolved
   - updated from 6134e2dc9c79 to f555d02af473
  
  $ cd ..
Close A's commit and try something new (#4 + #5)
  $ cd unity-A
  $ hg par --template "{rev} {branch} {desc}\n"
  4 foo foo
  $ hg -R Repositories/gfxtest par --template "{rev} {branch} {desc}\n"
  4 foo x1
  $ hg -R Repositories/gfxtest ci --close-branch -m x2
  $ hg -q -R Repositories/gfxtest push
  $ hg -q -R Repositories/gfxtest up 0
  $ echo better > Repositories/gfxtest/better
  $ hg -q -R Repositories/gfxtest branch foo --force
  $ hg -q -R Repositories/gfxtest ci -mx3 -A
  $ hg -q -R Repositories/gfxtest push --force
  $ cd ..
B pulls and gets x3 (#6)
  $ cd unity-B
  $ fetch
  Checking Repositories/gfxtest
   - pinned revision 6134e2dc9c79 is already available locally
   - trying to pull branch foo - expect failure if it doesn't exist (if it hangs you need to enter your username)
  pulling from http://localhost:$HGPORT1/
  searching for changes
  adding changesets
  adding manifests
  adding file changes
  added 1 changesets with 2 changes to 2 files (+1 heads)
  (run 'hg heads' to see heads, 'hg merge' to merge)
   - using foo revision 152df63ab537 which is a descendant of pinned revision 6134e2dc9c79
   - to merge it back to trunk, as janitor:
      hg -R Repositories/gfxtest up -c -r "branch('trunk') & 6134e2dc9c79::"
      hg -R Repositories/gfxtest merge -r "branch('foo') & 152df63ab537"
      hg -R Repositories/gfxtest ci -m "Merge foo to trunk"
      hg -R Repositories/gfxtest push -r ". & branch('trunk')"
  2 files updated, 0 files merged, 0 files removed, 0 files unresolved
   - updated from f555d02af473 to 152df63ab537
  
  $ hg -R Repositories/gfxtest par --template "{rev} {branch} {desc}\n"
  5 foo x3
  $ cd ..
A closes x3 (#7)
  $ cd unity-A
  $ hg -R Repositories/gfxtest ci --close-branch -m x4
  $ hg -q -R Repositories/gfxtest push
  $ cd ..
B fetches x4 but not x2 and updates to x1 (#8 + #9)
  $ cd unity-B
  $ fetch
  Checking Repositories/gfxtest
   - pinned revision 6134e2dc9c79 is already available locally
   - trying to pull branch foo - expect failure if it doesn't exist (if it hangs you need to enter your username)
  pulling from http://localhost:$HGPORT1/
  searching for changes
  adding changesets
  adding manifests
  adding file changes
  added 1 changesets with 0 changes to 0 files (-1 heads)
  (run 'hg update' to get a working copy)
   - branch foo not found, will use pinned revision 6134e2dc9c79
  0 files updated, 0 files merged, 2 files removed, 0 files unresolved
   - updated from 152df63ab537 to 6134e2dc9c79
  
  $ hg -R Repositories/gfxtest par --template "{rev} {branch} {desc}\n"
  0 trunk 0
  $ cd ..

