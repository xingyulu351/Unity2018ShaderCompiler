#!/usr/bin/perl

use strict;
use warnings;
use lib ("Tools/Build");
use PrepareWorkingCopy qw (PrepareExternalDependency);
use Tundra qw (call_tundra);

#This frontend script is invoked by tundra,  because jam.pl told tundra to run this script if it wants to get a .json file that has a buildgraph.

#This frontend does multiple steps, almost all of them require a functioning mono install, so let's get that setup first.
PrepareExternalDependency("External/MonoBleedingEdge");
system("cp External/MonoBleedingEdge/builds/monodistribution/bin-linux64/mono External/MonoBleedingEdge/builds/monodistribution/bin/mono") if ($^O eq "linux");
system("cp External/MonoBleedingEdge/builds/monodistribution/etc-linux64/mono/config External/MonoBleedingEdge/builds/monodistribution/etc/mono/config")
    if ($^O eq "linux");

#First thing we're going to do is use bee to build the buildprogram. We version a bee_bootstrap.exe which is a "mini version" of bee, that has just enough
#bits to be able to compile our build program. Building the buildprogram also requires Cecil and ILRepack to be unpacked.

PrepareExternalDependency("External/Unity.Cecil");
PrepareExternalDependency("External/ILRepack");
my $mono = "External/MonoBleedingEdge/builds/monodistribution/bin/mono";
$ENV{"MONO_EXECUTABLE"} = $mono;
system("$mono --debug Tools/BeeBootstrap/bee_bootstrap.exe --silence-if-possible BuildSystemProjectFiles BuildProgram") eq 0
    or die("Frontend failed running bee_bootstrap");
delete $ENV{"MONO_EXECUTABLE"};

#ok, that will have generated artifacts/UnityBuildSystem/Unity.BuildSystem/Unity.BuildSystem.exe, which is a standalone .net application which can just be run,
#and it will then generate a .json file as output.  This is also the only output/purpose of the program.  It emits a json file and that's it.

#Unfortunattely, our build is still split up in a part called Pass1, and in "the real part".  Unity.BuildSystem.exe is able to create .json files for both of them.
#lets first ask it to make a .json file for the Pass1 part of the build.  In order for it to do that, it needs to know the target that will be used for the real part
#of the build, so we'll use some yuck, regex to try to extract the targetname from the arguments.
my @processArgs = @ARGV;
my $targetName = "all";
while (my $el = shift(@processArgs))
{
    if ($el =~ /^([a-zA-Z0-9_]+)$/i)
    {
        $targetName = $1;
    }
}

my $monoinvocation =
      "$mono --debug artifacts/UnityBuildSystem/Unity.BuildSystem/Unity.BuildSystem.exe -sTUNDRA_DAG_FILE=artifacts/tundra/pass1_"
    . $targetName
    . "_original.dag.json -sJAMFILE=Pass1 -sUNPACK_FOR_TARGET=$targetName Pass1";
system($monoinvocation) eq 0 or die("failed running $monoinvocation");

#ok, now we have the .json file for pass1. We're going to ask tundra to "build it". This is a rather unconventional tundra setup. Normally tundra will decide when it's time for a .json file to be built,
#however, since this is happening as part of the frontend of the "real tundra" invocation in jam.pl, it makes things quite a bit simpler and easier to reason about if we just always create the pass1
#json, and then just tell tundra to shut up and deal with it.  let's do exact that:

my $copy = "cp";
my $copyargs = "artifacts/tundra/pass1_" . $targetName . "_original.dag.json artifacts/tundra/pass1_" . $targetName . ".dag.json";
if ($^O eq "MSWin32")
{
    $copy = "copy";
    $copyargs =~ s#/#\\#g;
}

call_tundra("", "artifacts/tundra/pass1_" . $targetName . ".dag", "$copy $copyargs", "-f --silence-if-possible");

#ok, now the pass1 part of the build is complete.  It will have unpacked some zips, which Unity.BuildSystem.exe needs to have been unpacked, as it will generate the "final" .json file that the real tundra invocation
#invoked this script for to generate. Tundra writes the path this should be written to in TUNDRA_FRONTEND_OPTIONS, and Unity.BuildSystem.exe will read it from there, so we don't need to do anything
#special for that here. we're just going to run Unity.BuildSystem.exe and then we're all done!

$monoinvocation = "$mono --debug artifacts/UnityBuildSystem/Unity.BuildSystem/Unity.BuildSystem.exe @ARGV";
system($monoinvocation) eq 0 or die("failed running $monoinvocation");
