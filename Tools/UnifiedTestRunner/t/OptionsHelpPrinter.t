use warnings;
use strict;

use Test::More;
use Dirs;
use lib Dirs::external_root();
use Option;
use Test::Differences;
use StringUtils qw (trim);

use_ok('OptionsHelpPrinter');

getText_NoOptions_PrintsNothing:
{
    my $text = OptionsHelpPrinter::getText();
    is($text, '');
}

getText_Options_HelpWithOptions:
{
    my $expected = <<'END';
--o1=value
    o1 descr
--o2[=value]
    o2 descr
--o3[=value] default: v1
    o3 descr
--o4[=value]
    o4 descr Values: v1, v2, v3
--o5, -o, --o7[=value] default: v2
    Values: v1, v2, v3
--o11
    o11 flag option
--o12[=value] default: all
    o12 descr
END
    my @options = Option::fromDescriptions(
        ['o1=s', 'o1 descr'],
        ['o2:s', 'o2 descr'],
        ['o3:s', 'o3 descr', { defaultValue => 'v1' }],
        ['o4:s', 'o4 descr', { allowedValues => ['v1', 'v2', 'v3'] }],
        ['o5|o|o7:s', undef, { defaultValue => 'v2', allowedValues => ['v1', 'v2', 'v3'] }],
        ['o10', undef, { hidden => 1 }],
        ['o11', 'o11 flag option'],
        ['o12:@s', 'o12 descr', { defaultValue => ['all'] }],
    );

    my $text = OptionsHelpPrinter::getText(@options);
    eq_or_diff($text, StringUtils::trim($expected));
}

done_testing();
