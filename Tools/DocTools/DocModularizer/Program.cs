using System;
using System.IO;
using UnderlyingModel;

namespace DocModularizer
{
    class Program
    {
        static void Main(string[] args)
        {
            var allPlatformsInfo = new AllPlatformsInfo();
            var newDataItemProject = new NewDataItemProject(scanForDlls: true);
            newDataItemProject.ReloadAllProjectData();
            var memberDirs = newDataItemProject.m_MemberItemDirectories;

            foreach (var plName in allPlatformsInfo.Keys)
            {
                var thisPlatform = allPlatformsInfo.GetPlatformInfo(plName);

                var memberItems = newDataItemProject.GetMem2AbleItems(true);
                foreach (var mem in memberItems)
                {
                    bool shouldBeMoved = thisPlatform.NamespaceList.Contains(mem.ActualNamespace);

                    foreach (var pref in thisPlatform.PrefixList)
                        if (mem.ItemName.StartsWith(pref)) // && !mem.ActualNamespace.StartsWith("UnityEditor"))
                            shouldBeMoved = true;
                    if (!shouldBeMoved)
                        continue;
                    if (MoveFileToPlayerDir(memberDirs, mem, thisPlatform.PlatformSourceDir))
                        Console.WriteLine("Unable to move files");
                }
            }
        }

        private static bool MoveFileToPlayerDir(MemberItemDirectories memberDirs, MemberItem mem, string playerBaseDir)
        {
            try
            {
                var sourceFile = Path.Combine(memberDirs.Mem2files, mem.Mem2FileName);
                if (!File.Exists(sourceFile))
                    return true;

                var destFullName = Path.Combine(DirectoryCalculator.GetDocSourceDirectory(playerBaseDir), mem.Mem2FileName);
                var destDirectory = Path.GetDirectoryName(destFullName);
                var destOnlyFileName = Path.GetFileName(destFullName);
                Directory.CreateDirectory(destDirectory);

                var destFile = Path.Combine(destDirectory, destOnlyFileName);
                File.Move(sourceFile, destFile);
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
            }
            return false;
        }
    }
}
