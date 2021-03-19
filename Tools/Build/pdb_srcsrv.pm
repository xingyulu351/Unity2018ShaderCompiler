#!/usr/bin/env perl
use strict;
use warnings;

use Cwd;
use File::Basename;
use File::Path qw(mkpath);
use File::Spec;
use PrepareWorkingCopy qw (PrepareExternalDependency);

my $hg_hash = `hg log -r . -T \"{node|short}\"`;
my $url = "https://ono.unity3d.com/unity/unity/raw/";

my $header = <<"END_HEADER";
SRCSRV: ini ------------------------------------------------
VERSION=2
INDEXVERSION=2
VERCTRL=http
SRCSRV: variables ------------------------------------------
HTTP_ALIAS=$url
HTTP_EXTRACT_TARGET=%HTTP_ALIAS%%changeset%\/%var2%
CHANGESET=$hg_hash
SRCSRVTRG=%http_extract_target%
SRCSRV: source files ---------------------------------------
END_HEADER
my $footer = "SRCSRV: end ------------------------------------------------";

sub extract_pdb_srclist
{
    my ($pdb) = File::Spec->canonpath(@_);
    my $cmd = "External/NonRedistributable/Microsoft/srcsrv/builds/srctool.exe";
    if (!-e $cmd)
    {
        return 0;
    }

    my @output = `$cmd $pdb -r`;

    if (@output)
    {
        chomp(@output);

        my $pdb_filter = quotemeta($pdb);
        @output = grep(!/$pdb_filter/, @output);

        return sort @output;
    }
    return @output;
}

sub insert_srcsrv_stream
{
    my ($pdb) = File::Spec->canonpath(shift);
    my ($stream_file) = File::Spec->canonpath(shift);

    my $cmd = "External/NonRedistributable/Microsoft/srcsrv/builds/pdbstr.exe";
    if (!-e $cmd)
    {
        return;
    }
    system($cmd, "-w", "-p:$pdb", "-s:srcsrv", "-i:$stream_file");
}

sub detect_srcsrv_stream
{
    my ($pdb) = File::Spec->canonpath(shift);

    my $cmd = "External/NonRedistributable/Microsoft/srcsrv/builds/pdbstr.exe";
    if (!-e $cmd)
    {
        return 0;
    }

    my @output = `$cmd -r -p:$pdb -s:srcsrv`;
    chomp(@output);

    if (@output)
    {
        return 1;
    }
    return 0;
}

sub in_filter
{
    my ($path) = shift;
    my ($filter_list) = shift;

    foreach my $filter (@$filter_list)
    {
        my $filter_qm = quotemeta($filter);
        if ($path =~ /$filter_qm/i) { return 1; }
    }
    return 0;
}

sub create_pdb_stream
{
    my ($pdb) = File::Spec->canonpath(shift);
    my ($srclist) = shift;

    my ($name, $path, $suffix) = fileparse($pdb);
    my $logPath = "artifacts/logs/pdb";
    mkpath($logPath);

    my $srcref_file = "$logPath/$name.src_refs.txt";
    my $stream_file = "$logPath/$name.txt";
    my $map_count = 0;

    my $cwd = quotemeta(File::Spec->canonpath(getcwd()));

    # Save off a log of the source references
    open(my $fh_src, '>', $srcref_file);
    foreach my $line (@$srclist) { print $fh_src "$line\n"; }
    close $fh_src;
    print "Total source references: " . scalar @$srclist . "\n";

    open(my $fh, '>', $stream_file);
    print $fh "$header";
    my @filter_list = ['artifacts\\', 'builds\\'];

    foreach my $line (@$srclist)
    {
        if ($line =~ /$cwd/i)
        {
            if (!(in_filter($line, @filter_list)))
            {
                my $canonpath = File::Spec->canonpath(Win32::GetLongPathName($line));
                my $relpath = File::Spec->abs2rel($canonpath);
                $relpath =~ s/\\/\//g;
                if ($relpath ne ".")
                {
                    print $fh $line . "*" . $relpath . "\n";
                    $map_count++;
                }
            }
        }
    }
    print $fh "$footer\n";
    close $fh;

    if ($map_count)
    {
        print "Entries created: $map_count\n";
        insert_srcsrv_stream($pdb, $stream_file);
    }
    else
    {
        print "No matching files to add to pdb srcsrv.\n";
    }
}

sub add_pdb_srcsrv
{
    my $pdb = shift;
    if (!detect_srcsrv_stream($pdb))
    {
        print "Processing: $pdb\n";
        my @srclist = extract_pdb_srclist($pdb);
        if (@srclist)
        {
            my $stream_file = create_pdb_stream($pdb, \@srclist);
        }
    }
}

sub pdb_srcsrv
{
    my @pdbfiles = @_;

    PrepareExternalDependency("External/NonRedistributable/Microsoft/srcsrv/builds.zip");

    print "Current hash: $hg_hash\n";
    print "Processing PDBs for Source Server...\n";
    foreach my $pdb (@pdbfiles)
    {
        if (-e $pdb)
        {
            add_pdb_srcsrv($pdb);
        }
    }
    print "Processing PDBs complete.\n";
}
