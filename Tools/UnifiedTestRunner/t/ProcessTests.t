use Test::More;

BEGIN
{
    use_ok('Process');
}

Creation:
{
    can_ok('Process', 'new');
    my $process = new Process();
    isa_ok($process, 'Process');
}

Start_ExistingExecutable_Starts:
{
    my $process = new Process();
    can_ok($process, 'start');
    $process->start('perl', '-v');
    is($process->started(), 1);
}

Start_ExistingExecutable_ReturnsZeroExitCode:
{
    my $process = new Process();
    $process->start('perl', '-v');
    is($process->exitCode(), 0);
}

Start_PerlExit1_ReturnExitCode1:
{
    my $process = new Process();
    $process->start('perl', '-e', 'exit 1');
    is($process->exitCode(), 1);
}

Start_PerlExit2_ReturnExitCode2:
{
    my $process = new Process();
    $process->start('perl', '-e', 'exit 2');
    is($process->exitCode(), 2);
}

Start_PerlExit1_ReturnStartedTrue:
{
    my $process = new Process();
    $process->start('perl', '-e', 'exit 1');
    is($process->started(), 1);
}

Start_ProgramThatOutputsToStdout_CaptureStdout:
{
    my $processOutput;
    my $process = newProcessCapturingoutput(\$processOutput);
    $process->start('perl', '-e', 'print 2');
    is($processOutput, '2');
}

Start_ProgramThatOutputsString_CapturesIt:
{
    my $processOutput;
    my $process = newProcessCapturingoutput(\$processOutput);
    $process->start('perl', '-e', "print 'abc'");
    is($processOutput, 'abc');
}

done_testing();

sub newProcessCapturingoutput
{
    my ($outputHolder) = @_;
    my $outputConsumer = sub
    {
        (${$outputHolder}) = @_;
    };
    return new Process($outputConsumer);
}
