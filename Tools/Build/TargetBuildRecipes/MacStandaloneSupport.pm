package MacStandaloneSupport;
use strict;
use warnings;
use Tools qw (Jam JamRaw PackageAndSendResultsToBuildServer);

## Get build options
use BuildConfig;
our $unityVersion = $BuildConfig::unityVersion;
our $copyright = $BuildConfig::copyright;

sub Register
{
    my $targetData = {
        macbuildfunction => \&BuildMacStandaloneSupport,
        options => {
            "codegen" => ["debug", "release"],
            "incremental" => [0, 1],
            "zipresults" => [0, 1],
            "abvsubset" => [0, 1],
            "lump" => [0, 1],
            "scriptingBackend" => ["default", "il2cpp"]
        }
    };

    Main::RegisterTarget("MacStandaloneSupport", $targetData);

    Main::RegisterTarget(
        "MacStandaloneVerifyModularBuild",
        {
            macbuildfunction => \&VerifyModularBuild,
            options => {
                "codegen" => ["debug", "release"],
                "incremental" => [0, 1],
                "zipresults" => [0, 1],
                "abvsubset" => [0, 1],
                "lump" => [0, 1],
                "scriptingBackend" => ["default", "il2cpp"]
            }
        }
    );
}

sub GetTargetFromOptions
{
    my ($root, $targetPath, $options) = @_;

    my $scriptingBackend = ('il2cpp' eq $options->{scriptingBackend}) ? 'IL2CPP' : '';
    my $developmentness = ($options->{developmentplayer} == 0 ? 'NoDevelopment' : '');
    my $targetName = "MacPlayer" . "$scriptingBackend" . "$developmentness";

    if (!($options->{lump}))
    {
        $targetName = "MacPlayerNoLump";
    }

    return $targetName;
}

sub BuildMacStandaloneSupport
{
    my ($root, $targetPath, $options) = @_;

    my $codegen = lc($options->{codegen});
    $options->{developmentplayer} = 1;
    my $targetName = GetTargetFromOptions($root, $targetPath, $options);

    my @cmd = ($root, $targetName, "-sCONFIG=$codegen");

    if ($options->{abvsubset})
    {
        push(@cmd, "-sASSERTS_ENABLED=1");
    }

    JamRaw(@cmd);

    if (!($options->{abvsubset}) && $options->{lump})
    {
        my $these_options = $options;
        $these_options->{developmentplayer} = 0;
        VerifyModularBuild($root, $targetPath, $options);
        JamRaw(($root, GetTargetFromOptions($root, $targetPath, $these_options), "-sCONFIG=$codegen"));
    }

    PackageAndSendResultsToBuildServer($targetPath, "MacStandalone") if $options->{zipresults};
}

sub BuildAllArchVariationsFor
{
    my ($jamoptions, $namepostfix, $root, $targetPath, $options) = @_;

    push @{$jamoptions}, "-sLUMP=$options->{lump}";

    Jam($root, "MacStandalonePlayer", $options->{codegen}, 'macosx64', $options->{incremental}, @{$jamoptions});

    my $variations = "$targetPath/Variations";
    my $release = index($namepostfix, "nondevelopment") != -1;
}

sub VerifyModularBuild
{
    my ($root, $targetPath, $options) = @_;

    my @devPlayerArgs = ("-sUSE_EXPERIMENTAL_MODULARITY=1", "-sLUMP=$options->{lump}");

    if ($options->{scriptingBackend} eq "il2cpp")
    {
        push @devPlayerArgs, "-sSCRIPTING_BACKEND=il2cpp";
    }
    my $these_options = {};
    $these_options->{codegen} = $options->{codegen};
    $these_options->{scriptingBackend} = $options->{scriptingBackend};
    $these_options->{developmentplayer} = 0;
    $these_options->{lump} = 1;

    # below is the command line for modular build
    Jam($root, GetTargetFromOptions($root, $targetPath, $these_options), $options->{codegen}, 'macosx64', $options->{incremental}, @devPlayerArgs);

    # We currently build this only for verification that it is not broken.
    # We don't want to keep the results.
    system("rm -r $targetPath/Variations/macosx64_modular_*");
}

1;
