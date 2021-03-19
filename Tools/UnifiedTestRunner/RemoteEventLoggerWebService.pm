package RemoteEventLoggerWebService;

use warnings;
use strict;

use Logger;
use Dirs;

use lib Dirs::external_root();

use LWP::UserAgent;
use JSON;

sub send
{
    my ($uri, $req) = @_;
    my $userAgent = new LWP::UserAgent();
    my $json = JSON->new(utf8 => 1, convblessed => 1);
    my $jsonText = $json->to_json($req);
    Logger::verbose("\nSending post request to $uri. Content:\n--- $jsonText\n\n---\n");

    my $response = $userAgent->post(
        $uri,
        Content_Type => 'application/json',
        Content => $jsonText
    );

    Logger::verbose("\n---Response\n" . $response->as_string() . "\n---\n");
    if ($response->is_success())
    {
        my $contentType = $response->header('Content_Type');
        if ($contentType and $contentType =~ 'application/json')
        {
            my $content = $response->decoded_content();
            return (0, jsonToObj($content));
        }
        return 0;
    }

    Logger::warning("Can not send messages to $uri " . $response->status_line());
    return 1;
}

1;
