package UtrWrapper;

use warnings;
use strict;

use Carp qw (croak);
use Cwd;
use File::Spec::Functions qw (catfile canonpath);
use IPC::Cmd qw (run);
use Params::Check qw (check);

sub new
{
    my ($package, %args) = @_;
    my $arg_scheme = {
        args => { required => 1, default => [], strict_type => 1 },
        utr_dir => { required => 1 },
        log_file_writer => {}
    };
    my $obj = check($arg_scheme, \%args, 1) or croak('Failed to parse arguments');
    return bless($obj, $package);
}

sub runTests
{
    my ($this) = @_;
    my $test_script = canonpath(catfile($this->{utr_dir}, 'test.pl'));
    my @command = ('perl', $test_script, '--progress=null', @{ $this->{args} });
    my ($success, $error_text, $output) = IPC::Cmd::run(command => \@command);
    my $log_file_writer = $this->{log_file_writer};
    if ($log_file_writer)
    {
        $log_file_writer->writeLog(module => 'utr', contents => $output);
    }

    if ($success)
    {
        return 'good';
    }
    else
    {
        return 'bad';
    }
}

1;
