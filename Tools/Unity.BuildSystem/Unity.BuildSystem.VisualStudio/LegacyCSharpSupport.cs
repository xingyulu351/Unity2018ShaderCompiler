using System;
using System.Collections.Generic;
using System.Runtime.Versioning;
using Bee.DotNet;
using JamSharp.Runtime;
using NiceIO;
using Unity.BuildSystem.CSharpSupport;
using Unity.BuildSystem.VisualStudio;
using Unity.BuildTools;

namespace Unity.BuildSystem
{
    // Helper code to support "old" C# assembly build rules or projects; the ones that aren't using CSharpProgram yet.
    public class LegacyCSharpSupport
    {
        public static CSharpProjectFileReference UnityEngineProjectReference { get; } = new CSharpProjectFileReference("UnityEngine", UnityEngineProjectGuid, UnityEngineProjectPath, keepPathRelative: true);
        public static CSharpProjectFileReference UnityEditorProjectReference { get; } = new CSharpProjectFileReference("UnityEditor", UnityEditorProjectGuid, UnityEditorProjectPath, keepPathRelative: true);

        public const string UnityEngineProjectPath = "Projects/CSharp/UnityEngine.csproj";
        public const string UnityEditorProjectPath = "Projects/CSharp/UnityEditor.csproj";
        public const string DataContractProjectPath = "Tools/PackageManager/DataContract/DataContract.csproj";
        public const string UnitySerializationLogicProjectPath = "Tools/Unity.SerializationLogic/Unity.SerializationLogic.csproj";
        public const string UnityCecilToolsProjectPath = "Tools/Unity.CecilTools/Unity.CecilTools.csproj";
        public const string UNetWeaverProjectPath = "Extensions/Networking/Weaver/UNetWeaver.csproj";

        public const string UnityEngineProjectGuid = "{F0499708-3EB6-4026-8362-97E6FFC4E7C8}";
        public const string UnityEditorProjectGuid = "{016C8D73-3641-47FB-8D33-7A015A7EC7DB}";
        public const string DataContractProjectGuid = "{A15E35A9-22E8-4A79-B6CE-C0984062DAC6}";
        public const string UnitySerializationLogicProjecttGuid = "{A6749DFF-E369-4FE6-9019-6B7C555E80EA}";
        public const string UnityCecilToolsProjecttGuid = "{35FF4EBD-85F0-4727-8AC0-32AE4F3723D0}";
        public const string UNetWeaverProjectGuid = "{709222FD-15C2-497D-8B31-366ADCC074CD}";

        public static string GuidForProject(string projectPath)
        {
            if (projectPath == UnityEngineProjectPath)
                return UnityEngineProjectGuid;
            if (projectPath == UnityEditorProjectPath)
                return UnityEditorProjectGuid;
            if (projectPath == DataContractProjectPath)
                return DataContractProjectGuid;
            if (projectPath == UnitySerializationLogicProjectPath)
                return UnitySerializationLogicProjecttGuid;
            if (projectPath == UnityCecilToolsProjectPath)
                return UnityCecilToolsProjecttGuid;
            if (projectPath == UNetWeaverProjectPath)
                return UNetWeaverProjectGuid;
            throw new Exception($"Generating IDE projects; GUID for a managed project '{projectPath}' is not known");
        }

        public static void BuildAssemblyToCSharpProject(
            string targetName,
            NPath destAssemblyPath,
            JamList csfiles,
            string sdk,
            string langVersion,
            bool unsafeFlag,
            JamList defines,
            JamList references,
            JamList systemreferences)
        {
            Framework fromSdk = FromSdk(sdk);

            var mp = new CSharpProgram
            {
                Sources = {csfiles.ToNPaths()},
                Framework = fromSdk,
                LanguageVersion = langVersion ?? "6",
                Path = destAssemblyPath,
                Defines = {defines.Elements},
                ProjectFilePath = $"Projects/VisualStudio/Projects/{targetName}.csproj",
                Unsafe = unsafeFlag,
                SystemReferences =
                {
                    "System.dll", "System.Xml.dll", // Mono mcs references it by default
                    systemreferences.Elements
                }
            };

            foreach (var r in references.Elements)
            {
                var rpath = new NPath(r);

                // The references that we might have here are either existing precompiled DLLs (simple), or
                // Jam build targets. In the latter case, we don't want to figure out possibly arbitrary build setups
                // for them, so only recognize several "known & widely used" targets.
                if (r == "DefaultEngineDll" || r.EndsWith("EditorExtensions_Engine"))
                {
                    // Just reference regular UnityEngine.dll when that is what we need, or we have a platforms-specific "*EditorExtensions_Engine".
                    // The latter is not 100% correct, but worst case the csproj build from VS IDE will fail with some platform
                    // specific APIs missing, isntead of failing with "everything" out of engine API missing.
                    mp.CsprojReferences.Add(UnityEngineProjectReference);
                }
                else if (r == "DefaultEditorDll")
                {
                    mp.CsprojReferences.Add(UnityEditorProjectReference);
                }
                else
                {
                    if (!rpath.IsRelative)
                        rpath = rpath.RelativeTo(Paths.UnityRoot);

                    mp.PrebuiltReferences.Add(rpath);
                }
            }

            // We want to replace the whole built-in C# build step with one that invokes our build system; in MSBuild
            // the "Build" target does all that work.
            mp.CprojBuildEvents = GetCsProjBuildEvents(targetName);
            RegisterProgram(targetName, mp);
        }

        private static Framework FromSdk(string sdk)
        {
            switch (sdk)
            {
                case "2":
                case "2.0":
                    return Framework.Framework20;
                case "3.5":
                    return Framework.Framework35;
                case "4":
                case "4.0":
                    return Framework.Framework40;
                case "4.6":
                case "roslyn4.6":
                    return Framework.Framework46;
                default:
                    throw new ArgumentException($"Unknown sdk: {sdk}");
            }
        }

        public static string GetCsProjBuildEvents(string targetName)
        {
            return $@"  <Target Name=""Build"" Outputs=""$(TargetPath)"">
    <Message Importance=""High"" Text=""Invoking Jam build on '{targetName}'"" />
    <Exec Command=""&quot;{Paths.UnityRoot.ToString(SlashMode.Backward)}\jam.bat&quot; -g -e -sPLATFORM=win64 {targetName}"" />
  </Target>
";
        }

        static readonly Dictionary<string, CSharpProgram> s_ProjectAssemblies = new Dictionary<string, CSharpProgram>();

        public static CSharpProgram GetProgram(string name)
        {
            CSharpProgram res;
            if (!s_ProjectAssemblies.TryGetValue(name, out res))
            {
                throw new Exception($"Generating IDE project files; C# assembly program '{name}' was not found.");
            }
            return res;
        }

        public static void RegisterProgram(string name, CSharpProgram program)
        {
            if (s_ProjectAssemblies.ContainsKey(name))
            {
                throw new Exception($"Generating IDE project files; C# assembly program '{name}' is already registered and now would be registered twice. This should not happen.");
            }
            s_ProjectAssemblies.Add(name, program);
        }
    }
}
