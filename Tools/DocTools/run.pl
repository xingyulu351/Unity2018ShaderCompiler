#!/usr/bin/env perl

use strict;
use warnings;
use File::Basename qw(dirname basename);
use File::Path qw(mkpath rmtree);
use File::Spec;
use Cwd;
use Cwd 'abs_path';
use Getopt::Long;
use UnTGZPackages;

my $mydir = File::Spec->rel2abs(dirname($0));
my $root = File::Spec->rel2abs(dirname($0) . "/../..");
use lib File::Spec->rel2abs(dirname($0) . "/../..") . '/Tools/Build';
use Tools qw (Jam JamRaw);
use Tools qw(GetMono64Exe);
use Carp qw( croak );
use feature qw(say);

my $noexamples = 0;
my $monodoc = 0;
my $onefile = 0;
my $help = 0;
Getopt::Long::GetOptions(
    'noexamples' => \$noexamples,
    'monodoc' => \$monodoc,
    'onefile=s' => \$onefile,
    'help|?' => \$help
);

if ($help)
{
    say("This is a utility for previewing ScriptRef documentation\n");
    say(" [--help]              Show this text");
    say(" [--monodoc]           Generate monodoc intellisense tooltips");
    say(" [--noexamples]        Skip example conversion (faster)");
    say(" [--onefile=<name>]    Only process single class (and all its members, no file extension needed)\n");
    exit;
}
say "Executing run.pl, noexamples: $noexamples";

Jam($root, "DocGen", "", "", 1);

(chdir "$mydir") == 1 or die "could not change directory to $mydir";

my $packagetemp = UnTGZPackages();

my $cli = GetMono64Exe();
my $exe = "APIDocumentationGenerator/bin/APIDocumentationGenerator.exe";
my $dir = getcwd;
say "dir = $dir";
my @monoargs = ($cli, $exe);
push(@monoargs, "-noexamples") if ($noexamples);
push(@monoargs, "-onefile", $onefile) if ($onefile);
push(@monoargs, "-monodoc") if ($monodoc);
system(@monoargs) == 0 or croak("Failed running APIDocumentationGenerator");

if (-d $packagetemp)
{
    rmtree($packagetemp) or die("Unable to remove $packagetemp (another process has a lock on it?)\n");
}
