using System;
using System.Collections.Generic;
using System.Linq;
using External.Compression.lz4;
using Modules;
using NiceIO;
using Projects.Jam;
using Unity.BuildSystem.NativeProgramSupport;
using Unity.BuildTools;
using static Unity.BuildSystem.NativeProgramSupport.NativeProgramConfiguration;
using External.TextureCompressors.Basis;

namespace Unity.BuildSystem.V2
{
    public class CoreModule : ModuleBase
    {
        public CoreModule()
        {
            LegacyBindings.AddRange(ConvertedJamFile.Vars.RUNTIME_BINDINGS.Select(e => new NPath(e)));
            Cs.Add(ConvertedJamFile.Vars.RUNTIME_EXPORT_SRCS.Select(e => new NPath(e)));

            PrebuiltLibraries.Add(c => IsMac(c) && c.ToolChain.Architecture is x64Architecture,
                $"External/libcurl/builds/macosx64/lib/libcurl.a",
                $"External/OpenSSL/builds/macosx64/lib/libcrypto.a",
                $"External/OpenSSL/builds/macosx64/lib/libssl.a"
            );

            var enlightenLibs = new[]
            {
                "Enlighten3",
                "Enlighten3HLRT",
                "GeoBase",
                "GeoCore",
                "ZLib"
            };
            PrebuiltLibraries.Add(IsMac, enlightenLibs.Select(n => new NPath($"External/Enlighten/builds/Lib/OSX-Release/lib{n}.a")));

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

            IncludeDirectories.Add(c => IsMac(c) || IsLinux(c) || IsWindows(c) || IsAndroid(c) || IsIosOrTvos(c) || IsUWP(c),
                "External/Enlighten/builds/Include",
                "External/Enlighten/builds/LibSrc"
            );
            IncludeDirectories.Add(c => IsMac(c) || IsLinux(c),
                "External/OpenSSL/builds/include",
                "External/Audio/libogg/include"
            );

            PrebuiltLibraries.Add(IsLinux, c => new NPath[]
            {
                $"External/libcurl/builds/linux{c.ToolChain.Architecture.Bits}/lib/libcurl.a",
                $"External/OpenSSL/builds/linux{c.ToolChain.Architecture.Bits}/lib/libssl.a",
                $"External/OpenSSL/builds/linux{c.ToolChain.Architecture.Bits}/lib/libcrypto.a"
            });

            PrebuiltLibraries.Add(IsLinux, c => enlightenLibs.Select(n => new NPath($"External/Enlighten/builds/Lib/LINUX{c.ToolChain.Architecture.Bits}_GCC/lib{n}.a")));
            PrebuiltLibraries.Add(c => IsLinux(c) && !c.HeadlessPlayer,
                "GLU",
                "GL"
            );

            PrebuiltLibraries.Add(IsWindows, c => enlightenLibs.Select(l =>
                new NPath($"External/Enlighten/builds/Lib/WIN{c.ToolChain.Architecture.Bits}_2015ST/{l}" +
                    (c.CodeGen == CodeGen.Debug ? "_u" : String.Empty) + ".lib")));

            ManagedPrebuiltReference.Add(Paths.UnityRoot.Combine("External/Mono/builds/monodistribution/lib/mono/2.0/Mono.Security.dll"));

            Dependencies.Add("SharedInternals");

            // pogostubs stuff is only used on PC (Win/Mac/Linux)
            Cpp.Add(c => c.ToolChain.Architecture is x64Architecture && (IsWindows(c) || IsMac(c) || IsLinux(c)), "External/pogostubs/pogo_x64.asm");
            Cpp.Add(c => c.ToolChain.Architecture is x86Architecture && (IsWindows(c) || IsMac(c) || IsLinux(c)), "External/pogostubs/pogo_x86.asm");

            StaticLibraries.Add(c => c.EnableUnitTests, UnitTestPlusPlus.NativeProgram);
            StaticLibraries.Add(lz4.LZ4NativeProgramLibrary);
            StaticLibraries.Add(External.draco.draco.GetLibs());
            StaticLibraries.Add(libbasis.LibBasisNativeProgram);

        }
    }

    static class Extensions
    {
        public static IEnumerable<ModuleBase> OrderByDependencies(this IEnumerable<ModuleBase> modules)
        {
            var processed = new List<ModuleBase>();
            var unprocessed = new List<ModuleBase>(modules);
            while (unprocessed.Any())
            {
                var clone = new List<ModuleBase>(unprocessed);
                bool processedOne = false;
                foreach (var module in clone)
                {
                    if (!module.Dependencies.ForAny().All(d => processed.Any(m => m.Name == d)))
                        continue;
                    processedOne = true;
                    processed.Add(module);
                    unprocessed.Remove(module);
                }
                if (!processedOne)
                    throw new ArgumentException("Cyclic dependency in OrderByDependencies");
            }

            return processed;
        }
    }
}
