package TestHelpers::ArgsGrabber;

use Dirs;
use lib Dirs::external_root();
use Test::MockObject;

sub createMock
{
    my $method = shift();
    my $args = shift();
    my $systemCallMock = Test::MockObject->new();
    $systemCallMock->mock(
        $method => sub
        {
            shift();
            foreach my $arg (@_)
            {
                push(@$args, split(' ', $arg));
            }
            return 0;
        }
    );
    return $systemCallMock;
}

1;
