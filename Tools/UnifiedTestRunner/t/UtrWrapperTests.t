use Dirs;
use lib Dirs::external_root();

use warnings;
use strict;

use Cwd;
use Test::MockModule;
use Test::More;
use File::Spec::Functions qw (canonpath rel2abs catfile);

BEGIN { use_ok('UtrWrapper'); }

CREATION_TEST:
{
    can_ok('UtrWrapper', 'new');
    my $utr_wrapper = new UtrWrapper(utr_dir => '.', args => []);
    isa_ok($utr_wrapper, 'UtrWrapper');
}

RUN_TEST:
{
    my $ipc_cmd = new Test::MockModule('IPC::Cmd');
    my (@called_commands, @working_dirs);
    $ipc_cmd->mock(
        run => sub
        {
            my %args = @_;
            push(@called_commands, join(' ', @{ $args{command} }));
            push(@working_dirs, canonpath(cwd()));
            $? = 0;
            return (1, undef, [""]);
        }
    );

    my $utr_wrapper = new UtrWrapper(utr_dir => '.', args => ['--suite=native', '--platform=editor']);
    can_ok($utr_wrapper, 'runTests');
    is($utr_wrapper->runTests(), 'good');
    is(@called_commands, 1);
    is($called_commands[0], 'perl test.pl --progress=null --suite=native --platform=editor');
    is(@working_dirs, 1);
    is($working_dirs[0], rel2abs('.'));
}

RUN_TEST_OUTSIDE_CUR_DIR:
{
    my $ipc_cmd = new Test::MockModule('IPC::Cmd');
    my (@called_commands, @working_dirs);
    $ipc_cmd->mock(
        run => sub
        {
            my %args = @_;
            push(@called_commands, join(' ', @{ $args{command} }));
            push(@working_dirs, canonpath(cwd()));
            $? = 0;
            return (1, undef, [""]);
        }
    );

    my $utr_wrapper = new UtrWrapper(utr_dir => '../', args => ['--suite=native', '--platform=editor']);
    can_ok($utr_wrapper, 'runTests');
    is($utr_wrapper->runTests(), 'good');
    is(@called_commands, 1);
    my $test_pl_path = catfile('..', 'test.pl');
    is($called_commands[0], "perl $test_pl_path --progress=null --suite=native --platform=editor");
    is(@working_dirs, 1);
    is($working_dirs[0], rel2abs('.'));
}

RUN_TEST_WITH_LOGGER:
{
    my $ipc_cmd = new Test::MockModule('IPC::Cmd');
    $ipc_cmd->mock(
        run => sub
        {
            my %args = @_;
            $? = 0;
            return (1, undef, ['IT RUNS!!!']);
        }
    );
    my (@module_names, @log_contents);
    my $log_file_writer = new Test::MockModule('LogFileWriter');
    $log_file_writer->mock(
        writeLog => sub
        {
            my ($this, %args) = @_;
            push(@module_names, $args{module});
            push(@log_contents, $args{contents});
        }
    );
    my $utr_wrapper = new UtrWrapper(
        utr_dir => '../',
        args => ['--suite=native', '--platform=editor'],
        log_file_writer => new LogFileWriter(log_root => undef)
    );
    ok($utr_wrapper->runTests());
    is(@module_names, 1);
    is($module_names[0], 'utr');
    is(@log_contents, 1);
    is($log_contents[0]->[0], 'IT RUNS!!!');
}

RUN_TEST_FAILING:
{
    my $ipc_cmd = new Test::MockModule('IPC::Cmd');
    $ipc_cmd->mock(
        run => sub
        {
            $? = 42 << 8;
            return (0, undef, [""]);
        }
    );

    my $utr_wrapper = new UtrWrapper(utr_dir => '.', args => ['--suite=native', '--platform=editor']);
    is($utr_wrapper->runTests(), 'bad');
}

done_testing();
