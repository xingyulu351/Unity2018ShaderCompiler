package SuiteRunners::FakeParallel;

use parent NUnitConfigBasedSuiteRunner;

use warnings;
use strict;
use Dirs;
use File::Spec::Functions qw (canonpath);
use Environments::Win;
use RepositoryInfo;
my $root = Dirs::getRoot();
my $proj = canonpath("$root/Tests/Unity.FakeParallelTestsAssembly/Unity.FakeParallelTestsAssembly.csproj");

sub getConfig
{
    my ($this) = @_;
    my $config = {
        name => 'fake-parallel',
        nunitInputFiles => [$proj],
        environment => new Environments::Win(),
        help => {
            description => "This suite is used to test parallel test capabilities of katana",
            examples => [],
        }
    };

    return $config;
}

sub queryPartitions
{
    my ($this) = @_;
    my %repositoryInfo = RepositoryInfo::get();
    $this->prepareArtifactsFolders();
    $this->buildDependencies();
    return ParallelTestsSelector::queryPartitions(
        %repositoryInfo,
        uri => $this->getOptionValue('parallel-tests-selector-uri'),
        configId => $this->getOptionValue('config-id'),
        submitter => $this->getOptionValue('submitter'),
        testPlan => $this->getTestPlan(),
        partitionCount => $this->getOptionValue('partition-count'),
        filterUsingSmartSelect => $this->getOptionValue('smart-select'),
        timeout => $this->getOptionValue('query-partitions-timeout'),
        retries => $this->getOptionValue('query-partitions-retries'),
    );
}

1;
