using System;
using System.Collections.Generic;
using System.IO;
using System.Text.RegularExpressions;
using System.Linq;
using UnderlyingModel.ItemSerializers;


namespace UnderlyingModel
{
    public partial class NewDataItemProject
    {
        public readonly MemberItemDirectories m_MemberItemDirectories;
        internal Dictionary<string, MemberItem> m_MapNameToItem;

        public bool Mem2Source { get; set; }

        public int ItemCount { get { return m_MapNameToItem.Count; } }

        public static bool SkipSpecialCaseMember(string name)
        {
            return name == "Application._absoluteUrl";
        }

        public NewDataItemProject(bool scanForDlls = false)
            : this(DocGenSettings.Instance.Directories, scanForDlls)
        {
        }

        public NewDataItemProject(string assembliesDir)
        {
            Mem2Source = true;
            LoadDllsFromDir(assembliesDir);
        }

        //scanForDlls = true, looks for dlls in the subdirectories, used for ordinary documentation builds for various platforms
        //scanForDlls => looks at CombinedAssemblies only, used for the DocBrowser internal
        public NewDataItemProject(MemberItemDirectories memberItemDirectories, bool scanForDlls, string folder = "")
        {
            Mem2Source = true;
            m_MemberItemDirectories = memberItemDirectories;
            PopulateDllsList(scanForDlls, folder);

            m_MapNameToItem = new Dictionary<string, MemberItem>();
        }

        public void PopulateDllsList(string[] dllList)
        {
            m_DllLocations.Clear();
            m_DllLocations.AddRange(dllList);
        }

        public void PopulateDllsList(bool scanForDlls, string folder)
        {
            if (folder == "")
                folder = "build";

            if (!scanForDlls)
            {
                LoadDllsFromDir(m_MemberItemDirectories.Assemblies);
            }
            else
            {
                var dirs = Directory.GetDirectories(Path.Combine(DirectoryCalculator.RootDirName, folder), "Managed",
                    SearchOption.AllDirectories);
                foreach (var dir in dirs)
                    LoadDllsFromDir(dir);

                var additionalPaths = new List<string>
                {
                    Path.Combine(DirectoryCalculator.RootDirName, "build/iOSSupport/UnityEditor.iOS.Extensions.Xcode.dll")
                };
                foreach (var dllPath in additionalPaths)
                {
                    if (File.Exists(dllPath))
                        m_DllLocations.Add(dllPath);
                }
            }

            var extensionDirs = Directory.GetDirectories(Path.Combine(DirectoryCalculator.RootDirName, folder),
                "UnityExtensions", SearchOption.AllDirectories);
            foreach (var dir in extensionDirs)
            {
                var dllFileNames = Directory.GetFiles(dir, "Unity*.dll", SearchOption.AllDirectories);
                foreach (var dllFile in dllFileNames)
                {
                    m_DllLocations.Add(dllFile);
                }
            }

            var packageDirs = Directory.GetDirectories(Path.Combine(DirectoryCalculator.RootDirName, folder),
                "DocTools_Temp", SearchOption.AllDirectories);
            foreach (var dir in packageDirs)
            {
                var dllFileNames = Directory.GetFiles(dir, "Unity*.dll", SearchOption.AllDirectories);
                foreach (var dllFile in dllFileNames)
                {
                    m_DllLocations.Add(dllFile);
                }
            }
        }

        private void LoadDllsFromDir(string dir)
        {
            var regex = "Unity(Engine|Editor)(\\.(\\w*Module))?.dll";
            string[] assemblies = Directory.GetFiles(dir, "UnityE*.dll", SearchOption.AllDirectories);

            foreach (string path in assemblies)
            {
                var fi = new FileInfo(path);
                if (Regex.Match(fi.Name, regex).Success && File.Exists(path))
                    m_DllLocations.Add(path);
            }

            m_MapNameToItem = new Dictionary<string, MemberItem>();
        }

        public void ReloadOnlyAssembly()
        {
            DateTime originalStartTime = DateTime.Now;
            LoadAsmModules(true, false);
            var totalDuration = (DateTime.Now - originalStartTime).TotalSeconds;
            Console.WriteLine("Total time for ReloadOnlyAssembly = {0} sec", totalDuration);
        }

        //empty string denotes the default platform
        //if DocGenSettings.Instance.IncludeAllPlatforms is on, the platform will be ignored
        public void ReloadAllProjectData(string platform = "")
        {
            m_MapNameToItem.Clear();

            DateTime originalStartTime = DateTime.Now;

            // Populate list from assembly
            LoadAsmModules(true, true);

            LoadMem2Files(platform);

            //populate parents before interfaces, so we can get the interfaces of ALL ancestors
            m_AssemblyHierarchyTemp.PopulateParentsContainersAndInterfaces(this);

            var totalDuration = (DateTime.Now - originalStartTime).TotalSeconds;
            Console.WriteLine("Total time for ReloadAllProjectData = {0} sec", totalDuration);
        }

        public void ReloadProjectDataMinimal(string oneFile, string platform = "")
        {
            Console.WriteLine("ReloadProjectDataMinimal -onefile {0}", oneFile);
            m_MapNameToItem.Clear();

            DateTime originalStartTime = DateTime.Now;

            LoadAsmModules(true, false);
            {
                var memberIO = new MemberItemIOMem2(this);
                var item = GetMember(oneFile);
                if (item != null)
                    memberIO.LoadDoc(item, platform);
                else
                {
                    var allPlatform = DirectoryCalculator.AllPlatformDependentMemFiles();
                    var oneFileWithExt = oneFile + "." + DirectoryUtil.Mem2Extension;
                    var findFile = Directory.GetFiles(m_MemberItemDirectories.Mem2files, oneFileWithExt,
                        SearchOption.AllDirectories).FirstOrDefault();

                    //if default file not found, search for the platform file
                    if (findFile == null)
                    {
                        findFile = allPlatform.First(m => m.EndsWith(oneFileWithExt));
                        if (findFile == null)
                        {
                            //the platform file not found either - FAIL
                            Console.WriteLine("Unable to find file for {0}", oneFile);
                            return;
                        }
                    }
                    var ser = new MemberItemIOMem2(this);
                    ItemFromMem2File(ser, findFile);
                }
            }
            var totalDuration = (DateTime.Now - originalStartTime).TotalSeconds;
            Console.WriteLine("Total time for ReloadProjectDataMinimal = {0} sec", totalDuration);
        }

        public bool ContainsMember(string memberName)
        {
            return m_MapNameToItem.ContainsKey(memberName);
        }

        public MemberItem GetMember(string memberName)
        {
            MemberItem item;
            if (m_MapNameToItem.TryGetValue(memberName, out item))
                return item;
            return null;
        }

        public MemberItem GetFirstMemberSimilarTo(string memberName)
        {
            var elements = m_MapNameToItem.Values.Where(m => m.ItemName.Contains(memberName)).ToArray();
            return !elements.Any() ? null : elements[0];
        }

        private int ZeroIsOne(int number)
        {
            return number == 0 ? 1 : number;
        }

        // TODO: Fix it! This currently returns number of all signatures, not number of signatures that are in assembly!
        public int NumAsmSignaturesForMember(string st)
        {
            var item = m_MapNameToItem[st];
            return item == null ? 0 : ZeroIsOne(item.Signatures.Count);
        }

        public int NumDocSignatures(string st)
        {
            var item = m_MapNameToItem[st];
            return item == null || item.DocModel == null ? 0 : ZeroIsOne(item.DocModel.SignatureCount);
        }

        public List<MemberItem> GetAllMembers()
        {
            return m_MapNameToItem.Values.ToList();
        }

        public List<MemberItem> GetFilteredMembersForProgramming(Presence api, Presence docs, bool includeUndoc = true)
        {
            IEnumerable<MemberItem> items = new List<MemberItem>(m_MapNameToItem.Values);
            items = items.Where(elem => elem.AllPrivate == false);

            FilterItemsFromPresence(ref items, api, x => x.AnyThatHaveDocHaveAsm, x => x.AllThatHaveDocHaveAsm, includeUndoc);
            FilterItemsFromPresence(ref items, docs, x => x.AnyThatHaveAsmHaveDoc, x => x.AllThatHaveAsmHaveDoc, includeUndoc);

            return items.ToList();
        }

        private static void FilterItemsFromPresence(ref IEnumerable<MemberItem> items, Presence presence, Predicate<MemberItem> predAny, Predicate<MemberItem> predAll, bool includeUndoc)
        {
            if (!includeUndoc)
                items = items.Where(m => !m.IsUndoc);
            switch (presence)
            {
                case Presence.AllAbsent:
                    items = items.Where(elem => predAny(elem) == false);
                    break;
                case Presence.SomeOrAllAbsent:
                    items = items.Where(elem => predAll(elem) == false);
                    break;
                case Presence.SomeOrAllPresent:
                    items = items.Where(elem => predAny(elem) == true);
                    break;
                case Presence.AllPresent:
                    items = items.Where(elem => predAll(elem) == true);
                    break;
            }
        }
    }
}
