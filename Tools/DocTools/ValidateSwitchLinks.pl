#!/usr/bin/perl

# Validates switch links on generated API documentation

use strict;
use warnings;

use FindBin qw($Bin);
use Cwd;
use Cwd 'abs_path';
use feature qw(say);

my $buildOutDir = "$Bin/../../build/UserDocumentation/Documentation/en/";

ValidateOneComponent("Transform");
ValidateOneComponent("Animator");
ValidateOneComponent("AudioSource");
ValidateOneComponent("Avatar");    #Note: Avatar is not a Component
ValidateOneComponent("AudioClip"); #Note: AudioClip is not a Component

sub ValidateOneComponent
{
    my ($compName) = @_;
    my $transformManualPage = $buildOutDir . "Manual/class-$compName.html";
    my $transformScriptingPage = $buildOutDir . "ScriptReference/$compName.html";
    if (!-e $transformManualPage)
    {
        die("Unable to find $transformManualPage");
    }
    if (!-e $transformScriptingPage)
    {
        die("Unable to find $transformScriptingPage");
    }
    my $manualContent = slurp($transformManualPage);
    my $scriptContent = slurp($transformScriptingPage);

    if ($scriptContent !~ m{<a href='../Manual/class-$compName.html'})
    {
        die("scriptRef page for $compName doesn't contain switch link to Manual");
    }
    if ($manualContent !~ m{<a href='../ScriptReference/$compName.html'})
    {
        die("manual page for $compName doesn't have a switch link to ScriptRef");
    }
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
print "Switch links work properly\n";
1;
