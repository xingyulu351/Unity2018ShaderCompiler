#!/usr/bin/perl -w
# Handle tracked repositories

package Repositories;

require Exporter;
our @ISA = qw (Exporter);
our @EXPORT_OK = qw (ReposFetch ReposApply ReposPin);

use strict;
use warnings;

use Carp qw (confess);
use File::Slurp;
use Cwd;

# Return first 12 characters of the string - intented to shorten Mercurial hashes
sub ShortHash
{
    shift =~ /(^.{12})/;
    return $1;
}

# Get a revision hash from a revset in a repo ... or die if given a message to die with
sub GetOneHgRev
{
    my ($repopath, $revset, $msg) = @_;

    my $cmd = "hg -R $repopath log -r \"$revset\" --template \"{node}\\n\"";
    my $out = `$cmd`;
    (my $rev) = ($out =~ /^(?:user: )?([0-9a-f]{40})$/m);
    if ($msg && !$rev)
    {
        print("command: $cmd\n");
        print("output:\n$out\n");
        die("Error in $repopath: $msg\n");
    }
    $rev;
}

# read $inifilename, return as hash hash
# if changes specified, write back and overwrite the original lines (which must exist)
sub ReadWriteIni
{
    my ($inifilename, %changes,) = @_;

    my $filedata = read_file($inifilename) or die("Error: Failed to read '$inifilename'");
    my @out;
    my $changed = 0;

    my %config;
    my $section = "";

    foreach my $line (split(/\n/, $filedata))
    {
        if ($line =~ /^\[(.*)\]\s*$/)
        {
            # TODO: preserve order so ini sections can be processed in order?
            $section = $1;
        }
        elsif ($line =~ /^([^\s=]+)\s*=\s*(\S*)\s*(?:#.*)?$/)
        {
            my $key = $1;
            my $val = $2;
            $config{$section}{$key} = $val;
            if (defined($changes{$section}{$key}))
            {
                $line = "$key = $changes{$section}{$key}";
                print("Setting [$section] $line\n");
                $changed = 1;
            }
        }
        elsif (!($line =~ /^\s*(?:#.*)?$/))
        {
            print("Error: invalid line in $inifilename: $line\n");
        }
        push(@out, $line);
    }
    if ($changed)
    {
        push(@out, "");
        print("Writing $inifilename\n");
        rename($inifilename, $inifilename . ".bak");
        write_file($inifilename, join("\n", @out));
    }
    %config;
}

# Show ini file content
sub ShowIni
{
    my (%config,) = @_;
    foreach my $section (sort (keys(%config)))
    {
        print("[$section]\n");
        my %values = %{ $config{$section} };
        foreach my $key (sort (keys(%values)))
        {
            print("$key = $values{$key}\n");
        }
        print("\n");
    }
}

# Run fetch for one repo, return actual hash
sub FetchOne
{
    my ($config, $repopath, $force) = @_;

    print("Checking $repopath\n");
    my $pinned = $config->{$repopath}{'revision'};
    my $s_pinned = ShortHash($pinned);

    if (system("hg -R $repopath log -r $pinned --template \" - pinned revision $s_pinned is already available locally\\n\"") != 0)
    {
        print(" - pinned revision $s_pinned was not available locally, pulling\n");
        system("hg -R $repopath pull -r \"$pinned\"") == 0    # TODO: pull everything like clone, or just pinned revision?
            or die("Error in '$repopath': Failure pulling pinned revision $s_pinned from $repopath");
    }

    my $updaterev = $pinned;

    my $curbranch = `hg branch`;
    chomp($curbranch);

    my $pinnedbranch = `hg log -R $repopath -r "$pinned" --template "{branch}\\n"`;
    chomp($pinnedbranch);

    if ($curbranch eq $pinnedbranch)
    {
        print(" - already on the pinned branch $pinnedbranch - not pulling\n");
    }
    else
    {
        print(" - trying to pull branch $curbranch - expect failure if it doesn't exist (if it hangs you need to enter your username)\n");
        my $localrev = GetOneHgRev($repopath, "present('$curbranch')&!closed()&outgoing()");
        my $out = `hg -R $repopath id default -i -r "$curbranch"`;
        (my $remotebranchrev) = ($out =~ /^(?:user: )?([0-9a-f]{12})$/m);

        my $branchrev;
        if ($localrev)
        {
            $branchrev = $localrev;
        }
        else
        {
            my $remotebranchrevlocal = $remotebranchrev && GetOneHgRev($repopath, "present('$remotebranchrev')");
            if ($remotebranchrev && !$remotebranchrevlocal)
            {
                system("hg -R $repopath pull -r \"$remotebranchrev\"");
            }
            my $remotebranchrevlocalopen = $remotebranchrev && GetOneHgRev($repopath, "present('$remotebranchrev')&!closed()");
            if ($remotebranchrevlocalopen)
            {
                $branchrev = $remotebranchrevlocalopen;
            }
        }

        if ($branchrev)
        {
            my $s_branchrev = ShortHash($branchrev);

            # There is a matching open branch in the tracked repo - now figure out how to use it

            my $pinnedbranchmax = GetOneHgRev($repopath, "max(('$pinned'::'$branchrev'-'$pinned')&branch('$pinnedbranch'))");
            if ($branchrev eq $pinned)
            {
                print(" - using $curbranch revision which also is the pinned revision $s_pinned\n");
            }
            elsif ($pinnedbranchmax)
            {
                my $s_pinnedbranchmax = ShortHash($pinnedbranchmax);

                # Ignore $curbranch if its ancestry contains changes on $pinnedbranch after $pinned
                print(
                    " - ignoring branch $curbranch, it has more than $s_pinned on the pinned branch $pinnedbranch (such as $s_pinnedbranchmax) and will thus probably match a later revision where $s_pinnedbranchmax has been pinned\n"
                );
            }
            elsif (GetOneHgRev($repopath, "$branchrev&'$pinned'::"))
            {
                # If an open branch is found and it is a descendant of the pinned, use that instead of the pinned revision.
                print(" - using $curbranch revision $s_branchrev which is a descendant of pinned revision $s_pinned\n");
                $updaterev = $branchrev;
                my $mergebackrev = GetOneHgRev($repopath, "children($branchrev) & '$pinned':: & branch('$pinnedbranch')");
                if ($mergebackrev)
                {
                    my $s_mergebackrev = ShortHash($mergebackrev);
                    print(" - has already been merged to $pinnedbranch in $s_mergebackrev, to get it, as janitor:\n");
                    print("    hg -R $repopath up -c -r \"$s_mergebackrev & branch('$pinnedbranch')\"\n");
                    print("    hg -R $repopath push -r \"$s_mergebackrev & branch('$pinnedbranch')\"\n");
                }
                else
                {
                    my $s_updaterev = ShortHash($updaterev);
                    print(" - to merge it back to $pinnedbranch, as janitor:\n");
                    print("    hg -R $repopath up -c -r \"branch('$pinnedbranch') & ${s_pinned}::\"\n");
                    print("    hg -R $repopath merge -r \"branch('$curbranch') & $s_updaterev\"\n");
                    print("    hg -R $repopath ci -m \"Merge $curbranch to $pinnedbranch\"\n");
                    print("    hg -R $repopath push -r \". & branch('$pinnedbranch')\"\n");
                }
            }
            elsif (GetOneHgRev($repopath, "$branchrev&::'$pinned'"))
            {
                # If an open branch is found and it is an ancestor of the pinned revision, ignore it.
                print(" - $curbranch revision $s_branchrev is an ancestor of pinned revision $s_pinned and ignored\n");
            }
            else
            {
                # If an unrelated open branch is found, it will fail.
                print(" - $curbranch head $s_branchrev is not a descendant of pinned revision $s_pinned, to merge it:\n");
                print("    hg -R $repopath up -r \"'$curbranch' & ${s_branchrev}::\"\n");
                print("    hg -R $repopath merge $s_pinned\n");
                print("    hg -R $repopath ci -m \"Merge pinned $s_pinned from $pinnedbranch\"\n");
                print("    hg -R $repopath push -r \". & '$curbranch'\"\n");
                die(
                    "Error in '$repopath': current head of '$curbranch' $s_branchrev is not a descendant of pinned revision $s_pinned\n- if you are using the branch, pull and merge the pinned revision to it\n- if you don't want to use the branch, close the branch\n"
                );
            }
        }
        else
        {
            # If no open matching branch is found, use the pinned revision.
            print(" - branch $curbranch not found, will use pinned revision $s_pinned\n");
        }
    }

    my $s_updaterev = ShortHash($updaterev);
    my $currev = GetOneHgRev($repopath, ".", "error getting current revision");
    my $s_currev = ShortHash($currev);

    if ($force)
    {
        system("hg -R $repopath update --clean -r \"$updaterev\"") == 0
            or die("Error in '$repopath': Failure updating to $s_updaterev");
        print(" - updated from $s_currev to $s_updaterev\n");
        system("hg -R $repopath --config extensions.purge= purge --all") == 0
            or die("Error in '$repopath': Failure purging");
        print(" - purged all\n");
    }
    elsif (`hg -R $repopath id -q` =~ /\+/)
    {
        die("Error in '$repopath': Uncommitted local changes.\n");
    }
    elsif ($currev eq $updaterev)
    {
        print(" - already at revision $s_updaterev, not updating\n");
    }
    else
    {
        system("hg -R $repopath update --check -r \"$s_updaterev\"") == 0
            or die("Error in '$repopath': Failure updating to $s_updaterev");
        print(" - updated from $s_currev to $s_updaterev\n");
    }
    print("\n");
    return $updaterev;
}

# Fetch tracked repositories
# Specified repositories will be cloned if they not already are available
# Specified (or all available) repositories will be updated correctly.
# Pinned repos are just checked, bad or symbolic references cause pull and update.
sub Fetch
{
    my ($inifilename, $repopaths, $force) = @_;
    my %config = ReadWriteIni($inifilename);
    my %fetched = ();
    my @fetchpaths = @{$repopaths};

    if (!@fetchpaths || $fetchpaths[0] eq '*')
    {
        # Use all defined repos if none listed. If unspecified: skip if missing. If '*': clone if missing.
        @fetchpaths = sort (keys(%config));
    }

    foreach my $repopath (@fetchpaths)
    {
        $repopath = CleanRepoPath($repopath);
        if (!defined($config{$repopath}))
        {
            die("Error: '$repopath' not defined in '$inifilename'\n");
        }

        if (!-d $repopath && @{$repopaths})
        {
            # Repo doesn't exist and was optional (not explicitly listed and not '*')
            my $source = $config{$repopath}{'source'};

            # TODO: use something that is less of a hack with hardcoded URLs
            if ($ENV{UNITY_THISISABUILDMACHINE})
            {
                $source =~ s/^https:\/\/ono.unity3d.com\//http:\/\/mercurial-mirror.hq.unity3d.com\//g;
            }

            print("Cloning $repopath from $source\n");
            system("hg clone -U --uncompressed $source $repopath") == 0
                or die("Error in '$repopath': Failure cloning from '$source'\n");
        }

        if (-d $repopath || @{$repopaths})
        {
            my $hash = FetchOne(\%config, $repopath, $force);
            $fetched{$repopath} = $hash;
        }
        else
        {
            print("Skipping non-existing repo $repopath\n\n");
        }
    }

    if (!-d 'build')
    {
        mkdir('build');
    }
    open(my $f, ">", "build/fetched.txt") or die("Cannot open build/fetched.txt: $!");
    for my $repopath (sort (keys(%fetched)))
    {
        my $hash = $fetched{$repopath};
        print $f ("$repopath = $hash\n");
    }
    close($f);
}

# apply a shell command to several repos
sub Apply
{
    my ($inifilename, $command, $repopaths) = @_;
    my %config = ReadWriteIni($inifilename);
    my $basedir = cwd();

    if (!@{$repopaths} || @{$repopaths}[0] eq '*')
    {
        foreach my $repopath (sort (keys(%config)))
        {
            if (-d "$basedir/$repopath")
            {
                print("$repopath:\n");
                chdir("$basedir/$repopath") or confess("Error changing directory to $repopath");
                system($command);
            }
        }
    }
    else
    {
        foreach my $repopath (@{$repopaths})
        {
            $repopath = CleanRepoPath($repopath);
            if (!defined($config{$repopath}))
            {
                die("Error: '$repopath' not defined in '$inifilename'.\n");
            }
            else
            {
                print("$repopath:\n");
                chdir("$basedir/$repopath") or die("Error: Failure changing directory to '$repopath'");
                system($command);
            }
        }
    }
    chdir($basedir) or confess("wat");
}

sub PinOne
{
    my ($config, $repopath, $changes, $pinbranch) = @_;

    my $pinned = $config->{$repopath}{'revision'};

    print("Pinning $repopath\n");
    if (!-d $repopath)
    {
        die("Error in '$repopath': unfetched repo cannot be pinned.\n");
    }
    if (GetOneHgRev($repopath, ".&'$pinned'"))
    {
        print(" - already pinned at revision $pinned\n");
    }
    else
    {
        my $pinrev = GetOneHgRev($repopath, ".", "WAT - error getting current repo revision");
        if (!defined($pinbranch) || !$pinbranch)
        {
            $pinbranch = $config->{$repopath}{'pin-branch'};
        }
        if (!defined($pinbranch) || !$pinbranch)
        {
            $pinbranch = `hg branch`;
            chomp($pinbranch);
        }
        GetOneHgRev($repopath, "$pinrev and '$pinned'::", "$pinrev is not a descendant of previous pinned revision $pinned");
        print(" - $pinrev is a descendant of previous pinned revision $pinned\n");
        GetOneHgRev($repopath, "$pinrev and branch('$pinbranch')", "$pinrev is not on branch $pinbranch");
        print(" - $pinrev is on pinning branch $pinbranch\n");

        $changes->{$repopath}{'revision'} = "$pinrev # on $pinbranch";
        print(" - pinning revision $pinrev on $pinbranch\n");
    }
}

# Pin all available tracked repositories to the current revision.
sub Pin
{
    my ($inifilename, $repopath, $pinbranch) = @_;
    my %config = ReadWriteIni($inifilename);
    my %changes = ();
    if (!defined($repopath) || $repopath eq '*')
    {
        foreach my $repopath (sort (keys(%config)))
        {
            PinOne(\%config, $repopath, \%changes, $pinbranch);
        }
    }
    else
    {
        $repopath = CleanRepoPath($repopath);
        defined($config{$repopath})
            or die("Error: '$repopath' is not a known tracked repository\n");
        PinOne(\%config, $repopath, \%changes, $pinbranch);
    }
    if (%changes)
    {
        ReadWriteIni($inifilename, %changes);
    }
}

# Wrappers for hardcoding inifilename

my $inifilename = "Repositories.ini";

sub ReposFetch
{
    my ($repopaths, $force) = @_;
    Fetch($inifilename, $repopaths, $force);
}

sub ReposApply
{
    my ($command, $repopaths) = @_;
    Apply($inifilename, $command, $repopaths);
}

sub ReposPin
{
    my ($repopath, $pinbranch) = @_;
    Pin($inifilename, $repopath, $pinbranch);
}

sub CleanRepoPath
{
    my ($repopath) = @_;

    $repopath =~ s/\\/\//g;    # replace back- with forward-slashes (for ease of use on Windows)
    $repopath =~ s/\/+$//g;

    return $repopath;
}

sub SampleCommands
{
    print("\nReading $inifilename ...\n\n");
    my %ini = ReadWriteIni($inifilename);
    print("\n");

    print("\nDumping config:\n\n");
    ShowIni(%ini);
    print("\n");

    print("\nCloning\n\n");
    ReposFetch(['Repositories/documentation']);
    print("\n");

    print("\nPull+Update\n\n");
    ReposFetch(['*']);
    print("\n");

    print("\nApply 'hg id'\n\n");
    ReposApply('hg id', []);
    print("\n");

    print("\nPull+Update\n\n");
    ReposFetch(['Repositories/documentation']);
    print("\n");

    print("\nPinning\n\n");
    ReposPin('*');
    print("\n");
}

#SampleCommands ();

1;
