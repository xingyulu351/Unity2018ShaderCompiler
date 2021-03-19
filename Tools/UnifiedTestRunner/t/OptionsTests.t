use warnings;
use strict;

use Test::More;
use Option;

use_ok('Options');

Register_AddsOptionToTheStorage:
{
    my $description = 'o1=s';
    my $options = create([$description]);
    my $expected = Option::fromDescription([$description]);

    my @options = $options->getOptions();
    is_deeply(\@options, [$expected]);
}

Create_AcceptsAnOptionList:
{
    my @options = Option::fromDescriptions(['o1=s']);
    my $options = new Options(options => \@options);
    my @restored = $options->getOptions();
    is_deeply(\@restored, \@options);
}

GetOptionValue_CommandLineParseNotdone_ReturnsDefaultValue:
{
    my $options = create(['o1=s', '', { defaultValue => 'option_default_value' }]);

    is($options->getOptionValue('o1'), 'option_default_value');
}

GetOptionValue_2OptionsWithDefaultValues_ReturnsDefaultValue:
{
    my $options = create(['o1=s', '', { defaultValue => 'option1_default_value' }], ['o2=s', '', { defaultValue => 'option2_default_value' }],);

    is($options->getOptionValue('o2'), 'option2_default_value');
}

CanReadPrototypeSection:
{
    my $options = create({ prototypes => ['o1:s', 'o2:s'] }, ['o3:s', '', { defaultValue => 'option3_default_value' }],);

    is($options->getOptionValue('o1'), undef);
    is($options->getOptionValue('o2'), undef);
    is($options->getOptionValue('o3'), 'option3_default_value');
}

GetOptionValue_CommandLineAndDefaultValues_ReturnsValueFromTheCommandLine:
{
    my $options = create(['o1=s', '', { defaultValue => 'option1_default_value' }]);

    $options->parse("--o1=command_line_value");

    is($options->getOptionValue('o1'), 'command_line_value');
}

GetOptionValue_ArrayOptionIsSpecified_ReturnsValueFromTheCommandLine:
{
    my $options = create(['o1=s@'],);

    $options->parse("--o1=v1", "--o1=v2", "--o1=v3");

    is_deeply($options->getOptionValue('o1'), ['v1', 'v2', 'v3']);
}

Parser_ArrayOptionWithDefaultValue_ResetsDefaultValue:
{
    my $options = create(['o1:s@', '', { defaultValue => ['default_value'] }],);

    $options->parse('--o1=new_value');

    is_deeply($options->getOptionValue('o1'), ['new_value']);
}

GetOptionValue_ReturnsCleanValue:
{
    my $options = create(['o1:s'], ['o2:s'],);

    $options->parse("--o1=v1");
    is_deeply($options->getOptionValue('o1'), 'v1');

    $options = create(['o1:s'], ['o2:s'],);

    $options->parse("--o2=v1");
    is($options->getOptionValue('o1'), undef);
}

GetOptionValue_SecondParseWillResetPreviousParsingResults:
{
    my $options = create(['o1:s'], ['o2:s'],);

    $options->parse("--o1=v1");

    is_deeply($options->getOptionValue('o1'), 'v1');

    $options->parse("--o2=v1");
    is($options->getOptionValue('o1'), undef);
}

registerOption_OverridesOptionAlreadyExists_HasAnAdvantage:
{
    my $option1 = Option::fromDescription(['o1:s']);
    my $option2 = Option::fromDescription(['o1=s']);

    my $options = new Options();

    $options->registerOption($option1);
    $options->registerOption($option2);

    my @options = $options->getOptions();

    is_deeply(\@options, [$option2]);
}

getOptionValue_NamespaceIsGiven_ReturnsAppopriateValue:
{
    my $option1 = new Option(
        names => ['o'],
        namespace => 'ns',
        valueType => Option->ValueTypeOptional,
        typeSpec => 's'
    );

    my $options = new Options();

    $options->registerOption($option1);

    $options->parse("--ns-o=v1");

    my $value = $options->getOptionValue('o', 'ns');

    is($value, 'v1');
}

getOptionValue_SameOptionExistsInAndOutOfNamespace_ReturnsAppopriateValue:
{
    my $option1 = new Option(
        names => ['o'],
        namespace => 'ns',
        valueType => Option->ValueTypeOptional,
        typeSpec => 's'
    );

    my $option2 = new Option(
        names => ['o'],
        namespace => undef,
        valueType => Option->ValueTypeOptional,
        typeSpec => 's'
    );

    my $options = new Options();

    $options->registerOption($option1);
    $options->registerOption($option2);

    $options->parse('--ns-o=v1', '--o=v2');

    is($options->getOptionValue('o'), 'v2');
    is($options->getOptionValue('o', 'ns'), 'v1');
}

resolve_ValueInGlobalNamespace_ReturnsIt:
{
    my $o11 = Option::fromDescription(['o1:s']);
    my $options = new Options(options => [$o11]);
    $options->parse('--o1=v1');

    my $v = $options->resolve('o1');

    is($v, 'v1');
}

resolve_NamespaceSpecified_ReturnsValueFromTheNamespace:
{
    my $o11 = Option::fromDescription(['o1:s']);
    my $o12 = Option::fromDescription(['o1:s']);
    $o12->moveToNamespace('ns');
    my $options = new Options(options => [$o11, $o12]);
    $options->parse('--ns-o1=v1');

    my $v = $options->resolve('o1', 'ns');

    is($v, 'v1');
}

resolve_ComandLineValueHasAnAdvantageOverDefaultValueInNamespace:
{
    my $o11 = Option::fromDescription(['o1:s']);
    my $o12 = Option::fromDescription(['o1:s', undef, { defaultValue => 'o1nsdefault' }]);
    $o12->moveToNamespace('ns');

    my $options = new Options(options => [$o11, $o12]);
    $options->parse('--o1=v1');
    my $v = $options->resolve('o1', 'ns');
    is($v, 'v1');
}

resolve_ComandLineValueHasAnAdvantageOverDefaultValueInNamespace:
{
    my $o1 = Option::fromDescription(['o1:s', { isPrototype => 1 }]);
    is($o1->{isPrototype}, 1);
}

resolve_CommandLineValueForNamespace_ReturnsValueFromNamespace:
{
    my $o11 = Option::fromDescription(['o1:s']);
    my $o12 = Option::fromDescription(['o1:s', undef, { defaultValue => 'o1nsdefault' }]);
    $o12->moveToNamespace('ns');

    my $options = new Options(options => [$o11, $o12]);
    $options->parse('--ns-o1=v2');

    my $v = $options->resolve('o1', 'ns');
    is($v, 'v2');
}

resolve_NamespaceCommandLineValueHasAnAdvantage:
{
    my $o11 = Option::fromDescription(['o1:s']);
    my $o12 = Option::fromDescription(['o1:s', undef, { defaultValue => 'o1nsdefault' }]);
    $o12->moveToNamespace('ns');

    my $options = new Options(options => [$o11, $o12]);
    $options->parse('--ns-o1=v2');

    my $v = $options->resolve('o1');
    is($v, undef);
}

resolve_NamespaceDefaultValueIsPreferedOverGlobalDefaultValue:
{
    my $o11 = Option::fromDescription(['o1:s'], undef, { defaultValue => 'globaldefault' });
    my $o12 = Option::fromDescription(['o1:s', undef, { defaultValue => 'o1nsdefault' }]);
    $o12->moveToNamespace('ns');

    my $options = new Options(options => [$o11, $o12]);
    $options->parse();

    my $v = $options->resolve('o1', 'ns');
    is($v, 'o1nsdefault');
}

resolve_NoGlobalDefinition_NamespaceDefaultValueIsPrefered:
{
    my $o12 = Option::fromDescription(['o1:s', undef, { defaultValue => 'o1nsdefault' }]);
    $o12->moveToNamespace('ns');

    my $options = new Options(options => [$o12]);
    $options->parse();
    my $v = $options->resolve('o1', 'ns');
    is($v, 'o1nsdefault');
}

resolve_OnlyGlobalValueIsDefined_PrefersGlobalValue:
{
    my $o12 = Option::fromDescription(['o1:s', undef, { defaultValue => 'globalDefault' }]);
    my $options = new Options(options => [$o12]);
    $options->parse();

    my $v = $options->resolve('o1', 'ns');
    is($v, 'globalDefault');
}

resolveOptionValue_OptionHasDefaultValueInNamespaceAndExplicitValueInGlobalNamespace_ReturnsIt:
{
    my $o12 = Option::fromDescription(['o1:s', undef, { defaultValue => 'o1nsdefault' }]);
    $o12->moveToNamespace('ns');

    my $options = new Options(options => [$o12]);
    $options->parse();

    my $v = $options->resolve('o1');
    is($v, undef);
}

getOptionByName_ResolvesOptionDefinedAsArray:
{
    my @options = Option::fromDescriptions(['o1=s'], ['o2=s']);

    my $o = Options::getOptionByName('o1', \@options);
    my @names = $o->getNames();
    is_deeply(\@names, ['o1']);
}

getOptionByName_ResolvesOptionDefinedAsObject:
{
    my $options = create(['o1=s'], ['o2=s']);
    my $o = Options::getOptionByName('o1', $options);
    my @names = $o->getNames();
    is_deeply(\@names, ['o1']);
}

getOptionByName_RetunrsUndefOnNonExistingOptions:
{
    my $options = create(['o1=s'], ['o2=s']);
    my $o = Options::getOptionByName('o3', $options);
    is_deeply($o, undef);
}

getOptionByName_RetunrsUndefOnNonExistingOptions:
{
    my @options = Option::fromDescriptions(['o1=s'], ['o2=s']);

    my $o = Options::getOptionByName('o3', \@options);
    is_deeply($o, undef);
}

getUnknownOptions_AllOptionsAreKnown_ReturnsAnEmptyArray:
{
    my $o11 = Option::fromDescription(['o1:s']);
    my $options = new Options(options => [$o11]);
    $options->parse('--o1=v1');
    my @unkownOptions = $options->getUnknownOptions();
    is_deeply(\@unkownOptions, []);
}

getUnknownOptions_OneUknownOption_ReturnsIt:
{
    my $o11 = Option::fromDescription(['o1:s']);
    my $options = new Options(options => [$o11]);
    $options->parse('--o2=v1');
    my @unkownOptions = $options->getUnknownOptions();
    is_deeply(\@unkownOptions, ['--o2=v1']);
}

done_testing();

sub create
{
    my (@descriptions) = @_;
    my @options = Option::fromDescriptions(@descriptions);
    my $options = new Options(options => \@options);
    return $options;
}
