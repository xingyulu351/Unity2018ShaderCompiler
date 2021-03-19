use warnings;
use strict;

use FindBin qw ($Bin);
use lib $Bin;

use Getopt::Long;
use SubmitTestDataRequestSender;

use Dirs;
use lib Dirs::external_root();
use IO::Handle;

sub main ()
{
    my ($hoarderUri, $format, $headerFile, $logFile, @dataFiles) = _readOptions(@ARGV);
    if ($logFile)
    {
        redirectOutputToLog($logFile);
    }
    my $res = open(OUTPUT, '>', $logFile);
    if (not defined($res))
    {
        die("failed to open $logFile\n");
    }
    my $jsonHeader = _readData($headerFile);
    SubmitTestDataRequestSender::send(
        uri => $hoarderUri . "?format=$format",
        header => $jsonHeader,
        files => [@dataFiles],
        retries => 1,
        timeout => 20,    # sec
        logger => sub { print(@_); }
    );

    exit(0);
}

sub _readData
{
    my ($file) = @_;
    my $openRes = open(my $fh, '<', $file);

    if (not defined($openRes))
    {
        die("\ncan not open file $file: $!\n");
    }

    my $header = <$fh>;
    close($fh);
    return $header;
}

sub _readOptions
{
    my ($hoarderUri, $format, $headerFile, $logFile, @dataFiles);

    my $resGetOpt = GetOptions(
        "hoarder-uri=s" => \$hoarderUri,
        "format=s" => \$format,
        "header-file=s" => \$headerFile,
        "log-file:s" => \$logFile,
        "data-file:s@" => \@dataFiles,
    );

    if (not $resGetOpt)
    {
        die("$!");
    }

    return ($hoarderUri, $format, $headerFile, $logFile, @dataFiles);
}

sub redirectOutputToLog
{
    my ($log) = @_;
    open(OUTPUT, '>', $log) or die("failed top open $log\n");
    STDOUT->fdopen(\*OUTPUT, 'w') or die($!);
    STDERR->fdopen(\*OUTPUT, 'w') or die($!);
}

main();
