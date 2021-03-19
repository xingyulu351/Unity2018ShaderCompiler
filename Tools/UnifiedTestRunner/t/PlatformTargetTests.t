use warnings;
use strict;
use Dirs;
use lib Dirs::external_root();
use File::Spec::Functions qw (catfile);
use Test::More;
use Test::Output;
use PlatformTarget;
use Test::Deep;

BEGIN { use_ok('PlatformTarget'); }
my $utrRoot = Dirs::UTR_root();
my $targetPath = catfile($utrRoot, 't/TestData/BuildTargets/Platform1/Platform1Player.pm');
my $buildTarget = new PlatformTarget($targetPath, 'Platform1');

is($buildTarget->getModule(), $targetPath);
is($buildTarget->getPlatform(), 'Platform1');
is($buildTarget->getName(), 'Platform1Player');
stdout_is(sub { $buildTarget->prepare($targetPath) }, "PreparePlatform1Player got called");
stdout_is(sub { $buildTarget->dispose($targetPath) }, "Dispose got called");

is_deeply(
    $buildTarget->getTestSettings($targetPath),
    {
        platform1Setting1 => 'platform1Setting1Value',
        platform1Setting2 => 'platform1Setting2Value'
    }
);

done_testing();
