using System.Collections.Generic;
using System.Linq;
using Bee.Core;
using Bee.CSharpSupport;
using Bee.DotNet;
using JamSharp.Runtime;
using NiceIO;
using Unity.BuildSystem;
using Unity.BuildSystem.CSharpSupport;
using Unity.BuildTools;

namespace Tools.BindingsGenerator
{
    class CommonTools
    {
        static CSharpProgram _instance;
        public static CSharpProgram Instance() => _instance ?? (_instance = MakeProgram());

        static CSharpProgram MakeProgram()
        {
            var p = new CSharpProgram
            {
                Path = "artifacts/Unity.CommonTools/Unity.CommonTools.dll",
                Sources = {"Tools/Unity.CommonTools"},
                SystemReferences = {"System.Drawing"},
                Unsafe = true
            };
            return p;
        }
    }

    class BindingsGeneratorTool : CSharpTool
    {
        readonly CSharpProgram _bindingsGeneratorExe;

        const string BuildDirectory = "artifacts/BuildTools/BindingsGenerator";
        const string SourceDirectory = "Tools/BindingsGenerator";

        public BindingsGeneratorTool()
        {
            var core = new CSharpProgram
            {
                Path = $"{BuildDirectory}/BindingsGenerator.Core.dll",
                Sources =
                {
                    "Tools/Unity.Options/Unity.Options",
                    $"{SourceDirectory}/Unity.BindingsGenerator.Core",
                    "Runtime/Export/Attributes.cs",
                    "Runtime/Export/Scripting/PreserveAttribute.cs",
                    "Modules/SharedInternals/Attributes.cs",
                    "Modules/SharedInternals/BindingsAttributes.cs",
                    "External/il2cpp/il2cpp/Unity.Cecil.Visitor/Visitor.cs"
                },
                References = { CommonTools.Instance() },
                PrebuiltReferences = { Cecil.Current40AllRefs, NRefactory.Net40Assemblies }
            };

            _bindingsGeneratorExe = new CSharpProgram
            {
                Path = $"{BuildDirectory}/BindingsGenerator.exe",
                Sources =
                {
                    "External/JsonParsers/MiniJson/MiniJSON.cs",
                    "Editor/Mono/Scripting/Compilers/NuGetPackageResolver.cs",
                    "Tools/Unity.NuGetPackageResolver/NuGetAssemblyResolver.cs",
                    "Tools/Unity.NuGetPackageResolver/SearchPathAssemblyResolver.cs",
                    $"{SourceDirectory}/BindingsGenerator"
                },
                References = { core },
                PrebuiltReferences = { Cecil.Current40AllRefs }
            };

            ProjectFilesTarget.DependOn(new SolutionFileBuilder().WithProgram(_bindingsGeneratorExe, core).Complete().Path);
        }

        protected override DotNetAssembly DotNetAssembly() => _bindingsGeneratorExe.SetupDefault();

        public JamTarget Target => _bindingsGeneratorExe.SetupDefault().Path;

        protected override bool IRealizeHowDangerousItIsToNotUseDefaultActionBody()
        {
            return true;
        }

        protected override bool SupportsResponseFile => true;

        //unfortunattely we need to pass output-file and patcher referecnes in the actionbody, as at the time
        //we setup the invocation, their bound path has not yet been set.
        protected override string ActionsBody =>
            $"{base.ActionsBody} --output-file=\"$(1[1]:T)\" @@(--patcher.references=$(patcher_references:TJ=,))";

        public JamTarget SetupPatchInvocation(JamTarget outputFile,
            JamTarget inputAssembly,
            JamTarget[] patcherReferences,
            string[] defines,
            ScriptingBackend scriptingBackend,
            JamList profile)
        {
            var args = new List<string>
            {
                "--patch",
                "--verify",
            };

            "patcher_references".On(outputFile).Assign(new JamList(patcherReferences));

            var sharedArgs = SharedArgsFor(inputAssembly, defines, scriptingBackend, profile);

            BuiltinRules.SetAllowedOutputSubstringsFor(outputFile, new[] {"WARNING"});
            BuiltinRules.DisallowUnexpectedOutputFor(outputFile);

            return base.SetupInvocation(
                outputFile,
                patcherReferences.Append(inputAssembly).ToArray(),
                args.Concat(sharedArgs));
        }

        static List<string> SharedArgsFor(JamTarget inputAssembly, string[] defines, ScriptingBackend scriptingBackend, JamList profile)
        {
            var sharedArgs = new List<string>
            {
                $"--assembly:{inputAssembly.BoundIfNoPath().InQuotes()}",
                $"--defines={defines.SeparateWithComma()}",
                $"--scripting-backend={scriptingBackend.ToString().ToUpper()}",
                $"--assembly-resolver=DefaultAssemblyResolver",
            };

            if (profile != null && profile.IsIn("uap"))
                sharedArgs.Add($"--codegen.reference-search-directories={Paths.UnityRoot.Combine("PlatformDependent/WinRT/External/ReferenceAssemblies/builds").ToString()}");

            return sharedArgs;
        }

        public void SetupInvocation(
            JamTarget outputCppFile,
            JamTarget outputHeaderFile,
            JamTarget inputAssembly,
            JamTarget inputFile,
            JamTarget[] codeGenReferences,
            string[] defines,
            ScriptingBackend scriptingBackend,
            JamList profile)
        {
            var args = new List<string>
            {
                $"--codegen.input-file={inputFile.BoundIfNoPath().InQuotes()}",
            };

            if (codeGenReferences.Any())
                args.Add($"--codegen.references={codeGenReferences.Select(r => r.BoundIfNoPath()).SeparateWithComma()}");

            var sharedArgs = SharedArgsFor(inputAssembly, defines, scriptingBackend, profile);

            //do not pass references to SetupInvocation because we do not want to depend on them.
            BuiltinRules.Needs(new JamList(outputCppFile, outputHeaderFile), codeGenReferences.Append(inputAssembly).Append(inputFile).ToArray());


            //todo: Enable these to start hiding bindinggeneratortool warnings
            //BuiltinRules.SetAllowedOutputSubstringsFor(outputCppFile, new[]{"WARNING"});
            //BuiltinRules.DisallowUnexpectedOutputFor(outputCppFile);

            base.SetupInvocation(
                new[] {outputCppFile, outputHeaderFile},
                new JamList(inputFile),
                args.Concat(sharedArgs));
        }

        static BindingsGeneratorTool _instance;

        public static BindingsGeneratorTool Instance() => _instance ?? (_instance = new BindingsGeneratorTool());
    }
}
