using Bee.Core;
using Bee.CSharpSupport;
using Bee.DotNet;
using JamSharp.Runtime;
using NiceIO;
using Projects.Jam;
using Unity.BuildSystem;
using Unity.BuildSystem.CSharpSupport;
using Unity.BuildTools;

namespace Tools.BindingsGenerator
{
    class UnsafeUtilityPatcherTool : CSharpTool
    {
        protected override DotNetAssembly DotNetAssembly()
        {
            var p = new CSharpProgram
            {
                Path = EditorFiles.ToolsPath.RelativeTo(Paths.UnityRoot).Combine("UnsafeUtilityPatcher/UnsafeUtilityPatcher.exe"),
                Sources =
                {
                    "Tools/UnsafeUtilityPatcher/Managed/Program.cs",
                    "Tools/Unity.Options/Unity.Options/Options.cs"
                },
                PrebuiltReferences = {Cecil.Current40AllRefs}
            };
            return p.SetupDefault();
        }

        public JamTarget SetupInvocation(JamTarget outputFile, JamTarget inputFile)
        {
            return base.SetupInvocation(
                outputFile,
                inputFile,
                new[] {$"--assembly={inputFile.GetBoundPath().InQuotes()}", $"--output={outputFile.GetBoundPath().InQuotes()}"});
        }

        static UnsafeUtilityPatcherTool _instance;
        public static UnsafeUtilityPatcherTool Instance => _instance ?? (_instance = new UnsafeUtilityPatcherTool());
    }
}
