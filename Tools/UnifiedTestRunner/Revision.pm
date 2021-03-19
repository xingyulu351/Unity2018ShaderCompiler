package Revision;

use warnings;
use strict;

use File::Spec::Functions qw (catdir);
use Carp qw(croak);
use MercurialRevision;
use GitRevision;
use SystemCall;

sub get
{
    use Test::More;
    my $repoRoot = Dirs::getRoot();
    my $sytemCall = new SystemCall();
    if (-d catdir($repoRoot, '.hg'))
    {
        return new MercurialRevision($sytemCall);
    }

    if (-d catdir($repoRoot, '.git'))
    {
        return new GitRevision($sytemCall);
    }

    croak("Can not determine repository type at '$repoRoot'");
}

1;
