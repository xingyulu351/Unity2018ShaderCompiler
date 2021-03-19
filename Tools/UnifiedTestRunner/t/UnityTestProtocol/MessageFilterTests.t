use warnings;
use strict;

use Test::More;

use_ok('UnityTestProtocol::MessageFilter');

Creation:
{
    can_ok('UnityTestProtocol::MessageFilter', 'new');
    my $filter = new UnityTestProtocol::MessageFilter();
    isa_ok($filter, 'UnityTestProtocol::MessageFilter');
}

check_NoRules_ReturnsFalse:
{
    my $filter = new UnityTestProtocol::MessageFilter();
    is($filter->check({ type => 't1' }), 0);
}

check_MessageTypeRule_ReturnsTrueOnlyForMessagesAWithGivenType:
{
    my $rules = [{ messageType => 't1' }];
    is(
        _check(
            rules => $rules,
            message => { type => 't1' }
        ),
        1
    );

    is(
        _check(
            rules => $rules,
            message => { type => 't2' }
        ),
        0
    );
}

check_SeveralMessageTypeRule_ReturnsTrueOnlyForMessagesWithGivenTypes:
{
    my $rules = [{ messageType => 't1' }, { messageType => 't2' }];
    is(
        _check(
            rules => $rules,
            message => { type => 't1' }
        ),
        1
    );

    is(
        _check(
            rules => $rules,
            message => { type => 't2' }
        ),
        1
    );

    is(
        _check(
            rules => $rules,
            message => { type => 't3' }
        ),
        0
    );
}

check_MessageTypeRuleWithFilter_ReturnsTrueOnlyForMatchingMessages:
{
    my $rules = [{ messageType => 't1', filter => '$.Success==0' }];

    is(
        _check(
            rules => $rules,
            message => { type => 't1', Success => 0 }
        ),
        1
    );

    is(
        _check(
            rules => $rules,
            message => { type => 't1', Success => 1 }
        ),
        0
    );
}

check_MessageTypeRuleWithFilterAndConditionStatement_ReturnsTrueOnlyForMatchingMessages:
{
    my $rules = [{ messageType => 't1', filter => '$.State==4 and $.Error' }];

    is(
        _check(
            rules => $rules,
            message => { type => 't1', State => 4, Error => "error_text" }
        ),
        1
    );

    is(
        _check(
            rules => $rules,
            message => { type => 't1', State => 4, Error => undef }
        ),
        0
    );
}

check_TwoRulesWithTheSameMessageTypeButDifferentFilters_ReturnsTrueForBoth:
{
    my $rules = [{ messageType => 't1', filter => '$.State==5' }, { messageType => 't1', filter => '$.State==6' }];

    is(
        _check(
            rules => $rules,
            message => { type => 't1', State => 5 }
        ),
        1
    );

    is(
        _check(
            rules => $rules,
            message => { type => 't1', State => 6 }
        ),
        1
    );
}

done_testing();

sub _check
{
    my %args = @_;
    my $rules = $args{rules};
    my $filter = new UnityTestProtocol::MessageFilter($args{rules});
    return $filter->check($args{message});
}
