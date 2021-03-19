using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using Bee.Core;
using JamSharp.Runtime;
using NiceIO;

public class BuildZipArtifact : IBackendRegistrable
{
    public BuildZipArtifact(NPath zipFile, bool useArtifactsDirToUnpack = false)
    {
        if (!zipFile.IsRelative)
            throw new ArgumentException($"The builds.zip file path \"{zipFile}\" is an absolute path. Builds.zip paths must be relative to the checkout root.");

        ZipFile = zipFile;

        if (useArtifactsDirToUnpack)
        {
            // Figure out a reasonable shortname for the unpack path
            // External/blah/builds.zip -> blah
            // External/subdir/blah/builds.zip -> subdir-blah
            // External/blah/customname.zip -> blah-customname
            string shortName;
            if (zipFile.IsChildOf("External"))
            {
                shortName = zipFile.Parent.RelativeTo("External").ToString(SlashMode.Forward).Replace('/', '-');
            }
            else if (zipFile.IsChildOf("PlatformDependent"))
            {
                var platformName = zipFile.RecursiveParents.First(d => d.Parent == "PlatformDependent").FileName;
                var platformExternalsPath = zipFile.RecursiveParents.First(d => d.FileName == "External");
                shortName =
                    $"{platformName}-{zipFile.Parent.RelativeTo(platformExternalsPath).ToString(SlashMode.Forward).Replace('/', '-')}";
            }
            else
            {
                shortName = zipFile.Parent.ToString(SlashMode.Forward).Replace('/', '-');
            }
            if (zipFile.FileNameWithoutExtension != "builds")
                shortName += $"-{zipFile.FileNameWithoutExtension}";

            Path = $"artifacts/BuildZips/{shortName}";
            ArtifactVersionFile = Path.Combine(".last_extracted_md5");
        }
        else
        {
            Path = zipFile.Parent.Combine("builds");
            ArtifactVersionFile = zipFile + ".last_extracted_md5";
        }
    }

    public NPath ZipFile { get; }

    public NPath Path { get; }

    // This is the `.last_extracted_md5` file - it's a slightly odd name but we're trying to make it easy to migrate to
    // Stevedore soon, and StevedoreArtifact.ArtifactVersionFile is the closest analog to this
    public NPath ArtifactVersionFile { get; }

    public void Register(IBackend backend)
    {
        var targetDir = Path.RelativeTo(ZipFile.Parent);

        backend.AddAction("UnpackBuildsZip",
            new[] {ArtifactVersionFile},
            new[] {ZipFile},
            "perl Tools/Build/PrepareBuildsZip.pl",
            new[] {ZipFile.ToString(SlashMode.Native), targetDir.ToString(), "0", ArtifactVersionFile.ToString()}
        );

        backend.AddImplicitDependenciesToAllActionsWithInputsThatLiveIn(Path, ArtifactVersionFile);
    }
}

public class BuildZips
{
    public static BuildZips Instance { get; private set; }

    public BuildZipArtifact FSBTool { get; } = new BuildZipArtifact("External/Audio/fsbtool/fsbtool.zip");
    public BuildZipArtifact DirectX { get; } = new BuildZipArtifact("External/DirectX/builds.7z");
    public BuildZipArtifact Enlighten { get; } = new BuildZipArtifact("External/Enlighten/builds.7z");
    public BuildZipArtifact FBXSDK { get; } = new BuildZipArtifact("External/FBXSDK/builds.7z");
    public BuildZipArtifact FMOD { get; } = new BuildZipArtifact("External/FMOD/builds.zip");
    public BuildZipArtifact FreeImage { get; } = new BuildZipArtifact("External/FreeImage/builds.zip");
    public BuildZipArtifact NSIS { get; } = new BuildZipArtifact("External/Installers/NSIS/builds.zip");
    public BuildZipArtifact MSBuild15 { get; } = new BuildZipArtifact("External/Microsoft/MSBuild15/builds.7z");
    public BuildZipArtifact DotNetFramework35 { get; } = new BuildZipArtifact("External/Microsoft/dotnet35/builds.zip");
    public BuildZipArtifact DotNetFramework45 { get; } = new BuildZipArtifact("External/Microsoft/dotnet45/builds.zip");
    public BuildZipArtifact VCRedist2010 { get; } = new BuildZipArtifact("External/Microsoft/vcredist2010/builds.zip");
    public BuildZipArtifact VCRedist2013 { get; } = new BuildZipArtifact("External/Microsoft/vcredist2013/builds.zip");
    public BuildZipArtifact VCRedist2015 { get; } = new BuildZipArtifact("External/Microsoft/vcredist2015/builds.zip");
    public BuildZipArtifact NDecompiler { get; } = new BuildZipArtifact("External/NDecompiler/builds.zip");
    public BuildZipArtifact NRefactory { get; } = new BuildZipArtifact("External/NRefactory/builds.7z");
    public BuildZipArtifact Nasm { get; } = new BuildZipArtifact("External/Nasm/builds.zip");
    public BuildZipArtifact OpenCL { get; } = new BuildZipArtifact("External/OpenCL/builds.zip");
    public BuildZipArtifact OpenSSL { get; } = new BuildZipArtifact("External/OpenSSL/builds.zip");
    public BuildZipArtifact FlexAndBison { get; } = new BuildZipArtifact("External/ParserGenerators/FlexBison/builds.7z");
    public BuildZipArtifact PhysX3 { get; } = new BuildZipArtifact("External/PhysX3/builds.7z");
    public BuildZipArtifact RadeonRays { get; } = new BuildZipArtifact("External/RadeonRays/builds.zip");
    public BuildZipArtifact Shouldly { get; } = new BuildZipArtifact("External/Shouldly/builds.zip");
    public BuildZipArtifact SpeedTree { get; } = new BuildZipArtifact("External/SpeedTree/builds.zip");
    public BuildZipArtifact DXT_ATI { get; } = new BuildZipArtifact("External/TextureCompressors/DXT_ATI/builds.zip");
    public BuildZipArtifact ISPCTextureCompressor { get; } = new BuildZipArtifact("External/TextureCompressors/ISPCTextureCompressor/builds.7z");
    public BuildZipArtifact PVRTextureTool { get; } = new BuildZipArtifact("External/TextureCompressors/PVRTextureTool/builds.zip");
    public BuildZipArtifact ETCCompress { get; } = new BuildZipArtifact("External/TextureCompressors/etccompress/builds.7z");
    public BuildZipArtifact UnityVCSPlugins { get; } = new BuildZipArtifact("External/VersionControl/builds.zip");
    public BuildZipArtifact WinPIXEventRuntime { get; } = new BuildZipArtifact("External/WinPIXEventRuntime/builds.7z");
    public BuildZipArtifact Windows10Metadata { get; } = new BuildZipArtifact("External/Windows10/metadata/builds.zip");
    public BuildZipArtifact Yasm { get; } = new BuildZipArtifact("External/Yasm/builds.zip");
    public BuildZipArtifact LibCurl { get; } = new BuildZipArtifact("External/libcurl/builds.zip");
    public BuildZipArtifact PLCrashReporterLib { get; } = new BuildZipArtifact("PlatformDependent/OSX/External/PLCrashReporter/builds.zip");
    public BuildZipArtifact UsymTool { get; } = new BuildZipArtifact("External/UsymTool/builds.zip");
    public BuildZipArtifact RuntimeResources { get; } = new BuildZipArtifact("Runtime/Resources/builds.zip", useArtifactsDirToUnpack: true);
    public BuildZipArtifact VuforiaSDK { get; } = new BuildZipArtifact("External/XR/vuforia/builds.zip");
    public BuildZipArtifact Gogs { get; } = new BuildZipArtifact("Tests/Unity.IntegrationTests/ExternalServices/Gogs/builds.zip");

    public BuildZips()
    {
        if (Instance != null)
            throw new InvalidOperationException(
                "You're trying to create a second instance of BuildZips, when one has already been created. Only one instance should ever be created in a single run of the buildprogram. To access the existing instance, use BuildZips.Instance.");

        Instance = this;

        foreach (var property in this.GetType().GetProperties(BindingFlags.Instance | BindingFlags.Public))
        {
            var zip = property.GetValue(this) as BuildZipArtifact;
            if (zip == null)
                continue;

            Backend.Current.Register(zip);
        }
    }

    // In the past, all of the builds.zip unzipping was done by a totally separate buildsystem invocation called Pass1.
    // It unzipped pretty much everything, and then the actual build would just use the unzipped files without making
    // sure a dependency was listed in the depgraph.
    //
    // Now we have transitioned some of these zips out of Pass1 and into the main buildgraph, but we still do not know
    // what the actual dependencies on them are - we know the explicit dependencies, where a file in one of the zips is
    // listed as an input to a build node, and we set those up, but we don't know implicit dependencies (i.e. included
    // header files).
    //
    // Sorting that out is going to take a while, so for now we just recreate the set of things Pass1 unpacked as a
    // list of dependencies that will be added to _all_ things that might have implicit dependencies (i.e. C++
    // compilation). Over time we'll gradually remove things from this list and have the programs that actually need
    // them explicitly list them as dependencies instead.
    public IEnumerable<BuildZipArtifact> LegacyPass1ZipsToDependOn
    {
        get
        {
            yield return FMOD;
            yield return PhysX3;
            yield return Nasm;
            yield return Yasm;
            yield return NRefactory;
            yield return DirectX;
            yield return WinPIXEventRuntime;
            yield return FBXSDK;
            yield return FSBTool;

            if (ConvertedJamFile.Vars.INCLUDE_GI == "1")
            {
                yield return Enlighten;
                yield return RadeonRays;

                // OpenCL 1.2 is present on macOS already.
                if (ConvertedJamFile.Vars.TargetPlatformIsWindows)
                    yield return OpenCL;
            }

            yield return ETCCompress;
            yield return ISPCTextureCompressor;
            yield return FreeImage;
            yield return OpenSSL;
            yield return LibCurl;
            yield return SpeedTree;
            yield return PVRTextureTool;
            yield return UnityVCSPlugins;
            yield return DXT_ATI;
            yield return MSBuild15;
            yield return NDecompiler;
            yield return FlexAndBison;

            if (ConvertedJamFile.Vars.TargetPlatformIsWindows)
                yield return Windows10Metadata;

            if (ConvertedJamFile.Vars.TargetPlatformIsWindows32)
            {
                yield return NSIS;
                yield return VCRedist2010;
                yield return VCRedist2013;
                yield return VCRedist2015;
                yield return DotNetFramework35;
                yield return DotNetFramework45;
            }

            if (GlobalVariables.Singleton["UNPACK_FOR_TARGET"] == "BuildSystemTests")
                yield return Shouldly;
        }
    }
}
