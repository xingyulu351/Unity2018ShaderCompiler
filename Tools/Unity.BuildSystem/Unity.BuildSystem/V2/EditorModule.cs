using System.Linq;
using Bee.DotNet;
using Configuration;
using Modules;
using NiceIO;
using Unity.BuildSystem.CSharpSupport;
using static Unity.BuildSystem.NativeProgramSupport.NativeProgramConfiguration;

namespace Unity.BuildSystem.V2
{
    public class EditorModule : ModuleBase
    {
        private static CSharpProgram CecilToolsAssembly { get; }
            = new CSharpAssemblyBuilder()
                .WithPath("artifacts/Unity.CecilTools/Unity.CecilTools.dll")
                .WithSource("Tools/Unity.CecilTools")
                .WithPrebuiltReference(Cecil.Current35AllRefs)
                .WithFramework(Framework.Framework35)
                .Complete();

        private static CSharpProgram SerializationLogicAssembly { get; }
            = new CSharpAssemblyBuilder()
                .WithPath("artifacts/Unity.SerializationLogic/Unity.SerializationLogic.dll")
                .WithSource("Tools/Unity.SerializationLogic")
                .WithPrebuiltReference(Cecil.Current35AllRefs)
                .WithPrebuiltReference(CecilToolsAssembly.Path)
                .WithFramework(Framework.Framework35)
                .Complete();

        private static CSharpProgram UNetWeaverAssembly { get; }
            = new CSharpAssemblyBuilder()
                .WithPath("artifacts/Unity.UNetWeaver/Unity.UNetWeaver.dll")
                .WithSource("Extensions/Networking/Weaver")
                .WithPrebuiltReference(Cecil.Current35AllRefs)
                .WithFramework(Framework.Framework35)
                .WithUnsafe()
                .Complete();

        public EditorModule()
        {
            LegacyBindings.AddRange(ConvertedJamFile.Vars.EDITOR_BINDINGS.Select(e => new NPath(e)));

            Cs.Add(ConvertedJamFile.Vars.EDITOR_EXPORT_SRCS.Select(e => new NPath(e)));
            Cs.Add("External/JsonParsers/MiniJson/MiniJSON.cs");

            ManagedPrebuiltReference.Add(Cecil.Current35AllRefs);
            ManagedPrebuiltReference.Add(NRefactory.Net35Assembly);
            ManagedPrebuiltReference.Add(IvyPackager.DataContract.Path);
            ManagedPrebuiltReference.Add(SerializationLogicAssembly.Path);
            ManagedPrebuiltReference.Add(UNetWeaverAssembly.Path);

            ManagedPrebuiltReference.Add("External/ExCSS/builds/builds/lib/net35/ExCSS.Unity.dll");

            var monoLibraries = new NPath[]
            {
                "Boo.Lang.dll",
                "Boo.Lang.Parser.dll",
                "Boo.Lang.Compiler.dll",
                "UnityScript.dll",
                "System.Xml.Linq.dll",
            };

            var monoLibDir = new NPath("External/Mono/builds/monodistribution/lib/mono/2.0");
            ManagedPrebuiltReference.Add(monoLibraries.Select(p => monoLibDir.Combine(p)));
            ManagedPrebuiltReference.Add(IsMac, monoLibDir.Combine("Mono.Posix.dll"));

            IncludeDirectories.Add(ConvertedJamFile.Vars.RUNTIME_INCLUDES.ToNPaths());

            var fbxsdk_version = "2018.1.1";

            // Must come after Editor files to be safe where the RUNTIME_INCLUDES are defined
            IncludeDirectories.Add(
                "External/Enlighten/builds/Include",
                "External/Enlighten/builds/LibSrc",
                "External/OpenRL/builds/include",

                "External/OpenSSL/builds/include",
                $"External/FBXSDK/builds/{fbxsdk_version}/include",

                "External/Audio/common_ogg",
                "External/Audio/libvorbis/include",
                "External/Audio/libogg/include"
            );

            IncludeDirectories.Add(IsMac,
                "External/Cef/builds/include/osx64/Cef",
                "External/Quicktime/MacOS");

            Frameworks.Add(IsMac,
                "Carbon",
                "Cocoa",
                "IOKit",
                "IOSurface",
                "OpenGL",
                "QuartzCore",
                "Security",
                "SecurityFoundation",
                "SystemConfiguration"
            );

            Cpp.Add(ConvertedJamFile.Vars.EDITOR_SOURCES.ToNPaths());
            Cpp.Add(IsMac, new NPath("Editor/Platform/OSX").Files(true));
            Cpp.Add(
                "External/MeshOptimizer/forsyth.cpp",
                "External/MeshOptimizer/forsyth.h",
                "External/MeshOptimizer/ForsythTriangleListEstimate.h"
            );
            Cpp.Add(GlobalDefines.EditorPlatformDefinesLocation);

            PrebuiltLibraries.Add(IsMac,
                "External/Audio/libogg/libs/macosx64/libogg.a",
                "External/Audio/libvorbis/libs/macosx64/libvorbis.a",
                "External/theora/libs/macosx64/libtheora.a",
                "External/TextureCompressors/DXT_ATI/builds/macosx64/libs3tc.dylib",
                "External/FreeImage/builds/libfreeimage-3.17.0.dylib",
                "External/Audio/lame/lib/libmp3lame.dylib",
                "External/TextureCompressors/ISPCTextureCompressor/builds/macos/libispc_texcomp.dylib",

                $"External/FBXSDK/builds/{fbxsdk_version}/lib/clang/release/libfbxsdk.dylib",
                "External/Enlighten/builds/Lib/OSX-Release/libEnlighten3.a",
                "External/Enlighten/builds/Lib/OSX-Release/libEnlightenBake.a",
                "External/Enlighten/builds/Lib/OSX-Release/libEnlighten3HLRT.a",
                "External/Enlighten/builds/Lib/OSX-Release/libEnlightenPrecomp2.a",
                "External/Enlighten/builds/Lib/OSX-Release/libGeoBase.a",
                "External/Enlighten/builds/Lib/OSX-Release/libGeoCore.a",
                "External/Enlighten/builds/Lib/OSX-Release/libGeoRayTrace.a",
                "External/Enlighten/builds/Lib/OSX-Release/libIntelTBB.a",
                "External/Enlighten/builds/Lib/OSX-Release/libZlib.a",
                "External/OpenRL/builds/lib/macosx/OpenRL.framework/Versions/A/OpenRL",

                "External/Umbra/bin/Frameworks/libumbraoptimizer64.dylib",
                "External/FMOD/builds/macosx64/lib/libfmodex.a",
                "External/libcurl/builds/macosx64/lib/libcurl.a",
                "External/openssl/builds/macosx64/lib/libssl.dylib",
                "External/openssl/builds/macosx64/lib/libcrypto.dylib",
                "External/Cef/builds/lib/osx64/libcef_dll_wrapper.a",
                "External/Cef/builds/lib/osx64/Chromium_Embedded_Framework.framework/Chromium_Embedded_Framework",
                "External/SpeedTree/builds/Lib/MacOSX/Release/libSpeedTreeCore.a");
        }

        public override bool IsEditorModule { get; } = true;
    }
}
