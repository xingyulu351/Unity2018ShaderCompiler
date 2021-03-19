package GfxTestsReferenceImagesDownloader;

use warnings;
use strict;
use LWP::UserAgent;
use Tools qw(extractArtifact);
use File::Path;
use Carp;
use Repositories qw (ReposFetch ReposApply ReposPin);

sub new
{
    my ($class, $url, $graphicsReferenceImagesFolder, $logOutputPath, $logFilename, $dontFetchGraphicsRepo) = @_;

    my $httpClient = _InitializeHttpClient();

    my $regex = qr/\/d\/([0-9a-fA-F]+)\/(\d+)/;
    my @urlArguments = $url =~ $regex;

    my $self = {
        httpClient => $httpClient,
        url => $url,
        selectionRevision => $urlArguments[0],
        selectionId => $urlArguments[1],
        graphicsReferenceImagesFolder => $graphicsReferenceImagesFolder,
        logOutputPath => $logOutputPath,
        logFilename => $logFilename,
        dontFetchGraphicsRepo => $dontFetchGraphicsRepo
    };

    return bless($self, $class);
}

sub DownloadAndExtractReferenceImages
{
    my ($self) = @_;
    my $targetFile = "gfxComparisonToolImages.zip";

    my $logFh = $self->_OpenLogFile();

    if (!$self->{dontFetchGraphicsRepo})
    {
        $self->_UpdateRepository($logFh, $self->{graphicsReferenceImagesFolder}, $self->{selectionRevision});
    }
    else
    {
        _Log($logFh, "Skipping repository update and fetch\n");
    }

    $self->_Download($logFh, $self->{url}, $targetFile);
    $self->_Extract($logFh, $targetFile, $self->{graphicsReferenceImagesFolder});
    my $out = $self->_AddToRepository($logFh, $self->{graphicsReferenceImagesFolder});

    close($logFh);

    return $out;
}

sub NewGraphicsBranchName
{
    my ($self) = @_;

    my $currentBranch = $self->_GetRepositoryBranch();
    my $graphicsBranch = $self->_GetRepositoryBranch($self->{graphicsReferenceImagesFolder});

    if ($currentBranch eq $graphicsBranch)
    {
        return "";
    }
    else
    {
        return $currentBranch;
    }
}

sub _GetRepositoryBranch
{
    my ($self, $path) = @_;

    my $cmd = "hg branch";
    if ($path)
    {
        $cmd = "hg --cwd $path branch";
    }

    my ($status, $output) = _ExecuteCommand($cmd);
    if ($status ne 0)
    {
        croak("Unable to determine current branch name");
    }

    return $output;
}

sub _Download
{
    my ($self, $logFh, $url, $targetFile) = @_;

    _Log($logFh, "Downloading " . $self->{url});
    my @extraOptions = (':content_file' => $targetFile);
    my $response = $self->{httpClient}->get($url, @extraOptions);
    if (!$response->is_success)
    {
        _Log($logFh, "Failed to download from '$url'. - " . $response->status_line);
        croak("Failed to download from '$url'. - " . $response->status_line . ".\n");
    }

    _Log($logFh, "File downloaded: " . $response->status_line);
    return;
}

sub _Extract
{
    my ($self, $logFh, $targetFile, $destinationFolder) = @_;

    _Log($logFh, "Extracting artifacts to $destinationFolder");
    my $output = extractArtifact($targetFile, $destinationFolder, 1);
    unlink($targetFile);

    _Log($logFh, $output);
    return;
}

sub _UpdateRepository
{
    my ($self, $logFh, $repositoryFolder, $revision) = @_;

    my $cmd = "hg id -r $revision";
    _Log($logFh, "$cmd\n", 1);
    if (system($cmd) ne 0)
    {
        $cmd = "hg pull -r $revision";
        _Log($logFh, "$cmd\n", 1);
        if (system($cmd) ne 0)
        {
            _Log($logFh, "Failed to pull revision $revision");
            croak("Failed to pull revision $revision");
        }
    }

    $cmd = "hg up -cr $revision";
    _Log($logFh, "$cmd\n", 1);
    my ($status, $output) = _ExecuteCommand("$cmd");
    if ($status ne 0)
    {
        _Log($logFh, $output, 1);
        _Log($logFh, "Failed to checkout revision $revision");
        croak("Failed to update repository to revision $revision");
    }

    _Log($logFh, "Updating $repositoryFolder to pinned revision");
    ReposFetch([$repositoryFolder]);
    return;
}

sub _AddToRepository
{
    my ($self, $logFh, $repositoryFolder) = @_;

    _Log($logFh, "Adding files to repository");
    ReposApply('hg add "glob:TemplateImages/**.png"', [$repositoryFolder]);
    my ($status, $output) = _ExecuteCommand("hg -R $repositoryFolder status");
    chomp($output);

    _Log($logFh, "Mercurial status:\n$output");

    return $output;
}

sub _InitializeHttpClient
{
    my $httpClient = LWP::UserAgent->new;
    $httpClient->agent('Unity GraphicsTests Comparison Tool Downloader/0.01');
    return $httpClient;
}

sub _OpenLogFile
{
    my ($self) = @_;

    my $logFilepath = "$self->{logOutputPath}/$self->{logFilename}";

    if (!-d $self->{logOutputPath})
    {
        mkpath($self->{logOutputPath})
            or croak("Unable to create directory $self->{logOutputPath}");
    }

    open(my $logFh, ">>", $logFilepath)
        or croak("Unable to open log file '$logFilepath'");

    return $logFh;
}

sub _Log
{
    my ($logFh, $text, $useStdout) = @_;
    my $timestamp = _GetLoggingTime();

    if ($useStdout)
    {
        print "$text\n";
    }

    print $logFh "$timestamp: $text\n";
    return;
}

sub _ExecuteCommand
{
    my $command = join ' ', @_;
    my ($output, $status) = ($_ = qx($command 2>&1), $? >> 8);
    return ($status, $output);
}

sub _GetLoggingTime
{
    my ($sec, $min, $hour, $mday, $mon, $year, $wday, $yday, $isdst) = localtime(time);
    my $niceTimestamp = sprintf("%04d%02d%02d %02d:%02d:%02d", $year + 1900, $mon + 1, $mday, $hour, $min, $sec);
    return $niceTimestamp;
}

1;
