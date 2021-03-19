package ArtifactsMapBuilder;

use warnings;
use strict;

use Carp qw (croak);

use ArtifactsMap;
use ArtifactsMapEntry;
use FileUtils qw (openForReadOrCroak closeOrCroak);
use File::Spec::Functions qw (canonpath);
use UnityTestProtocol::Utils qw(:all);

sub fromIntegrationSuiteLog
{
    my ($filename, $artifactsRoot) = @_;
    my $fh = openForReadOrCroak($filename);
    my $result = _fromIntegrationSuiteLog($fh, $artifactsRoot);
    closeOrCroak($fh, $filename);
    return $result;
}

sub fromRuntimeSuiteLog
{
    return fromNativeSuiteLog(@_);
}

sub fromNativeSuiteLog
{
    my ($filename, $artifactsRoot) = @_;
    my $fh = openForReadOrCroak($filename);
    my $result = _fromNativeSuiteLog($fh, $artifactsRoot);
    closeOrCroak($fh, $filename);
    return $result;
}

sub _fromIntegrationSuiteLog
{
    my ($fh, $artifactsRoot) = @_;
    my $result = new ArtifactsMap();
    my $lastEntry;
    my $foundSummary = 0;
    while (<$fh>)
    {
        my $line = $_;
        chomp($line);

        #TODO: fragile, fix it when nunit callbacks are introduced
        if ($line =~ m/^Test Run Summary/s)
        {
            last;
        }

        my $msg = UnityTestProtocol::Utils::getMessage($line);
        if (defined($msg) and isTestBegin($msg))
        {
            $lastEntry = new ArtifactsMapEntry($msg->{name});
            $result->addTest($lastEntry);
        }
        my ($publishedName) = ($line =~ m/^##teamcity\[message.*text=\'Published:\s(.*?)'/s);
        if ($publishedName)
        {
            my $artifact = FileUtils::removeSubPath($publishedName, $artifactsRoot);
            if ($lastEntry)
            {
                $lastEntry->addArtifact($artifact);
            }
            else
            {
                $result->addArtifact($artifact);
            }
        }
    }
    return $result;
}

sub _fromNativeSuiteLog
{
    my ($fh, $artifactsRoot) = @_;
    my $result = new ArtifactsMap();
    my $lastEntry;
    my $foundSummary = 0;
    while (<$fh>)
    {
        my $line = $_;
        chomp($line);
        my ($publishedName) = ($line =~ m/^##teamcity\[message.*text=\'Published:\s(.*?)'/s);
        if ($publishedName)
        {
            $result->addArtifact($publishedName);
        }
    }
    return $result;
}

1;
