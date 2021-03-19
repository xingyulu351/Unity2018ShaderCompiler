package LinuxStandaloneSupport;

use strict;
use warnings;
use File::Basename qw (dirname);
use File::Spec;
use File::chdir;
use lib File::Spec->rel2abs(dirname($0) . "/../../..") . '/External/Perl/lib';
use lib File::Spec->rel2abs(dirname($0) . "/..");
use Tools qw (Jam JamRaw AmRunningOnBuildServer ProgressMessage PackageAndSendResultsToBuildServer);

use lib File::Spec->rel2abs(dirname(__FILE__) . "/../../SDKDownloader");
use SDKDownloader;

## Get build options
use BuildConfig;
our $unityVersion = $BuildConfig::unityVersion;
our $copyright = $BuildConfig::copyright;

sub Register()
{
    my $targetData = {
        linuxbuildfunction => \&BuildLinuxStandaloneSupport,
        options => {
            "codegen" => ["debug", "release"],
            "incremental" => [0, 1],
            "zipresults" => [0, 1],
            "abvsubset" => [0, 1],
            "scriptingBackend" => ["default", "il2cpp"],
            "platform" => ['linux64', 'linux32'],
            "lump" => [0, 1]
        }
    };

    Main::RegisterTarget("LinuxStandaloneSupport", $targetData);
}

sub GetHumanJamTargetsFromOptions
{
    my ($options, $targetPrefix) = @_;

    my @targets = ();
    my $default = {
        platform => 'linux64',
        scriptingBackend => 'default',
        developmentPlayer => 1,
        headlessPlayer => 0,
        lump => 1,
    };

    my $possibleCombinationsCount = 0;

    # for each option which is not fixed and has an invalid/undef default value
    foreach my $headless (0, 1)
    {
        foreach my $platform ('linux64', 'linux32')
        {
            foreach my $development (0, 1)
            {
                $possibleCombinationsCount++;

                if (    (!defined($options->{headlessPlayer}) or ($options->{headlessPlayer} eq $headless))
                    and (!defined($options->{developmentPlayer}) or ($options->{developmentPlayer} eq $development))
                    and (!defined($options->{platform}) or ($options->{platform} eq $platform)))
                {
                    my $targetName = $targetPrefix;

                    if ($platform ne $default->{platform})
                    {
                        $targetName .= substr($platform, 5);
                    }
                    if ($options->{lump} ne $default->{lump})
                    {
                        $targetName .= $options->{lump} ? 'Lump' : 'NoLump';
                    }
                    if ($options->{scriptingBackend} ne $default->{scriptingBackend})
                    {
                        $targetName .= uc($options->{scriptingBackend});
                    }
                    if ($development ne $default->{developmentPlayer})
                    {
                        $targetName .= $development ? 'Development' : 'NoDevelopment';
                    }
                    if ($headless ne $default->{headlessPlayer})
                    {
                        $targetName .= $headless ? 'Headless' : 'NoHeadless';
                    }

                    my $buildPath = $platform;
                    $buildPath .= $headless ? '_headless' : '_withgfx';
                    $buildPath .= $development ? '_development' : '_nondevelopment';
                    $buildPath .= $options->{scriptingBackend} eq 'default' ? '_mono' : '_il2cpp';

                    push(@targets, [$targetName, $buildPath]);
                }
            }
        }
    }

    my $fullCartesianProduct = scalar(@targets) == $possibleCombinationsCount;

    return $fullCartesianProduct, @targets;
}

sub CheckInstallSDK
{
    my ($root) = @_;
    if (AmRunningOnBuildServer() || exists $ENV{'UNITY_USE_LINUX_SDK'})
    {
        ProgressMessage('Setting up the Linux SDK');
        SDKDownloader::PrepareSDK('linux-sdk', '20180406', "$root/artifacts");
    }
}

sub BuildLinuxStandaloneSupport
{
    my ($root, $targetPath, $options) = @_;
    my $scriptingBackend = ('il2cpp' eq $options->{scriptingBackend}) ? 'il2cpp' : 'mono';

    CheckInstallSDK($root);

    my @targetsToBuild = ();
    my @targetsToStrip = ();

    if ($options->{platform} eq '')
    {
        # to avoid dealing with both '' and undefined
        $options->{platform} = undef;
    }

    if (defined($options->{lump}) and ($options->{lump} eq 0))
    {
        # The non-lumped player is only a development, non-headless 64 bit player
        # This allows perl build.pl --target=LinuxStandaloneSupport --lump=0 to
        # work out of the box, without providing other arguments.
        my %defaultLumpConfig = (
            'platform' => 'linux64',
            'headlessPlayer' => 0,
            'developmentPlayer' => 1,
        );

        foreach my $key (keys(%defaultLumpConfig))
        {
            if (defined($options->{$key}) and ($options->{$key} ne $defaultLumpConfig{$key}))
            {
                warn("warning: setting value --$key=$defaultLumpConfig{$key}\n");
            }
            $options->{$key} = $defaultLumpConfig{$key};
        }
    }
    else
    {
        # We cannot respect developmentPlayer from build.pl because it doesn't have
        # an undefined/invalid default and we have to treat it as a wildcard when
        # we construct the jam target names to build. Otherwise, we will build only
        # one of the two variation sets or the non-development builds from the "All"
        # subset will not be stripped. This is similar with V1 code.
        $options->{developmentPlayer} = undef;
    }

    my $targetPrefix = 'LinuxPlayer';

    if ($options->{abvsubset})
    {
        my $subset = 'AbvSubset';
        my $targetName = "$targetPrefix${subset}_$scriptingBackend";
        push(@targetsToBuild, $targetName);
    }
    else
    {
        my ($full, @humanTargets) = GetHumanJamTargetsFromOptions($options, $targetPrefix);

        if ($full)
        {
            my $subset = 'All';
            my $targetName = "$targetPrefix${subset}_$scriptingBackend";
            push(@targetsToBuild, $targetName);
        }
        else
        {
            for my $targetPair (@humanTargets)
            {
                push(@targetsToBuild, @$targetPair[0]);
            }
        }

        for my $targetPair (@humanTargets)
        {
            if (@$targetPair[1] =~ /_nondevelopment_/)
            {
                push(@targetsToStrip, File::Spec->catfile($targetPath, 'Variations', @$targetPair[1], $targetPrefix));
            }
        }
    }

    for my $targetName (@targetsToBuild)
    {
        my $codegen = lc($options->{codegen});
        my @cmd = ($root, $targetName, "-sCONFIG=$codegen");

        if ($options->{abvsubset})
        {
            push(@cmd, '-sASSERTS_ENABLED=1');
        }

        JamRaw(@cmd);
    }

    if (scalar(@targetsToStrip) gt 0)
    {
        ProgressMessage("Stripping Symbols from release binaries.\n");
        for my $targetPath (@targetsToStrip)
        {
            system('strip', '-s', $targetPath);
        }
    }

    PackageAndSendResultsToBuildServer($targetPath, 'LinuxStandalone') if $options->{zipresults};
}

1;
