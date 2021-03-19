package RepositoryInfo;

use warnings;
use strict;

use Dirs;
use MercurialRevision;
use SystemCall;

sub get
{
    my %args = @_;
    my %result;
    my $systemCall = new SystemCall();
    my $revision = new MercurialRevision($systemCall);
    $result{branch} = $revision->getBranch();
    $result{revision} = $revision->getId();
    $result{date} = $revision->getDate();
    $result{vcs} = $revision->getVcsType();
    return %result;
}

1;
