use Dirs;
use lib Dirs::external_root();

use Config;
use Test::MockModule;
use Test::More;

BEGIN { use_ok('SuiteDependenciesBuilder') }

CREATION_TEST:
{
    can_ok('SuiteDependenciesBuilder', 'new');
    my $builder = new SuiteDependenciesBuilder(os => 'win', suite => 'runtime', platform => 'editor');
    isa_ok($builder, 'SuiteDependenciesBuilder');
}

WIN_RUNTIME_SUITE_BUILD_TEST:
{
    my $ipc_cmd = new Test::MockModule('IPC::Cmd');
    my (@called_commands, @native_tests_state);
    $ipc_cmd->mock(
        run => sub
        {
            my %args = @_;
            push(@called_commands, join(' ', @{ $args{command} }));
            my $native_tests_enabled = $ENV{'UNITY_RUN_NATIVE_TESTS_DURING_BUILD'};
            push(@native_tests_state, $native_tests_enabled);
            return (1, undef, "");
        }
    );
    my $builder = new SuiteDependenciesBuilder(os => 'win', suite => 'runtime', platform => 'editor');
    ok($builder->build());
    is(@called_commands, 1);
    is($called_commands[0], 'perl build.pl --target=WindowsEditor');
    is(@native_tests_state, 1);
    is($native_tests_state[0], 0);
}

MAC_RUNTIME_SUITE_BUILD_TEST:
{
    my $ipc_cmd = new Test::MockModule('IPC::Cmd');
    my (@called_commands, @native_tests_state);
    $ipc_cmd->mock(
        run => sub
        {
            my %args = @_;
            push(@called_commands, join(' ', @{ $args{command} }));
            my $native_tests_enabled = $ENV{'UNITY_RUN_NATIVE_TESTS_DURING_BUILD'};
            push(@native_tests_state, $native_tests_enabled);
            return (1, undef, "");
        }
    );
    my $builder = new SuiteDependenciesBuilder(os => 'mac', suite => 'runtime', platform => 'editor');
    ok($builder->build());
    is(@called_commands, 1);
    is($called_commands[0], 'perl build.pl --target=MacEditor');
    is(@native_tests_state, 1);
    is($native_tests_state[0], 0);
}

WIN_CGBATH_SUITE_BUILD_TEST:
{
    my $ipc_cmd = new Test::MockModule('IPC::Cmd');
    my (@called_commands, @native_tests_state);
    $ipc_cmd->mock(
        run => sub
        {
            my %args = @_;
            push(@called_commands, join(' ', @{ $args{command} }));
            my $native_tests_enabled = $ENV{'UNITY_RUN_NATIVE_TESTS_DURING_BUILD'};
            push(@native_tests_state, $native_tests_enabled);
            return (1, undef, "");
        }
    );
    my $builder = new SuiteDependenciesBuilder(os => 'win', suite => 'cgbath');
    ok($builder->build());
    is(@called_commands, 1);
    is($called_commands[0], 'perl build.pl --target=WindowsEditor');
    is(@native_tests_state, 1);
    is($native_tests_state[0], 0);
}

WIN_INTEGRATION_SUITE_BUILD_TEST:
{
    my $ipc_cmd = new Test::MockModule('IPC::Cmd');
    my (@called_commands, @native_tests_state);
    $ipc_cmd->mock(
        run => sub
        {
            my %args = @_;
            push(@called_commands, join(' ', @{ $args{command} }));
            my $native_tests_enabled = $ENV{'UNITY_RUN_NATIVE_TESTS_DURING_BUILD'};
            push(@native_tests_state, $native_tests_enabled);
            return (1, undef, "");
        }
    );
    my $builder = new SuiteDependenciesBuilder(os => 'win', suite => 'integration');
    ok($builder->build());
    is(@called_commands, 1);
    is($called_commands[0], 'perl build.pl --target=WindowsEditor,WindowsStandaloneSupport');
    is(@native_tests_state, 1);
    is($native_tests_state[0], 0);
}

WIN_RUNTIME_STANDALONE_SUITE_BUILD_TEST:
{
    my $ipc_cmd = new Test::MockModule('IPC::Cmd');
    my (@called_commands, @native_tests_state);
    $ipc_cmd->mock(
        run => sub
        {
            my %args = @_;
            push(@called_commands, join(' ', @{ $args{command} }));
            my $native_tests_enabled = $ENV{'UNITY_RUN_NATIVE_TESTS_DURING_BUILD'};
            push(@native_tests_state, $native_tests_enabled);
            return (1, undef, "");
        }
    );
    my $builder = new SuiteDependenciesBuilder(os => 'win', suite => 'runtime', platform => 'standalone');
    ok($builder->build());
    is(@called_commands, 1);
    is($called_commands[0], 'perl build.pl --target=WindowsEditor,WindowsStandaloneSupport');
    is(@native_tests_state, 1);
    is($native_tests_state[0], 0);
}

BUILD_TEST_WITH_LOGGER:
{
    my $ipc_cmd = new Test::MockModule('IPC::Cmd');
    $ipc_cmd->mock(
        run => sub
        {
            my %args = @_;
            return (1, undef, ['IT BUILDS!!!']);
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
    my $builder = new SuiteDependenciesBuilder(
        os => 'win',
        suite => 'runtime',
        platform => 'editor',
        log_file_writer => new LogFileWriter(log_root => undef)
    );
    ok($builder->build());
    is(@module_names, 1);
    is($module_names[0], 'build');
    is(@log_contents, 1);
    is($log_contents[0]->[0], 'IT BUILDS!!!');
}

done_testing();
