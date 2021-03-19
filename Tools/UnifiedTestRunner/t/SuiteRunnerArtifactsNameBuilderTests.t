use Test::More;

use warnings;
use strict;

use Dirs;
use lib Dirs::external_root();

use Test::MockObject;
use ArtifactsNameBuilder;

use File::Spec::Functions qw (canonpath);

my $artifactsRoot = "/tmp/artifacts";

BEGIN { use_ok('SuiteRunnerArtifactsNameBuilder') }

creation:
{
    can_ok('SuiteRunnerArtifactsNameBuilder', 'new');
    my $builder = new SuiteRunnerArtifactsNameBuilder();
    isa_ok($builder, 'SuiteRunnerArtifactsNameBuilder');
}

getFullFolderPathForArtifactType_ReturnsPathMatchesConventions:
{
    my $runnerMock = _createRunnerMock(artifacts_path => $artifactsRoot, suite => 'runtime', platform => 'editor');
    my $builder = new SuiteRunnerArtifactsNameBuilder($runnerMock);

    my $suiteRunArtifactsRoot = ArtifactsNameBuilder::getSuiteRunArtifactsFolderName(
        suite => 'runtime',
        platform_or_target => 'editor'
    );

    my $artifactLeafFolder = ArtifactsNameBuilder::getFolderNameForArtifactType(type => 'Project');
    my $result = $builder->getFullFolderPathForArtifactType(type => 'Project');
    is($result, canonpath("$artifactsRoot/$suiteRunArtifactsRoot/$artifactLeafFolder"));
}

getFullAtrifactFileName_ReturnsFileNameWithMatchConventions:
{
    my $runnerMock = _createRunnerMock(artifacts_path => $artifactsRoot, suite => 'runtime', platform => 'editor');
    my $builder = new SuiteRunnerArtifactsNameBuilder($runnerMock);

    my $result = $builder->getFullAtrifactFileName('parent-type' => 'AllInOneRunner', type => 'stdout', extension => 'txt');

    my $suiteRunArtifactsRoot = ArtifactsNameBuilder::getSuiteRunArtifactsFolderName(
        suite => 'runtime',
        platform_or_target => 'editor'
    );
    my $artifactsLeafFolder = ArtifactsNameBuilder::getFolderNameForArtifactType(type => 'AllInOneRunner');
    my $filename = ArtifactsNameBuilder::getName(type => 'stdout', extension => 'txt');
    is($result, canonpath("$artifactsRoot/$suiteRunArtifactsRoot/$artifactsLeafFolder/$filename"));
}

getSuiteRunRoot_RetunsCorrectValue:
{
    my $runnerMock = _createRunnerMock(artifacts_path => $artifactsRoot, suite => 'runtime', platform => 'editor');
    my $suiteRunArtifactsRoot = ArtifactsNameBuilder::getSuiteRunArtifactsFolderName(suite => 'runtime', platform_or_target => 'editor');
    my $builder = new SuiteRunnerArtifactsNameBuilder($runnerMock);
    is($builder->getSuiteRunRoot(), $suiteRunArtifactsRoot);
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
