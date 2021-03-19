#!/usr/bin/perl
use strict;
use warnings;
use File::Basename qw(dirname basename);
use File::Spec;
use lib File::Spec->rel2abs(dirname($0) . '/../../Configuration');
use BuildConfig;

my $root = File::Spec->rel2abs(dirname($0) . '/../../');

chdir($root);

my $hgRevision;
if ($ENV{BUILD_VCS_NUMBER})
{
    $hgRevision = $ENV{BUILD_VCS_NUMBER};
    print "Revison set by TC: $hgRevision\n";
}
else
{
    $hgRevision = `hg id -i`;
    chomp($hgRevision);
    my $lastCharacter = substr($hgRevision, length($hgRevision) - 1, 1);
    if ($lastCharacter eq "+")
    {
        print
            "Last character of 'hg id -i' ($hgRevision) is a +. Stripping it. The plus means the repo has modified files. This is the status of the repo, you may want to look into this:\n";
        my $hgStatus = `hg status -q`;
        print "$hgStatus\n";
        $hgRevision = substr($hgRevision, 0, length($hgRevision) - 1);
    }
    print "Using working copy revision: $hgRevision\n";
}

my $hgUrl = `hg path default`;
chomp($hgUrl);
print "Using working copy url: $hgUrl\n";

our $unityVersion = $BuildConfig::unityVersion;
print "Using unity version: $unityVersion\n";

my $hgBranch = `hg branch`;
chomp($hgBranch);

print "Using Branch: $hgBranch\n";

open my $versionFile, ">$root/build/releasableartifacts/buildInfo" or die("failed to open $root/build/releasableartifacts/buildInfo");
print $versionFile "URL: $hgUrl\n";
print $versionFile "SVN: $hgRevision\n";
print $versionFile "Version: $unityVersion\n";
print $versionFile "Branch: $hgBranch\n";
close $versionFile;
