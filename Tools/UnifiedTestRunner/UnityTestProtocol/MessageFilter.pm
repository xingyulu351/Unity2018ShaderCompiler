package UnityTestProtocol::MessageFilter;

use warnings;
use strict;

use UnityTestProtocol::FilterExpressionBuilder;

sub new
{
    my ($pkg, $rules) = @_;
    my $instance = { rules => $rules };

    return bless($instance, __PACKAGE__);
}

sub check
{
    my ($this, $msg) = @_;
    foreach my $r (@{ $this->{rules} })
    {
        if ($r->{messageType} eq $msg->{type})
        {
            if ($r->{filter})
            {
                if (_checkUsingFilter($r->{filter}, $msg))
                {
                    return 1;
                }
                else
                {
                    next;
                }
            }
            return 1;
        }
    }
    return 0;
}

sub _checkUsingFilter
{
    my ($filter, $msg) = @_;
    my $expression = UnityTestProtocol::FilterExpressionBuilder::convert($filter, 'msg');
    my $result = eval $expression;
    if ($result)
    {
        return 1;
    }
    return 0;
}

1;
