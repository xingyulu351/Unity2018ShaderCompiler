use warnings;
use strict;

use Cwd;
use File::Path qw (rmtree);
use File::Spec::Functions qw (catfile);
use IO::File;
use Test::More;

use Dirs;
use lib Dirs::external_root();

BEGIN { use_ok('LogFileWriter') }

CREATION_TEST:
{
    can_ok('LogFileWriter', 'new');
    my $log_file_writer = new LogFileWriter(log_root => '.');
}

WRITE_LOG_FILE:
{
    my $log_file_writer = new LogFileWriter(log_root => '.', log_subdir => '.');
    can_ok($log_file_writer, 'setLogSubdir');
    $log_file_writer->setLogSubdir('test_module_dir');
    can_ok($log_file_writer, 'writeLog');
    $log_file_writer->writeLog(module => 'test_module', contents => ['foo', 'bar']);
    {
        my $logfile = catfile('test_module_dir', 'test_module.log');
        my $written_file = new IO::File($logfile, O_RDONLY);
        local $/;
        is(<$written_file>, 'foobar');
    }
    rmtree('test_module_dir');
}

done_testing();
