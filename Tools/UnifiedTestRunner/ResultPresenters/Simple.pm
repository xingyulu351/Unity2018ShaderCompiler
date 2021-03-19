package ResultPresenters::Simple;
use StringUtils qw (trim);

sub new
{
    return bless({}, __PACKAGE__);
}

sub present
{
    my ($this, $result) = @_;
    return '' if !defined($result->getTest());
    my $text = '';
    $text .= $result->getStateAsString() . ' ';
    $text .= join(' ', $result->getSuite(), $result->getFixture(), $result->getTest(), $result->getTime());
    $text = trim($text);
    my $message = $result->getMessage();
    $text .= "\n" if $message;
    $text .= $message if $message;
    return $text;
}

1;
