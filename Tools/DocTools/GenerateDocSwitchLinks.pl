#!/usr/bin/perl

# Generates switch links between corresponding Component
# reference pages and Scripting API pages.
#
# Run with a command line arg that points to folder containing
# Components and ScriptReference subfolders

use strict;
use warnings;

use FindBin qw($Bin);
use Cwd;
use Cwd 'abs_path';
use feature qw(say);

if (!@ARGV)
{
    my $newDir = "$Bin/../../build/UserDocumentation/";
    chdir($newDir) == 1 or die "couldn't chdir to $newDir";
}
else
{
    chdir($ARGV[0]);
}
my $dir = abs_path(getcwd);

my $textlinks = 0;
if (@ARGV >= 2 && $ARGV[1] eq 'text')
{
    print "Doc switch links: text mode\n";
    $textlinks = 1;
}

my $lang = 'en';
chdir("Documentation/$lang") == 1 or die("couldn't change directory to 'Documentation/$lang'");
my $manualPath = "Manual";
my $scriptRefPath = "ScriptReference";
my @pages_comp = glob("$manualPath/class-*.html");
my @pages_script = glob("$scriptRefPath/*.html");

if ((scalar @pages_comp) == 0)
{
    my $errstring = "No Component pages found (Manual/class-*.html)!";
    $ENV{'UNITY_THISISABUILDMACHINE'} ? die $errstring : warn $errstring;
}

if ((scalar @pages_script) == 0)
{
    my $errstring = "No script reference found (ScriptRef/*.html)";
    $ENV{'UNITY_THISISABUILDMACHINE'} ? die $errstring : warn $errstring;
}

my $switchLinkButtonStyle = "switch-link gray-btn sbtn left";

# component pages
my $comp_count = 0;
for my $comp_file (@pages_comp)
{
    $comp_file =~ m{class-(\w+)\.html$};
    my $name = $1;
    my $script_file = "$scriptRefPath/$name.html";

    # do matching in case-insensitive way
    my @matches = grep { /^$script_file$/i } @pages_script;
    next unless @matches;

    die "Multiple doc-switch-link possibilities for $name"
        if @matches > 1;

    my $script_file_exact = $matches[0];

    $comp_count++;
    my $comp_html = slurp($comp_file);

    $comp_html =~ s{(<!--\s*BeginSwitchLink\s*-->)(.*?)(<!--\s*EndSwitchLink\s*-->)}
    {
        my $html = '';
        $html .= "<a href='../$script_file_exact' title='Go to $name page in the Scripting Reference' class='$switchLinkButtonStyle'>Switch to Scripting</a>";
        $1 . $html . $3
    }egs;

    gulp($comp_file, \$comp_html);
}

# script pages
my $script_count = 0;
for my $script_file (@pages_script)
{
    $script_file =~ m{ScriptReference/(\w+).*\.html$};
    my $name = $1;
    my $comp_file = "$manualPath/class-$name.html";

    # do matching in case-insensitive way
    my @matches = grep { /^$comp_file$/i } @pages_comp;
    next unless @matches;

    die "Multiple doc-switch-link possibilities for $name"
        if @matches > 1;

    my $comp_file_exact = $matches[0];

    $script_count++;
    my $script_html = slurp($script_file);

    $script_html =~ s{<a href="" class="$switchLinkButtonStyle hide">([^<]*)</a>}
    {<a href='../$comp_file_exact' title='Go to $name Component in the Manual' class='$switchLinkButtonStyle show'>Switch to Manual</a>}gs;

    gulp($script_file, \$script_html);
}

print "Done doc switches for $comp_count component pages and $script_count script pages\n";

sub gulp
{
    my ($filename, $html_ref) = @_;

    system('touch', $filename)
        if not -e $filename;

    gulp_safe($filename, $html_ref);
}

sub gulp_safe
{
    my ($filename, $html_ref) = @_;

    return 0
        if $$html_ref eq slurp($filename);

    open my $fh, '>', $filename
        or die "$filename: $!\n";
    print $fh $$html_ref;

    return 1;
}

sub slurp
{
    my ($filename) = @_;

    open my $fh, '<', $filename
        or do { use Carp; croak "$filename: $!\n" };

    if (wantarray)
    {
        return <$fh>;
    }
    else
    {
        local $/;
        undef $/;
        return scalar <$fh>;
    }
}

1;
