package FolderDeleter;

use warnings;
use strict;

use File::Path qw (rmtree);

sub new
{
    return bless({}, __PACKAGE__);
}

sub delete
{
    my ($ths, $dir) = @_;
    if (-d $dir)
    {
        rmtree($dir);
    }
}

1;
