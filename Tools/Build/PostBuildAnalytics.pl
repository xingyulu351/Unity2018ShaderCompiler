#!/usr/bin/perl

use strict;
use warnings;
use English;

use File::Spec;
use File::Basename qw(dirname basename fileparse);

my $top;

BEGIN
{
    $top = File::Spec->rel2abs("../../", dirname($0));
}

use lib ("$top/External/Perl/lib");
use File::Slurp qw(read_file);
use HTTP::Request::Common;
use JSON;
require LWP::UserAgent;

# Helper function for writing a hash to a json formatted file.
sub WriteJsonFile
{
    my ($dataref, $filepath) = @_;
    my %data = %{$dataref};
    my $json = JSON->new->allow_nonref;
    my $jsondata = $json->pretty->encode(\%data);
    my $handle = IO::File->new($filepath, "w+");
    $handle->print($jsondata);
    close($handle);
}

# Read json file with the analytics data
if (!-e $ARGV[0])
{
    exit(0);    # It's on purpose that we exit silently here. If there is no data just exit.
}
my $data = read_file($ARGV[0]);
my %build = %{ decode_json($data) };

# Patch analytics data
my %hg;
if (-d File::Spec->join($top, ".hg"))
{
    if (`hg parents --template "{node|short} {branch}\n"` =~ /(\S*) (\S*)\n/g)
    {
        $hg{currentRevision} = $1;
        $hg{currentBranch} = $2;
    }

    if (`hg log -r "max(::. & branch('trunk'))" --template "{node|short}\n"` =~ /(\S*)\n/g)
    {
        $hg{trunkRevision} = $1;
    }

    WriteJsonFile(\%hg, File::Spec->join($top, "build/log/hg_analytics.log"));
}

# Create the http sneder and set options
my $ua = LWP::UserAgent->new;
if (defined $ENV{'https_proxy'})
{
    $ua->proxy("https" => $ENV{'https_proxy'});
}
$ua->timeout(3);

# Create HTTP request for sending to ElasticSearch service
my %content = (%build, %hg);
my $request = HTTP::Request->new(POST => "https://listener.logz.io:8071/?token=uHnRfIgWUDevAZRYaEVqbsZrWiNONxgY&type=buildsystem");
$request->content_type("application/json");
$request->content(encode_json(\%content));

# Send the request
$ua->request($request);    # Don't care if the request succeeds or not
