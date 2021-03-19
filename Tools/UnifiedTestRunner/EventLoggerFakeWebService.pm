package EventLoggerFakeWebService;

use warnings;
use strict;

use Dirs;
use lib Dirs::external_root();
use JSON;
use Logger;

sub new
{
    return bless({}, __PACKAGE__);
}

sub send
{
    my ($this, $req) = @_;
    my $json = JSON->new(utf8 => 1, convblessed => 1);
    my $jsonText = $json->to_json($req);
    Logger::minimal("SessionEvent:$jsonText");
}

1;
