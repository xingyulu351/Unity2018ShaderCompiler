package Logger;

use warnings;
use strict;

use Dirs;
use lib Dirs::external_root();
use Log::Log4perl qw(:easy);
use Carp qw (croak);
use File::Spec::Functions qw (canonpath catfile);
use StringUtils qw (isNullOrEmpty);
use FileUtils;
use Switch;

my $runContext;

my @blacklistedPatterns;

my $failColor = 'red';
my $successColor = 'green';
my $warningColor = 'yellow';

sub initialize
{
    (my $loglevel) = @_;
    _initColorSupport();
    switch (lc($loglevel))
    {
        case 'verbose' { $loglevel = "DEBUG"; }
        case 'minimal' { $loglevel = "INFO"; }
        case 'error' { $loglevel = "ERROR"; }
        case 'none' { $loglevel = "OFF"; }
        else { croak("Unknown log level $loglevel"); }
    }

    my $appender = "Log::Log4perl::Appender::Screen";
    if (not _isStdOutRedirected())
    {
        $appender = "Log::Log4perl::Appender::ScreenColoredLevels";
    }

    my $config = qq (
log4perl.logger = $loglevel, Screen, Error, Warning
log4perl.filter.ErrorOrFatal = sub { my \%p = \@_;  \$p{log4p_level} eq "ERROR" || \$p{log4p_level} eq "FATAL" }
log4perl.filter.IsWarning = sub { my \%p = \@_;  \$p{log4p_level} eq "WARN" }
log4perl.filter.DefaultFilter = sub { my \%p = \@_;  \$p{log4p_level} eq "INFO" || \$p{log4p_level} eq "DEBUG" || \$p{log4p_level} eq "VERBOSE" }

log4perl.appender.Screen = Log::Log4perl::Appender::Screen
log4perl.appender.Screen.stderr  = 0
log4perl.appender.Screen.Filter = DefaultFilter
log4perl.appender.Screen.layout = PatternLayout
log4perl.appender.Screen.layout.ConversionPattern=%m

# Error appender
log4perl.appender.Error = $appender
log4perl.appender.Error.stderr = 1
log4perl.appender.Error.layout = PatternLayout
log4perl.appender.Error.layout.ConversionPattern=error: %m
log4perl.appender.Error.Filter = ErrorOrFatal
log4perl.appender.Error.color.ERROR=$failColor

# Warning appender

log4perl.appender.Warning = $appender
log4perl.appender.Warning.stderr = 0
log4perl.appender.Warning.layout = PatternLayout
log4perl.appender.Warning.layout.ConversionPattern=warning: %m
log4perl.appender.Warning.Filter = IsWarning
log4perl.appender.Warning.color.WARN=$warningColor
);
    Log::Log4perl->init(\$config);

    @blacklistedPatterns = FileUtils::readAllLines(catfile(Dirs::UTR_root(), 'BlacklistedPatterns.txt'));
}

sub setRunContext
{
    ($runContext) = @_;
}

sub minimal
{
    INFO(formatMessage(@_));
}

sub verbose
{
    DEBUG(formatMessage(@_));
}

sub error
{
    my (undef, $message) = _getSourceAndMessage(@_);

    if (grep { $message =~ qr/$_/ } @blacklistedPatterns)
    {
        return;
    }

    ERROR(formatMessage(@_));
}

sub warning
{
    WARN(formatMessage(@_));
}

sub fatal
{
    if (Log::Log4perl->initialized())
    {
        FATAL(formatMessage(@_));
    }
    else
    {
        print("FATAL: @_\n");
    }
}

sub raw
{
    my (@params) = @_;
    my $message = _getMessage(@params);
    print("$message");
}

sub formatMessage
{
    my (@params) = @_;
    my ($src, $message) = _getSourceAndMessage(@params);
    if ($src)
    {
        return "[$src] $message\n";
    }
    return "$message\n";
}

sub _getSourceAndMessage
{
    my (@params) = @_;
    my $src = _getSource(@params);
    my $message = _getMessage(@params);
    return ($src, $message);
}

sub _getSource
{
    my (@params) = @_;
    my $result;
    if (scalar(@params) == 1)
    {
        return $result;
    }

    my %params = @params;
    if (not isNullOrEmpty($params{src}))
    {
        $result = _removeRoot($params{src});
    }
    return $result;
}

sub _cutToFitToTerminal
{
    my ($message) = @_;
    if (_isStdOutRedirected())
    {
        return $message;
    }
    my $width = _getConsoleCharWidth();
    if (length($message) >= $width)
    {
        $message = substr($message, 0, $width);
    }
    return $message;
}

sub _getMessage
{
    my (@params) = @_;
    if (scalar(@params) == 1)
    {
        my ($message) = @_;
        return $message;
    }

    my %params = @params;
    if (not defined($params{msg}))
    {
        return '';
    }
    return $params{msg};
}

sub _removeRoot
{
    my ($file) = @_;
    if (not defined($runContext))
    {
        return $file;
    }
    my $artifactsRoot = $runContext->getArtifactsPath();
    return FileUtils::removeSubPath($file, $artifactsRoot);
}

sub _initColorSupport
{
    if (_isStdOutRedirected())
    {
        $ENV{ANSI_COLORS_DISABLED} = 1;
        return;
    }
    if ($^O eq 'MSWin32')
    {
        my $conEmuAnsi = $ENV{ConEmuANSI};
        if (defined($conEmuAnsi) and ($conEmuAnsi eq 'ON'))
        {
            return;
        }
        my $term = $ENV{TERM};
        if (defined($term) and ($term eq 'xterm'))
        {
            return;
        }
        $ENV{ANSI_COLORS_DISABLED} = 1;
    }
}

sub _isStdOutRedirected
{
    ## no critic (ProhibitInteractiveTes)
    return not(-t STDOUT);
}

1;
