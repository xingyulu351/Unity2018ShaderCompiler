package UtrUsage;

use warnings;
use strict;
use Commands;

sub get
{
    return (
        description => "Unified Test Runner. The single entry point to run tests locally on on the build farm.\n",
        url => 'http://internaldocs.hq.unity3d.com/automation/UnifiedTestRunner/About/index.html',
        commandPrefix => Commands::getUtrPrefix(),
        examples => [
            {
                description => "Run all tests from native suite matching 'VR'",
                command => "--suite=native --testfilter=VR"
            },
            {
                description => "Run integration tests matching 'Can' from a single assembly",
                command => "--suite=integration --testfilter=Can --testprojects=UnityEngine.Common.Tests.csproj"
            },
            {
                description => "List integration tests matching 'Can' from a single assembly",
                command => "--suite=integration --list --testfilter=Can --testprojects=UnityEngine.Common.Tests.csproj"
            },
            {
                description => "Run integration tests matching 'Can' from a single assembly. Do not rebuild CS projects.",
                command => "--suite=integration --skip-build-deps --testfilter=Can --testprojects=UnityEngine.Common.Tests.csproj"
            },
            {
                description => "Run the Weekly integration tests",
                command => "--suite=integration --testtarget=TestWeekly --testprojects=All"
            },
            {
                description => "Print all available 'global' options. Applicable for each suite.",
                command => "--help"
            },
            {
                description => "List available options for the 'native' suite",
                command => "--help --suite=native"
            },
        ]
    );
}

1;
