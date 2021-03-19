package MSBuildProjectGenerator;

use warnings;
use strict;

use File::Spec::Functions qw (canonpath);

use FileUtils;

sub new
{
    my $instance = { projects => [] };
    return bless($instance, __PACKAGE__);
}

sub generate
{
    my ($this, $fileName) = @_;
    my @lines;
    $this->_writeHeader(\@lines);
    $this->_writeProjectsItemGroup(\@lines);
    $this->_writeBuildTarget(\@lines);
    $this->_writeTail(\@lines);
    FileUtils::writeAllLines($fileName, @lines);
}

sub addProject
{
    my ($this, $proj, $customOutputPath) = @_;
    my $key = canonpath($proj->getProjectPath());
    $this->{$key} = $customOutputPath;
    push(@{ $this->{projects} }, $proj);
}

sub setBaseIntermediateOutputPath
{
    my ($this, $path) = @_;
    $this->{BaseIntermediateOutputPath} = $path;
}

sub _writeHeader
{
    my ($this, $lines) = @_;
    push(@$lines, '<?xml version="1.0" encoding="utf-8"?>');
    push(@$lines, '<Project xmlns="http://schemas.microsoft.com/developer/msbuild/2003" DefaultTargets="Build">');
}

sub _writeProjectsItemGroup
{
    my ($this, $lines) = @_;
    push(@$lines, '<ItemGroup>');
    foreach my $proj (@{ $this->{projects} })
    {
        $this->_writeTestProjectDefinition($proj, $lines);
    }
    push(@$lines, '</ItemGroup>');
}

sub _writeTestProjectDefinition
{
    my ($this, $project, $lines) = @_;

    my $projectPath = $project->getProjectPath();
    my $key = canonpath($projectPath);
    my $customOutputPath = $this->{$key};
    push(@$lines, "<TestProject Include=\"$projectPath\">");
    $this->_writeCustomOutputPath($project, $lines);
    push(@$lines, "</TestProject>");
}

sub _writeCustomOutputPath
{
    my ($this, $project, $lines) = @_;
    my $projectPath = $project->getProjectPath();
    my $key = canonpath($projectPath);
    my $customOutputPath = $this->{$key};
    if (defined($customOutputPath))
    {
        push(@$lines, "<OutputPath>$customOutputPath</OutputPath>");
    }
}

sub _writeBuildTarget
{
    my ($this, $lines) = @_;
    push(@$lines, '<Target Name="Build">');
    $this->_writeMSBuildTask($lines);
    push(@$lines, '</Target>');
}

sub _writeMSBuildTask
{
    my ($this, $lines) = @_;
    push(@$lines, '<MSBuild Projects="@(TestProject)"');
    my $propertiesString = 'Properties="Configuration=Debug;OutputPath=%(TestProject.OutputPath)/;OutDir=%(TestProject.OutputPath)/';

    my $intermPath = $this->{BaseIntermediateOutputPath};
    if (defined($intermPath))
    {
        $propertiesString .= ";BaseIntermediateOutputPath=$intermPath";
    }
    $propertiesString .= '"';
    push(@$lines, $propertiesString);

    push(@$lines, 'Targets="Build">');
    push(@$lines, '</MSBuild>');
}

sub _writeTail
{
    my ($this, $lines) = @_;
    push(@$lines, "</Project>\n");
}

1;
