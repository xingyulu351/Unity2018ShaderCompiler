package ArtifactsFolderDeleterGuard;

use File::Spec;
use Carp qw (croak);
use File::Spec::Functions qw (canonpath);

use warnings;
use strict;
use Dirs;

sub safeFolderDelete
{
    my %params = @_;

    if (not $params{'deleter'})
    {
        croak('deleter must be specified when deleting artifacts folder');
    }

    if (not $params{'suite'})
    {
        croak('suite must be specified when deleting artifacts folder');
    }

    if (not $params{'platform_or_target'})
    {
        croak('platform must be specified when deleting artifacts folder');
    }

    if (not $params{'folder'})
    {
        croak('artifacts folder must be specified');
    }

    if (Dirs::isDriveRoot($params{'folder'}))
    {
        croak('attempt to delete the root of the drive detected!!!');
    }

    if (Dirs::isRepositoryRoot($params{'folder'}))
    {
        croak('attempt to delete the root of the repository');
    }

    $params{'deleter'}->delete(canonpath($params{'folder'}));
}

1;
