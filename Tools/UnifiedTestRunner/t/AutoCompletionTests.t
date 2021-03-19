use warnings;
use strict;

use Test::More;
use Option;
use Options;
use AutoComplete;

use_ok('AutoComplete');

complete_DoesNotResolveNonExistingOption:
{
    my @completions = _complete(
        globalOptions => _makeOptions(['o1:s'], ['o2:s']),
        inputInfo => {
            argv => ['./utr', '--z='],
            wordIdx => 1
        }
    );
    is_deeply(\@completions, []);
}

complete_NoInput_ResolvesAllTheGlobalOptions:
{
    my @completions = _complete(
        globalOptions => _makeOptions(['o1:s'], ['o2:s']),
        inputInfo => {
            argv => ['./utr'],
            wordIdx => 1
        }
    );
    is_deeply(\@completions, ['--o1=', '--o2=']);
}

complete_ResolvesOneOptionByPartialMatch:
{
    my @completions = _complete(
        globalOptions => _makeOptions(['o1:s']),
        inputInfo => {
            argv => ['./utr', '--o'],
            wordIdx => 1
        }
    );
    is_deeply(\@completions, ['--o1=']);
}

complete_ResolvesMultipleOptionsByPartialMatch:
{
    my @completions = _complete(
        globalOptions => _makeOptions(['o1:s'], ['o2:s']),
        inputInfo => {
            argv => ['./utr', 'o'],
            wordIdx => 1
        }
    );
    is_deeply(\@completions, ['--o1=', '--o2=']);
}

complete_ResolvesOptionValues:
{
    my @completions = _complete(
        globalOptions => _makeOptions(['o1:s', { allowedValues => ['1', '2', '3'] }]),
        inputInfo => {
            argv => ['./utr', '-o1='],
            wordIdx => 1
        }
    );
    is_deeply(\@completions, ['1', '2', '3']);
}

complete_ResolvesSuiteOptions:
{
    my @completions = _complete(
        globalOptions => _makeOptions(['suite:s']),
        suiteOptions => {
            native => _makeOptions(['so1:s'], ['so2:s']),
        },
        inputInfo => {
            argv => ['./utr', '-suite=native'],
            wordIdx => 2
        }
    );
    is_deeply(\@completions, ['--suite=', '--so1=', '--so2=']);
}

complete_CanResolveOptionValueInTheMiddle:
{
    my @completions = _complete(
        globalOptions => _makeOptions(['suite:s', { allowedValues => ['native', 'runtime', 'editor'] }]),
        suiteOptions => {
            native => _makeOptions(['so1:s'], ['so2:s']),
        },
        inputInfo => {
            argv => ['./utr', '-suite=native', '--testfilter=blabla'],
            wordIdx => 1
        }
    );
    is_deeply(\@completions, ['native']);
}

complete_DoesNotSuggestHiddenOptions:
{
    my @completions = _complete(
        globalOptions => _makeOptions(['o1:s', { hidden => 1 }], ['o2:s'], ['o3:s']),
        inputInfo => {
            argv => ['./utr'],
            wordIdx => 1
        }
    );
    is_deeply(\@completions, ['--o2=', '--o3=']);
}

complete_ResolvesBothSuiteAndGlobalOptions:
{
    my @completions = _complete(
        globalOptions => _makeOptions(['o1:s'], ['o2:s']),
        suiteOptions => {
            native => _makeOptions(['o3:s'], ['o4:s']),
        },
        inputInfo => {
            argv => ['./utr', '--suite=native', '--o'],
            wordIdx => 2
        }
    );
    is_deeply(\@completions, ['--o1=', '--o2=', '--o3=', '--o4=']);
}

complete_ReturnsUniqueOptionSet:
{
    my @completions = _complete(
        globalOptions => _makeOptions(['suite:s'], ['testfilter:s']),
        suiteOptions => {
            native => _makeOptions(['testfilter:s']),
        },
        inputInfo => {
            argv => ['./utr', '--suite=native', '--testfi'],
            wordIdx => 2
        }
    );
    is_deeply(\@completions, ['--testfilter=']);
}

complete_DoesNotSuggestOptionsAlreadyAddedToCommandLine:
{
    my @completions = _complete(
        globalOptions => _makeOptions(['a:s'], ['ab:s'], ['abc:s'], ['abcd']),
        inputInfo => {
            argv => ['./utr', '--ab=1', '--a'],
            wordIdx => 2
        }
    );
    is_deeply(\@completions, ['--a=', '--abc=', '--abcd']);
}

done_testing();

sub _complete
{
    my (%args) = @_;
    my $autoComplete = new AutoComplete(%args);
    return $autoComplete->complete($args{inputInfo});
}

sub _makeOptions
{
    my (@descriptions) = @_;
    my @options = Option::fromDescriptions(@descriptions);
    return new Options(options => \@options);
}
