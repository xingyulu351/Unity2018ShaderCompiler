use warnings;
use strict;

use FindBin qw ($Bin);

use lib "$Bin/../..";

use Dirs;
use lib Dirs::external_root();

use Test::More;
use Test::Mojo;

require "$FindBin::Bin/../Desk.pl";

my $t = Test::Mojo->new;
$t->get_ok('/')->status_is(200);

done_testing();
