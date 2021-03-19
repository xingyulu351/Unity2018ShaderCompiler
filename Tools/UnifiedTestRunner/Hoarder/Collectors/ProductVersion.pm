package Hoarder::Collectors::ProductVersion;

use warnings;
use strict;
use Cwd qw (realpath);

use Dirs;

sub new
{
    my ($pkg, $revision) = @_;
    my $buildConfiModulePath = Dirs::getRoot() . "/Configuration/BuildConfig.pm";
    require $buildConfiModulePath;
    my $instance = {
        unity_version => $BuildConfig::unityVersion,
        revision => 'revision_' . $revision->getId(),
        revision_date => $revision->getDate(),
        branch => $revision->getBranch(),
        vcs => $revision->getVcsType(),
        branches => $revision->getBranches(),
    };
    return bless($instance, $pkg);
}

sub name
{
    return 'product-version';
}

sub data
{
    return { %{ shift() } };
}

1;
