#if !STANDALONE

using System;
using UnityEditor;
using UnityEngine;
using Unity.PackageManager.Ivy;

namespace Unity.PackageManager.UI
{
    internal class Window : EditorWindow
    {
        public const string kTaskName = "ReleaseNotes";

        private static Vector2 kWindowSize = new Vector2(500f, 400f);
        private const float kListViewHeight = 200f;

        [SerializeField] private ListView listView;
        [SerializeField] private InfoView infoView;

        static readonly string[] kValues = new string[] {"external", "internal", "custom"};
        static readonly string[] kNames = new string[] {"External", "Internal", "Custom"};
        string m_Repo;
        string m_RepoUrl;
        bool m_PrefsRead;

        [NonSerialized]
        private bool m_RequestRepaint;

        // this method is called from Unity... do not change without updating Unity
        public static void ShowPackageManagerWindow()
        {
            var window = GetWindow<Window>(true, "Module Manager");
            window.minSize = window.maxSize = kWindowSize;
        }

        public void OnEnable()
        {
            PackageManager.Database.OnUpdateAvailable += HandleOnUpdate;
            PackageManager.Database.OnUpdateLocal += HandleOnUpdate;
            PackageManager.Instance.OnTask += HandleOnTask;
            EditorApplication.update += ApplicationUpdate;

            InitViews();
        }

        public void OnDisable()
        {
            PackageManager.Database.OnUpdateAvailable -= HandleOnUpdate;
            PackageManager.Database.OnUpdateLocal -= HandleOnUpdate;
            PackageManager.Instance.OnTask -= HandleOnTask;
            EditorApplication.update -= ApplicationUpdate;
        }

        private void ApplicationUpdate()
        {
            if (m_RequestRepaint)
            {
                Repaint();
                m_RequestRepaint = false;
            }
        }

        private void HandleOnUpdate(IvyModule[] modules)
        {
            m_RequestRepaint = true;
        }

        void HandleOnTask(Task task)
        {
            if (task.Name == kTaskName)
                m_RequestRepaint = true;
        }

        private void InitViews()
        {
            if (listView == null)
            {
                listView = new ListView();
            }
            if (infoView == null)
                infoView = new InfoView();
        }

        public void ReadPreferences()
        {
            m_Repo = Settings.repoType;
            m_RepoUrl = Settings.baseRepoUrl;
        }

        public void WritePreferences()
        {
            m_RepoUrl = Settings.SelectRepo(m_Repo, m_RepoUrl);
            Settings.CacheAllSettings();
        }

        public void OnGUI()
        {
            // Utility windows are silly and reserve extra space at the top... Use our own area to not reserve the extra space.
            GUILayout.BeginArea(new Rect(0f, 0f, kWindowSize.x, kWindowSize.y));

            if (PackageManager.Instance == null)
            {
                GUILayout.Box(new GUIContent("Package Manager Unavailable"), Styles.debugBox);
                return;
            }

            if (Unsupported.IsDeveloperMode())  // should be is internal build?
                ChannelSelector();

            GUILayout.BeginVertical(GUILayout.Height(kListViewHeight), GUILayout.ExpandWidth(true));
            listView.parent = this;
            listView.OnGUI();
            GUILayout.EndVertical();

            if (Event.current.type == EventType.Layout)
                infoView.package = listView.SelectedPackage;

            infoView.OnGUI();

            GUILayout.EndArea();
        }

        private void ChannelSelector()
        {
            // Can't do this in OnEnable since the PackageManager may not have been initialized yet
            if (!m_PrefsRead)
            {
                ReadPreferences();
                m_PrefsRead = true;
            }

            int selected = Array.IndexOf(kValues, m_Repo);
            if (selected < 0)
                selected = 0;

            EditorGUI.BeginChangeCheck();
            GUILayout.Space(5f);
            GUILayout.BeginHorizontal();
            EditorGUIUtility.labelWidth = 70f;
            selected = EditorGUILayout.Popup("Repository", selected, kNames);
            m_Repo = kValues[selected];

            GUILayout.Space(10f);

            if (m_Repo == "custom")
            {
                // Clear url when switching to custom
                if (GUI.changed)
                    m_RepoUrl = string.Empty;
                EditorGUIUtility.labelWidth = 30f;
                m_RepoUrl = EditorGUILayout.TextField("URL", m_RepoUrl, GUILayout.Width(200f));
            }

            GUILayout.FlexibleSpace();
            GUILayout.EndHorizontal();
            GUILayout.Space(5f);

            if (EditorGUI.EndChangeCheck())
            {
                WritePreferences();
            }
        }
    }
}

#endif
