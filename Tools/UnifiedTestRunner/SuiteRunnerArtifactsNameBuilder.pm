package SuiteRunnerArtifactsNameBuilder;

use File::Spec::Functions qw (canonpath);

sub new
{
    my ($pkg, $runner) = @_;
    my $instance = { runner => $runner };
    return bless($instance, $pkg);
}

sub getSuiteRunRoot
{
    my ($this) = @_;
    my $suiteRunRoot = ArtifactsNameBuilder::getSuiteRunArtifactsFolderName(
        suite => $this->_getName(),
        platform_or_target => $this->_getPlatform()
    );
}

sub getFullFolderPathForArtifactType
{
    my ($this, %params) = @_;

    my $artifactType = $params{type};

    my $suiteArtifactsRoot = $this->_getArtifactsPath();

    my $artifactLeafFolder = ArtifactsNameBuilder::getFolderNameForArtifactType(type => $artifactType);

    my $suiteRunRoot = $this->getSuiteRunRoot();

    return canonpath("$suiteArtifactsRoot/$suiteRunRoot/$artifactLeafFolder");
}

sub getFullAtrifactFileName :
{
    my ($this, %params) = @_;
    my $artifactFolder = $this->getFullFolderPathForArtifactType(type => $params{'parent-type'});
    my $filename = ArtifactsNameBuilder::getName(type => $params{type}, extension => $params{extension});
    return canonpath("$artifactFolder/$filename");
}

sub _getArtifactsPath :
{
    my ($this) = @_;
    return $this->{runner}->getArtifactsPath();
}

sub _getName :
{
    my ($this) = @_;
    return $this->{runner}->getName();
}

sub _getPlatform :
{
    my ($this) = @_;
    return $this->{runner}->getPlatform();
}

1;
