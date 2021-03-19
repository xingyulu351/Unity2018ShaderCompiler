using System;
using System.IO;
using System.Text;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;
using UnityEditor;
using Unity.DataContract;
using Unity.PackageManager.Ivy;
using PackageInfo = Unity.DataContract.PackageInfo;

namespace Unity.PackageManager.UI
{
    [Serializable]
    internal class InfoView
    {
        [SerializeField] private Vector2 scrollPosition;

        [NonSerialized]
        private PackageInfo m_Package;
        public PackageInfo package
        {
            get
            {
                return m_Package.Refresh();
            }
            set
            {
                if (m_Package == value)
                    return;
                m_Package = value.Refresh();
                UpdateReleaseNotes();
            }
        }

        [NonSerialized]
        private List<DownloaderState> downloaderStates;
        private class DownloaderState
        {
            public DownloaderTask downloader;
            public PackageInfo package;
        }

        private Dictionary<string, HashSet<PackageInfo>> packageCache;

        public InfoView()
        {
            downloaderStates = new List<DownloaderState>();
            packageCache = new Dictionary<string, HashSet<PackageInfo>>();
        }

        void UpdateReleaseNotes()
        {
            if (m_Package == null)
                return;
            foreach (PackageInfo package in PackageManager.Database.AllVersionsOfPackage(m_Package.packageName).Select(m => m.ToPackageInfo()))
            {
                if (!packageCache.ContainsKey(package.packageName))
                    packageCache[package.packageName] = new HashSet<PackageInfo>();
                packageCache[package.packageName].Add(package);
                DownloadReleaseNotes(package);
            }
        }

        void DownloadReleaseNotes(PackageInfo version)
        {
            if (downloaderStates.Any(i => i.package == version))
                return;

            var notes = m_Package.files.FirstOrDefault(x => x.Value.type == PackageFileType.ReleaseNotes);
            if (notes.Key == null)
                return;

            var task = new DownloaderTask(new Uri(notes.Value.url));
            task.Name = Window.kTaskName;
            task.OnFinish += (t, success) => {
                var tmpFile = t.Result as string;
                if (!File.Exists(tmpFile))
                    return;
                File.Copy(tmpFile, Path.Combine(Settings.installLocation, Path.GetFileName(tmpFile)), true);
                var downloader = downloaderStates.First(x => x.downloader == t);
                if (downloader.package == version)
                    version.Refresh();
                return;
            };
            downloaderStates.Add(new DownloaderState() {
                downloader = task,
                package = version
            });

            task.Run();
        }

        void CacheDownloaderState()
        {
            downloaderStates.RemoveAll(x => x.downloader.IsCompleted);
        }

        bool IsDownloadingNotes()
        {
            return downloaderStates.Any(x => x.package.packageName == package.packageName);
        }

        public void OnGUI()
        {
            if (package == null)
                return;

            if (Event.current.type == EventType.Layout)
                CacheDownloaderState();

            GUILayout.BeginVertical(Styles.infoContainer);

            DoHeader();
            GUILayout.Space(10f);
            DoDescription();

            GUILayout.EndVertical();

            scrollPosition = GUILayout.BeginScrollView(scrollPosition, GUILayout.ExpandWidth(true), GUILayout.ExpandHeight(true));
            GUILayout.BeginVertical(Styles.infoContainer);

            DoReleaseNotes();

            GUILayout.EndVertical();
            GUILayout.EndScrollView();
        }

        void DoHeader()
        {
            GUILayout.BeginHorizontal();

            GUILayout.Label(String.Format("{0} ({1})", package.name, package.version), Styles.infoHeader);

            GUILayout.EndHorizontal();
        }

        void DoDescription()
        {
            GUILayout.Label(package.description, Styles.infoText);
        }

        void DoReleaseNotes()
        {
            GUILayout.Label("Release Notes", Styles.infoHeader);

            if (IsDownloadingNotes())
                GUILayout.Label("Refreshing...", Styles.infoText);
            else
            {
                var buffer = new StringBuilder();
                foreach (PackageInfo version in packageCache[package.packageName].OrderByDescending(p => p.version))
                {
                    buffer.AppendFormat("<b>{0}</b>:\n", version.version);
                    buffer.AppendLine(version.releaseNotes);
                    buffer.AppendLine(string.Empty);
                }
                GUILayout.Label(buffer.ToString(), Styles.infoText);
            }
            /*GUILayout.Label (@"•Lorem ipsum dolor sit amet, consectetur adipiscing elit.
•Phasellus ac est a quam hendrerit tempor.
    •Curabitur a augue sagittis, cursus orci non, elementum quam.
    •Proin vitae neque quis quam fringilla congue in a lorem.
    •Vestibulum nec diam quis risus facilisis blandit sed ut ligula.

•Donec placerat justo non neque facilisis, at porta lectus euismod.
•Integer dignissim turpis vitae sodales tempor.

•Sed accumsan justo eu ornare tincidunt.
•Cras convallis nisi vitae nunc adipiscing lobortis ac quis quam.
•Quisque et nibh vehicula, dapibus est eu, suscipit erat.
•Sed nec enim ut nisl elementum elementum.
•In sit amet orci ac sem pellentesque elementum.

•Proin venenatis mi eget erat auctor, eget laoreet urna vulputate.
•Sed placerat nulla eget est euismod volutpat.
•Nulla sit amet nulla porttitor, tincidunt mi sed, convallis lorem.

•Donec commodo nisi at lacus auctor tincidunt.
    •Phasellus sodales justo id arcu egestas ultrices.
    •Phasellus condimentum quam at ante eleifend sollicitudin.
    •Donec eleifend augue varius leo auctor, vel elementum elit ornare.
    •Sed congue neque non elementum sagittis.
    •Fusce pulvinar diam eu magna pharetra euismod.
    •Sed sit amet mauris dapibus, tempor libero eget, tempor lorem.

•In iaculis nibh at turpis tincidunt vehicula.
•Praesent sit amet dolor et dui volutpat fermentum.
•Donec iaculis turpis non dui vehicula suscipit.
•Nunc suscipit ligula et vestibulum eleifend.
•Morbi vehicula metus non magna dictum, et commodo elit lacinia.

•Fusce dapibus lectus at ligula imperdiet, quis dictum ligula pellentesque.
•Vestibulum semper augue eu libero blandit, sit amet auctor velit malesuada.
•Donec sed velit placerat, suscipit risus eu, commodo orci.
•Integer rhoncus arcu sed ullamcorper faucibus.", Styles.infoDescription);*/
        }
    }
}
