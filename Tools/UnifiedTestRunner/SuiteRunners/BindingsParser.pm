package SuiteRunners::BindingsParser;

use parent NUnitConfigBasedSuiteRunner;

use warnings;
use strict;
use Environments::Win;

use File::Spec::Functions qw (canonpath);

use Dirs;

my $root = Dirs::getRoot();
my $proj = canonpath("$root/Tools/UnityBindingsParser/UnityBindingsParser.sln");

sub getConfig
{
    my ($this) = @_;
    my $config = {
        name => 'bindings-parser',
        targetFrameworkVersion => '4.5',
        nunitInputFiles => [$proj],
        environment => new Environments::Win(),

        # disableResolveDlls => 1,
        help => {
            description => "Binding parser tests hosted in $proj",
            examples => [
                { description => "Run all tests matching 'NodeParsing'", command => '--testfilter=NodeParsing' },
                { description => "List all matching 'NodeParsing'", command => '--list --testfilter=NodeParsing' },
            ],
        }
    };
}

1;
