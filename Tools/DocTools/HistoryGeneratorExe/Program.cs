using System.IO;
using System.Text;
using HistoryGenerator;
using UnderlyingModel;

namespace HistoryGeneratorExe
{
    class Program
    {
        //should be run from Tools/DocTools/HistoryGenerator
        static void Main()
        {
            GenerateNextHistoryFile("PreviousVersionDlls", "PreviousVersionAPI.mem",
                Path.Combine(DirectoryCalculator.RootDirName,
                    Path.Combine("CurrentVersionDlls", "builds")),
                "LatestAPI.mem", HistoryUtils.CurrentVersionString);
        }

        private static IMemberGetter AssemblyOnlyProjectFromDll(string dllDir)
        {
            var memberItemProject = new NewDataItemProject(dllDir);
            memberItemProject.ReloadOnlyAssembly();
            return new MemberGetter(memberItemProject);
        }

        private static void GenerateNextHistoryFile(string oldDllDir, string oldHistoryMem, string dllDir, string newHistoryMem, string newVersion)
        {
            var dirs = DirectoryCalculator.GetMemberItemDirectoriesFromJson("../MemberDirs.json");
            var newItemProject = new NewDataItemProject(dirs, true);
            newItemProject.ReloadAllProjectData();

            var oldItemProject = AssemblyOnlyProjectFromDll(oldDllDir);
            var entries = HistoryUtils.GenerateNextHistoryEntries(new MemberGetter(newItemProject), oldItemProject, oldHistoryMem, newVersion);
            var sb = new StringBuilder();
            foreach (var entry in entries)
                sb.AppendUnixLine(entry.ToString());
            File.WriteAllText(newHistoryMem, sb.ToString());
        }
    }
}
