use warnings;
use strict;

use Test::More;

BEGIN { use_ok('Hoarder::Collectors'); }

my @result = Hoarder::Collectors::getAllCollectors();

done_testing();
