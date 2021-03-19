using System;
using System.Linq;
using JamSharp.Runtime;
using External.Jamplus.builds.bin.modules;
using External.Jamplus.builds.bin;
using NiceIO;
using Unity.BuildSystem;
using Unity.BuildSystem.CSharpSupport;
using Unity.BuildTools;

namespace Tools.rrw
{
    class ReferenceRewriter : ConvertedJamFile
    {
        internal static void TopLevel()
        {
            Jambase.SubDir("TOP");
            JamList common_refs = Cecil.Current40AllRefs;
            JamList rrwSources = JamList(
                new NPath("Tools/rrw").Files("*.cs"),
                new NPath("Tools/rrw/Properties").Files("*.cs"),
                "External/JsonParsers/MiniJson/MiniJSON.cs",
                "Editor/Mono/Scripting/Compilers/NuGetPackageResolver.cs",
                "Tools/Unity.NuGetPackageResolver/NuGetAssemblyResolver.cs",
                "Tools/Unity.NuGetPackageResolver/SearchPathAssemblyResolver.cs");
            rrwSources.Assign(rrwSources.Exclude(".jam.cs"));
            NPath rrwTargetDir = $"{Vars.TOP}/build/MetroSupport/Tools/rrw";

            if (Vars.TargetPlatformIsXboxOne)
                rrwTargetDir = $"{Vars.TOP}/build/XboxOnePlayer/Tools/rrw";

            NPath rrwExe = $"{rrwTargetDir}/rrw.exe";
            copydirectory.CopyDirectory("ReferenceRewriter", rrwTargetDir, Cecil.Current40);
            Jamrules.BuildAssembly("ReferenceRewriter", "roslyn4.6", rrwExe, rrwSources, JamList(), JamList(), common_refs);
        }

        public static Lazy<CSharpProgram> Program => new Lazy<CSharpProgram>(SetupWithCSharpProgram);

        private static CSharpProgram SetupWithCSharpProgram()
        {
            var rrw = new CSharpProgram();
            var name = "ReferenceRewriter";
            rrw.Path = new NPath($"artifacts/{name}/rrw.exe");

            rrw.Sources.Add(new NPath("Tools/rrw").Files("*.cs").Where(f => !f.ToString().EndsWith("jam.cs")));
            rrw.Sources.Add(
                new NPath("Tools/rrw/Properties").Files("*.cs").Where(f => !f.ToString().EndsWith("jam.cs")));
            rrw.Sources.Add("External/JsonParsers/MiniJson/MiniJSON.cs",
                "Editor/Mono/Scripting/Compilers/NuGetPackageResolver.cs",
                "Tools/Unity.NuGetPackageResolver/NuGetAssemblyResolver.cs",
                "Tools/Unity.NuGetPackageResolver/SearchPathAssemblyResolver.cs");

            rrw.PrebuiltReferences.Add(Cecil.Current40AllRefs);
            rrw.CopyReferencesNextToTarget = true;
            return rrw;
        }
    }
}
