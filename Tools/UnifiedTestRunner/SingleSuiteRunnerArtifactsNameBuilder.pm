package SingleSuiteRunnerArtifactsNameBuilder;

use parent SuiteRunnerArtifactsNameBuilder;
use File::Spec::Functions qw (canonpath);

use warnings;
use strict;

sub new
{
    my ($pkg, $runner) = @_;
    my $instance = $pkg->SUPER::new($runner);
    return bless($instance, $pkg);
}

sub getSuiteRunRoot
{
    my ($this) = @_;
    $this->_getArtifactsPath();
}

sub getFullFolderPathForArtifactType
{
    my ($this, %params) = @_;
    my $artifactType = $params{type};
    my $suiteArtifactsRoot = $this->_getArtifactsPath();
    my $artifactLeafFolder = ArtifactsNameBuilder::getFolderNameForArtifactType(type => $artifactType);
    return canonpath("$suiteArtifactsRoot/$artifactLeafFolder");
}

1;
