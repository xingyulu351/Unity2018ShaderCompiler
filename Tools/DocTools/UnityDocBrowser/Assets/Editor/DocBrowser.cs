using UnityEngine;
using UnityEditor;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Diagnostics;
using System.IO;
using ICSharpCode.SharpZipLib.GZip;
using ICSharpCode.SharpZipLib.Tar;
using UnderlyingModel;

public partial class DocBrowser : EditorWindow
{
    public const int kListWidth = 600;
    public bool m_ShowList = true;
    private bool m_ShowMatchList;
    public bool ShowMatchList
    {
        get { return m_ShowMatchList; }
        set
        {
            if (m_ShowMatchList == value)
                return;
            m_MatchList.SelectNone();
            m_ShowMatchList = value;
            if (m_ShowMatchList)
            {
                m_ShowList = true;
            }
            OnSelectMember(m_List, m_List.selectedMemberName);
        }
    }

    // General
    private NewDataItemProject m_DocProject;
    public NewDataItemProject docProject { get { return m_DocProject; } }

    public bool ShowDiff { get; set; }
    public bool ShowRawNames { get; set; }

    private bool m_EditGrouping = false;
    public bool EditGrouping { get { return m_EditGrouping; } set { m_EditGrouping = value; }  }

    private MemberList m_List;
    private MemberList m_MatchList;
    internal MemberEditor m_Editor;

    // Other
    static Styles s_Styles;
    public static Styles styles { get { return s_Styles; } }
    private int m_FirstLoad = 3;
    private string[] m_OptionsList;
    public string[] OptionsList { get { return m_OptionsList; } }

    // Add menu named "Doc Browser" to the Window menu
    [MenuItem("Window/Internal/Doc Browser")]
    static void Init()
    {
        // Get existing open window or if none, make a new one:
        DocBrowser window = (DocBrowser)EditorWindow.GetWindow(typeof(DocBrowser));
        window.minSize = new Vector2(700, 400);
    }

    void OnEnable()
    {
        //the DocBrowser needs to see all platforms
        DocGenSettings.Instance.IncludeAllPlatforms = true;
        DocGenSettings.Instance.RunningFromDocBrowser = true;
        DocGenSettings.Instance.LoadIniFiles();

        ExtractPackages();

        // Code below is executed both when the window is opened and at mono reloads.
        LoadMembers(m_FirstLoad > 0);
        if (string.IsNullOrEmpty(m_List.selectedMemberName))
            m_Editor = null;
        if (m_Editor != null)
        {
            var selectedMember = m_List.GetSelectedMemberSlow();
            if (selectedMember != null)
                m_Editor.OnEnable(selectedMember);
        }
        if (m_List != null)
            m_List.SetCallbacks(MaySelectNewMember, OnSelectMember);
        if (m_MatchList != null)
            m_MatchList.SetCallbacks(MaySelectNewMatch, OnSelectMatch);
        InitPlatformOptionsList();
    }

    public void InitPlatformOptionsList()
    {
        var optionsList = new List<string>();
        optionsList.Add("Default Platform");

        foreach (var plat in DocGenSettings.Instance.PlatformsInfo.Keys)
        {
            optionsList.Add(plat);
        }
        optionsList.Add("Unknown");

        m_OptionsList = optionsList.ToArray();
    }

    void OnDestroy()
    {
        DeleteExtractedPackages();

        if (m_Editor != null)
            m_Editor.OnDestroy();
    }

    private bool MaySelectNewMember()
    {
        return (m_Editor == null || m_Editor.LeaveWithPermission());
    }

    private void OnSelectMember(MemberList list, string memberName)
    {
        if (ShowMatchList)
        {
            UpdateMatch();
            return;
        }

        MemberItem item = list.GetSelectedMemberSlow();
        if (item == null)
            return;
        m_Editor = new MemberEditor(this, item);
    }

    private bool MaySelectNewMatch()
    {
        return (m_Editor == null || m_Editor.LeaveWithPermission());
    }

    private void OnSelectMatch(MemberList list, string memberName)
    {
        UpdateMatch();
    }

    private void UpdateMatch()
    {
        MemberItem docItem = m_List.GetSelectedMemberSlow();
        MemberItem asmItem = m_MatchList.GetSelectedMemberSlow();
        if (docItem == null)
        {
            m_Editor = null;
            return;
        }
        if (asmItem == null)
        {
            m_Editor = new MemberEditor(this, docItem);
            return;
        }
        m_Editor = new MemberEditor(this, docItem, asmItem);
    }

    void OnGUI()
    {
        if (s_Styles == null)
            s_Styles = new Styles();

        ToolbarGUI();

        EditorGUILayout.BeginHorizontal();
        {
            if (m_ShowList && m_List != null)
            {
                m_List.OnGUI();
                GUILayout.Space(styles.dividerSpace);
            }

            if (ShowMatchList)
            {
                m_MatchList.OnGUI();
                GUILayout.Space(styles.dividerSpace);
            }

            EditorGUILayout.BeginVertical();
            {
                if (m_Editor != null)
                    m_Editor.OnGUI();
                else
                    MemberEditor.NoEditorGUI(this);
            }
            EditorGUILayout.EndVertical();
        }
        EditorGUILayout.EndHorizontal();

        if (m_FirstLoad > 0 && Event.current.type == EventType.Repaint)
        {
            m_FirstLoad--;
            if (m_FirstLoad == 0)
                LoadMembers();
            Repaint();
        }
    }

    void ToolbarGUI()
    {
        EditorGUILayout.BeginHorizontal(EditorStyles.toolbar);
        GUILayout.Space(1);

        if (!m_ShowMatchList)
        {
            // Hide List
            m_ShowList = !GUILayout.Toggle(!m_ShowList, "Hide List", EditorStyles.toolbarButton);

            GUILayout.Space(100);

            // Translation

            EditorGUI.BeginChangeCheck();

            if (EditorGUI.EndChangeCheck())
            {
                EditorGUIUtility.keyboardControl = 0;
                if (m_Editor == null || m_Editor.LeaveWithPermission())
                {
                    OnSelectMember(m_List, m_List.selectedMemberName);
                }
            }
        }

        GUILayout.FlexibleSpace();

        // Raw Names button
        ShowRawNames = GUILayout.Toggle(ShowRawNames, "Show Raw Names", EditorStyles.toolbarButton);

        // Diff button
        ShowDiff = GUILayout.Toggle(ShowDiff, "Show Diff", EditorStyles.toolbarButton);

        GUILayout.Space(100);

        // Reload and Legend buttons
        if (GUILayout.Button("Reload", EditorStyles.toolbarButton))
        {
            DeleteExtractedPackages();
            ExtractPackages();
            RegenerateCombinedAssemblies();
            LoadMembers();
        }

        if (GUILayout.Button("Legend", EditorStyles.toolbarButton))
        {
            EditorWindow.GetWindow<DocBrowserLegendWindow>();
        }

        EditorGUILayout.EndHorizontal();
    }

    void ExtractPackages()
    {
        var packageDirs = Directory.GetDirectories(Path.Combine(DirectoryCalculator.RootDirName, "build"),
            "PackageManager", SearchOption.AllDirectories);
        foreach (var dir in packageDirs)
        {
            string editorDir = Path.Combine(dir, "Editor");
            if (!Directory.Exists(editorDir))
            {
                continue;
            }

            string tempDir = Path.Combine(editorDir, "DocTools_Temp");
            if (!Directory.Exists(tempDir))
            {
                Directory.CreateDirectory(tempDir);
            }

            var tgzFileNames = Directory.GetFiles(editorDir, "*.tgz", SearchOption.AllDirectories);
            foreach (var tgzFile in tgzFileNames)
            {
                string targetPath = Path.Combine(tempDir, Path.GetFileNameWithoutExtension(tgzFile));
                if (!Directory.Exists(targetPath))
                {
                    Directory.CreateDirectory(targetPath);
                }

                Stream inStream = File.OpenRead(tgzFile);
                Stream gzipStream = new GZipInputStream(inStream);

                TarArchive tarArchive = TarArchive.CreateInputTarArchive(gzipStream);
                tarArchive.ExtractContents(targetPath);
                tarArchive.Close();

                gzipStream.Close();
                inStream.Close();
            }
        }
    }

    void DeleteExtractedPackages()
    {
        m_DocProject = null;

        // remove extracted package tgz directory
        var packageDirs = Directory.GetDirectories(Path.Combine(DirectoryCalculator.RootDirName, "build"),
            "PackageManager", SearchOption.AllDirectories);
        foreach (var dir in packageDirs)
        {
            string tempDir = Path.Combine(Path.Combine(dir, "Editor"), "DocTools_Temp");
            if (!Directory.Exists(tempDir))
            {
                continue;
            }
            Directory.Delete(tempDir, true);
        }
    }

    void RegenerateCombinedAssemblies()
    {
        ProcessStartInfo startInfo = new ProcessStartInfo
        {
            WorkingDirectory = Unsupported.GetBaseUnityDeveloperFolder(),
            RedirectStandardOutput = true,
            RedirectStandardError = false,
            // Jam execution has some failures since it's trying to overwrite managed DLLs that the editor has open right now;
            // have to continue in presence of errors.
            Arguments = "jam.pl -e CombinedAssemblies",
            FileName = "perl",
        };

        int exitCode;
        string messages = GetOutputStream(startInfo, out exitCode);
        if (exitCode != 0)
            UnityEngine.Debug.LogError(messages);
    }

    private static string GetOutputStream(ProcessStartInfo startInfo, out int exitCode)
    {
        startInfo.UseShellExecute = false;
        startInfo.CreateNoWindow = true;
        var p = new Process { StartInfo = startInfo };

        // Read data asynchronously
        var sbStandardOut = new StringBuilder();
        var sbStandardError = new StringBuilder();
        p.OutputDataReceived += (sender, data) => sbStandardOut.AppendLine(data.Data);
        p.ErrorDataReceived += (sender, data) => sbStandardError.AppendLine(data.Data);
        p.Start();
        if (startInfo.RedirectStandardError)
            p.BeginErrorReadLine();
        else
            p.BeginOutputReadLine();

        // Wain until process is done
        p.WaitForExit();

        var output = startInfo.RedirectStandardError ? sbStandardError.ToString() : sbStandardOut.ToString();
        exitCode = p.ExitCode;
        return output;
    }

    void LoadMembers(bool loadNone = false)
    {
        try
        {
            var dirs = DirectoryCalculator.GetMemberItemDirectoriesFromJson("../MemberDirs.json");
            m_DocProject = new NewDataItemProject(dirs, false) { Mem2Source = true };
        }
        catch (Exception e)
        {
            UnityEngine.Debug.LogError("Unable to initialize NewDataItemProject: " + e.Message);
        }

        if (!loadNone)
        {
            m_DocProject.ReloadAllProjectData();
        }
        else
        {
            var content = new GUIContent("Loading data...");

            //EditorStyles.notificationText seems to be null sometimes
            if (content != null)
                ShowNotification(content);
        }

        if (m_List == null)
            m_List = new MemberList(this);
        m_List.SetCallbacks(MaySelectNewMember, OnSelectMember);

        if (m_MatchList == null)
            m_MatchList = new MemberList(this);
        m_MatchList.filter = MemberFilter.AllApi;
        m_MatchList.SetCallbacks(MaySelectNewMatch, OnSelectMatch);

        LoadFilteredList(m_DocProject, m_List, m_List.filter, m_List.search);
        LoadFilteredList(m_DocProject, m_MatchList, m_MatchList.filter, m_MatchList.search);
    }

    public static void LoadFilteredList(NewDataItemProject project, MemberList list, MemberFilter filter, string search)
    {
        List<MemberItem> filteredMembers;
        switch (filter)
        {
            case MemberFilter.Everything:
                filteredMembers = project.GetFilteredMembersForProgramming(Presence.DontCare, Presence.DontCare);
                break;
            case MemberFilter.AllApi:
                filteredMembers = project.GetFilteredMembersForProgramming(Presence.SomeOrAllPresent, Presence.DontCare);
                break;
            case MemberFilter.AllDocumented:
                filteredMembers = project.GetFilteredMembersForProgramming(Presence.DontCare, Presence.SomeOrAllPresent);
                break;
            case MemberFilter.ApiWithoutDocs:
                filteredMembers = project.GetFilteredMembersForProgramming(Presence.SomeOrAllPresent, Presence.SomeOrAllAbsent);
                break;
            case MemberFilter.ApiWithoutDocsNoUndoc:
                filteredMembers = project.GetFilteredMembersForProgramming(Presence.SomeOrAllPresent, Presence.SomeOrAllAbsent, includeUndoc: false);
                break;
            case MemberFilter.DocsWithoutApi:
                filteredMembers = project.GetFilteredMembersForProgramming(Presence.SomeOrAllAbsent, Presence.SomeOrAllPresent);
                break;
            case MemberFilter.MismatchedApiAndDocs:
                filteredMembers = project.GetFilteredMembersForProgramming(Presence.SomeOrAllAbsent, Presence.SomeOrAllAbsent);
                break;
            default:
                filteredMembers = new List<MemberItem>();
                break;
        }

        if (search != "")
        {
            string searchAdjusted = search.ToLower().Replace("_", "");
            filteredMembers = filteredMembers.Where(elem => elem.ItemName.ToLower().Replace("_", "").IndexOf(searchAdjusted) >= 0).ToList();
        }

        filteredMembers = filteredMembers.OrderBy(m => m.ItemName).ToList();
        list.members = filteredMembers;
    }
}
