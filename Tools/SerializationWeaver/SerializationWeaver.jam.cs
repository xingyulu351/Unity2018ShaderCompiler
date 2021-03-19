using System;
using JamSharp.Runtime;
using static JamSharp.Runtime.BuiltinRules;
using External.Jamplus.builds.bin.modules;
using External.Jamplus.builds.bin;
using NiceIO;
using Projects.Jam;
using Unity.BuildSystem;
using Unity.BuildSystem.CSharpSupport;

namespace Tools.SerializationWeaver
{
    class SerializationWeaver : ConvertedJamFile
    {
        internal static void TopLevel()
        {
            Jambase.SubDir("TOP");
            SubIncludeRunner.RunOnce(Extensions.Networking.UNetWeaver.TopLevel);
            JamList common_refs = JamList(
                "UnityCecilTools",
                "UnitySerializationLogic");     //   DefaultEngineDll
            common_refs.Append(Cecil.Current35AllRefs);

            JamList serialization_weaver_refs = JamList(common_refs, "SerializationWeaverCommon");
            JamList sw_refs = JamList(serialization_weaver_refs, "SerializationWeaverDll", "UNetWeaver");

            NPath weaverTargetDir = "";

            if (Vars.TargetPlatformIsMetro)
            {
                weaverTargetDir = $"{Vars.TOP}/build/MetroSupport/Tools/SerializationWeaver";
            }
            else if (Vars.PLATFORM.IsIn("macosx64", "macosx", "win32", "win64", "win", "linux32", "linux64"))
            {
                // EditorFiles.frameworksPath caused codesign certification errors after building MacEditor installer (case 819351)
                weaverTargetDir = $"{EditorFiles.ToolsPath}/SerializationWeaver";
            }

            Jamrules.BuildAssemblyFromFolderWithCSFiles(
                "SerializationWeaverCommon",
                "roslyn4.6",
                $"{Vars.TOP}/build/temp/Unity.SerializationWeaver.Common.dll",
                $"{Vars.TOP}/Tools/SerializationWeaver/Unity.SerializationWeaver.Common",
                common_refs);
            Depends("SerializationWeaverCommon", JamList("UnitySerializationLogic", "UnityCecilTools"));
            Jamrules.BuildAssemblyFromFolderWithCSFiles(
                "SerializationWeaverDll",
                "roslyn4.6",
                $"{Vars.TOP}/build/temp/Unity.SerializationWeaver.dll",
                $"{Vars.TOP}/Tools/SerializationWeaver/Unity.SerializationWeaver",
                serialization_weaver_refs);
            Depends("SerializationWeaverDll", JamList("UnitySerializationLogic", "UnityCecilTools", "SerializationWeaverCommon"));
            JamList serializationWeaverSources = JamList(
                new NPath("Tools/SerializationWeaver/sw").Files("*.cs"),
                new NPath("Tools/SerializationWeaver/sw/Configuration").Files("*.cs"),
                new NPath("Tools/SerializationWeaver/sw/Properties").Files("*.cs"),
                "External/JsonParsers/MiniJson/MiniJSON.cs",
                "Editor/Mono/Scripting/Compilers/NuGetPackageResolver.cs",
                "Tools/Unity.NuGetPackageResolver/NuGetAssemblyResolver.cs",
                "Tools/Unity.NuGetPackageResolver/SearchPathAssemblyResolver.cs");
            Jamrules.BuildAssembly(
                "SerializationWeaver",
                "roslyn4.6",
                Combine(
                    weaverTargetDir,
                    "/SerializationWeaver.exe"),
                serializationWeaverSources,
                JamList(),
                JamList(),
                sw_refs);

            Depends("SerializationWeaver", JamList("UnitySerializationLogic", "UnityCecilTools", "SerializationWeaverCommon", "SerializationWeaverDll"));
            foreach (JamList @ref in sw_refs.ElementsAsJamLists)
            {
                copyfile.CopyFile("SerializationWeaver", Combine(weaverTargetDir, "/", @ref.GetBoundPath().BaseAndSuffix()), @ref);
            }
        }

        public static Lazy<CSharpProgram> Program => new Lazy<CSharpProgram>(SetupWithCSharpProgram);

        private static CSharpProgram SetupWithCSharpProgram()
        {
            var swc = new CSharpProgram();
            swc.Path = new NPath($"artifacts/SerializationWeaver/Unity.SerializationWeaver.Common.dll");
            swc.Sources.Add(new NPath("Tools/SerializationWeaver/Unity.SerializationWeaver.Common").Files("*cs"));
            swc.PrebuiltReferences.Add(new JamTarget("UnityCecilTools").GetBoundPath(),
                new JamTarget("UnitySerializationLogic").GetBoundPath());
            swc.PrebuiltReferences.Add(Cecil.Current35AllRefs);

            var swdll = new CSharpProgram();
            swdll.Path = "artifacts/SerializationWeaver/Unity.SerializationWeaver.dll";
            swdll.Sources.Add(new NPath("Tools/SerializationWeaver/Unity.SerializationWeaver").Files("*cs"));
            swdll.References.Add(swc);
            swdll.PrebuiltReferences.Add(swc.PrebuiltReferences.ForAny());

            var sw = new CSharpProgram();
            sw.Path = "artifacts/SerializationWeaver/SerializationWeaver.exe";
            sw.Sources.Add(new NPath("Tools/SerializationWeaver/sw").Files("*.cs"));

            sw.Sources.Add(new NPath("Tools/SerializationWeaver/sw/Configuration").Files("*.cs"));
            sw.Sources.Add(new NPath("Tools/SerializationWeaver/sw/Properties").Files("*.cs"));
            sw.Sources.Add(
                "External/JsonParsers/MiniJson/MiniJSON.cs",
                "Editor/Mono/Scripting/Compilers/NuGetPackageResolver.cs",
                "Tools/Unity.NuGetPackageResolver/NuGetAssemblyResolver.cs",
                "Tools/Unity.NuGetPackageResolver/SearchPathAssemblyResolver.cs");
            sw.References.Add(swdll, swc);
            sw.PrebuiltReferences.Add(new JamTarget("UNetWeaver").GetBoundPath());
            sw.PrebuiltReferences.Add(swdll.PrebuiltReferences.ForAny());

            return sw;
        }
    }
}
