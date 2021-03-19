use warnings;
use strict;

use Dirs;
use lib Dirs::external_root();

use Array::Utils qw(:all);
use Dirs;

use lib Dirs::getRoot();
use CSProj;
use File::Spec::Functions qw (canonpath);
use File::Temp qw (tempdir);
use Getopt::Long;
use Carp qw (croak);
use File::Basename qw (fileparse);

use NUnitSuiteRunnerBase;

my $targetTests = {};
my $artifactsRoot;
my $skipBuildDeps = 0;

sub main
{
    (my $project, $artifactsRoot, my @targets) = _readOptions();
    print("Project: $project \n$project: $artifactsRoot \ntargets @targets\n");
    _writeTestsForEachTarget($project, @targets);
    _buildIntersections($project, @targets);
    _findDeadTests($project, @targets);
}

sub _getTestsForTarget
{
    my ($target, $project) = @_;

    if (not exists $targetTests->{$target})
    {
        $targetTests->{$target} = [];
        push($targetTests->{$target}, _getTestList($target, $project));
    }

    return @{ $targetTests->{$target} };
}

sub _findDeadTests
{
    my ($project, @targets) = @_;
    my @targetTests = _getTestsFromAllTargets($project, @targets);
    my @allTests = _getTestList('TestAll', 'All');
    my @diff = array_minus(@allTests, @targetTests);
    my $fh = _openFile("$artifactsRoot/DeadTests.txt", '>');
    print($fh join("\n", @diff));
    close($fh);
}

sub _getTestsFromAllTargets
{
    my ($project, @targets) = @_;
    my $targetsNum = scalar(@targets);
    my (@result);
    for (my $i = 0; $i < $targetsNum; $i++)
    {
        my $t = $targets[$i];
        push(@result, _getTestsForTarget($t, $project));
    }
    return @result;
}

sub _writeTestsForEachTarget ()
{
    my ($project, @targets) = @_;
    my $targetsNum = scalar(@targets);
    for (my $i = 0; $i < $targetsNum; $i++)
    {

        my $t = $targets[$i];
        my @tests = _getTestsForTarget($t, $project);
        print("Processing $t\n");
        my $outfile = "$t.txt";

        my $fh = _openFile("$artifactsRoot/$outfile", '>');
        print($fh join("\n", @tests));
        close($fh);
    }
}

sub _buildIntersections ()
{
    my ($project, @targets) = @_;
    my $targetsNum = scalar(@targets);
    for (my $i = 0; $i < $targetsNum; $i++)
    {
        for (my $j = $i; $j < $targetsNum; $j++)
        {
            my $t1 = $targets[$i];
            my $t2 = $targets[$j];
            next if $t1 eq $t2;
            print("Processing $t1 --> $t2\n");

            my @t1Tests = _getTestsForTarget($t1);
            my @t2Tests = _getTestsForTarget($t2);
            my @isect = intersect(@t1Tests, @t2Tests);

            my $outfile = "$t1-$t2";
            my $fh = _openFile("$artifactsRoot/intersection-$outfile.txt", ">");

            print($fh join("\n", @isect));
            close($fh);
        }
    }
}

sub _getTestList
{
    my ($target, $project) = @_;

    my $projectName = fileparse($project);

    my $command = "perl test.pl --list --suite=integration --env=win --testtarget=$target --testprojects=$projectName";
    if ($skipBuildDeps)
    {
        $command .= ' --skip-build-deps';
    }
    print("$command\n");

    my @stdout = qx ($command);

    my $testsCount = scalar(@stdout);
    for (my $i = 0; $i < $testsCount; $i++)
    {
        chomp($stdout[$i]);
    }
    $skipBuildDeps = 1;
    return @stdout;
}

sub _writeTestPlan
{
    my ($filename, $testPlan) = @_;
    my $fh = _openFile($filename, '>');
    foreach ($testPlan->getAllTestItems())
    {
        my $testName = $_->getName();
        print($fh "$testName\n");
    }
    close($fh);
}

sub _openFile
{
    my ($filename, $mode) = @_;
    my $openRes = open(my $fh, $mode, $filename);
    if (not defined($openRes))
    {
        croak("can not open file $filename");
    }
    return $fh;
}

sub _readOptions
{
    my ($targets, $projects) = @_;

    my ($project, $artifacts_path, @targets);

    my $resGetOpt = GetOptions(
        "project=s" => \$project,
        "artifacts_path:s" => \$artifactsRoot,
        "targets:s@" => \@targets,
    );

    if (not defined($artifacts_path))
    {
        $artifacts_path = tempdir();
    }

    if (not $resGetOpt)
    {
        croak("failed to parse options$!");
    }

    return ($project, $artifacts_path, @targets);
}

main();
