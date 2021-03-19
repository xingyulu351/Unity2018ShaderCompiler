use warnings;
use strict;

use Dirs;
use lib Dirs::external_root();
use Test::Tester;
use TestHelpers::ArrayAssertions qw (arrayStartsWith arrayContains arrayDoesNotContain arrayEndsWith isEmpty arraysAreEquivalent arraysAreEqual);
use Test::More;
use Test::Deep;

my $Success = { actual_ok => 1, diag => '' };
my $Fail = { actual_ok => 0 };

arraysAreEqual_IdenticalArrays_Passes:
{
    check_test(
        sub
        {
            arraysAreEqual([], []);
        },
        $Success
    );

    check_test(
        sub
        {
            arraysAreEqual([1], [1]);
        },
        $Success
    );

    check_test(
        sub
        {
            arraysAreEqual([1, 2, 3], [1, 2, 3]);
        },
        $Success
    );

    check_test(
        sub
        {
            arraysAreEqual(["some string"], [re('some')]);
        },
        $Success
    );
}

arraysAreEqual_DifferentArrays_Fails:
{
    check_test(
        sub
        {
            arraysAreEqual([0], []);
        },
        $Fail
    );

    check_test(
        sub
        {
            arraysAreEqual([0], [1]);
        },
        $Fail
    );

    check_test(
        sub
        {
            arraysAreEqual([1], [0], 'Test name must be there');
        },
        { name => 'Test name must be there' }
    );
}

arrayStartsWith_MatchesStartingElements_Passes:
{
    check_test(
        sub
        {
            arrayStartsWith([1], [1]);
        },
        $Success
    );
    check_test(
        sub
        {
            arrayStartsWith([1, 2], [1]);
        },
        $Success
    );
    check_test(
        sub
        {
            arrayStartsWith([1], 1);
        },
        $Success
    );
    check_test(
        sub
        {
            arrayStartsWith(["hello"], re(qr/hello/));
        },
        $Success
    );
    check_test(
        sub
        {
            arrayStartsWith(['aa', 'bb', 'cc'], ['aa', 'bb']);
        },
        $Success
    );
}

arrayStartsWith_NotMatchesStartingElements_Fails:
{
    check_test(
        sub
        {
            arrayStartsWith([1, 2], [3, 3]);
        },
        $Fail
    );
    check_test(
        sub
        {
            arrayStartsWith([1], [1, 2]);
        },
        $Fail
    );
    check_test(
        sub
        {
            arrayStartsWith([1, 2], [1, 2, 3]);
        },
        $Fail
    );

    check_test(
        sub
        {
            arrayStartsWith([1], [0], 'Test name must be there');
        },
        { name => 'Test name must be there' }
    );
}

arrayContains_AllElementsArePresent_Passes:
{
    check_test(
        sub
        {
            arrayContains([1], [1]);
        },
        $Success
    );

    check_test(
        sub
        {
            arrayContains([1, 2], [1, 2]);
        },
        $Success
    );

    check_test(
        sub
        {
            arrayContains([1, 2, 3], [2, 1]);
        },
        $Success
    );

    check_test(
        sub
        {
            arrayContains([], []);
        },
        $Success
    );

    check_test(
        sub
        {
            arrayContains(['aa', 'bb', 'cc'], ['bb']);
        },
        $Success
    );

    check_test(
        sub
        {
            arrayContains(['text1', 'text2', 'text3'], [re(qr/^t.*1/), re(qr/^t.*2/)]);
        },
        $Success
    );
}

arrayContains_SomeElementsAreMissing_Fails:
{
    check_test(
        sub
        {
            arrayContains([1], [2]);
        },
        $Fail
    );

    check_test(
        sub
        {
            arrayContains([1, 2], [1, 2, 3]);
        },
        $Fail
    );

    check_test(
        sub
        {
            arrayContains([1], [0], 'Test name must be there');
        },
        { name => 'Test name must be there' }
    );
}

arrayDoesNotContain_NoMatchingElements_Passes:
{
    check_test(
        sub
        {
            arrayDoesNotContain([1], [0]);
        },
        $Success
    );

    check_test(
        sub
        {
            arrayDoesNotContain([1, 2], [3, 4]);
        },
        $Success
    );

    check_test(
        sub
        {
            arrayDoesNotContain([1, 2, 3], [0, 4]);
        },
        $Success
    );

    check_test(
        sub
        {
            arrayDoesNotContain([], []);
        },
        $Success
    );

    check_test(
        sub
        {
            arrayDoesNotContain(['text1', 'text2', 'text3'], [re(qr/^t.*5/), re(qr/^t.*6/)]);
        },
        $Success
    );
}

arrayDoesNotContain_Fails:
{
    check_test(
        sub
        {
            arrayDoesNotContain([1], [1]);
        },
        $Fail
    );

    check_test(
        sub
        {
            arrayDoesNotContain([1, 2], [2, 1]);
        },
        $Fail
    );

    check_test(
        sub
        {
            arrayDoesNotContain([1], [1], 'Test name must be there');
        },
        { name => 'Test name must be there' }
    );
}

arraysAreEquivalent_EquivalenttArrays_Passes:
{
    check_test(
        sub
        {
            arraysAreEquivalent([], []);
        },
        $Success
    );

    check_test(
        sub
        {
            arraysAreEquivalent([1], [1]);
        },
        $Success
    );

    check_test(
        sub
        {
            arraysAreEquivalent([1, 2, 3], [3, 1, 2]);
        },
        $Success
    );

    check_test(
        sub
        {
            arraysAreEquivalent(["some string"], [re('some')]);
        },
        $Success
    );
}

arraysAreEquivalent_DifferentArrays_Fails:
{
    check_test(
        sub
        {
            arraysAreEquivalent([0], []);
        },
        $Fail
    );

    check_test(
        sub
        {
            arraysAreEquivalent([0], [1]);
        },
        $Fail
    );

    check_test(
        sub
        {
            arraysAreEquivalent([1, 2, 3], [1, 0, 3]);
        },
        $Fail
    );

    check_test(
        sub
        {
            arraysAreEquivalent([1], [0], 'Test name must be there');
        },
        { name => 'Test name must be there' }
    );
}

arrayEndsWith_MatchesEndingElements_Passes:
{
    check_test(
        sub
        {
            arrayEndsWith([1], [1]);
        },
        $Success
    );

    check_test(
        sub
        {
            arrayEndsWith([1, 2], [1, 2]);
        },
        $Success
    );

    check_test(
        sub
        {
            arrayEndsWith([1, 2], [2]);
        },
        $Success
    );

    check_test(
        sub
        {
            arrayEndsWith([1, 2, 3], [2, 3]);
        },
        $Success
    );

    check_test(
        sub
        {
            arrayEndsWith(["ab"], [re(qr/ab/)]);
        },
        $Success
    );
}

arrayEndsWith_NotMatchesEndingElements_Passes:
{
    check_test(
        sub
        {
            arrayEndsWith([1], [0]);
        },
        $Fail
    );

    check_test(
        sub
        {
            arrayEndsWith([1, 2], [2, 1]);

        },
        $Fail
    );

    check_test(
        sub
        {
            arrayEndsWith([1, 2], [1]);
        },
        $Fail
    );

    check_test(
        sub
        {
            arrayEndsWith([1, 2, 3], [3, 3]);

        },
        $Fail
    );

    check_test(
        sub
        {
            arrayEndsWith([1], [0], 'Test name must be there');
        },
        { name => 'Test name must be there' }
    );
}

arrayStartsWith_DoesNotModifyInputArrays:
{
    my (@a1) = (3, 2, 1);
    my (@a1_expected) = @a1;

    my (@a2) = (3, 2);
    my (@a2_expected) = @a2;

    arrayStartsWith(\@a1, \@a2);

    ok(eq_array(\@a1, \@a1_expected));
    ok(eq_array(\@a2, \@a2_expected));
}

arrayEndsWith_DoesNotModifyInputArrays:
{
    my (@a1) = (3, 2, 1);
    my (@a1_expected) = @a1;

    my (@a2) = (2, 1);
    my (@a2_expected) = @a2;

    arrayEndsWith(\@a1, \@a2);

    ok(eq_array(\@a1, \@a1_expected));
    ok(eq_array(\@a2, \@a2_expected));
}

arrayContains_DoesNotModifyInputArrays:
{
    my (@a1) = (3, 2, 1);
    my (@a1_expected) = @a1;

    my (@a2) = (1, 3);
    my (@a2_expected) = @a2;

    arrayContains(\@a1, \@a2);

    ok(eq_array(\@a1, \@a1_expected));
    ok(eq_array(\@a2, \@a2_expected));
}

isEmpty:
{
    check_test(sub { isEmpty([1]) }, $Fail);

    check_test(sub { isEmpty([1], 'Test name must be there') }, { name => 'Test name must be there' });

    check_test(sub { isEmpty([]) }, $Success);
}

done_testing();
