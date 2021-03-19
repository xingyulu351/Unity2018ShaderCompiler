using System.Collections.Generic;
using System.Linq;
using Bee.DotNet;
using JamSharp.Runtime;
using NiceIO;
using Unity.BuildSystem;
using Unity.BuildSystem.CSharpSupport;
using Unity.BuildTools;

namespace Tools.UnityBindingsParser.BindingsToCsAndCpp
{
    class LegacyBindingsConverterTool : CSharpTool
    {
        protected override DotNetAssembly DotNetAssembly()
        {
            var lib = new CSharpProgram
            {
                Path = "artifacts/BuildTools/LegacyBindingsParser/UnityBindingsParser.dll",
                Sources = {new NPath("Tools/UnityBindingsParser/UnityBindingsParser").Files("*.cs").Where(p => !p.FileName.EndsWith(".jam.cs"))}
            };

            var exe = new CSharpProgram
            {
                Path = "artifacts/BuildTools/LegacyBindingsParser/LegacyBindingsParser.exe",
                Sources = {new NPath("Tools/UnityBindingsParser/BindingsToCsAndCpp").Files("*.cs").Where(p => !p.FileName.EndsWith(".jam.cs"))},
                References = { lib, BindingsGenerator.CommonTools.Instance() },
                PrebuiltReferences = { NRefactory.Net40Assemblies, NUnitPaths.Framework }
            };

            ProjectFilesTarget.DependOn(new SolutionFileBuilder().WithProgram(exe, lib).Complete().Path);

            return exe.SetupDefault();
        }

        protected override string ActionName { get; }
            = "LegacyBindings_MakeCsOrCpp";

        protected override ActionsFlags ActionFlags => ActionsFlags.Together | base.ActionFlags;

        public struct Result
        {
            public JamTarget CsFile;
            public JamTarget CppFile;
        }

        public Result SetupInvocationToGenerateCsAndCpp(NPath outpath, JamTarget inputFile, string platform)
        {
            var prefix = $"{outpath}/{new NPath(inputFile.JamTargetName).FileNameWithoutExtension}Bindings.gen";
            JamList csFile = prefix + ".cs";
            JamList cppFile = prefix + ".cpp";

            List<string> commandLine = new List<string>
            {
                $"-p {platform}",
                $"-o {outpath}",
                inputFile.JamTargetName,
            };

            var inputFiles = new JamList(inputFile, "Runtime/Export/common_include", "Runtime/Export/common_structs");

            //todo: figure out why we cannot merge this to a single invocation. when generating the .cs and .cpp
            //in a single invocation, we find jam sometimes trying to invoke the action twice
            base.SetupInvocation(csFile, inputFiles, commandLine.Prepend("--nocpp"));
            base.SetupInvocation(cppFile, inputFiles, commandLine.Prepend("--nocs"));

            return new Result() {CsFile = csFile, CppFile = cppFile};
        }

        static LegacyBindingsConverterTool s_Instance;
        public static LegacyBindingsConverterTool Instance() => s_Instance ?? (s_Instance = new LegacyBindingsConverterTool());
    }

    class BindingsToCsAndCpp : ConvertedJamFile
    {
        internal static void TopLevel()
        {
            JamList uct = $"{Vars.TOP}/build/temp/Unity.CommonTools.dll";
            Jamrules.BuildAssemblyFromFolderWithCSFiles(
                "UnityCommonTools",
                "3.5",
                uct,
                $"{Vars.TOP}/Tools/Unity.CommonTools",
                JamList(),
                "System.Drawing",
                "-unsafe");
        }
    }
}
