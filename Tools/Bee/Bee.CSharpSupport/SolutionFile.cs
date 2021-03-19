using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Bee.Core;
using NiceIO;
using Unity.BuildTools;

namespace Unity.BuildSystem.CSharpSupport
{
    public class SolutionFile
    {
        public NPath Path { get; }

        Tuple<string, IProjectFile>[] GroupAndProjectFiles { get; set; }
        public IEnumerable<IProjectFile> ProjectFiles => GroupAndProjectFiles.Select(t => t.Item2);

        internal SolutionFile(NPath path, List<Tuple<string, IProjectFile>> groupAndPrograms)
        {
            Path = path;
            GroupAndProjectFiles = groupAndPrograms.ToArray();

            var contents = MakeSolutionContents();
            TextFile.Setup(path, contents, "CSSolution");
            Backend.Current.AddDependency(Path, GroupAndProjectFiles.Select(c => c.Item2.Path));
        }

        string MakeSolutionContents()
        {
            var sb = new StringBuilder();
            sb.AppendLine(@"
Microsoft Visual Studio Solution File, Format Version 12.00
# Visual Studio 14
VisualStudioVersion = 14.0.24720.0
MinimumVisualStudioVersion = 10.0.40219.1
");

            var solutionGuid = ProjectFile.GuidForName(Path.ToString());
            // FailIfHasDuplicates(projectFiles, Projects);

            var rootFolderGuid = "{2150E333-8FDC-42A3-9474-1A3956D46DE8}";
            var groups = GroupAndProjectFiles.Select(gp => gp.Item1).Distinct();
            foreach (var group in groups.Exclude((string)null))
            {
                // All "folders" in the solution are represented as "fake projects" too; with a special parent GUID and
                // instead of pointing to vcxproj files they just point to their own names.
                var groupGuid = GroupGUIDFor(@group);
                sb.Append($"Project(\"{rootFolderGuid}\") = \"{group}\", \"{group}\", \"{groupGuid}\"\nEndProject\n");
            }

            var sbNesting = new StringBuilder();
            foreach (var groupAndProjectFile in GroupAndProjectFiles)
            {
                var group = groupAndProjectFile.Item1;
                var projectFile = groupAndProjectFile.Item2;

                var guid = projectFile.Guid;
                var name = projectFile.Name;
                var path = projectFile.Path.RelativeTo(Path.Parent).ToString(SlashMode.Backward);
                sb.Append($"Project(\"{solutionGuid}\") = \"{name}\", \"{path}\", \"{guid}\"\nEndProject\n");

                if (!string.IsNullOrEmpty(group))
                    sbNesting.Append($"\t\t{guid} = {GroupGUIDFor(@group)}\n");
            }

            sb.AppendLine(@"Global
       GlobalSection(SolutionConfigurationPlatforms) = preSolution
                Debug|Any CPU = Debug|Any CPU
                Release|Any CPU = Release|Any CPU
        EndGlobalSection
        GlobalSection(ProjectConfigurationPlatforms) = postSolution");

            foreach (var projectFile in ProjectFiles)
                EmitProjectConfigMapping(sb, projectFile.Guid);

            sb.AppendLine(@"
       EndGlobalSection
        GlobalSection(SolutionProperties) = preSolution
                HideSolutionNode = FALSE
        EndGlobalSection");
            if (sbNesting.Length > 0)
            {
                sb.AppendLine($"\tGlobalSection(NestedProjects) = preSolution\n{sbNesting}\tEndGlobalSection");
            }
            sb.AppendLine("EndGlobal");

            return sb.ToString();
        }

        private static string GroupGUIDFor(string @group)
        {
            return ProjectFile.GuidForName($"FolderGuid-{@group}");
        }

        private static void EmitProjectConfigMapping(StringBuilder sb, string projectGuid)
        {
            sb.AppendLine($"\t\t{projectGuid}.Debug|Any CPU.ActiveCfg = Debug|Any CPU");
            sb.AppendLine($"\t\t{projectGuid}.Debug|Any CPU.Build.0 = Debug|Any CPU");
            sb.AppendLine($"\t\t{projectGuid}.Release|Any CPU.ActiveCfg = Debug|Any CPU");
            sb.AppendLine($"\t\t{projectGuid}.Release|Any CPU.Build.0 = Debug|Any CPU");
        }

        void FailIfHasDuplicates(ProjectFile[] projectFiles, Tuple<string, CSharpProjectFileReference>[] projectRefs)
        {
            var paths = projectFiles.Select(p => p.Path).Concat(projectRefs.Select(p => p.Item2.Path)).OrderBy(p => p).ToArray();
            for (var i = 0; i != paths.Length - 1; i++)
            {
                if (paths[i] == paths[i + 1])
                {
                    Errors.Exit($"C# solution '{Path}' contains duplicate projects in it; '{paths[i]}' found multiple times.");
                }
            }
        }
    }
}
