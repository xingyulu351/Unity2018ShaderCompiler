use warnings;
use strict;
use Dirs;

use Test::More;

BEGIN { use_ok('ProfileArgsReader') }

can_ok('ProfileArgsReader', 'read');

my $fileContent = "--suite=integration\t--testtarget=TestABV \n--suite=runtime\t  \t--suite=native --suite=cgbatch\n";

open(my $fh, '<', \$fileContent);
my @args = ProfileArgsReader::read($fh);

is($args[0], '--suite=integration');
is($args[1], '--testtarget=TestABV');
is($args[2], '--suite=runtime');
is($args[3], '--suite=native');
is($args[4], '--suite=cgbatch');

can_ok('ProfileArgsReader', 'resolveProfilePath');

my $root = Dirs::getRoot();
my $fullProfilePath = ProfileArgsReader::resolveProfilePath('abv');
is($fullProfilePath, "$root/Tools/UnifiedTestRunner/Profiles/abv");

$fullProfilePath = ProfileArgsReader::resolveProfilePath('/tmp/abv');
is($fullProfilePath, "/tmp/abv");

done_testing();
