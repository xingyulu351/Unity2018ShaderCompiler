use warnings;
use strict;

use File::Temp;
use IO::File;
use Dirs;
use lib Dirs::external_root();
use Test::More;
use Test::MockObject;
use File::Spec::Functions qw (catfile);
use Cwd qw (getcwd abs_path);

use Dirs;
use lib Dirs::external_root();

BEGIN { use_ok('CodePatcher') }

CREATION_TEST:
{
    can_ok('CodePatcher', 'new');
    my $log_file_writer = new CodePatcher(code_path => '.', config => []);
    isa_ok($log_file_writer, 'CodePatcher');
}

PATCHING_TEST_EMPTY_CONFIG:
{
    my $log_file_writer = new CodePatcher(code_path => '.', config => []);
    can_ok($log_file_writer, 'applyPatches');
    is($log_file_writer->applyPatches(), 1);
}

PATCHING_TEST_SHOULD_SKIP:
{
    my $input_contents = 'hey' . "\n" . 'ShouldRunNativeTests  push (@args, "-sUNITY_RUN_NATIVE_TESTS_DURING_BUILD=1");';
    my $test_file_name;
    {
        my $testfile = new File::Temp(DIR => '.', UNLINK => 0);
        print($testfile $input_contents);
        $test_file_name = $testfile->filename();
    }
    my $log_file_writer = new CodePatcher(
        code_path => '.',
        config => [[$test_file_name, 'ShouldRunNativeTests', 'push (@args, "-sUNITY_RUN_NATIVE_TESTS_DURING_BUILD=1");', '42']]
    );
    is($log_file_writer->applyPatches(), 1);
    my $result_contents;
    {
        local $/;
        my $testfile = new IO::File($test_file_name);
        $result_contents = <$testfile>;
    }
    is($result_contents, $input_contents);
    unlink($test_file_name) or warn("Failed to unlink $test_file_name");
}

PATCHING_TEST_SHOULD_PATCH:
{
    my $input_contents = 'hey' . "\n" . '  push (@args, "-sUNITY_RUN_NATIVE_TESTS_DURING_BUILD=1");' . "\n\n";
    my $expected_contents = 'hey' . "\n" . '  # push (@args, "-sUNITY_RUN_NATIVE_TESTS_DURING_BUILD=1");' . "\n\n";
    my $test_file_name;
    {
        my $testfile = new File::Temp(DIR => '.', UNLINK => 0);
        print($testfile $input_contents);
        $test_file_name = $testfile->filename();
    }
    my $log_file_writer = new CodePatcher(
        code_path => '.',
        config => [
            [
                $test_file_name, qr/ShouldRunNativeTests/,
                'push (@args, "-sUNITY_RUN_NATIVE_TESTS_DURING_BUILD=1");', '# push (@args, "-sUNITY_RUN_NATIVE_TESTS_DURING_BUILD=1");'
            ]
        ]
    );
    is($log_file_writer->applyPatches(), 1);
    my $result_contents;
    {
        local $/;
        my $testfile = new IO::File($test_file_name);
        $result_contents = <$testfile>;
    }
    is($result_contents, $expected_contents);
    unlink($test_file_name) or warn("Failed to unlink $test_file_name");
}

PATCHING_TEST_SHOULD_APPLY_ADDITIONAL_PATCH:
{
    my (@called_commands);
    my (@working_dirs);
    my $systemCall = new Test::MockObject();
    $systemCall->mock(
        executeAndGetAllOutput => sub
        {
            shift();
            push(@called_commands, join(' ', @_));
            push(@working_dirs, getcwd());
            return (0, ['']);
        }
    );
    my $cur_dir = abs_path(getcwd());
    my $code_path = abs_path(catfile($cur_dir, ".."));
    my $code_patcher = new CodePatcher(
        code_path => $code_path,
        config => [],
        system_call => $systemCall,
        bisect_custom_patch_command => 'patch < file.patch'
    );

    my $result = $code_patcher->executeApplyCustomPatch();

    ok($result);
    is($cur_dir, getcwd());
    is(@called_commands, 1);
    is($called_commands[0], 'patch < file.patch');
    is($working_dirs[0], $code_path);
}

PATCHING_TEST_SHOULD_RETURN_ERROR_ON_PATCH_FAILURE:
{
    my $systemCall = new Test::MockObject();
    $systemCall->mock(
        executeAndGetAllOutput => sub
        {
            return (1, ['patching error']);
        }
    );
    my $code_patcher = new CodePatcher(
        code_path => '.',
        config => [],
        system_call => $systemCall,
        bisect_custom_patch_command => 'false'
    );

    my $result = $code_patcher->executeApplyCustomPatch();

    is($result, '');
}

done_testing();
