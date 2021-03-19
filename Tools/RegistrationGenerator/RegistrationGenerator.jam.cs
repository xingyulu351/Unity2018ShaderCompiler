using System;
using System.Collections.Generic;
using JamSharp.Runtime;
using NiceIO;
using System.Linq;
using Bee.Core;
using Bee.CSharpSupport;
using Bee.DotNet;
using Tools.BindingsGenerator;
using Unity.BuildSystem;
using Unity.BuildSystem.CSharpSupport;
using Unity.BuildTools;

namespace Tools.RegistrationGenerator
{
    class RegistrationGeneratorTool : CSharpTool
    {
        RegistrationGeneratorTool()
        {
        }

        protected override DotNetAssembly DotNetAssembly()
        {
            var lib = new CSharpProgram
            {
                Path = "artifacts/BuildTools/RegistrationGenerator/Unity.RegistrationGenerator.dll",
                PrebuiltReferences = { new NPath("External/Unity.Cecil/builds/lib/net40/Unity.Cecil.dll") },
                References = { CommonTools.Instance() },
                Sources =
                {
                    "Tools/RegistrationGenerator/Unity.RegistrationGenerator",
                    "Modules/SharedInternals/Attributes.cs",
                    "Modules/SharedInternals/BindingsAttributes.cs",
                    "Tools/Unity.Options/Unity.Options",
                }
            };

            var exe = new CSharpProgram
            {
                Path = "artifacts/BuildTools/RegistrationGenerator/RegistrationGenerator.exe",
                PrebuiltReferences = { new NPath("External/Unity.Cecil/builds/lib/net40/Unity.Cecil.dll") },
                References = { lib },
                Sources = { "Tools/RegistrationGenerator/RegistrationGenerator" }
            };

            var solutionFile = new SolutionFileBuilder().WithProgram(exe, lib).Complete();
            ProjectFilesTarget.DependOn(solutionFile.Path);
            return exe.SetupDefault();
        }

        public NPath SetupInvocation(NPath outputFile, IEnumerable<NPath> inputAssemblies, string moduleName, string assembliesPath)
        {
            return SetupInvocation(new JamTarget(outputFile), new JamList(inputAssemblies), moduleName, assembliesPath).Path;
        }

        public JamTarget SetupInvocation(JamTarget outputFile, JamList inputAssemblies, string moduleName, string assembliesPath)
        {
            if (moduleName == "SharedInternals")
                throw new ArgumentException("The registration generator tool should not run on the shared internals module. This is because the tool itself" +
                    " actually has a dependency on the coremodule to read some attributes from. If you run ClassRegistration on sharedinternals module," +
                    " and you get unlucky with scheduling, the tool will try to read coremodule, while that doesnt exist yet. There's no classes in SharedInternals module" +
                    "to register anyway, so not running the generator on it is not a problem");

            var monoApiPath = Paths.UnityRoot.Combine("External/MonoBleedingEdge/builds/monodistribution/lib/mono/4.5-api");
            var args = new List<string>
            {
                moduleName.InQuotes(),
                inputAssemblies.GetBoundPath().InQuotes().SeparateWithSpace(),
                $"{assembliesPath};{monoApiPath}",
                outputFile.GetBoundPath().InQuotes(),
            };

            base.SetupInvocation(outputFile, inputAssemblies, args);

            return outputFile;
        }

        protected override bool SupportsResponseFile => true;

        static RegistrationGeneratorTool s_Instance;
        public static RegistrationGeneratorTool Instance() => s_Instance ?? (s_Instance = new RegistrationGeneratorTool());
    }

    class RegistrationGenerator : ConvertedJamFile
    {
        internal static JamList RegistrationGenerator_Files(JamList PARENT, JamList TARGET, string moduleName = "Core", JamList parentTargetName = null)
        {
            if (ProjectFiles.IsRequested)
                return JamList();

            JamList UnprocessedTarget = BindingsGenerator.BindingsGenerator.BindingsGenerator_UnprocessedAssemblyTargetFor(TARGET, Vars.targetdll);
            JamList outpath = $"artifacts/{parentTargetName??PARENT}/modules/{moduleName}/ClassRegistrations.gen.cpp";
            JamList gen_path = JamList();

            if (moduleName != "SharedInternals")
            {
                gen_path = outpath.SetGrist(PARENT);
                RegistrationGeneratorTool.Instance().SetupInvocation(gen_path, UnprocessedTarget, moduleName,
                    TARGET.GetBoundPath().GetDirectory().Single());
            }

            return gen_path;
        }
    }
}
