package LogFileWriter;

use warnings;
use strict;

use Carp qw (croak);
use File::Path qw (mkpath);
use File::Spec::Functions qw (catdir catfile);
use Params::Check qw (check);
use IO::File;

sub new
{
    my ($package, %args) = @_;
    my $arg_scheme = {
        log_root => { required => 1 },
        log_subdir => { default => "" }
    };
    my $obj = check($arg_scheme, \%args, 1) or croak('Failed to parse arguments');
    return bless($obj, $package);
}

sub setLogSubdir
{
    my ($this, $log_subdir) = @_;
    $this->{log_subdir} = $log_subdir;
    my $log_dir = $this->logDir();
    mkpath($log_dir) if defined($log_dir);
}

sub writeLog
{
    my ($this, %args) = @_;
    my $arg_scheme = {
        module => { required => 1 },
        contents => { default => [], required => 1, strict_type => 1 }
    };
    my $args = check($arg_scheme, \%args, 1) or croak('Failed to parse arguments');
    my $log_dir = $this->logDir();
    if (defined($log_dir))
    {
        my $log_file_path = catfile($log_dir, $args->{module} . '.log');
        my $logfile = new IO::File($log_file_path, O_WRONLY | O_APPEND | O_CREAT);
        print($logfile @{ $args->{contents} });
    }
}

sub logDir
{
    my ($this) = @_;
    if (defined($this->{log_root}) and defined($this->{log_subdir}))
    {
        return catdir($this->{log_root}, $this->{log_subdir});
    }
    return undef;
}

1;
