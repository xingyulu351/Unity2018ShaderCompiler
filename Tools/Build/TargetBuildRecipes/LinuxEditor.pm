package LinuxEditor;

use strict;
use warnings;
use File::Path qw (mkpath rmtree);
use File::Basename qw (dirname basename);
use File::Spec;
use File::Copy;
use lib File::Spec->rel2abs(dirname($0) . "/../../..") . '/External/Perl/lib';
use OTEE::BuildUtils qw (build_i386);
use lib File::Spec->rel2abs(dirname($0) . "/..");
use MacTools qw (Cleanup LinkFileOrDirectory LinkFlattenedDirs);
use Tools qw (GenerateUnityConfigure XBuild UCopy Jam AmRunningOnBuildServer ProgressMessage);
use Tools qw (PackageAndSendResultsToBuildServer);
use PrepareWorkingCopy qw (PrepareExternalDependency);
use Licenses qw(VerifyExternalLicenses BuildLicensesFile);
use Carp qw (croak carp);

use lib File::Spec->rel2abs(dirname(__FILE__) . "/../../SDKDownloader");
use SDKDownloader;

####
# TODO: This Build Recipe still needs a lot of work.
####

sub CheckInstallSDK
{
    my ($root) = @_;
    if (AmRunningOnBuildServer() || exists $ENV{'UNITY_USE_LINUX_SDK'})
    {
        ProgressMessage('Setting up the Linux SDK');
        SDKDownloader::PrepareSDK('linux-sdk', '20180406', "$root/artifacts");
    }

}

sub GetRegistrationData
{
    my ($buildFunction) = @_;

    return {
        linuxbuildfunction => $buildFunction,
        options => {
            "codegen" => ["debug", "release"],
            "incremental" => [0, 1]
        },
        executable => "Unity",
    };
}

sub Register
{
    if ($^O ne 'linux')
    {
        # The target is not Linux at all
        return;
    }

    # Check the architecture
    chomp(my $arch = `uname -m`);

    # Are we are building for 32bit or 64bit arch?
    if ((index($arch, "x86") == -1) && (index($arch, "i686") == -1))
    {
        # The $arch is none of the known architectures;
        # it is neither x86, nor x86_64, nor i686, which
        # means that the Editor is not supported on that
        # particular platform, so we save a lot of grief
        # and just don't register the target.
        return;
    }

    Main::RegisterTarget("LinuxEditor", GetRegistrationData(\&BuildLinuxEditor));
}

sub PrepareLinuxEditorResources
{
    my ($root, $targetPath) = @_;

    PrepareExternalDependency("Runtime/Resources");
    LinkFileOrDirectory("$root/Runtime/Resources/builds/unity default resources", "$targetPath/Data/Resources/");
}

sub BuildLinuxEditor
{
    my ($root, $targetPath, $options) = @_;

    if (!$options->{incremental})
    {
        Cleanup();
    }

    CheckInstallSDK($root);

    my $platform = 'linux64';    # 32-bit editor is no longer supported
    PrepareEditor($root, $targetPath, $options);

    # Temporary, we need to clear out the FBXSDK md5
    print("Removing FBXSDK md5 to avoid weird failures introduced by platform filter mismatch in UnpackBuildZips.\n");
    print("This should be removed as soon as all the branches have the fix.\n");
    my $removed = unlink('External/FBXSDK/builds.zip.last_extracted_md5');

    print("Removed $removed file.\n");

    # Temporary, hackish workaround for cli script's hackish workaround to our hackish handling of cross-os name collision
    system('ln', '-sf', 'bin', "$targetPath/Data/Mono/bin-linux32");
    system('ln', '-sf', 'bin', "$targetPath/Data/Mono/bin-linux64");

    # Build editor and tools (Binary2Text/WebExtract are not used by editor itself, and are not setup as dependency; but we need
    # them to be included in the editor distribution for end users).
    Jam($root, 'LinuxEditor', $options->{codegen}, $platform, $options->{incremental}, 'Binary2Text', 'WebExtract');

    ProgressMessage("Building legal.txt...");
    BuildLicensesFile("$targetPath/Data/Resources/legal.txt") and die "legal.txt build failed\n";

    PackageAndSendResultsToBuildServer($targetPath, "LinuxEditor", ".hg");
}

sub PrepareEditor
{
    my ($root, $targetPath, $options, $skipGeneration) = @_;
    my $platform = 'linux64';    # 32-bit editor is no longer supported

    my $app = $targetPath;
    rmtree("$app/Data");

    if (!$skipGeneration)
    {
        GenerateUnityConfigure();
    }

    # Link Resources
    mkpath("$app/Data/Resources") or croak("Failed to make path $app/Data/Resources");

    PrepareLinuxEditorResources($root, $app);

    LinkFileOrDirectory("External/AssetStore/offline.html", "$app/Data/Resources");
    LinkFileOrDirectory("External/AssetStore/loader.html", "$app/Data/Resources");

    LinkFileOrDirectory("Editor/Resources/Common/ScriptTemplates", "$app/Data/Resources/");
    LinkFileOrDirectory("Editor/Resources/Common/Layouts", "$app/Data/Resources/");

    # Link Frameworks
    mkpath("$app/Data/Managed") or croak("Failed to make path $app/Data/Managed");
    my $destination = "$app/Data";
    my $libs = 'libs';
    if ('linux64' eq $platform)
    {
        $libs = 'libs64';
    }

    my @monos = ('Mono');

    for my $mono (@monos)
    {
        mkpath("$destination/$mono/bin") or croak("Failed to make path $destination/$mono/bin");
        my @monoBinFiles = <$root/External/$mono/builds/monodistribution/bin/*>;
        my $monoBinFile;
        foreach $monoBinFile (@monoBinFiles)
        {
            UCopy($monoBinFile, "$destination/$mono/bin/");
        }

        LinkFileOrDirectory("$root/External/$mono/builds/monodistribution/lib", "$destination/$mono/lib");
        LinkFileOrDirectory("$root/External/$mono/builds/monodistribution/etc", "$destination/$mono/etc");
    }

    rmtree("$destination/MonoEmbedRuntime");
    LinkFileOrDirectory("$root/External/Mono/builds/embedruntimes/$platform", "$destination/MonoEmbedRuntime");

    # Link Tools
    mkpath("$app/Data/Tools");

    UCopy("Editor/Tools/AssetImporting/MayaImporter/FBXMayaExport.mel", "$app/Data/Tools");
    UCopy("Editor/Tools/AssetImporting/MayaImporter/FBXMayaExport5.mel", "$app/Data/Tools");
    UCopy("Editor/Tools/AssetImporting/MayaImporter/FBXMayaMain.mel", "$app/Data/Tools");
    UCopy("Editor/Tools/AssetImporting/BlenderImporter/Unity-BlenderToFBX.py", "$app/Data/Tools");
    UCopy("Editor/Tools/AssetImporting/ModoToUnity.py", "$app/Data/Tools");
    UCopy("Tools/PermissionsVerifier/verify_permissions", "$app/Data/Tools");
    UCopy("Tools/AutoQuitter/Unix/auto_quitter", "$app/Data/Tools");
    mkpath("$root/build/LinuxEditor/Data/Tools");

    # LinkFileOrDirectory("External/Umbra/toolchain/bin/osx32/libumbrapvs.dylib", "$app/Contents/Frameworks");
    # LinkFileOrDirectory("External/Umbra/toolchain/bin/osx32/umbrapvsprocess", "$app/Contents/Tools");

    # Tools required for specific build targets

    # iPhone
    my $buildTargetsTools = "$app/Data/BuildTargetTools";

    # mkpath "$buildTargetsTools/iPhonePlayer";
    # system ("chmod a+x External/Mono/builds/crosscompiler/iphone/mono-xcompiler");
    # LinkFileOrDirectory ("External/Mono/builds/crosscompiler/iphone/mono-xcompiler", "$buildTargetsTools/iPhonePlayer");
    # #LinkFileOrDirectory("Tools/iphone/texturetool", "$app/Contents/Tools");
    # LinkFileOrDirectory("Tools/iPhone/MonoLinker/Mono.Cecil.dll", "$buildTargetsTools/iPhonePlayer");
    # # LinkFileOrDirectory("Tools/iPhone/MonoLinker/link.xml", "$buildTargetsTools/iPhonePlayer");
    # LinkFileOrDirectory("Tools/iPhone/MonoLinker/mono-cil-strip.exe", "$buildTargetsTools/iPhonePlayer");

    # Android build target tools
    mkpath("$buildTargetsTools/AndroidPlayer") or croak("Failed to make path $buildTargetsTools/AndroidPlayer");

    # LinkFileOrDirectory ("Tools/Android/Mono/link.xml", "$buildTargetsTools/AndroidPlayer");
    # LinkFileOrDirectory("External/Compression/lzma/lzma", "$app/Contents/Tools");
}

1;
