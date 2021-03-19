use warnings;
use strict;

use Test::More;
use Dirs;
use File::Spec::Functions qw (canonpath);

use lib Dirs::external_root();
use CSProj;
use Test::MockModule;
use Test::Differences;

my $utrRoot = Dirs::UTR_root();
my $projectPath = canonpath("$utrRoot/t/TestData/AssemblyWithTests1/AssemblyWithTests1.csproj");

BEGIN { use_ok('MSBuildProjectGenerator'); }

Creation:
{
    my $gen = new MSBuildProjectGenerator();
    isa_ok($gen, 'MSBuildProjectGenerator');
}

generate_NoProjects_GeneratesMBuildFileWithoutProjects:
{
    my $actual_text = generate();
    my $pattern =
        "<MSBuild Projects=\"@(TestProject)\"\nProperties=\"Configuration=Debug;OutputPath=%(TestProject.OutputPath)/;OutDir=%(TestProject.OutputPath)/\"";
    my $idx = index($actual_text, $pattern);
    ok($idx != -1);
}

generate_OneProject_GeneratesMBuildProjects:
{
    my $actual_text = generate($projectPath);
    my $pattern = "<TestProject Include=\"$projectPath\">\n</TestProject>";
    my $idx = index($actual_text, $pattern);
    ok($idx != -1);
}

generate_OneProjectWithCustomOutputPath_GeneratesProperDefinition:
{
    my $actual_text = generate($projectPath, 'custom_output_path');
    my $pattern = "<TestProject Include=\"$projectPath\">\n<OutputPath>custom_output_path</OutputPath>\n</TestProject>";
    my $idx = index($actual_text, $pattern);
    ok($idx != -1);
}

generate_CustomIntermediatePathIsSet_GeneratesProperMSBuildTaskDefinition:
{
    my $actual_text = generate($projectPath, undef, 'interm_path');
    my $pattern = 'Properties="Configuration=Debug;OutDir=%(TestProject.OutputPath)/;BaseIntermediateOutputPath=interm_path"';
    my $idx = index($actual_text, $pattern);
    ok($idx != 1);
}

RealityCheck:
{
    my $projectPath = canonpath("$utrRoot/t/TestData/AssemblyWithTests1/AssemblyWithTests1.csproj");
    my $expected = qq (<?xml version="1.0" encoding="utf-8"?>
<Project xmlns="http://schemas.microsoft.com/developer/msbuild/2003" DefaultTargets="Build">
<ItemGroup>
<TestProject Include="$projectPath">
<OutputPath>custom_output_path</OutputPath>
</TestProject>
</ItemGroup>
<Target Name="Build">
<MSBuild Projects="@(TestProject)"
Properties="Configuration=Debug;OutputPath=%(TestProject.OutputPath)/;OutDir=%(TestProject.OutputPath)/;BaseIntermediateOutputPath=interm_path"
Targets="Build">
</MSBuild>
</Target>
</Project>
);
    my $actual_text = generate($projectPath, 'custom_output_path', 'interm_path');
    eq_or_diff($actual_text, $expected);
}

done_testing();

sub generate
{
    my ($projectPath, $outputPath, $intermPath) = @_;
    my $generator = new MSBuildProjectGenerator();
    if ($projectPath)
    {
        my $project = new CSProj($projectPath);
        if ($outputPath)
        {
            $generator->addProject($project, 'custom_output_path');
        }
        else
        {
            $generator->addProject($project);
        }
    }
    if ($intermPath)
    {
        $generator->setBaseIntermediateOutputPath($intermPath);
    }
    my @lines;
    my $fileUtilsMock = new Test::MockModule('FileUtils');
    $fileUtilsMock->mock(writeAllLines => sub { (undef, @lines) = @_; });
    $generator->generate('file_name_does_not_matter');
    my $actualText = join("\n", @lines);
    return $actualText;
}
