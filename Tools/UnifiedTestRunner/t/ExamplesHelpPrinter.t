use warnings;
use strict;

use Test::More;

use Dirs;
use lib Dirs::external_root();
use Test::Differences;

BEGIN { use_ok('ExamplesHelpPrinter'); }

getText_NoExamples_ReturnsEmptyString:
{
    my $text = ExamplesHelpPrinter::getText();
    is($text, '');
}

print_Example_PrintsIt:
{
    my $expected = <<'END';
    example 1
END
    my $text = ExamplesHelpPrinter::getText(examples => [{ description => 'example 1' },]);

    eq_or_diff($text, $expected);
}

getText_ExampleWithCommand_PrintsIt:
{
    my $expected = <<'END';
    example 1
        perl build.pl
END
    my $text = ExamplesHelpPrinter::getText(examples => [{ description => 'example 1', command => 'perl build.pl' },]);
    eq_or_diff($text, $expected);
}

getText_TwoExamplesWithCommandAndPrefix_PrintsIt:
{
    my $expected = <<'END';
    example 1
        ./test -suite=native --testfilter=blabla
    example 2
        ./test -suite=native --area=Psysics
END

    my $text = ExamplesHelpPrinter::getText(
        commandPrefix => './test -suite=native',
        examples => [{ description => 'example 1', command => '--testfilter=blabla' }, { description => 'example 2', command => '--area=Psysics' },]
    );

    eq_or_diff($text, $expected);
}

done_testing();
