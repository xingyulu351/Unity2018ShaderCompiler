using System.Collections.Generic;
using System.Linq;
using Bee.Core;
using Bee.DotNet;
using External.Jamplus.builds.bin.modules;
using JamSharp.Runtime;
using NiceIO;
using Projects.Jam;
using Unity.BuildSystem.CSharpSupport;
using Unity.BuildTools;
using static JamSharp.Runtime.BuiltinRules;

namespace Unity.BuildSystem
{
    class GenerateNativeDependenciesForManagedCode : ConvertedJamFile
    {
        public enum FileMode
        {
            Cpp,
            Xml,
            Header
        }

        internal class CodeGeneratorTool : CSharpTool
        {
            protected override DotNetAssembly DotNetAssembly()
            {
                var p = new CSharpProgram
                {
                    Path = "artifacts/BuildTools/CodeGeneratorTool/CodeGeneratorTool.exe",
                    Sources = { new NPath("Tools/UnityPreserveAttributeParser").Files("*.cs", true).Where(f =>
                        !f.FileName.EndsWith(".jam.cs") && !f.FileNameWithoutExtension.EndsWith("Tests")),
                                "External/Mono.Options/Options.cs"},
                    PrebuiltReferences = { Cecil.Current40AllRefs }
                };
                Backend.Current.AddDependency(ProjectFilesTarget.Path, new SolutionFileBuilder().WithProgram(p).Complete().Path);

                return p.SetupDefault();
            }

            public JamTarget SetupInvocation(JamList targetFile, JamList inputAssemblies, FileMode filemode)
            {
                "inputAssemblies".On(targetFile).Assign(inputAssemblies);

                base.SetupInvocation(targetFile, inputAssemblies, CommandLineArgumentsFor(targetFile, filemode));
                return targetFile;
            }

            static IEnumerable<string> CommandLineArgumentsFor(JamList targetFile, FileMode filemode)
            {
                yield return $"--{filemode.ToString().ToLower()}output={targetFile.GetBoundPath()}";
            }

            protected override bool SupportsResponseFile => true;
            protected override string ActionsBody => base.ActionsBody + " $(inputAssemblies:T) @@(--searchpath=$(inputAssemblies:TD))";
            protected override ActionsFlags ActionFlags => ActionsFlags.Together | base.ActionFlags;

            protected override bool IRealizeHowDangerousItIsToNotUseDefaultActionBody()
            {
                return true;
            }

            static CodeGeneratorTool _instance;
            public static CodeGeneratorTool Instance() => _instance ?? (_instance = new CodeGeneratorTool());

            public class SetupAllInvocationArgumentsForModule
            {
#pragma warning disable
                public NPath InputAssembly;

                public NPath OutputCpp;
                public NPath OutputHeader;
                public NPath OutputWhiteList;
#pragma warning restore
            }

            public SetupAllInvocationArgumentsForModule SetupAllInvocations(SetupAllInvocationArgumentsForModule args)
            {
                SetupInvocation(args.OutputCpp, args.InputAssembly, FileMode.Cpp);
                SetupInvocation(args.OutputHeader, args.InputAssembly, FileMode.Header);
                SetupInvocation(args.OutputWhiteList, args.InputAssembly, FileMode.Xml);

                return args;
            }
        }

        static string TargetIdentifierFor(string target)
        {
            if (target.Contains("_Engine") || target.Contains("_Editor") || (target.Contains("Dll") || (target.Contains("Managed") || (target.Contains("_PreRuntime") || target == "MacEditor"))) ||
                target == "Editor" ||
                target == "LinuxEditor" || target == "EditorModularEngine")
            {
                // These targets don't need generated headers, so skip them
                return null;
            }

            if ((target.Contains("iOSPlayer") || target.Contains("iOSSimulatorPlayer")))
            {
                // Different iOS targets share the same generated files
                return "iOSPlayerUnityLib";
            }

            if ((target.Contains("AppleTVPlayer") || target.Contains("AppleTVSimulatorPlayer")))
            {
                // Different tvOS targets share the same generated files
                return "AppleTVPlayerUnityLib";
            }

            if (target.Contains("AndroidPlayer"))
            {
                // Different Android targets share the same generated files
                return "AndroidPlayer";
            }

            if (target.Contains("EditorExtension"))
            {
                // Native Editor Extensions share the editor generated files
                return "EditorApp";
            }

            if (target.Contains("WebGLSupport"))
            {
                // Make all WebGL modules share generated files.
                return "WebGLSupport";
            }

            if (target.Contains("MacStandalonePlayer"))
                return "MacStandalonePlayer";

            if (target.Contains("PS") || (target.Contains("XBox") || target.Contains("Xbox")) || target.Contains("Switch") || target.Contains("Lumin"))
            {
                // These platforms use the NDA setup and are not included in the main jam files, so we don't get their dlls.

                // Make them all use the win standalone player generated files for now.
                return "DefaultUnityRuntime";
            }

            return target;
        }

        // MakeSourceFilesDependOnGeneratedSources sets up all the required dependencies to generated header files required
        // to build cpp source files "SRCS", so that jam will know that it should generate these headers before attempting
        // to compile the source files. "targets" is the jam build targets for which the dependencies should be generated.
        // "untiyEngineForPlatformTargetName" is the jam build target for the UnityEngine dll to be used as inputs for UnityPreserveAttribyteParser,
        // when it generates the headers.
        // This needs to be called in the setup code for each Unity runtime platform to ensure that sources can be built.
        // This method does not actually tell jam how to generate the headers, (that happens in SetupFor)

        public static void MakeSourceFilesDependOnGeneratedSources(JamList SRCS, JamList targets, JamList untiyEngineForPlatformTargetName)
        {
            foreach (var target in targets)
            {
                var targetIdentifier = TargetIdentifierFor(target);

                if (targetIdentifier == null)
                    continue;

                var generatedTargets = ModulesWithManagedCodeFor(targetIdentifier)
                    .Select(m => GeneratedTargets.For(targetIdentifier, m))
                    .ToArray();

                //the  ModuleNameScriptingClasses.cpp files get setup in SetupFor with a nongristed targetname.
                //inside C.Application, they get gristed with <target>.  in order for jam to realize that the gristed
                //one is the same as the nongristed one one that actually has the action, we make the gristed one depend on
                //the nongristed gristed one:
                foreach (JamList generatedCppFile in generatedTargets.Select(g => g.Cpp))
                {
                    var gristedGeneratedCppFile = generatedCppFile.SetGrist(target);
                    NotFile(gristedGeneratedCppFile);
                    Depends(gristedGeneratedCppFile, generatedCppFile);
                }

                JamList generatedFilesAllNormalFilesShouldDependOn = new JamList();
                generatedFilesAllNormalFilesShouldDependOn.Append(generatedTargets.Select(g => g.Header));

                // we grist the SRCS here, as C.Application will also grist them, so if
                // we won't C.Application won't realize these include dependencies.
                // Android is an exception because it does not use C.Application
                var srcsForIncludes = SRCS.SetGrist(target);
                if (target.Contains("AndroidPlayer"))
                    srcsForIncludes.Assign(SRCS.Where(g => g.EndsWith(".cpp")));

                MakeManyThingsIncludeManyOtherThings(srcsForIncludes, generatedFilesAllNormalFilesShouldDependOn, $"GeneratedUsedByNativeInformation_Dummy_For_{target}");

                //SetupFor has already set up all the depgraph for all generated sources. however at the time that depgraph
                //was was setup, we actually didn't actually yet know the path of the dll's that would have to be actually
                //read to generate all those codefiles. To deal with this problem, we invented a dummy target per identifier
                //whose name can ge gotten through UnityEngineTargetNameFor(targetIdentifier).  The previously ran depgraph
                //setup code setup their actions specifying this dummy target as the input dll.
                //
                //now we are later in code execution, and at this point we do actually know the actual path of the unityengine.dll
                //that we need to use as input. we get passed the jam targetname of this dll, and we'll read the BINDING variable from it
                //and take that value, and place it as BINDING variable on the dummy target that was setup in the actions.

                var dummyUnityEngineTargetName = DummyUnityEngineTargetNameFor(targetIdentifier);
                Depends(dummyUnityEngineTargetName, untiyEngineForPlatformTargetName);

                GetOnTargetVariable(dummyUnityEngineTargetName, "BINDING").Assign(untiyEngineForPlatformTargetName.GetBoundPath());

                // Add generated headers to target include paths
                C.C_IncludeDirectories(target, new JamList(generatedTargets.First().Header).SetGrist("").GetDirectory());
            }
        }

        //this is a performance optimizations for when you want to have many things include many other things. instead
        //of setting up the includes directly, which would cause N*M connections in the depgraph, we introduce
        //a dummy node in the middle, which means we only need to add N+M connections to the depgraph. The dummyNode
        //does need to be unique, and needs to be provided by the caller.
        static void MakeManyThingsIncludeManyOtherThings(JamList manyThings, JamList manyOtherThings, string dummyName)
        {
            NotFile(dummyName);
            Includes(manyThings, dummyName);
            Includes(dummyName, manyOtherThings);
        }

        static JamList GetOnTargetVariable(JamList target, string variableName)
        {
            return GlobalVariables.Singleton.GetOrCreateVariableOnTargetContext(target, variableName).Single();
        }

        static IEnumerable<string> ModulesWithManagedCodeFor(string targetIdentifier)
        {
            var modules = SetupRuntimeModules2.GetModulesWithManagedCode(targetIdentifier).Append("Core");
            if (IsEditorTarget(targetIdentifier))
                modules = modules.Append("Editor");
            return modules;
        }

        internal class GeneratedTargets
        {
            public string Cpp { get; private set; }
            public string Header { get; private set; }
            public string Xml { get; private set; }

            public static GeneratedTargets For(string target, string module)
            {
                string targetIdentifier = TargetIdentifierFor(target);
                var pathPrefix = $"artifacts/generated/UsedByNativeInformation/{targetIdentifier}/{module}";
                return new GeneratedTargets()
                {
                    Cpp = $"{pathPrefix}ScriptingClasses.cpp",
                    Header = $"<{targetIdentifier}>{pathPrefix}ScriptingClasses.h",
                    Xml = $"{pathPrefix}.xml",
                };
            }
        }


        // SetupFor will set up all the jam targets which can be generated from the managed code
        // (using [UsedByNativeCode]/[RequiredByNativeCode] attributes) for the module "module" of jam build target
        // "target".
        //
        // Targets generated by this method are:
        // artifacts/generated/UsedByNativeInformation/[strippingInformationTarget]/[module]ScriptingClasses.h
        // artifacts/generated/UsedByNativeInformation/[strippingInformationTarget]/[module]ScriptingClasses.cpp
        // artifacts/generated/UsedByNativeInformation/[strippingInformationTarget]/[module]ScriptingClasses.xml
        //
        // "allGeneratedSources".On(strippingInformationTarget) is set up to contain all [module]ScriptingClasses.cpp files
        // [strippingInformationTarget]AllHeaders is a NotFile target set up to depend on all [module]ScriptingClasses.h files
        //
        public static GeneratedTargets SetupFor(JamList target, JamList module, JamList dlls)
        {
            var targetIdentifier = TargetIdentifierFor(target.Single());

            if (targetIdentifier == null)
            {
                // These targets don't need generated headers, so skip them
                return null;
            }
            var generatedTargets = GeneratedTargets.For(target.Single(), module.Single());

            var thisModuleTargets = JamList(generatedTargets.Header, generatedTargets.Cpp, generatedTargets.Xml);

            "LOCATE".On(thisModuleTargets).Assign(Vars.TOP);


            Needs(thisModuleTargets, dlls);
            if (IsEditorTarget(targetIdentifier))
            {
                // we need the modular engine to build the editor module header, so we can find out which module headers to include for used engine classes.
                Needs(thisModuleTargets, "EditorModularEngine");
            }

            var codeGeneratorTool = CodeGeneratorTool.Instance();
            var unprocessedDlls = Combine(dlls, "_UnprocessedAssembly");
            codeGeneratorTool.SetupInvocation(generatedTargets.Cpp, unprocessedDlls, FileMode.Cpp);
            codeGeneratorTool.SetupInvocation(generatedTargets.Header, unprocessedDlls, FileMode.Header);
            codeGeneratorTool.SetupInvocation(generatedTargets.Xml, unprocessedDlls, FileMode.Xml);

            return generatedTargets;
        }

        static string DummyUnityEngineTargetNameFor(string targetIdentifier)
        {
            return $"Dll{targetIdentifier}";
        }

        public static JamList IsEditorTarget(JamList strippingInformationTarget)
        {
            return strippingInformationTarget.Include("\\EditorApp");
        }
    }
}
