use warnings;
use strict;

use Test::More;
use Dirs;
use lib Dirs::external_root();
use Test::MockModule;
use Test::MockObject;

BEGIN { use_ok('RepositoryInfo'); }

my $revisionMock = new Test::MockModule('MercurialRevision');
$revisionMock->mock(getBranch => sub { return 'trunk' });
$revisionMock->mock(getId => sub { return '1234' });
$revisionMock->mock(getDate => sub { return '321' });

my %info = RepositoryInfo::get();
is($info{branch}, 'trunk');
is($info{revision}, '1234');
is($info{date}, '321');
is($info{vcs}, 'mercurial');

done_testing();
