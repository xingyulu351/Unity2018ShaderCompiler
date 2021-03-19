use Test::More;

use warnings;
use strict;

use Dirs;
use lib Dirs::external_root();

use Test::MockObject;
use ArtifactsNameBuilder;

use File::Spec::Functions qw (canonpath);

my $artifactsRoot = "/tmp/artifacts";

BEGIN { use_ok('SingleSuiteRunnerArtifactsNameBuilder') }

creation:
{
    can_ok('SingleSuiteRunnerArtifactsNameBuilder', 'new');
    my $builder = new SingleSuiteRunnerArtifactsNameBuilder();
    isa_ok($builder, 'SingleSuiteRunnerArtifactsNameBuilder');
}

getFullFolderPathForArtifactType_ReturnsPathMatchesConventions:
{
    my $runnerMock = _createRunnerMock(artifacts_path => $artifactsRoot, suite => 'runtime', platform => 'editor');
    my $builder = new SingleSuiteRunnerArtifactsNameBuilder($runnerMock);

    my $suiteRunArtifactsRoot = ArtifactsNameBuilder::getSuiteRunArtifactsFolderName(
        suite => 'runtime',
        platform_or_target => 'editor'
    );

    my $artifactLeafFolder = ArtifactsNameBuilder::getFolderNameForArtifactType(type => 'Project');
    my $result = $builder->getFullFolderPathForArtifactType(type => 'Project');
    is($result, canonpath("$artifactsRoot/$artifactLeafFolder"));
}

getFullAtrifactFileName_ReturnsFileNameWithMatchConventions:
{
    my $runnerMock = _createRunnerMock(artifacts_path => $artifactsRoot, suite => 'runtime', platform => 'editor');
    my $builder = new SingleSuiteRunnerArtifactsNameBuilder($runnerMock);

    my $result = $builder->getFullAtrifactFileName('parent-type' => 'AllInOneRunner', type => 'stdout', extension => 'txt');

    my $suiteRunArtifactsRoot = ArtifactsNameBuilder::getSuiteRunArtifactsFolderName(
        suite => 'runtime',
        platform_or_target => 'editor'
    );

    my $artifactsLeafFolder = ArtifactsNameBuilder::getFolderNameForArtifactType(type => 'AllInOneRunner');
    my $filename = ArtifactsNameBuilder::getName(type => 'stdout', extension => 'txt');
    is($result, canonpath("$artifactsRoot/$artifactsLeafFolder/$filename"));
}

getSuiteRunRoot_RetunsCorrectValue:
{
    my $runnerMock = _createRunnerMock(artifacts_path => $artifactsRoot, suite => 'runtime', platform => 'editor');
    my $suiteRunArtifactsRoot = ArtifactsNameBuilder::getSuiteRunArtifactsFolderName(suite => 'runtime', platform_or_target => 'editor');
    my $builder = new SingleSuiteRunnerArtifactsNameBuilder($runnerMock);
    is($builder->getSuiteRunRoot(), $artifactsRoot);
}

done_testing();

sub _createRunnerMock
{
    my %params = @_;

    my $runnerMock = new Test::MockObject();
    $runnerMock->mock('getArtifactsPath', sub { return $params{'artifacts_path'}; });
    $runnerMock->mock('getName', sub { return $params{'suite'}; });
    $runnerMock->mock('getPlatform', sub { return $params{'platform'}; });

    return $runnerMock;
}
