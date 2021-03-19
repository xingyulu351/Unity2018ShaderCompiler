use warnings;
use strict;

use Test::More;
use Dirs;
use lib Dirs::external_root();
use Test::Output;
use Test::Trap;
use Test::MockModule;
use Option;
use Commands;
use Test::Differences;
use StringUtils;

BEGIN { use_ok('HelpPrinter') }

getText_NoParams_Croaks:
{
    my @trap = trap
    {
        HelpPrinter::getText();
    };
    like($trap->die, qr/parameters/);
}

getText_ReturnsExpectedText:
{

    my $expected = <<'END';
description

Details: url

Prerequisites: perl build.pl --prepare

Usage examples:
EXAMPLES HELP
OPTIONS HELP
END
    my $examplesHelpPrinter = new Test::MockModule('ExamplesHelpPrinter');
    $examplesHelpPrinter->mock(getText => sub { return "EXAMPLES HELP" });

    my $optionsHelpPrinter = new Test::MockModule('OptionsHelpPrinter');
    $optionsHelpPrinter->mock(getText => sub { return "OPTIONS HELP" });

    my $text = HelpPrinter::getText(
        description => 'description',
        url => 'url',
        prerequisites => "perl build.pl --prepare",
        options => [],
        examples => []
    );

    eq_or_diff($text, StringUtils::trim($expected));
}

done_testing();
