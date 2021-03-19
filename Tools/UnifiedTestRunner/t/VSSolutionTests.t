use warnings;
use strict;

use Test::More;
use File::Spec::Functions qw (canonpath);
use TestHelpers::ArrayAssertions qw (arrayContains arraysAreEqual);
use Dirs;

my $utrRoot = Dirs::UTR_root();
my $solutionPath = canonpath("$utrRoot/t/TestData/VSSolution/VSSolution.sln");

my $testProj1Path = canonpath("$utrRoot/t/TestData/AssemblyWithTests1/AssemblyWithTests1.csproj");
my $testProj2Path = canonpath("$utrRoot/t/TestData/AssemblyWithTests2/AssemblyWithTests2.csproj");
my $testProj1 = _getAssemblyFullPath($testProj1Path);
my $testProj2 = _getAssemblyFullPath($testProj2Path);

BEGIN { use_ok('VSSolution'); }

Creation:
{
    can_ok('VSSolution', 'new');
    my $sln = new VSSolution($solutionPath);
    isa_ok($sln, 'VSSolution');
}

getCSProjects_RetunsCSAllProjectsFromGivenSolution:
{
    my $sln = new VSSolution($solutionPath);
    my @projects = $sln->getCSProjects();
    my @pathes = map { $_->getAssemblyFullPath() } @projects;
    arraysAreEqual(\@pathes, [$testProj1, $testProj2]);
}

getCSProjects_NameFilerIsSpecified_ReturnsOnlyProjectWhichMatchTheFiler:
{
    my $sln = new VSSolution($solutionPath);
    my @projects = $sln->getCSProjects(qr/Tests.[.]csproj$/i);
    my @pathes = map { $_->getAssemblyFullPath() } @projects;
    arraysAreEqual(\@pathes, [$testProj1, $testProj2]);
}

getCSProjects_NameFilerIsSpecified_ReturnsOnlyProjectWhichMatchTheFiler:
{
    my $sln = new VSSolution($solutionPath);
    my @projects = $sln->getCSProjects();
    my @pathes = map { $_->getProjectPath() } @projects;
    arraysAreEqual(\@pathes, [$testProj1Path, $testProj2Path]);
}

getCSProjects_RetunrsProjectsWithProperGuids:
{
    my $sln = new VSSolution($solutionPath);
    my @projects = $sln->getCSProjects();
    my @guids = map { $_->getGUID() } @projects;
    arraysAreEqual(\@guids, ["9A10130F-0739-4A50-89E3-F81131358CC5", "029E9440-98F2-41F7-B5DE-AEB87A68658C"]);
}

getCSProjects_ReturnsProjectsWithSolutionFolders:
{
    my $sln = new VSSolution($solutionPath);
    my @projects = $sln->getCSProjects();
    is($projects[0]->getSolutionFolder(), 'SolutionFolder1/SolutionFolder11');
}

getPath_ReturnsSolutionPath:
{
    my $sln = new VSSolution($solutionPath);
    is($sln->getPath(), $solutionPath);
}

done_testing();

sub _getAssemblyFullPath
{
    my ($project) = @_;
    return (new CSProj($project))->getAssemblyFullPath();
}
