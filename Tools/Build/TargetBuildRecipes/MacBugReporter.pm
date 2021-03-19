package MacBugReporter;

use Tools;
use Carp;
use Tools qw (JamRaw);

sub Register
{
    Main::RegisterTarget(
        "BugReporter",
        {
            macbuildfunction => \&BuildBugReporter,
            options => {
                codegen => ["debug", "release"],
                platform => undef,
                enableBugReporterTests => [0, 1]
            }
        }
    );
}

sub BuildBugReporter
{
    my ($root, $targetPath, $options) = @_;
    my $platform = GetPlatform($options);
    my $config = GetConfig($options);
    Tools::ProgressMessage("Building bug reporter");

    JamRaw($root, "-sPLATFORM=$platform", "-sCONFIG=$config", "bugreporter");
    JamRaw($root, "-sPLATFORM=$platform", "-sCONFIG=$config", "bugreporter_test") if $options->{"enableBugReporterTests"} eq "1";
}

sub GetPlatform
{
    my ($options) = @_;
    my $platform = $options->{platform};
    if (!defined($platform) or !$platform)
    {
        $platform = "macosx64";
    }
    return lc $platform;
}

sub GetConfig
{
    my ($options) = @_;
    my $config = $options->{codegen};
    if (!defined($config) or !$config)
    {
        $config = "release";
    }
    return lc $config;
}

1;
