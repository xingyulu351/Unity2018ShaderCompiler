using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using UnityEditor;
using UnityEditor.AnimatedValues;
using Unity.DataContract;
using Unity.PackageManager.Ivy;
using PackageInfo = Unity.DataContract.PackageInfo;

namespace Unity.PackageManager.UI
{
    [Serializable]
    internal class ListView
    {
        [SerializeField] private Vector2 scrollPosition;

        [SerializeField]
        private bool m_ListViewShouldTakeKeyboardControl = true;

        [SerializeField] private int selectedPackageHash = -1;
        private static Dictionary<int, PackageInfo> hashToPackageCache = new Dictionary<int, PackageInfo>();

        public PackageInfo SelectedPackage
        {
            get
            {
                // default to the first package
                if (selectedPackageHash == -1 && allListedPackages.Count() > 0)
                    selectedPackageHash = allListedPackages.First().GetHashCode();

                if (hashToPackageCache.ContainsKey(selectedPackageHash))
                    return hashToPackageCache[selectedPackageHash];

                foreach (var package in allListedPackages)
                {
                    if (package.GetHashCode() == selectedPackageHash)
                    {
                        hashToPackageCache.Add(selectedPackageHash, package);
                        return package;
                    }
                }

                return null;
            }
            set
            {
                if (value != null)
                    selectedPackageHash = value.GetHashCode();
                else
                    selectedPackageHash = 0;
            }
        }

        private Window m_Parent;
        internal Window parent
        {
            get { return m_Parent; }
            set { m_Parent = value; }
        }

        private IEnumerable<PackageInfo> m_LocalPackages;
        private IEnumerable<PackageInfo> localPackages
        {
            get
            {
                // Don't list package manager itself outside of developer build
                if (Unsupported.IsDeveloperMode())
                    return m_LocalPackages;
                return m_LocalPackages.Where(p => p.type != PackageType.PackageManager);
            }
            set
            {
                m_LocalPackages = value;
            }
        }

        private IEnumerable<PackageInfo> m_AvailablePackages;
        private IEnumerable<PackageInfo> availablePackages
        {
            get
            {
                // Don't list package manager itself outside of developer build
                if (Unsupported.IsDeveloperMode())
                    return m_AvailablePackages;
                return m_AvailablePackages.Where(p => p.type != PackageType.PackageManager);
            }
            set
            {
                m_AvailablePackages = value;
            }
        }

        private IEnumerable<PackageInfo> allListedPackages
        {
            get
            {
                return availablePackages.Union(localPackages, packageNameComparer);
            }
        }

        private IEnumerable<InstallerState> installerStates { get; set; }
        private class InstallerState
        {
            public Installer installer;
            public IvyModule module;
            public float progress;
            public string message;
            public bool successful;
            public bool running;
        }

        private static readonly Dictionary<PackageType, GUIContent> packageTypeContents = new Dictionary<PackageType, GUIContent>()
        {
            { PackageType.PackageManager, new GUIContent("Module Manager") },
            { PackageType.PlaybackEngine, new GUIContent("Playback Engines") },
            { PackageType.UnityExtension, new GUIContent("Unity Extensions") },
            { PackageType.Unknown, new GUIContent("Unknown") },
        };
        private Dictionary<string, AnimBool> faders = new Dictionary<string, AnimBool>();

        void CachePackageManagerState()
        {
            // Local Packages
            RefreshLocalPackages();

            // Remote Packages
            availablePackages = PackageManager.Database.NewOrUpdatedPackages.Select(p => p.ToPackageInfo().Refresh());

            // Installers
            installerStates = PackageManager.Instance.Installers.Values.Select(i => new InstallerState() { installer = i, module = i.Package, progress = i.Progress, message = i.ProgressMessage, successful = i.IsSuccessful, running = i.IsRunning });

            // Faders
            foreach (PackageInfo package in allListedPackages.Where(p => !faders.ContainsKey(p.packageName)))
                faders[package.packageName] = FaderForPackage(package);
        }

        private AnimBool FaderForPackage(PackageInfo package)
        {
            AnimBool fader = new AnimBool();
            fader.speed = 4f;
            fader.valueChanged.AddListener(m_Parent.Repaint);
            return fader;
        }

        public void OnGUI()
        {
            if (Event.current.type == EventType.Layout)
                CachePackageManagerState();

            Toolbar();

            scrollPosition = GUILayout.BeginScrollView(scrollPosition, Styles.listBackground, GUILayout.ExpandWidth(true), GUILayout.ExpandHeight(true));

            int listsKeyboardID = GUIUtility.GetControlID(FocusType.Keyboard);

            // When the window is first opened, we want the list view to have keyboard control
            if (m_ListViewShouldTakeKeyboardControl)
            {
                GUIUtility.keyboardControl = listsKeyboardID;
                m_ListViewShouldTakeKeyboardControl = false;
            }

            foreach (object packageTypeObject in Enum.GetValues(typeof(PackageType)))
            {
                PackageType packageType = (PackageType)packageTypeObject;
                IEnumerable<PackageInfo> packages = allListedPackages.Where(p => p.type == packageType);
                if (!packages.Any())
                    continue;

                if (packageTypeContents.ContainsKey(packageType))
                    SectionHeader(packageTypeContents[packageType].text);
                else
                    SectionHeader(packageTypeContents[PackageType.Unknown].text);
                ListPackages(packages, listsKeyboardID);
            }

            HandleKeyEvents(listsKeyboardID);
            HandleMouseEvents(listsKeyboardID);

            GUILayout.EndScrollView();
        }

        private void Toolbar()
        {
            EditorGUILayout.BeginHorizontal(EditorStyles.toolbar);

            GUILayout.FlexibleSpace();
            EditorGUILayout.EndHorizontal();
        }

        private void HandleMouseEvents(int listsKeyboardID)
        {
            var evt = Event.current;

            int id = GUIUtility.GetControlID(FocusType.Passive);

            switch (evt.GetTypeForControl(id))
            {
                case EventType.MouseDown:
                    GUIUtility.hotControl = id;
                    GUIUtility.keyboardControl = listsKeyboardID;
                    // we received a mouse down in the background of the list.. deselect the current package
                    SelectedPackage = null;
                    evt.Use();
                    break;
                case EventType.MouseDrag:
                    if (GUIUtility.hotControl == id)
                        evt.Use();
                    break;
                case EventType.MouseUp:
                    if (GUIUtility.hotControl == id)
                    {
                        GUIUtility.hotControl = 0;
                        evt.Use();
                    }
                    break;
            }
        }

        private void HandleKeyEvents(int listsKeyboardID)
        {
            var evt = Event.current;
            if (evt.type != EventType.KeyDown || GUIUtility.keyboardControl != listsKeyboardID)
                return;

            switch (evt.keyCode)
            {
                case KeyCode.Escape:
                    SelectedPackage = null;
                    GUIUtility.keyboardControl = 0;
                    evt.Use();
                    break;
                case KeyCode.DownArrow:
                    SelectNext();
                    evt.Use();
                    break;
                case KeyCode.UpArrow:
                    SelectPrev();
                    evt.Use();
                    break;
            }
        }

        private void SelectNext()
        {
            // if we currently have nothing selected, select the first element
            bool selectNext = SelectedPackage == null;
            IEnumerable<PackageInfo> packages = allListedPackages.OrderBy(p => p.type);

            foreach (var package in packages)
            {
                if (selectNext)
                {
                    SelectedPackage = package;
                    return;
                }

                // if this was already the selected package, we want to select the next in the list
                if (SelectedPackage == package)
                    selectNext = true;
            }

            // if we get here then the currently selected package is at the end of the list, select the first package
            SelectedPackage = packages.First();
        }

        private void SelectPrev()
        {
            PackageInfo prevPackage = null;
            IEnumerable<PackageInfo> packages = allListedPackages.OrderBy(p => p.type);

            foreach (var package in packages)
            {
                // if there is a prev package and this is the currently selected package, select the prev
                if (prevPackage != null && SelectedPackage == package)
                {
                    SelectedPackage = prevPackage;
                    return;
                }

                prevPackage = package;
            }

            // if we got here, then there wasn't a package previous to the selected package or there wasn't a selected package, so select the last listed package
            SelectedPackage = prevPackage;
        }

        private void ListPackages(IEnumerable<PackageInfo> packages, int listsKeyboardID)
        {
            if (packages.Any())
            {
                foreach (var package in packages)
                    DoPackageElement(package, listsKeyboardID);
            }
            else
                GUILayout.Label("None");
        }

        private void InstallPackage(IvyModule module)
        {
            // early out if package is already installing
            if (installerStates.Any(i => i.module == module))
                return;

            PackageManager.Instance.SetupPackageInstall(module).Run();
        }

        void CancelPackage(IvyModule module)
        {
            var installerState = installerStates.FirstOrDefault(i => i.module == module);
            if (installerState != null)
            {
                Debug.Log(string.Format("Cancelling installation of {0}", installerState.module.Name));
                installerState.installer.Cancel(false);
            }
        }

        private void RefreshLocalPackages()
        {
            localPackages = PackageManager.Database.NewestLocalPackages.Select(m => m.ToPackageInfo());
        }

        private static string BuildPackageText(PackageInfo package, IEnumerable<IvyModule> allVersions)
        {
            var packageText = new StringBuilder(package.name);
            PackageVersion loadedVersion = null;
            if (package.loaded)
                loadedVersion = package.version;
            else
            {
                IvyModule module = allVersions.FirstOrDefault(v => null != v.BasePath);
                if (null != module)
                    loadedVersion = module.Version;
            }

            if (null != loadedVersion)
                packageText.AppendFormat(" {0}", loadedVersion);
            if (null == loadedVersion || allVersions.Any(v => null == v.BasePath && v.Version > loadedVersion))
                packageText.AppendFormat(" (Update available)");
            return packageText.ToString();
        }

        private void DoPackageElement(PackageInfo package, int listsKeyboardID)
        {
            var evt = Event.current;
            int elementID = GUIUtility.GetControlID(FocusType.Passive);
            IEnumerable<IvyModule> allVersions = PackageManager.Database.AllVersionsOfPackage(package.packageName).OrderByDescending(p => p.Version);

            var packageContent = new GUIContent(BuildPackageText(package, allVersions));
            var elementRect = GUILayoutUtility.GetRect(packageContent, Styles.listElement);
            bool thisPackageIsSelected = (SelectedPackage == package);
            AnimBool fader = faders[package.packageName];
            fader.target = thisPackageIsSelected;

            // Background Repaint
            if (evt.GetTypeForControl(elementID) == EventType.Repaint)
                Styles.listElement.Draw(elementRect, packageContent, false, false, thisPackageIsSelected, GUIUtility.keyboardControl == listsKeyboardID);

            if (EditorGUILayout.BeginFadeGroup(fader.faded))
            {
                var indent = 30;
                foreach (var module in allVersions)
                {
                    bool isInstalled = (module.BasePath != null);
                    var versionContent = new GUIContent(string.Format("{0} {1}",
                        module.Info.Version,
                        (isInstalled ? "(downloaded)" : string.Empty /* TODO: Inject download size */)));

                    GUILayout.BeginHorizontal();
                    GUILayout.Space(indent);
                    GUILayout.Label(versionContent);
                    GUILayout.FlexibleSpace();

                    // Download Progress
                    InstallerState installerState = installerStates.FirstOrDefault(i => i.module == module);
                    if (installerState != null)
                    {
                        if (!installerState.running && !installerState.successful)
                        {
                            GUILayout.Label(installerState.message);
                            if (GUILayout.Button(Styles.okButtonContent))
                                CancelPackage(module);
                        }
                        else
                        {
                            GUILayout.Label(String.Format("{0}... ({1})", installerState.message, installerState.progress.ToString("P")));
                            if (GUILayout.Button(Styles.cancelButtonContent))
                                CancelPackage(module);
                        }

                        HandleUtility.Repaint();
                    }
                    // Download Button
                    else if (!isInstalled && GUILayout.Button(Styles.downloadButtonContent))
                    {
                        InstallPackage(module);
                    }
                    else if (module.Loaded)
                    {
                        EditorGUILayout.HelpBox(Styles.loadedContent.text, MessageType.None);
                    }
                    else if (isInstalled)
                    {
                        if (module == package)
                        {
                            // This is the selected package, but it's not loaded
                            if (GUILayout.Button(Styles.restartUnityContent))
                                EditorApplication.OpenProject(Environment.CurrentDirectory);
                        }
                        else if (GUILayout.Button(Styles.activateContent))
                        {
                            PackageManager.Database.SelectPackage(module);
                            PackageManager.Instance.LocalIndexer.CacheResult();
                            RefreshLocalPackages();
                        }
                    }

                    GUILayout.EndHorizontal();
                }
            }
            EditorGUILayout.EndFadeGroup();

            // Background Mouse Input
            if (evt.type == EventType.MouseDown &&
                GUIUtility.hotControl == 0 &&
                elementRect.Contains(evt.mousePosition))
            {
                GUIUtility.keyboardControl = listsKeyboardID;
                SelectedPackage = package;
                evt.Use();
            }
        }

        private static void SectionHeader(string title)
        {
            GUILayout.Label(title, Styles.listHeader);
        }

        // Because there's no Union overload that takes a predicate
        static PackageNameComparer packageNameComparer = new PackageNameComparer();
        private class PackageNameComparer : IEqualityComparer<PackageInfo>
        {
            #region IEqualityComparer implementation
            public bool Equals(PackageInfo x, PackageInfo y)
            {
                return x.packageName.Equals(y.packageName);
            }

            public int GetHashCode(PackageInfo obj)
            {
                return obj.packageName.GetHashCode();
            }

            #endregion
        }
    }
}
