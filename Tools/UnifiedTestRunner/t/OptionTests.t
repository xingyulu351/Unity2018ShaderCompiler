use warnings;
use strict;

use Test::More;
use TestHelpers::ArrayAssertions qw (arraysAreEqual);

use Dirs;
use lib Dirs::external_root();
use Test::Trap;

use_ok('Option');

Creation:
{
    my $option = new Option(names => 'option_name', valueType => Option->ValueTypeNone);
    isa_ok($option, 'Option');
}

getNames_ReturnsExpectedNames:
{
    my $option = new Option(names => ['option_name'], valueType => Option->ValueTypeNone);
    my @names = $option->getNames();
    arraysAreEqual(\@names, ['option_name']);
}

getValueType_ReturnsExpectedName:
{
    my $option = new Option(names => ['option_name'], valueType => Option->ValueTypeNone);
    is($option->getValueType(), Option->ValueTypeNone);
}

getValue_ReturnsUndef:
{
    my $option = new Option(names => ['option_name'], valueType => Option->ValueTypeNone);
    is($option->getValue(), undef);
}

getValue_ValueSet_ReturnsIt:
{
    my $option = new Option(names => ['option_name'], valueType => Option->ValueTypeNone);

    $option->setValue('default_value');

    is($option->getValue(), 'default_value');
}

getDescription_ReturnsExpectedValue:
{
    my $option = new Option(
        names => ['option_name'],
        valueType => Option->ValueTypeNone,
        description => 'option description'
    );
    is($option->getDescription(), 'option description');
}

getAllowedValues_ReturnsThem:
{
    my $option = new Option(
        names => ['option_name'],
        valueType => Option->ValueTypeNone,
        description => 'option description',
        allowedValues => ['v1', 'v2']
    );
    my @allowedValues = $option->getAllowedValues();
    arraysAreEqual(\@allowedValues, ['v1', 'v2']);
}

isHidden_NotSpecified_ReturnsFalse:
{
    my $option = new Option(
        names => ['option_name'],
        valueType => Option->ValueTypeNone,
    );

    is($option->isHidden(), 0);
}

isHidden_NotSpecified_ReturnsFalse:
{
    my $option = new Option(
        names => ['option_name'],
        valueType => Option->ValueTypeNone,
        hidden => 1
    );

    ok($option->isHidden());
}

defaultValue_NoDefaultValueIsSpecified_ReturnsNull:
{
    my $option = new Option(
        names => ['option_name'],
        valueType => Option->ValueTypeNone,
        hidden => 1
    );

    is($option->getValue(), undef);
}

defaultValue_NoDefaultValueIsSpecified_ReturnsNull:
{
    my $option = new Option(
        names => ['option_name'],
        valueType => Option->ValueTypeNone,
        hidden => 1,
        defaultValue => 'Editor'
    );

    is($option->getValue(), 'Editor');
}

toSpec_OneNoValueOption_ReturnsExpectedSpec:
{
    my $option = new Option(
        names => ['o1'],
        valueType => Option->ValueTypeNone
    );

    is($option->toSpec($option), 'o1');
}

toSpec_OneOptionWithOptionalValue_ReturnsExpectedSpec:
{
    my $option = new Option(
        names => ['o1'],
        valueType => Option->ValueTypeOptional,
        typeSpec => 's'
    );
    is($option->toSpec($option), 'o1:s');
}

toSpec_OneOptionWithRequiredValue_ReturnsExpectedSpec:
{
    my $option = new Option(
        names => ['o1'],
        valueType => Option->ValueTypeRequired,
        typeSpec => 's'
    );
    is($option->toSpec($option), 'o1=s');
}

croaks_IfOptionNameIsNotSpecified:
{
    my @trap = trap
    {
        my $option = new Option(valueType => Option->ValueTypeNone);
    };

    like($trap->die, qr/names/);
}

croaks_IfValueTypeIsNotSpecified:
{
    my @trap = trap
    {
        my $option = new Option(names => ['option_name']);
    };

    like($trap->die, qr/value type/);
}

fromDescription_SimpleOption_ParsedCorrectly:
{
    my $option = Option::fromDescription(['option_name']);

    my @names = $option->getNames();
    arraysAreEqual(\@names, ['option_name']);
    is($option->getValueType(), Option->ValueTypeNone);
}

fromDescription_OptionWithOptionalValue_ParsedCorrectly:
{
    my $option = Option::fromDescription(['option_name:s']);

    my @names = $option->getNames();
    arraysAreEqual(\@names, ['option_name']);
    is($option->getValueType(), Option->ValueTypeOptional);
}

fromDescription_OptionWithRequiredValue_ParsedCorrectly:
{
    my $option = Option::fromDescription(['option_name=s']);

    my @names = $option->getNames();
    arraysAreEqual(\@names, ['option_name']);
    is($option->getValueType(), Option->ValueTypeRequired);
}

fromDescription_OptionWithRequiredValue_ParsedCorrectly:
{
    my $option = Option::fromDescription(['option_name=s']);

    my @names = $option->getNames();
    arraysAreEqual(\@names, ['option_name']);
    is($option->getValueType(), Option->ValueTypeRequired);
}

fromDescription_OptionWithRequiredValue_ParsedCorrectly:
{
    my $description = ['option_name|on=s'];
    my $option = Option::fromDescription($description);

    my @names = $option->getNames();

    arraysAreEqual(\@names, ['option_name', 'on']);
    is($option->getValueType(), Option->ValueTypeRequired);
}

fromDescription_OptionWithDescription_ParsedCorrectly:
{
    my $option = Option::fromDescription(['option_name', 'option description']);

    is($option->getDescription(), 'option description');
}

fromDescription_AllowedValuesAreSpecified_ParsedCorrectly:
{
    my $option = Option::fromDescription(['option_name=s', 'description', { allowedValues => ['v1', 'v2'] }]);

    my @allowedValues = $option->getAllowedValues();

    arraysAreEqual(\@allowedValues, ['v1', 'v2']);
}

fromDescription_HiddenIsSpecified_ParsedCorrectly:
{
    my $option = Option::fromDescription(['option_name=s', 'description', { hidden => 1 }]);

    ok($option->isHidden());
}

fromDescription_DefaultValueIsSpecified_ParsedCorrectly:
{
    my $option = Option::fromDescription(['option_name=s', 'description', { defaultValue => 'default_value' }]);

    is($option->getValue(), 'default_value');
}

toSpec_NumericSpecifier_BuildsCorrectSpecification:
{
    my $option = Option::fromDescription(['option_name:i']);

    is($option->typeSpec(), 'i');
}

toSpec_ArraySpecifier_BuildsCorrectSpecification:
{
    my $option = Option::fromDescription(['option_name=s@']);

    arraysAreEqual($option->typeSpec(), 's@');
}

errorHandling:
{
    my @trap = trap
    {
        Option::fromDescription(['option_name=']);
    };
    like($trap->die, qr/invalid/i);

    @trap = trap
    {
        Option::fromDescription(['option_name:']);
    };
    like($trap->die, qr/invalid/i);

    @trap = trap
    {
        Option::fromDescription(['o1=s|o2=s']);
    };
    like($trap->die, qr/only one/i);
}

hasName_NoMatch_RetunsFalse:
{
    my $option = Option::fromDescription(['o1:s']);
    is($option->hasName('o2'), 0);
}

hasName_HasMatch_RetunsTrue:
{
    my $option = Option::fromDescription(['o1:s']);
    is($option->hasName('o1'), 1);
}

hasName_HasOnAliasMatch_RetunsFalse:
{
    my $option = Option::fromDescription(['o1|o2:s']);
    is($option->hasName('o2'), 1);
}

moveToNamespace_Names:
{
    my $option = Option::fromDescription(['o1|o2:s']);
    $option->moveToNamespace('ns');
    is($option->toSpec(), 'ns-o1|ns-o2:s');
}

Create_Namespace_AffectsNames:
{
    my $option = new Option(
        names => ['o1', 'o2'],
        valueType => Option->ValueTypeOptional,
        namespace => 'ns',
        typeSpec => 's'
    );
    is($option->toSpec(), 'ns-o1|ns-o2:s');
}

clone_DoesTheProperCopy:
{
    my $src = new Option(
        names => ['option_name'],
        valueType => Option->ValueTypeNone,
        hidden => 1,
        defaultValue => 'Editor'
    );

    my $clone = $src->clone();
    is_deeply($clone, $src);
}

getValueChanged:
{
    my $option = Option::fromDescription(['o1|o2:s']);

    is($option->getValueChanged(), 0);

    $option->setValue(1);

    is($option->getValueChanged(), 1);
}

resetValue_ResetsValueToDefault:
{
    my $option = Option::fromDescription(['o1|o2:s', '', { defaultValue => 2 }]);

    $option->setValue(1);
    $option->reset();
    is($option->getValue(), 2);
    is($option->getValueChanged(), 0);
}

done_testing();
