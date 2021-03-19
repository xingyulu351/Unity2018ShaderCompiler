package Capabilities;

my $capabilities = {
    SmartSelectProtocolVersion => 1,

    # version 1 - initial support for integration tests only
    ParallelTestExecutionVersion => 1,

    # katana can place some arugments into ENV variable
    SupportsHiddenArgs => 1,

    #utr.pl is in repo root
    SupportsUtrPlInRepoRoot => 1
};

sub getCapabilities
{
    return $capabilities;
}

1;
