package WinTools;

use warnings;
use strict;
use Carp qw(croak carp);
use File::Basename qw(dirname basename );
use File::Spec;
use File::chdir;
use Tools qw ( DecryptFile );
use pdb_srcsrv qw ( pdb_srcsrv );

require Exporter;
our @ISA = qw(Exporter);
our @EXPORT_OK = qw( SignFile SignClickOnceFile AddSourceServerToPDB );

my $root;

my $signpath;
my $pfx;
my $encyptedcertpasswordfile;

BEGIN
{
    $root = File::Spec->rel2abs(dirname(__FILE__)) . '/../..';

    $signpath = "$root/Tools/Certificates";
    my $certspath = Tools::GetCertificateFilesPath();
    $pfx = "$certspath/unitycert_microsoft.pfx";
    $encyptedcertpasswordfile = "$certspath/unitycert_microsoft.pwd_encrypted";
}

sub SignFile
{
    my ($file) = @_;

    if ($ENV{UNITY_DO_NOT_SIGN_EXECUTABLES})
    {
        return;
    }

    if (!-e $file)
    {
        croak("SignFile: File '$file' not found");
    }

    my $key = $ENV{UNITY_KEY};
    if (!defined($key))
    {
        warn "Skipping code signing '$file' as no encryption key was found\n";

        if ($ENV{'UNITY_THISISABUILDMACHINE'} && $ENV{'UNITY_THISISABUILDMACHINE'} eq 1)
        {
            die "On a build machine code signing must be performed";
        }

        return;
    }

    my $unitycertpassword = Tools::DecryptAndReadFile($encyptedcertpasswordfile);
    DecryptFile("${pfx}_encrypted", $pfx);

    local $CWD = $signpath;
    print "Attempting to sign: " . $file . "\n";

    if (system("signtool.exe", "sign", "/du", "http://unity3d.com", "/f", $pfx, "/p", $unitycertpassword, "\"$file\"") eq 1)
    {
        unlink $pfx;
        croak("Failed signing $file");
    }

    my @timestampServers = (
        "http://timestamp.verisign.com/scripts/timstamp.dll", "http://timestamp.globalsign.com/scripts/timstamp.dll",
        "http://timestamp.comodoca.com/authenticode", "http://tsa.starfieldtech.com",
        "http://www.trustcenter.de/codesigning/timestamp"
    );

    my $success = 0;
    for (my $i = 0; $i < scalar(@timestampServers) and !$success; ++$i)
    {
        print("====================================\n");

        my $result = system("signtool.exe", "timestamp", "/t", $timestampServers[$i], "\"$file\"");
        $success = $result eq 0;

        print("Server: $timestampServers[$i] (result = $result)\n");
    }

    unlink $pfx;

    if (!$success)
    {
        croak("Failed timestamping $file");
    }
}

sub SignClickOnceFile
{
    my ($mage, $file) = @_;

    my $key = $ENV{UNITY_KEY};
    if (!defined($key))
    {
        warn "Skipping code signing as no encryption key was found\n";

        if ($ENV{'UNITY_THISISABUILDMACHINE'} && $ENV{'UNITY_THISISABUILDMACHINE'} eq 1)
        {
            die "On a build machine code signing must be performed";
        }

        return;
    }

    my $unitycertpassword = Tools::DecryptAndReadFile($encyptedcertpasswordfile);
    DecryptFile("${pfx}_encrypted", $pfx);

    local $CWD = $signpath;
    print "Attempting to sign ClickOnce file: " . $file . "\n";

    my @timestampServers = (
        "http://timestamp.verisign.com/scripts/timstamp.dll", "http://timestamp.globalsign.com/scripts/timstamp.dll",
        "http://timestamp.comodoca.com/authenticode", "http://tsa.starfieldtech.com",
        "http://www.trustcenter.de/codesigning/timestamp"
    );

    my $success = 0;
    for (my $i = 0; $i < scalar(@timestampServers) and !$success; ++$i)
    {
        my $result = system($mage, "-Sign", "\"$file\"", "-CertFile", $pfx, "-Password", $unitycertpassword, "-timestampUri", $timestampServers[$i]);

        $success = $result eq 0;

        print("Mage.exe signing: $timestampServers[$i] (result = $result)\n");
    }

    unlink $pfx;

    if (!$success)
    {
        croak("Failed timestamping $file");
    }
}

# extractArtifactOnWindows wrapper function:
# it accepts a relative path, an action and an array of options you want the sevenzip to perform as an argument.
# File path should the relative the to the source root.
# Valid actions:
#   1. content_listing - List the content of the archive
#                    and return that text as a string
#   2. extract - Extract the content of the archive
#
# Valid options:
#   1. silent - extract silently
#   2. yes - answer yes to all prompts
sub extractArtifactOnWindows
{
    my ($relativeFilePath, $action, $relativeOutputPath, $options) = @_;

    my $outputFullPath = File::Spec->rel2abs($relativeOutputPath);
    my $fullPath = File::Spec->rel2abs($relativeFilePath);
    my $sevenZipExecutable = File::Spec->rel2abs("External\\7z\\win32\\7za.exe");

    my $fileList;
    my %optionsHash = ('silent' => ' > NUL:', 'yes' => ' -y');

    if ($action eq "content_listing")
    {
        $fileList = `$sevenZipExecutable l $relativeFilePath`;
        return $fileList;
    }

    my $cmd = "$sevenZipExecutable x $fullPath -o$outputFullPath";

    if (grep { $_ eq 'yes' } @{$options})
    {
        $cmd = $cmd . $optionsHash{'yes'};
    }

    if (grep { $_ eq 'silent' } @{$options})
    {
        $cmd = $cmd . $optionsHash{'silent'};
    }

    system($cmd) and die("Unable to extract $fullPath to $outputFullPath");
}

sub AddSourceServerToPDB
{
    my ($root) = shift;
    my ($pdblist) = shift;

    if (scalar @$pdblist)
    {
        # pdb_srcsrv.pl assumes it's at root
        local $CWD = $root;
        pdb_srcsrv(@$pdblist);
    }
}
