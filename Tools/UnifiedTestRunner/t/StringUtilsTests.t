use warnings;
use strict;

use Test::More;
use StringUtils qw (isNullOrEmpty trimStart trimEnd trim);

is(isNullOrEmpty(''), 1);
is(isNullOrEmpty(undef), 1);
is(isNullOrEmpty("a"), 0);

is(trimStart(' 1'), '1');
is(trimStart(undef), undef);
is(trimStart('    1'), '1');
is(trimStart(''), '');

is(trimEnd('1 '), '1');
is(trimEnd("1    "), '1');
is(trimEnd(undef), undef);
is(trimEnd(''), '');

is(trim(' 1 '), '1');

my $originalString = ' orig ';
trim($originalString);
is($originalString, ' orig ', 'trim does not modify original string');

done_testing();
