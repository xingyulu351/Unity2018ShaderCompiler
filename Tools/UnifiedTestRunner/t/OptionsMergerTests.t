use warnings;
use strict;

use Test::More;
use Option;

use Dirs;
use lib Dirs::external_root();

use TestHelpers::ArrayAssertions qw (isEmpty);

BEGIN { use_ok('OptionsMerger'); }

Merge_2EmptySets_ReturnsEmptyResult:
{
    my $result = OptionsMerger::merge([], []);

    isEmpty($result);
}

Merge_FirstSetIsNotDefined_ReturnsSecond:
{
    my $o1 = new Option(
        names => ['o1'],
        valueType => Option->ValueTypeNone
    );

    my $result = OptionsMerger::merge(undef, [$o1]);

    is_deeply($result, [$o1]);
}

Merge_SecondSetIsNotDefined_ReturnsFirst:
{
    my $o1 = new Option(
        names => ['o1'],
        valueType => Option->ValueTypeNone
    );

    my $result = OptionsMerger::merge([$o1], undef);

    is_deeply($result, [$o1]);
}

Merge_BothSetsAreNotDefined_ReturnsEmptyArray:
{
    my $o1 = new Option(
        names => ['o1'],
        valueType => Option->ValueTypeNone
    );

    my $result = OptionsMerger::merge(undef, undef);

    is_deeply($result, []);
}

Merge_FirstSetIsNotDefined_ReturnsSecond:
{
    my $o1 = new Option(
        names => ['o1'],
        valueType => Option->ValueTypeNone
    );

    my $result = OptionsMerger::merge([], undef);

    isEmpty($result);
}

Merge_2SetsNoIntersections_ReturnsUnion:
{
    my $o1 = new Option(
        names => ['o1'],
        valueType => Option->ValueTypeNone
    );

    my $o2 = new Option(
        names => ['o2'],
        valueType => Option->ValueTypeNone
    );

    my $result = OptionsMerger::merge([$o1], [$o2]);

    is_deeply($result, [$o1, $o2]);
}

Merge_2SetsWithIntersection_FirstSetHasAnAdvantage:
{
    my $o1Set1 = new Option(
        names => ['o1'],
        valueType => Option->ValueTypeNone
    );

    my $o2Set1 = new Option(
        names => ['o2'],
        valueType => Option->ValueTypeNone
    );

    my $o1Set2 = new Option(
        names => ['o1'],
        valueType => Option->ValueTypeRequired
    );

    my $result = OptionsMerger::merge([$o1Set1, $o2Set1], [$o1Set2]);

    is_deeply($result, [$o1Set1, $o2Set1]);
}

Merge_2SetsIntersectionWithAliasedOptions_FirstUnion:
{
    my $o1Set1 = new Option(
        names => ['o1', 'o2'],
        valueType => Option->ValueTypeNone
    );

    my $o2Set1 = new Option(
        names => ['o2'],
        valueType => Option->ValueTypeNone
    );

    my $o1Set2 = new Option(
        names => ['o1'],
        valueType => Option->ValueTypeRequired
    );

    my $result = OptionsMerger::merge([$o1Set1, $o2Set1], [$o1Set2]);

    is_deeply($result, [$o1Set1, $o2Set1]);
}

Merge_PrototypeOptionIsOverriden_IfDefinedInDestSet:
{
    my $o1 = new Option(
        names => ['o1'],
        valueType => Option->ValueTypeNone,
        isPrototype => 1
    );

    my $o11 = new Option(
        names => ['o1'],
        valueType => Option->ValueTypeNone,
        description => 'o1 option sescription'
    );

    my $result = OptionsMerger::merge([$o1], [$o11]);
    is_deeply($result, [$o11]);
}

Exclude_2Sets_FirstUnion:
{
    my $o1Set1 = new Option(
        names => ['o1'],
        valueType => Option->ValueTypeNone
    );

    my $o2Set1 = new Option(
        names => ['o2'],
        valueType => Option->ValueTypeNone
    );

    my $o3Set1 = new Option(
        names => ['o3'],
        valueType => Option->ValueTypeNone
    );

    my $o1Set2 = new Option(
        names => ['o1'],
        valueType => Option->ValueTypeRequired
    );

    my $result = OptionsMerger::exclude([$o1Set1, $o2Set1, $o3Set1], [$o1Set2]);

    is_deeply($result, [$o2Set1, $o3Set1]);
}

done_testing();
