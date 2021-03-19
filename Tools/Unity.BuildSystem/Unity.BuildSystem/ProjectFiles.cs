using JamSharp.Runtime;
using Projects.Jam;
using Unity.BuildSystem.Common;

namespace Unity.BuildSystem
{
    [Help(
        BuildTargetName,
        "IDE projects (all)",
        category: Help.Category.Projects,
        configs: Help.Configs.None)]
    public class ProjectFiles
    {
        // Are we generating IDE project files via ProjectFiles/NativeProjectFiles?
        public static bool IsRequested => NativeProjectFiles.IsRequested;

        public const string BuildTargetName = "ProjectFiles";

        public static void Setup()
        {
            NativeProjectFiles.Setup();

            // target that builds all project files we have
            BuiltinRules.NotFile(BuildTargetName);
            BuiltinRules.NotFile("BuildSystemProjectFiles");
            BuiltinRules.Depends(BuildTargetName, ManagedProjectFiles.BuildTargetName);
            BuiltinRules.Depends(BuildTargetName, NativeProjectFiles.BuildTargetName);
        }
    }
}
