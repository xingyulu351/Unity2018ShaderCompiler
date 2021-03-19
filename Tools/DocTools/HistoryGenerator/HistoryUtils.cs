using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using UnderlyingModel;
using XMLFormatter;

namespace HistoryGenerator
{
    public static class HistoryUtils
    {
        public static List<APIVersionEntry> GenerateNextHistoryEntries(IMemberGetter newItemProject, IMemberGetter oldItemProject, string oldHistoryMemFile, string newVersion)
        {
            var historyPop = new HistoryPopulator(newItemProject);

            var entries = historyPop.PopulateHistoryFromMem(oldHistoryMemFile);

            var addedEntries = PopulateAddedAPIs(newVersion, newItemProject, oldItemProject);
            entries.AddRange(addedEntries);
            PopulateRemovedAPIs(newVersion, newItemProject, oldItemProject, entries);
            return entries;
        }

        private static List<APIVersionEntry> PopulateRemovedAPIs(string newVersion, IMemberGetter newProject, IMemberGetter oldProject, List<APIVersionEntry> allEntries)
        {
            var removedAPIs = new List<APIVersionEntry>();
            var oldVersionMembers = oldProject.GetAllMembers();
            foreach (var oldAPI in oldVersionMembers)
            {
                var apiStatus = APIStatus.Unchanged;
                var itemName = oldAPI.ItemName;
                if (CecilHelpers.IsNamespaceInternal(oldAPI.ActualNamespace)) continue;

                var newAPI = newProject.GetMember(itemName);

                if (newAPI == null || !newAPI.AnyHaveAsm && !newAPI.IsDocOnly)
                    apiStatus = APIStatus.Removed;
                if (newAPI != null && newAPI.IsObsolete && !oldAPI.IsObsolete)
                    apiStatus = newAPI.ObsoleteInfo.IsError ? APIStatus.Removed : APIStatus.Obsolete;

                if (apiStatus == APIStatus.Unchanged)
                    continue;

                MemberItem parentItem = null;
                var lastDotIndex = itemName.LastIndexOf('.');
                if (lastDotIndex >= 0)
                {
                    var parentID = itemName.Substring(0, lastDotIndex);
                    parentItem = newProject.GetMember(parentID);
                }

                //if the parent is also new, then exclude the child (enum vals, methods, and properties of new types don't need to show up)
                if (parentItem != null && !newProject.ContainsMember(parentItem.ItemName))
                    continue;

                var foundAPI = allEntries.FirstOrDefault(m => m.MemberID == itemName);
                if (foundAPI != null)
                {
                    if (apiStatus == APIStatus.Removed)
                    {
                        foundAPI.VersionRemoved = new VersionNumber(newVersion);

                        removedAPIs.Add(foundAPI);
                    }
                    else if (apiStatus == APIStatus.Obsolete)
                    {
                        foundAPI.VersionObsolete = new VersionNumber(newVersion);
                    }
                    foundAPI.ApiStatus = apiStatus;
                }
            }
            return removedAPIs;
        }

        private static IEnumerable<APIVersionEntry> PopulateAddedAPIs(string newVersion, IMemberGetter newProject, IMemberGetter oldProject)
        {
            var addedEntries = new List<APIVersionEntry>();
            var apisNotInOldAssembly = new List<string>();
            var latestVersionMembers = newProject.GetAllMembers().Where(m => m.AnyHaveAsm && m.AnyHaveDoc && !m.IsDocOnly);
            foreach (var latestAPI in latestVersionMembers)
            {
                var itemName = latestAPI.ItemName;

                if (CecilHelpers.IsNamespaceInternal(latestAPI.ActualNamespace)) continue;
                var oldMember = oldProject.GetMember(itemName);
                if (oldMember != null)
                    continue;

                var lastDotIndex = itemName.LastIndexOf('.');
                if (lastDotIndex >= 0)
                {
                    var parentID = itemName.Substring(0, lastDotIndex);
                    var parentItem = newProject.GetMember(parentID);
                    //if the parent is also new, then exclude the child (enum vals, methods, and properties of new types don't need to show up)
                    if (parentItem != null)
                    {
                        if (parentItem.IsUndoc) //children of undoc parents won't have pages
                            continue;
                        var oldParentItem = oldProject.GetMember(parentItem.ItemName);
                        if (oldParentItem == null || !oldParentItem.AnyHaveAsm)
                            continue;
                    }
                }

                apisNotInOldAssembly.Add(itemName);
                var entry = new APIVersionEntry
                {
                    AssemblyType = latestAPI.ItemType,
                    IsStatic = latestAPI.IsStatic,
                    MemberID = latestAPI.ItemName,
                    Namespace = latestAPI.ActualNamespace,
                    VersionAdded = new VersionNumber(newVersion),
                    ApiStatus = APIStatus.Added
                };
                addedEntries.Add(entry);
            }
            return addedEntries;
        }

        public static string CurrentVersionString
        {
            get
            {
                var buildConfigFile = Path.Combine(DirectoryCalculator.RootDirName, "Configuration", "BuildConfig.pm");
                if (!File.Exists(buildConfigFile))
                {
                    Console.WriteLine("could not find the version file");
                    return "0.0.0";
                }
                var sCurrentVersionString =
                    File.ReadAllText(buildConfigFile);
                sCurrentVersionString =
                    sCurrentVersionString.Substring(sCurrentVersionString.IndexOf("$unityVersion") + "$unityVersion".Length);
                sCurrentVersionString = sCurrentVersionString.Substring(sCurrentVersionString.IndexOf("\"") + 1);
                sCurrentVersionString = sCurrentVersionString.Substring(0, sCurrentVersionString.IndexOf("\""));
                int postfixStartsWith = sCurrentVersionString.IndexOfAny(new[] {'a', 'b', 'f'});
                if (postfixStartsWith > 0)
                    sCurrentVersionString = sCurrentVersionString.Substring(0, postfixStartsWith);
                return sCurrentVersionString;
            }
        }

        public static void MakeHistory(IMemberGetter memberItemProject, IMemberGetter oldDataItemProject, string staticFilesSource)
        {
            var txtParserPath = Path.Combine(DirectoryCalculator.RootDirName, "Tools/DocTools");

            var items = GenerateNextHistoryEntries(memberItemProject,
                oldDataItemProject, Path.Combine(txtParserPath, "HistoryGenerator/PreviousVersionAPI.mem"), CurrentVersionString);

            var hisOutputHTML = new HistoryOutputterXML(memberItemProject, oldDataItemProject, items);
            var miniHistoryPath = Path.Combine(staticFilesSource, "history.xml");
            var miniHistoryXml = hisOutputHTML.GetXML();
            XmlUtils4.WriteToFileWithHeader(miniHistoryPath, miniHistoryXml);
        }
    }
}
