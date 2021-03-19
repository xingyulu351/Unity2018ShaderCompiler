package SuiteRunners::BuildSystem;

use parent NUnitConfigBasedSuiteRunner;

use warnings;
use strict;
use File::Spec::Functions qw (canonpath);
use Dirs;
use CSProj;
use Environments::MonoBleedingEdge;

sub getConfig
{
    my ($this) = @_;
    my $config = {
        name => 'buildsystem',
        prepareEnv => sub
        {
        },
        nunitInputFiles => getDlls(),
        environment => new Environments::MonoBleedingEdge(),
        help => {
            description => "Runs all Build System Tests",
            examples => [
                { description => "Run all tests where the string 'Compiler' is somewhere in the name of the test", command => '--testfilter=Compiler' },
                { description => "List all tests", command => '--list' },
            ]
        }
    };
}

sub getDlls
{
    my $path = "artifacts/UnityBuildSystem";
    my @myTestDlls;
    if (not -e $path)
    {
        return \@myTestDlls;
    }
    opendir(my $DIR, $path);
    while (my $entry = readdir $DIR)
    {
        next unless -d $path . '/' . $entry;
        next if $entry eq '.' or $entry eq '..';
        next if !($entry =~ /Tests$/);
        push(@myTestDlls, "$path/$entry/$entry.dll");
    }

    closedir($DIR);
    return \@myTestDlls;
}

sub cleanPreviousLogsIfAny
{
    Logger::minimal("Skip cleaning the artifacts folder");
}

1;
