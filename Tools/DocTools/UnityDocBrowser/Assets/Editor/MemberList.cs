using UnityEngine;
using UnityEditor;
using System;
using System.Collections.Generic;
using System.IO;
using UnderlyingModel;
using UnderlyingModel.MemDoc;
using UnderlyingModel.ItemSerializers;

[System.Serializable]
public class MemberList
{
    private DocBrowser m_Browser;

    private int m_ListTypeWidth = 100;
    private const int kListPresenceWidth = 30;
    private const int kListCommentWidth = 60;
    private const int kListModifierWidth = 70;
    private const int kListItemHeight = 16;
    private const int kSliderWidth = 15;

    private MemberFilter m_Filter = MemberFilter.Everything;
    public MemberFilter filter { get { return m_Filter; } set { m_Filter = value; } }

    private List<MemberItem> m_FilteredMembers;
    public List<MemberItem> members { get { return m_FilteredMembers; } set { m_FilteredMembers = value; } }

    private string m_FilterSearch = "";
    public string search { get { return m_FilterSearch; } set { m_FilterSearch = value; } }

    private string m_SelectedMemberName = null;
    public string selectedMemberName { get { return m_SelectedMemberName; } }
    public void SelectNone()
    {
        m_SelectedMemberName = null;
        onSelectCallback(this, m_SelectedMemberName);
    }

    public delegate bool MemberSelectionPermissionCallback();
    public delegate void MemberSelectionCallback(MemberList list, string memberName);
    private MemberSelectionPermissionCallback getSelectPermission;
    private MemberSelectionCallback onSelectCallback;

    private Vector2 m_ListScroll;
    private static GUIContent s_Content = new GUIContent();

    public MemberList(DocBrowser browser)
    {
        m_Browser = browser;
    }

    public void SetCallbacks(MemberSelectionPermissionCallback permission, MemberSelectionCallback callback)
    {
        getSelectPermission = permission;
        onSelectCallback = callback;
    }

    public MemberItem GetSelectedMemberSlow()
    {
        for (int i = 0; i < m_FilteredMembers.Count; i++)
            if (m_FilteredMembers[i].ItemName == m_SelectedMemberName)
                return m_FilteredMembers[i];
        return null;
    }

    public int GetSelectedMemberIndexSlow()
    {
        for (int i = 0; i < m_FilteredMembers.Count; i++)
            if (m_FilteredMembers[i].ItemName == m_SelectedMemberName)
                return i;
        return -1;
    }

    public void OnGUI()
    {
        EditorGUILayout.BeginVertical(GUILayout.ExpandWidth(false), GUILayout.Width(DocBrowser.kListWidth));

        ToolbarGUI();

        EditorGUILayout.BeginVertical(DocBrowser.styles.frameWithMargin);

        // List header
        EditorGUILayout.BeginHorizontal();
        GUILayout.Label("Name", DocBrowser.styles.columnHeader, GUILayout.ExpandWidth(true));
        GUILayout.Label("Type", DocBrowser.styles.columnHeader, GUILayout.Width(m_ListTypeWidth));
        if (m_Browser != null)
            GUILayout.Label("Asm", DocBrowser.styles.columnHeader, GUILayout.Width(kListPresenceWidth));
        GUILayout.Label("Doc", DocBrowser.styles.columnHeader, GUILayout.Width(kListPresenceWidth));
        GUILayout.Label("Special", DocBrowser.styles.columnHeader, GUILayout.Width(kListCommentWidth));
        GUILayout.Label("Modifiers", DocBrowser.styles.columnHeader, GUILayout.Width(kListModifierWidth));
        GUILayout.Label("", DocBrowser.styles.columnHeader, GUILayout.Width(kSliderWidth));
        EditorGUILayout.EndHorizontal();

        // Make GUI for the list itself
        int id = GUIUtility.GetControlID(FocusType.Keyboard);
        Rect listRect = GUILayoutUtility.GetRect(100, 100, GUILayout.ExpandHeight(true), GUILayout.ExpandWidth(true));
        if (m_FilteredMembers == null)
            Debug.LogError("m_FilteredMembers is null");
        m_ListScroll = GUI.BeginScrollView(listRect, m_ListScroll, new Rect(0, 0, listRect.width - 20, Mathf.Max(m_FilteredMembers.Count * kListItemHeight, listRect.height)), false, true);
        int startIndex = Mathf.Max(0, Mathf.FloorToInt(m_ListScroll.y / kListItemHeight));
        int endIndex = Mathf.Min(m_FilteredMembers.Count - 1, Mathf.CeilToInt((m_ListScroll.y + listRect.height) / kListItemHeight));
        for (int i = startIndex; i <= endIndex; i++)
        {
            ListElementGUI(new Rect(0, i * kListItemHeight, listRect.width - kSliderWidth, kListItemHeight), i, id);
        }
        GUI.EndScrollView();

        // Keyboard navigation with up/down arrow
        if (GUIUtility.keyboardControl == id && Event.current.type == EventType.KeyDown)
        {
            Event evt = Event.current;
            int delta = 0;
            if (evt.keyCode == KeyCode.UpArrow)
                delta = -1;
            else if (evt.keyCode == KeyCode.DownArrow)
                delta = 1;
            if (delta != 0)
            {
                int selected = GetSelectedMemberIndexSlow() + delta;
                selected = Mathf.Clamp(selected, 0, m_FilteredMembers.Count - 1);
                if (getSelectPermission())
                {
                    m_SelectedMemberName = m_FilteredMembers[selected].ItemName;
                    onSelectCallback(this, m_SelectedMemberName);
                    m_ListScroll.y = Mathf.Clamp(m_ListScroll.y, (selected + 1) * kListItemHeight - listRect.height, selected * kListItemHeight);
                }
                evt.Use();
            }
        }

        // Status bar
        GUILayout.Label(m_FilteredMembers.Count + " members shown.", DocBrowser.styles.frameStatusBar);

        EditorGUILayout.EndVertical();

        EditorGUILayout.EndVertical();
    }

    void ToolbarGUI()
    {
        GUILayout.Space(3);
        EditorGUILayout.BeginHorizontal(GUILayout.Height(23));

        GUILayout.Space(1);

        var oldFilter = filter;
        filter = (MemberFilter)EditorGUILayout.EnumPopup(filter, GUILayout.ExpandWidth(false));
        if (oldFilter != filter)
            DocBrowser.LoadFilteredList(m_Browser.docProject, this, filter, search);

        EditorGUILayout.BeginVertical();
        GUILayout.Space(4);
        EditorGUILayout.BeginHorizontal();
        var oldFilterSearch = search;
        search = EditorGUILayout.TextField(search, DocBrowser.styles.searchField, GUILayout.ExpandWidth(true));
        if (GUILayout.Button(string.Empty, DocBrowser.styles.searchFieldCancelButton))
        {
            search = string.Empty;
            EditorGUIUtility.keyboardControl = 0;
        }
        if (oldFilterSearch != search)
            DocBrowser.LoadFilteredList(m_Browser.docProject, this, filter, search);
        EditorGUILayout.EndHorizontal();
        EditorGUILayout.EndVertical();

        EditorGUILayout.EndHorizontal();
    }

    void PresenceCellGUI(ref Rect pos, PresenceSimple presence, bool selected, bool focus)
    {
        pos.x += pos.width;
        pos.width = kListPresenceWidth;
        s_Content.text = presence == PresenceSimple.All ? "Y" : (presence == PresenceSimple.SomeButNotAll ? "N" : "N !");
        if (presence == PresenceSimple.All)
            DocBrowser.styles.listItem.Draw(pos, s_Content, false, false, selected, focus);
        else
            DocBrowser.styles.listItem.Draw(pos, GUIContent.none, false, false, selected, focus);

        if (presence != PresenceSimple.All)
        {
            Color col = (presence == PresenceSimple.SomeButNotAll ? Color.yellow : Color.red);
            col.a = (!selected ? 0.2f : (focus ? 0.6f : 0.4f));
            GUI.backgroundColor = col;
            DocBrowser.styles.listItemWhite.Draw(pos, s_Content, false, false, selected, focus);
            GUI.backgroundColor = Color.white;
        }
    }

    void ListElementGUI(Rect position, int index, int controlID)
    {
        Event evt = Event.current;
        MemberItem item = m_FilteredMembers[index];
        if (evt.type == EventType.Repaint)
        {
            Rect pos = position;

            bool selected = item.ItemName == m_SelectedMemberName;
            bool focus = (GUIUtility.keyboardControl == controlID);

            pos.width -= (m_ListTypeWidth + kListPresenceWidth * 2 + kListCommentWidth + kListModifierWidth);
            s_Content.text = item.GetNames().ClassAndMember;
            DocBrowser.styles.listItem.Draw(pos, s_Content, false, false, selected, focus);

            pos.x += pos.width;
            pos.width = m_ListTypeWidth;
            s_Content.text = item.ItemType.ToString();
            DocBrowser.styles.listItem.Draw(pos, s_Content, false, false, selected, focus);


            PresenceCellGUI(ref pos, item.AsmPresenceComparedToDoc, selected, focus);
            PresenceCellGUI(ref pos, item.DocPresenceComparedToAsm, selected, focus);

            var memDocModel = item.DocModel;
            if (memDocModel == null)
                s_Content.text = "doc missing";
            else
                s_Content.text = memDocModel.IsUndoc() ? "UNDOC" : memDocModel.IsDocOnly() ? "DOCONLY" : string.Empty;

            pos.x += pos.width;
            pos.width = kListCommentWidth;
            DocBrowser.styles.listItem.Draw(pos, s_Content, false, false, selected, focus);

            pos.x += pos.width;
            pos.width = kListModifierWidth;
            s_Content.text = item != null && item.AreAllProtected() ? "protected" : "";
            DocBrowser.styles.listItem.Draw(pos, s_Content, false, false, selected, focus);
        }

        bool didSelect = false;
        bool selectionChanged = false;

        if (evt.type == EventType.MouseDown && evt.button == 0 && position.Contains(evt.mousePosition))
            didSelect = true;

        if (evt.type == EventType.ContextClick && position.Contains(evt.mousePosition))
        {
            didSelect = true;

            var menu = new GenericMenu();
            if (item.AnyHaveDoc && !item.AnyHaveAsm)
                menu.AddItem(new GUIContent("Find Matching API"), false, MatchMemberFileToApi, item);
            if (item.AnyHaveDoc)
            {
                menu.AddItem(new GUIContent("Open Member Doc File"), false, OpenMemberFile, item);
                menu.AddItem(new GUIContent("Delete this member's docs"), false, DeleteMemberFile, item);
            }

            menu.ShowAsContext();
            evt.Use();
        }

        if (didSelect)
        {
            GUIUtility.keyboardControl = controlID;
            if (getSelectPermission())
            {
                m_SelectedMemberName = item.ItemName;
                onSelectCallback(this, m_SelectedMemberName);
                Event.current.Use();
                selectionChanged = true;
            }
        }

        if (selectionChanged)
            GUIUtility.ExitGUI();
    }

    void OpenMemberFile(object obj)
    {
        MemberItem member = (MemberItem)obj;

        string filePath = DirectoryCalculator.GetFullPathFromMember(member);

        if (string.IsNullOrEmpty(filePath) || !File.Exists(filePath))
        {
            Debug.LogError("could not find file at " + filePath);
            return;
        }
        if (Environment.OSVersion.Platform != PlatformID.MacOSX && Environment.OSVersion.Platform != PlatformID.Unix)
            System.Diagnostics.Process.Start(filePath);
        else
            System.Diagnostics.Process.Start(new System.Diagnostics.ProcessStartInfo("open", "-t " + filePath));
    }

    void DeleteMemberFile(object obj)
    {
        MemberItem member = (MemberItem)obj;

        member.DocModel = new MemDocModel();

        m_Browser.m_Editor.Save();
        SelectNone();
    }

    void MatchMemberFileToApi(object obj)
    {
        m_Browser.ShowMatchList = true;
    }
}
