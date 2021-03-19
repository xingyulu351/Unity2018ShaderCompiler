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

BEGIN { use_ok('SuiteHelpPrinter') }

getText_SuiteName_PrintsIt:
{
    my $text = SuiteHelpPrinter::getText(name => 'native');
    is($text, '');
}

getText_Description_PrintsIt:
{
    my $text = SuiteHelpPrinter::getText(name => 'suite_name', description => 'playmode tests');
    eq_or_diff($text, 'playmode tests');
}

getText_SuiteUrl_PrintsIt:
{
    my $text = SuiteHelpPrinter::getText(name => 'suite_name', url => 'test_framework_url');
    eq_or_diff($text, 'Details: test_framework_url');
}

getText_Prerequisites_PrintsIt:
{
    my $text = SuiteHelpPrinter::getText(name => 'suite_name', prerequisites => 'do something special');
    eq_or_diff($text, 'Prerequisites: do something special');
}

getText_Examples_PrintsIt:
{
    my $optionsHelpPrinter = new Test::MockModule('ExamplesHelpPrinter');

    my $commandPrefix = undef;
    $optionsHelpPrinter->mock(
        getText => sub
        {
            my %args = @_;
            $commandPrefix = $args{commandPrefix};
        }
    );

    my $text = SuiteHelpPrinter::getText(
        name => 'suite_name',
        examples => [{ description => 'ex1', command => 'ex1.pl' }]
    );

    my $utrPrefix = Commands::getUtrPrefix();
    is($commandPrefix, "$utrPrefix --suite=suite_name");
}

getText_Options_HelpWithOptions:
{
    my $optionsHelpPrinter = new Test::MockModule('OptionsHelpPrinter');
    my $optionsHelpCalled = 0;
    $optionsHelpPrinter->mock(getText => sub { $optionsHelpCalled = 1; });
    my @options = Option::fromDescriptions(['o1=s', 'o1 descr'],);
    SuiteHelpPrinter::getText(name => 'suite_name', options => \@options);
    is($optionsHelpCalled, 1);
}

getText_NoNameIsSpecified_croaks:
{
    my @trap = trap
    {
        SuiteHelpPrinter::getText();
    };
    like($trap->die, qr/name/);
}

done_testing();
