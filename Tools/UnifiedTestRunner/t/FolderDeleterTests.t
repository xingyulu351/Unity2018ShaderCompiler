use Test::More;
use File::Temp qw (tempdir);

use warnings;
use strict;

BEGIN { use_ok('FolderDeleter') }

can_ok('FolderDeleter', 'new');
my $fd = new FolderDeleter();

isa_ok($fd, 'FolderDeleter');
my $dir = tempdir();
ok(-d $dir);

$fd->delete($dir);
ok(not -d $dir);

done_testing();
