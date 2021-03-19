package SuiteDependenciesBuilder;

use strict;
use warnings;

use Carp qw (croak);
use Config;
use IPC::Cmd;
use Params::Check qw (check);

sub new
{
    my ($package, %args) = @_;
    my $arg_scheme = {
        os => { required => 1, allow => ['mac', 'win', 'linux'] },
        suite => { required => 1, allow => ['cgbath', 'integration', 'graphics', 'native', 'runtime', 'performance-runtime', 'stress-runtime'] },
        platform => { default => 'editor', allow => ['editor', 'standalone'], },
        log_file_writer => {}
    };
    my $obj = check($arg_scheme, \%args, 1) or croak("Don't know how to build $args{suite} for $args{platform}");
    return bless($obj, $package);
}

sub build
{
    my ($this) = @_;
    my @targets = $this->_selectBuildTargets();
    $ENV{'UNITY_RUN_NATIVE_TESTS_DURING_BUILD'} = 0;
    my $build_params;
    if (scalar(@targets))
    {
        $build_params = '--target=' . join(',', @targets);
    }
    else
    {
        $build_params = '--prepare';
    }
    my ($success, $error_msg, $output) = IPC::Cmd::run(command => ['perl', 'build.pl', $build_params],);
    my $log_file_writer = $this->{log_file_writer};
    if ($log_file_writer)
    {
        $log_file_writer->writeLog(module => 'build', contents => $output);
    }
    return $success;
}

sub _selectBuildTargets
{
    my ($this) = @_;
    my $suite = $this->{suite};
    my $platform = $this->{platform};
    my @targets;
    push(@targets, $this->_thisPlatformEditor());    # For cgbath suite probably should jam directly to build shader compiler only
    if (($suite eq 'graphics') or ($suite eq 'integration') or ($platform eq 'standalone'))
    {
        push(@targets, $this->_thisPlatformStandalone());
    }
    return @targets;
}

sub _thisPlatformEditor
{
    my ($this) = @_;
    if ($this->{'os'} eq 'win')
    {
        return 'WindowsEditor';
    }
    elsif ($this->{'os'} eq 'mac')
    {
        return 'MacEditor';
    }
    elsif ($this->{'os'} eq 'linux')
    {
        return 'LinuxEditor';
    }
    else
    {
        croak("Unsupported platform $this->{'os'}");
    }
}

sub _thisPlatformStandalone
{
    my ($this) = @_;
    if ($this->{'os'} eq 'win')
    {
        return 'WindowsStandaloneSupport';
    }
    elsif ($this->{'os'} eq 'mac')
    {
        return 'MacStandaloneSupport';
    }
    elsif ($this->{'os'} eq 'linux')
    {
        return 'LinuxStandaloneSupport';
    }
    else
    {
        croak("Unsupported platform $this->{'os'}");
    }
}

1;
