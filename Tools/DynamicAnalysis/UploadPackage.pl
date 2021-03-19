use Getopt::Long;
use LWP::UserAgent;
use HTTP::Request::Common;
use Carp (croak);
use warnings;
use strict;

sub main
{
    my ($packageUri, $webServiceUri) = readOptions();
    upload($packageUri, $webServiceUri);
}

sub upload
{
    my ($packageUri, $webServiceUri) = @_;
    my $userAgent = LWP::UserAgent->new();
    my $reqUrl = "$webServiceUri";
    my $response = $userAgent->post($reqUrl, Content_Type => 'multipart/form-data', Content => [file_0 => ["$packageUri"]]);
    if ($response->is_error)
    {
        croak($response->error_as_HTML);
    }
}

sub readOptions
{
    my $packageUri;
    my $webServiceUri;
    my $resGetOpt = GetOptions(
        "package-uri:s" => \$packageUri,
        "webservice-uri:s" => \$webServiceUri
    );

    if (not $resGetOpt)
    {
        croak("$!");
    }

    return ($packageUri, $webServiceUri);
}

main();
