package WindowsStandaloneSupport;

use strict;
use warnings;
use File::Basename qw (dirname basename fileparse);
use File::Copy;
use File::Spec;
use File::Path;
use File::Glob;
use File::Find;
use Carp qw (croak carp);
use File::chdir;
use Tools qw (Jam JamRaw UCopy ProgressMessage PackageAndSendResultsToBuildServer);
use Tools qw (NewXBuild);
use WinTools qw (AddSourceServerToPDB SignFile);

sub Register
{
    my $targetData = {
        windowsbuildfunction => \&BuildWindowsStandaloneSupport,
        options => {
            "codegen" => ["debug", "release"],
            "incremental" => [0, 1],
            "zipresults" => [0, 1],
            "abvsubset" => [0, 1],
            "enableMonoSGen" => [0, 1],
            "scriptingBackend" => ["default", "il2cpp"],
            "lump" => [1, 0],
            "platform" => ["win64", "win32"],
            "jamArgs" => []
        }
    };

    Main::RegisterTarget("WindowsStandaloneSupport", $targetData);
}

sub GetTargetFromOptions
{
    my ($root, $targetPath, $options) = @_;

    my $bits = ('win32' eq $options->{platform}) ? '32' : '';
    my $scriptingBackend = ('il2cpp' eq $options->{scriptingBackend}) ? 'IL2CPP' : '';
    my $developmentness = (0 eq $options->{developmentPlayer}) ? 'NoDevelopment' : '';
    my $targetName = "WinPlayer" . "$bits" . "$scriptingBackend" . "$developmentness";

    if (!($options->{lump}))
    {
        $targetName = "WinPlayerNoLump";
    }

    return $targetName;
}

sub GetSkip32Bit
{
    my ($root, $targetPath, $options) = @_;
    return (defined $options->{platform}) && ($options->{platform} ne '') && ($options->{platform} ne 'win32');
}

sub GetSkip64Bit
{
    my ($root, $targetPath, $options) = @_;
    return (defined $options->{platform}) && ($options->{platform} ne '') && ($options->{platform} ne 'win64');
}

sub SignFiles
{
    my ($root, $targetPath, $options) = @_;

    # Sign the file so we can get crash information from MS via WER
    my $skip32Bit = GetSkip32Bit($root, $targetPath, $options);
    my $skip64Bit = GetSkip64Bit($root, $targetPath, $options);
    if ($options->{scriptingBackend} ne "il2cpp")
    {
        # Sign UnityPlayer.dll
        SignFile("$targetPath/Variations/win32_development_mono/UnityPlayer.dll") unless $skip32Bit;
        SignFile("$targetPath/Variations/win32_nondevelopment_mono/UnityPlayer.dll") unless $skip32Bit;
        SignFile("$targetPath/Variations/win64_development_mono/UnityPlayer.dll") unless $skip64Bit;
        SignFile("$targetPath/Variations/win64_nondevelopment_mono/UnityPlayer.dll") unless $skip64Bit;

        # Sign the mono.dll
        SignFile("$targetPath/Variations/win32_development_mono/Mono/EmbedRuntime/mono.dll") unless $skip32Bit;
        SignFile("$targetPath/Variations/win32_nondevelopment_mono/Mono/EmbedRuntime/mono.dll") unless $skip32Bit;
        SignFile("$targetPath/Variations/win64_development_mono/Mono/EmbedRuntime/mono.dll") unless $skip64Bit;
        SignFile("$targetPath/Variations/win64_nondevelopment_mono/Mono/EmbedRuntime/mono.dll") unless $skip64Bit;

        my $monoLibName = "mono-2.0-bdwgc.dll";
        if ($options->{enableMonoSGen})
        {
            $monoLibName = "mono-2.0-sgen.dll";
        }

        SignFile("$targetPath/Variations/win32_development_mono/MonoBleedingEdge/EmbedRuntime/$monoLibName") unless $skip32Bit;
        SignFile("$targetPath/Variations/win32_nondevelopment_mono/MonoBleedingEdge/EmbedRuntime/$monoLibName") unless $skip32Bit;
        SignFile("$targetPath/Variations/win64_development_mono/MonoBleedingEdge/EmbedRuntime/$monoLibName") unless $skip64Bit;
        SignFile("$targetPath/Variations/win64_nondevelopment_mono/MonoBleedingEdge/EmbedRuntime/$monoLibName") unless $skip64Bit;

        # Sign UnityCrashHandlerXX.exe
        SignFile("$targetPath/Variations/win32_development_mono/UnityCrashHandler32.exe") unless $skip32Bit;
        SignFile("$targetPath/Variations/win32_nondevelopment_mono/UnityCrashHandler32.exe") unless $skip32Bit;
        SignFile("$targetPath/Variations/win64_development_mono/UnityCrashHandler64.exe") unless $skip64Bit;
        SignFile("$targetPath/Variations/win64_nondevelopment_mono/UnityCrashHandler64.exe") unless $skip64Bit;
    }
    else
    {
        # Sign UnityPlayer.dll
        SignFile("$targetPath/Variations/win32_development_il2cpp/UnityPlayer.dll") unless $skip32Bit;
        SignFile("$targetPath/Variations/win32_nondevelopment_il2cpp/UnityPlayer.dll") unless $skip32Bit;
        SignFile("$targetPath/Variations/win64_development_il2cpp/UnityPlayer.dll") unless $skip64Bit;
        SignFile("$targetPath/Variations/win64_nondevelopment_il2cpp/UnityPlayer.dll") unless $skip64Bit;

        # Sign UnityCrashHandlerXX.exe
        SignFile("$targetPath/Variations/win32_development_il2cpp/UnityCrashHandler32.exe") unless $skip32Bit;
        SignFile("$targetPath/Variations/win32_nondevelopment_il2cpp/UnityCrashHandler32.exe") unless $skip32Bit;
        SignFile("$targetPath/Variations/win64_development_il2cpp/UnityCrashHandler64.exe") unless $skip64Bit;
        SignFile("$targetPath/Variations/win64_nondevelopment_il2cpp/UnityCrashHandler64.exe") unless $skip64Bit;
    }
}

sub BuildWindowsStandaloneSupport
{
    my ($root, $targetPath, $options) = @_;

    my $codegen = lc($options->{codegen});
    $options->{developmentPlayer} = 1;

    my @devPlayerArgs = ();
    if ($options->{abvsubset})
    {
        push @devPlayerArgs, "-sASSERTS_ENABLED=1";
    }

    $options->{platform} = "win32";
    my $targetName = GetTargetFromOptions($root, $targetPath, $options);
    JamRaw(($root, $targetName, "-sCONFIG=$codegen", @devPlayerArgs));
    $options->{platform} = "win64";
    $targetName = GetTargetFromOptions($root, $targetPath, $options);
    JamRaw(($root, $targetName, "-sCONFIG=$codegen", @devPlayerArgs));

    if (!($options->{abvsubset}) && $options->{lump})
    {

        my $these_options = $options;
        $these_options->{developmentPlayer} = 0;
        $these_options->{platform} = "win32";
        $targetName = GetTargetFromOptions($root, $targetPath, $these_options);

        # always build nondev players in release, because we don't list DebugNonDev as
        # a variation, because nobody wants to use it and it pollutes the VS solution.
        if ($codegen eq "debug" || $codegen eq "default")
        {
            print "warning: changing codegen $codegen to release for nondevelopment players";
            $codegen = "release";
        }
        JamRaw(($root, $targetName, "-sCONFIG=$codegen"));

        $these_options->{platform} = "win64";
        $targetName = GetTargetFromOptions($root, $targetPath, $these_options);
        JamRaw(($root, $targetName, "-sCONFIG=$codegen"));

        # Sign the file so we can get crash information from MS via WER
        SignFiles($root, $targetPath, $options);
    }

    my @pdbs = ();
    find({ wanted => sub { /\.pdb$/ && push @pdbs, $File::Find::name; }, no_chdir => 1 }, $targetPath);
    AddSourceServerToPDB($root, \@pdbs);

    if ($options->{buildAutomation})
    {
        JamRaw($root, 'WindowsStandaloneAutomation');
    }

    PackageAndSendResultsToBuildServer($targetPath, "WindowsStandalone") if $options->{zipresults};
}

1;
