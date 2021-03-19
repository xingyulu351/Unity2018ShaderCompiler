using System.Diagnostics;
using System.IO;
using UnityEngine;
using UnityEditor;
using UnityEditorInternal;
using System;
using System.CodeDom.Compiler;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using Microsoft.CSharp;
using UnderlyingModel;
using UnderlyingModel.MemDoc;
using Debug = UnityEngine.Debug;

[Serializable]
public class MemberEditor
{
    private DocBrowser m_Browser;
    private MemberItem m_Item;
    private MemberItem m_OrphanDocItem;

    private Vector2 m_DocScroll;
    private Vector2 m_RawTextScroll;

    private MemberSession m_EnglishMember;
    private MemberSession EditMember { get { return m_EnglishMember; } }
    private int m_SelectedPlatform = -1;
    private int m_OriginalSelectedPlatform = -1;
    private int m_SelectedSection = -1;
    private int m_SelectedText = -1;
    private bool EditGrouping { get { return m_Browser.EditGrouping; } set { m_Browser.EditGrouping = value; } }
    private bool ShowDiff { get { return m_Browser.ShowDiff; } set { m_Browser.ShowDiff = value; } }
    private bool m_ConversionError = false;
    private bool m_CovariantTypesInSameSection = false;
    private static string sTodoText = "TODO.";

    private enum CompilationResult
    {
        Unknown,
        Pass,
        Fail
    };

    private CompilationResult m_CompilationResult;
    private string m_CompilationErrorString;

    public MemberEditor(DocBrowser browser, MemberItem item)
    {
        this.m_Browser = browser;
        Init(item);
    }

    public MemberEditor(DocBrowser browser, MemberItem docItem, MemberItem asmItem)
    {
        this.m_Browser = browser;
        Init(docItem, asmItem);
    }

    private void Init(MemberItem item)
    {
        m_Item = item;
        m_EnglishMember = new MemberSession(m_Browser.docProject, m_Item);
        UpdateSelectedPlatformIndex();
    }

    private void Init(MemberItem docItem, MemberItem asmItem)
    {
        m_Item = asmItem;
        m_OrphanDocItem = docItem;
        m_EnglishMember = new MemberSession(m_Browser.docProject, m_Item);

        // Find pure member name of asm and doc
        string docMemberName = docItem.Signatures.First(e => e.InDoc).Name;
        int paramsStartIndexChar =  docMemberName.IndexOfAny(new char[] {'(', '<'});
        if (paramsStartIndexChar >= 0)
            docMemberName = docMemberName.Substring(0, paramsStartIndexChar);
        string asmMemberName = asmItem.Signatures.First(e => e.InAsm).Asm.Name;

        // Create merged doc text
        string merged = docItem.DocModel.ToString();
        merged = merged.Replace(docMemberName, asmMemberName);

        // Load merged
        m_Item.LoadDoc(merged);
        m_EnglishMember.DirtyAutoChanges = true;

        // Set diff
        m_EnglishMember.TextCurrent = m_Item.DocModel.ToString();
        m_EnglishMember.Diff.Compare(m_EnglishMember.TextOrig, m_EnglishMember.TextCurrent);
        UpdateSelectedPlatformIndex();
    }

    private void UpdateSelectedPlatformIndex()
    {
        string itemFilePath = DirectoryCalculator.GetFullPathFromMember(m_Item);

        string itemPlatform = DocGenSettings.Instance.PlatformsInfo.GetPlatformNameFromPath(itemFilePath);
        m_SelectedPlatform = -1;
        if (m_Browser.OptionsList == null)
        {
            m_Browser.InitPlatformOptionsList();
        }
        if (AllPlatformsInfo.IsDefault(itemPlatform))
        {
            m_SelectedPlatform = 0;
        }
        else
        {
            m_SelectedPlatform = m_Browser.OptionsList.ToList().IndexOf(itemPlatform);
            if (m_SelectedPlatform < 0)
            {
                m_SelectedPlatform = m_Browser.OptionsList.Length - 1;
            }
        }
        m_OriginalSelectedPlatform = m_SelectedPlatform;
    }

    public bool LeaveWithPermission()
    {
        return EditMember == null || EditMember.LeaveWithPermission();
    }

    public void OnEnable(MemberItem selectedMember)
    {
        // Code below is executed both when the window is opened and at mono reloads.
        if (selectedMember != null)
        {
            m_Item = selectedMember;
            m_EnglishMember.OnEnable(selectedMember);
        }
        else
        {
            m_EnglishMember = null;
        }
    }

    public void OnDestroy()
    {
        if (EditMember != null)
            EditMember.LeaveForced();
    }

    public void OnGUI()
    {
        DocGUI();
        if (ShowDiff)
        {
            GUILayout.Space(DocBrowser.styles.dividerSpace);
            RawTextDiffGUI();
        }
    }

    private void ToolbarGUI()
    {
        EditorGUILayout.BeginHorizontal(EditorStyles.toolbar);

        DocEditButtons();

        GUILayout.FlexibleSpace();

        if (m_Browser.ShowMatchList)
        {
            CancelAcceptButtons(m_Browser, this);
        }
        else
        {
            RevertSaveButtons();
        }

        EditorGUILayout.EndHorizontal();
    }

    private void DocEditButtons()
    {
        bool somethingSelected = m_SelectedText >= 0;

        using (new EditorGUI.DisabledScope(EditMember == null || !EditMember.Loaded || !somethingSelected))
        {
            if (GUILayout.Button("Insert Example", EditorStyles.toolbarButton))
            {
                MemberSubSection section = EditMember.Model.SubSections[m_SelectedSection];
                section.TextBlocks.Insert(m_SelectedText + 1, new ExampleBlock(""));
                EditMember.Model.SanitizeForEditing();
                EditMember.OnModelEdited();
                ClearVerifyScriptExamples();
            }
        }

        EditorGUILayout.Space();

        if (GUILayout.Button("Check Examples(C# only)", EditorStyles.toolbarButton))
        {
            VerifyScriptExamples();
        }

        if (m_CompilationResult != CompilationResult.Unknown)
        {
            EditorGUILayout.Space();
            if (GUILayout.Button("Clear Compile Results", EditorStyles.toolbarButton))
            {
                ClearVerifyScriptExamples();
            }
        }

        EditorGUILayout.Space();
        EditGrouping = GUILayout.Toggle(EditGrouping, "Toggle Grouping Mode", EditorStyles.toolbarButton);
        EditorGUILayout.Space();

        if (GUILayout.Toggle(false, "Preview HTML", EditorStyles.toolbarButton))
        {
            PreviewHtmlFor(EditMember.Member);
        }

        var oldSelectedPlatform = m_SelectedPlatform;
        m_SelectedPlatform = EditorGUILayout.Popup(m_SelectedPlatform, m_Browser.OptionsList);
        if (m_SelectedPlatform != oldSelectedPlatform)
        {
            Debug.LogFormat("selectedPlatform ({0}=>{1})", oldSelectedPlatform, m_SelectedPlatform);
            EditMember.Dirty = true;
        }
    }

    private void PreviewHtmlFor(MemberItem member)
    {
        var docToolsDir = "Tools/DocTools";

        var platformName = m_Browser.OptionsList[m_SelectedPlatform];

        var psi = ProcessUtil.GetStartInfo(Path.Combine(docToolsDir, "APIDocumentationGenerator/bin/APIDocumentationGenerator.exe"),
            "-onefile " + member.ItemName + " -platform " + platformName);

        psi.WorkingDirectory = Path.Combine(DirectoryCalculator.RootDirName, docToolsDir);
        var p = new Process { StartInfo = psi };
        try
        {
            p.Start();
            p.WaitForExit(10000);
        }
        catch (Exception ex)
        {
            Debug.LogError(ex.Message + " ::Unable to execute html preview, Filename = " + psi.FileName + " working directory " +
                psi.WorkingDirectory);
        }
        finally
        {
            var filePath = Path.Combine(DirectoryCalculator.RootDirName,
                "build/UserDocumentation/Documentation/en/ScriptReference/" + member.HtmlName + ".html");
            try
            {
                if (Environment.OSVersion.Platform != PlatformID.MacOSX && Environment.OSVersion.Platform != PlatformID.Unix)
                    Process.Start(filePath);
                else
                    Process.Start(new ProcessStartInfo("open", filePath));
            }
            catch (Exception ex)
            {
                Debug.Log(ex.Message + " ::HTML is ready for preview, but unable to start a browser process (filePath=" + filePath + ")");
            }
        }
    }

    private void RevertSaveButtons()
    {
        using (new EditorGUI.DisabledScope(EditMember == null || !EditMember.Loaded || !EditMember.Dirty))
        {
            if (GUILayout.Button("Revert", EditorStyles.toolbarButton))
            {
                EditorGUIUtility.keyboardControl = 0;
                EditMember.Load();
            }
        }

        using (new EditorGUI.DisabledScope(EditMember == null || !EditMember.Loaded || (!EditMember.Dirty && !EditMember.DirtyAutoChanges)))
        {
            if (GUILayout.Button("Save", EditorStyles.toolbarButton))
                Save();
        }
    }

    private static void CancelAcceptButtons(DocBrowser browser, MemberEditor editor)
    {
        if (GUILayout.Button("Cancel", EditorStyles.toolbarButton))
            browser.ShowMatchList = false;

        using (new EditorGUI.DisabledScope(editor == null || editor.m_OrphanDocItem == editor.m_Item || editor.m_OrphanDocItem == null))
        {
            if (GUILayout.Button("Save Match", EditorStyles.toolbarButton))
                editor.Save();
        }
    }

    public void Save()
    {
        if (m_ConversionError)
        {
            EditorUtility.DisplayDialog("cannot save", "You cannot save a file with a conversion error in it", "OK");
            return;
        }
        if (m_CovariantTypesInSameSection)
        {
            string separateSectionsText = "Signatures with different return types are required to be in separate sections";
            if (!EditGrouping)
            {
                //remind the user of Grouping Mode, if he's not currently in it
                separateSectionsText += " (see 'Toggle Grouping Mode')";
            }
            EditorUtility.DisplayDialog("cannot save",  separateSectionsText, "OK");
            return;
        }
        EditorGUIUtility.keyboardControl = 0;

        var newPlatformName = m_Browser.OptionsList[m_SelectedPlatform];
        var isUnknownSelected = AllPlatformsInfo.IsUnknown(newPlatformName);

        //always reject if Unknown is selected
        if (isUnknownSelected)
        {
            Debug.LogError("Cannot save file to Unknown platform, please select a platform");
            m_SelectedPlatform = m_OriginalSelectedPlatform;
            return;
        }

        var parent = m_Browser.docProject.GetParentMember(EditMember.Member);
        var path = DirectoryCalculator.GetFullPathFromMember(parent);
        bool fileAlreadyExists = !string.IsNullOrEmpty(path);

        //if a specific platform is selected and the file already existed, Save and then Move
        //if a specific platform is selected and the file did not exist, Save at that platform
        //if no platform is selected and the file already exists, Save at the path we found
        if (!fileAlreadyExists)
        {
            Debug.Log("Saving new file for platform " + newPlatformName);
            var pathWithPlatform = DirectoryCalculator.GetFullPathWithPlatform(parent, newPlatformName);
            EditMember.SaveAtPath(pathWithPlatform);
            return;
        }
        Debug.Log("Saving existing file");
        EditMember.SaveAtPath(path);

        if (m_SelectedPlatform != m_OriginalSelectedPlatform)
        {
            var oldPlatformName = m_Browser.OptionsList[m_OriginalSelectedPlatform];

            try
            {
                EditMember.Move(oldPlatformName, newPlatformName);
                Debug.LogFormat("Move from [{0}] to [{1}] successful", oldPlatformName, newPlatformName);
                m_OriginalSelectedPlatform = m_SelectedPlatform; //update the "original" platform to the new one
            }
            catch (System.IO.DirectoryNotFoundException dnfe)
            {
                Debug.LogError(dnfe.Message);
                m_SelectedPlatform = m_OriginalSelectedPlatform; //restore the selected platform
            }
        }
    }

    public static void NoEditorGUI(DocBrowser browser)
    {
        // Member header
        GUILayout.Label("No member selected", DocBrowser.styles.docHeader);

        EditorGUILayout.BeginVertical(DocBrowser.styles.frameWithMargin, GUILayout.ExpandWidth(true));

        EditorGUILayout.BeginHorizontal(EditorStyles.toolbar);
        GUILayout.FlexibleSpace();
        if (browser.ShowMatchList)
            CancelAcceptButtons(browser, null);
        EditorGUILayout.EndHorizontal();

        EditorGUILayout.EndVertical();
    }

    private void DocGUI()
    {
        // Member header
        if (EditMember == null || EditMember.Member == null)
            return;
        string header = EditMember.Member.ItemName;
        if (m_Browser.ShowMatchList)
            if (m_OrphanDocItem != null)
                header = "Match Docs for " + m_OrphanDocItem.ItemName + " to API " + m_Item.ItemName + " ?";
            else
                header = "Match Docs for " + m_Item.ItemName + " to API [No API Item Selected] ?";
        GUILayout.Label(header, DocBrowser.styles.docHeader);

        EditorGUILayout.BeginVertical(DocBrowser.styles.frameWithMargin);

        // Doc toolbar
        ToolbarGUI();

        if (EditMember != null && EditMember.Loaded)
        {
            float width = m_Browser.position.width;
            if (m_Browser.m_ShowList)
                width -= DocBrowser.kListWidth + DocBrowser.styles.dividerSpace + 1;
            if (m_Browser.ShowMatchList)
                width -= DocBrowser.kListWidth + DocBrowser.styles.dividerSpace + 1;

            EditorGUILayout.BeginHorizontal();

            // English Doc scrollview
            DocColumnGUI(m_EnglishMember, false, ref m_DocScroll, width, "");

            EditorGUILayout.EndHorizontal();
        }

        EditorGUILayout.EndVertical();

        DocBrowser.DragEnd();
    }

    private void DocColumnGUI(MemberSession member, bool readOnly, ref Vector2 scroll, float width, string title)
    {
        GUILayout.BeginVertical();

        scroll = GUILayout.BeginScrollView(scroll, DocBrowser.styles.docArea, GUILayout.Width(width));
        DocEditGUI(member, readOnly);
        GUILayout.EndScrollView();

        GUILayout.EndVertical();
    }

    private string TextArea(string text, GUIStyle style, bool readOnly)
    {
        // @TODO: Make SelectableLabel work correctly and then use that,
        /*if (readOnly)
        {
            EditorGUILayout.SelectableLabel (text, style);
            return text;
        }*/

        Rect rect = GUILayoutUtility.GetRect(new GUIContent(text), style);
        //Debug.Log(rect);

        // Hack to prevent select all when clicking in a text field
        Event evt = Event.current;
        Color oldCursorColor = GUI.skin.settings.cursorColor;
        if (evt.type == EventType.MouseDown && rect.Contains(evt.mousePosition))
            GUI.skin.settings.cursorColor = Color.clear;

        //GUI.enabled = !readOnly || Event.current.type == EventType.Repaint;

        EditorGUI.BeginChangeCheck();

        string newText = InternalEditorUtility.TextAreaForDocBrowser(rect, text, style);
        //string newText = EditorGUI.TextArea (rect, text, style);
        if (EditorGUI.EndChangeCheck() && !readOnly)
        {
            text = newText;
            if (style != DocBrowser.styles.example)
                text = MemberSubSection.EnforcePunctuation(text);
            m_Browser.Repaint();
        }

        GUI.skin.settings.cursorColor = oldCursorColor;

        return text;
    }

    void DocEditGUI(MemberSession memberSession, bool readOnly)
    {
        if (EditGrouping)
            GUILayout.Label(
                "Change order of signatures or whole sections by dragging the handles next to signatures or sections headers.\nThe descriptions and examples are hidden in this mode.",
                "helpbox");

        IEnumerable<SignatureEntry> missingDocSignatures =
            memberSession.Member.Signatures.Where(s => s.InAsmOrDoc).Where(s => !s.InDoc);
        if (missingDocSignatures.Any())
        {
            if (!EditGrouping)
            {
                string missingSigsString = string.Join("\n", missingDocSignatures.Select(e => e.Formatted).ToArray());
                EditorGUILayout.HelpBox(
                    "Some signatures are missing from the docs (click 'Edit Grouping' to edit):\n" + missingSigsString,
                    MessageType.Warning);
            }
            else
            {
                // Show signatures with missing docs and allow moving down into doc sub sections
                GUILayout.BeginHorizontal();
                GUILayout.Space(24);
                GUILayout.Label("Signatures Missing from Docs", DocBrowser.styles.docSectionHeader);
                GUILayout.EndHorizontal();
                GUI.backgroundColor = GetColor(true, false);
                GUILayout.BeginVertical(DocBrowser.styles.docArea, GUILayout.ExpandHeight(false));
                GUI.backgroundColor = Color.white;
                List<string> missingSigs = missingDocSignatures.Select(e => e.Name).ToList();
                for (int i = 0; i < missingSigs.Count; i++)
                    SignatureGUI(memberSession, -1, missingSigs, i, readOnly);
                GUILayout.Space(3);
                GUILayout.EndVertical();
            }

            EditorGUILayout.Space();
        }

        m_SelectedSection = -1;
        m_SelectedText = -1;
        if (memberSession.Model.SubSections.Count == 0)
            memberSession.Model.SubSections.Add(new MemberSubSection());

        // Display each section
        m_CovariantTypesInSameSection = false;
        for (int i = 0; i < memberSession.Model.SubSections.Count; i++)
            SubSectionEditGUI(memberSession, i, readOnly);

        // Section header for adding new section
        if (EditGrouping && memberSession.Member.MultipleSignaturesPossible)
        {
            SectionHeaderGUI(memberSession, -1, readOnly);
            EditorGUILayout.Space();
        }

        EditorGUILayout.Space();

        GUILayout.FlexibleSpace();
    }

    bool MiniButton(Rect rect, Texture2D icon)
    {
        return GUI.Button(rect, icon, DocBrowser.styles.miniButton);
    }

    void SectionHeaderGUI(MemberSession memberSession, int sectionIndex, bool readOnly)
    {
        MemberSubSection section = sectionIndex < 0 ? null : memberSession.Model.SubSections[sectionIndex];

        string headerText;
        if (sectionIndex >= 0)
            headerText = "Section " + (sectionIndex + 1) + " of " + memberSession.Model.SubSections.Count;
        else
            headerText = "New Section";

        EditorGUI.BeginChangeCheck();

        if (EditGrouping && !readOnly)
        {
            // Handle dragging of section
            Rect dragTargetRect = GUILayoutUtility.GetRect(10, 0);
            dragTargetRect.y -= 4;
            dragTargetRect.height = 16;
            DocBrowser.DragTarget(dragTargetRect, typeof(MemberSubSection), memberSession.Model.SubSections, sectionIndex >= 0 ? sectionIndex : memberSession.Model.SubSections.Count);

            GUILayout.BeginHorizontal(GUILayout.Height(25));

            GUILayout.Space(4);
            Rect rect = GUILayoutUtility.GetRect(14, 14, GUILayout.ExpandWidth(false));
            rect.y += 11;

            if (sectionIndex >= 0)
            {
                if (memberSession.Model.SubSections.Count > 1)
                {
                    DocBrowser.DragHandle(rect, typeof(MemberSubSection), memberSession.Model.SubSections, sectionIndex, DocBrowser.styles.dragHandle);

                    if (section.SignatureList.Count == 0)
                    {
                        GUILayout.Space(4);
                        rect = GUILayoutUtility.GetRect(14, 14, GUILayout.ExpandWidth(false));
                        rect.y += 11;
                        if (MiniButton(rect, DocBrowser.styles.iconRemove))
                        {
                            if (EditorUtility.DisplayDialog(
                                "Remove Section docs",
                                "Remove the section and all its documentation for " + memberSession.Member.ItemName + "?",
                                "Remove",
                                "Cancel"
                            ))
                            {
                                memberSession.Model.SubSections.RemoveAt(sectionIndex);
                            }
                        }
                    }
                }
            }
            else
            {
                if (MiniButton(rect, DocBrowser.styles.iconAdd))
                {
                    memberSession.Model.SubSections.Add(new MemberSubSection(sTodoText));
                }
            }

            GUILayout.Label(headerText, DocBrowser.styles.docSectionHeader);
            GUILayout.EndHorizontal();
        }
        else
            GUILayout.Label(headerText, DocBrowser.styles.docSectionHeader);

        if (sectionIndex >= 0)
        {
            // Handle dragging of signature
            Rect dragTargetRect = GUILayoutUtility.GetRect(10, 0);
            dragTargetRect.y -= 3;
            dragTargetRect.height = 14;
            DocBrowser.DragTarget(dragTargetRect, typeof(SignatureEntry), section.SignatureList, 0);
        }

        if (EditorGUI.EndChangeCheck())
            memberSession.OnModelEdited(true);

        if (EditGrouping && section != null && section.SignatureList.Count == 0 && memberSession.Member.MultipleSignaturesPossible && memberSession.Model.SubSections.Count > 1)
        {
            if ((string.IsNullOrEmpty(section.Summary) || section.Summary == sTodoText) && section.Parameters.Count == 0 && section.ReturnDoc == null)
                GUILayout.Label("Section has no documentation. It can safely be deleted.", DocBrowser.styles.docSectionMessage);
            else
                GUILayout.Label("Section has documentation. (Exit 'Edit Grouping' mode to see it.) Deleting this section will delete its docs too.", DocBrowser.styles.docSectionMessageWarning);
        }
        if (section != null && section.SignatureList.Count > 0)
        {
            IEnumerable<SignatureEntry> asmEntries = section.SignatureList.Select(e => memberSession.Member.GetSignature(e, true));
            asmEntries = asmEntries.Where(e => e.InAsm);
            IEnumerable<string> returnTypes = asmEntries.Select(e => e.Asm.ReturnType);
            returnTypes = returnTypes.Distinct();
            if (returnTypes.Count() > 1)
            {
                GUILayout.Label("Section has signatures with multiple different return types: " + string.Join(", ", returnTypes.ToArray()) + ".", DocBrowser.styles.docSectionMessageWarning);
                m_CovariantTypesInSameSection = true;
            }
        }
    }

    Color GetColor(bool hasSource, bool hasDoc)
    {
        if (!hasSource)
            return new Color(1, 0.7f, 0.7f, 1);
        if (!hasDoc)
            return new Color(1, 1, 0.7f, 1);
        return Color.white;
    }

    void SignatureGUI(MemberSession memberSession, int sectionIndex, List<string> signatureList, int signatureIndex, bool readOnly)
    {
        string sig = signatureList[signatureIndex];
        SignatureEntry sigEntry = memberSession.Member.GetSignature(sig, true);

        bool edit = !readOnly && memberSession.Member.MultipleSignaturesPossible;

        EditorGUI.BeginChangeCheck();

        GUILayout.BeginHorizontal();

        if (edit)
        {
            GUILayout.Space(4);
            Rect rect = GUILayoutUtility.GetRect(14, 14, GUILayout.ExpandWidth(false));
            rect.y += 4;

            DocBrowser.DragHandle(rect, typeof(SignatureEntry), signatureList, signatureIndex, DocBrowser.styles.dragHandle);

            if (!sigEntry.InAsm || sigEntry.Asm.Private)
            {
                GUILayout.Space(4);
                rect = GUILayoutUtility.GetRect(14, 14, GUILayout.ExpandWidth(false));
                rect.y += 4;
                if (MiniButton(rect, DocBrowser.styles.iconRemove))
                {
                    bool isLastSignatureForThisSection = signatureList.Count == 1;
                    if (isLastSignatureForThisSection)
                    {
                        if (EditorUtility.DisplayDialog(
                            "Remove section?",
                            "This is the last signature for this section. \nRemove the section and all of its documentation for " + memberSession.Member.ItemName + "?",
                            "Remove",
                            "Cancel"))
                        {
                            memberSession.Model.SubSections.RemoveAt(sectionIndex);
                        }
                    }
                    else
                    {
                        signatureList.RemoveAt(signatureIndex);
                    }
                }
            }
        }

        string sigString = m_Browser.ShowRawNames ? sigEntry.Name : sigEntry.FormattedHTML;
        if (sigEntry.InAsm && sigEntry.Asm.Private)
            sigString = "<b><color=red>private</color></b> " + sigString;

        GUILayout.Label(sigString, DocBrowser.styles.signature);

        GUILayout.EndHorizontal();

        // Handle dragging
        Rect dragTargetRect = GUILayoutUtility.GetRect(10, 0);
        dragTargetRect.y -= 3;
        dragTargetRect.height = 14;
        DocBrowser.DragTarget(dragTargetRect, typeof(SignatureEntry), signatureList, signatureIndex + 1);

        if (EditorGUI.EndChangeCheck())
            memberSession.OnModelEdited(true);
    }

    void SubSectionEditGUI(MemberSession member, int sectionIndex, bool readOnly)
    {
        MemberSubSection section = member.Model.SubSections[sectionIndex];

        if ((member.Member.MultipleSignaturesPossible && member.Model.SubSections.Count > 1) || EditGrouping)
            SectionHeaderGUI(member, sectionIndex, readOnly);
        else
            GUILayout.Space(18);

        // Toggle for NoDoc
        Rect toggleRect = GUILayoutUtility.GetRect(10, 0);
        toggleRect.height = 16;
        toggleRect.y -= 16;
        toggleRect.xMin = toggleRect.xMax - 175;
        EditorGUI.BeginChangeCheck();
        section.IsUndoc = GUI.Toggle(toggleRect, section.IsUndoc,
            new GUIContent("Exclude section from docs", "also known as UNDOC"));
        if (EditorGUI.EndChangeCheck())
            member.OnModelEdited();

        // Show signatures
        for (int i = 0; i < section.SignatureList.Count; i++)
            SignatureGUI(member, sectionIndex, section.SignatureList, i, readOnly);

        if (section.IsUndoc)
        {
            EditorGUILayout.HelpBox(
                "This section will be excluded from the documentation but it will still show up in intelli-sense etc. Many users may not realize it's not officially supported. Consider either making the API internal or include it in the documentation.",
                MessageType.Warning);
        }
        else
        {
            if (!EditGrouping)
            {
                EditorGUI.BeginChangeCheck();
                ParametersAndReturnGUI(member, sectionIndex, readOnly);
                DescriptionAndExamplesGUI(member, sectionIndex, readOnly);
                if (EditorGUI.EndChangeCheck())
                    member.OnModelEdited();
            }
        }

        EditorGUILayout.Space();

        GUI.enabled = true;
    }

    string ParamOrReturnGUI(bool hasAsm, bool hasDoc, string label, string text, bool readOnly, out bool remove, IList list = null, int index = -1)
    {
        remove = false;

        EditorGUILayout.BeginHorizontal();

        bool edit = !readOnly;

        bool hasSource, hasDest;

        hasSource = hasAsm;
        hasDest = hasDoc;

        Rect labelRect = GUILayoutUtility.GetRect(160 + (edit ? 18 : 0), 16, GUILayout.ExpandWidth(false));
        labelRect.xMin += 4;
        labelRect.y += 2;

        if (edit)
        {
            // Drag handle
            Rect dragRect = new Rect(labelRect.x, labelRect.y + 2, 14, 14);
            if (list != null)
                DocBrowser.DragHandle(dragRect, typeof(ParameterWithDoc), list, index, DocBrowser.styles.dragHandle);
            labelRect.xMin += 14 + 4;

            // remove button
            if (!hasSource)
            {
                Rect buttonRect = new Rect(labelRect.x, labelRect.y + 2, 14, 14);
                if (MiniButton(buttonRect, DocBrowser.styles.iconRemove))
                    remove = true;
                labelRect.xMin += 14;
            }
        }

        // Label
        GUI.Label(labelRect, label, DocBrowser.styles.paramLabel);

        // Text field
        if (!readOnly)
            GUI.backgroundColor = GetColor(hasSource, hasDest);
        text = TextArea(text, DocBrowser.styles.param, readOnly);
        GUI.backgroundColor = Color.white;

        EditorGUILayout.EndHorizontal();

        // Handle dragging
        if (list != null)
        {
            Rect dragTargetRect = GUILayoutUtility.GetRect(10, 0);
            dragTargetRect.y -= 6;
            dragTargetRect.height = 14;
            DocBrowser.DragTarget(dragTargetRect, typeof(ParameterWithDoc), list, index + 1);
        }

        return text;
    }

    void ParametersAndReturnGUI(MemberSession memberSession, int sectionIndex, bool readOnly)
    {
        if (sectionIndex >= memberSession.Model.SubSections.Count)
        {
            return;
        }
        MemberSubSection section = memberSession.Model.SubSections[sectionIndex];

        // Parameters
        if (section.Parameters.Count > 0)
        {
            GUILayout.Label("Parameters", DocBrowser.styles.docSectionHeader);

            // Handle dragging parameter into first slot
            Rect dragTargetRect = GUILayoutUtility.GetRect(10, 0);
            dragTargetRect.y -= 6;
            dragTargetRect.height = 14;
            DocBrowser.DragTarget(dragTargetRect, typeof(ParameterWithDoc), section.Parameters, 0);

            ParameterWithDoc paramToRemove = null;
            for (int i = 0; i < section.Parameters.Count; i++)
            {
                ParameterWithDoc p = section.Parameters[i];
                string label = "<b>" + p.Name + " : </b>" + (p.HasAsm ? Extensions.GetNiceName(p.TypeString) : "?");
                bool remove;
                p.Doc = ParamOrReturnGUI(p.HasAsm, p.HasDoc, label, p.Doc, readOnly, out remove, list: section.Parameters, index: i);
                if (remove)
                    paramToRemove = p;
            }
            if (paramToRemove != null)
                section.Parameters.Remove(paramToRemove);
        }

        // Returns
        String returnType = "void";
        if (section.SignatureList.Count > 0)
        {
            SignatureEntry sigEntry = memberSession.Member.GetSignature(section.SignatureList[0], true);
            if (sigEntry.InAsm && sigEntry.Asm.EntryType == AssemblyType.Method)
            {
                returnType = sigEntry.Asm.ReturnType;
            }
        }
        if (section.ReturnDoc != null || (returnType != "void" && returnType != "" && returnType != null))
        {
            GUILayout.Label("Returns", DocBrowser.styles.docSectionHeader);

            string label = "?";
            if (section.ReturnDoc != null)
            {
                ReturnWithDoc p = section.ReturnDoc;
                label = (p.HasAsm ? Extensions.GetNiceName(p.ReturnType) : Extensions.GetNiceName(returnType));
                bool remove;
                p.Doc = ParamOrReturnGUI(p.HasAsm, p.HasDoc, label, p.Doc, readOnly, out remove);
                if (remove)
                    section.ReturnDoc = null;
            }
            else
            {
                label = Extensions.GetNiceName(returnType);
                bool remove;
                section.ReturnDoc = new ReturnWithDoc();
                section.ReturnDoc.Doc = ParamOrReturnGUI(true, false, returnType, "", readOnly, out remove);
                if (remove)
                    section.ReturnDoc = null;
            }
        }

        EditorGUILayout.Space();
    }

    void DescriptionAndExamplesGUI(MemberSession memberSession, int sectionIndex, bool readOnly)
    {
        if (sectionIndex >= memberSession.Model.SubSections.Count)
        {
            return;
        }
        MemberSubSection section = memberSession.Model.SubSections[sectionIndex];

        // Summary
        string controlName = "Section_" + sectionIndex + "_Summary";
        GUI.SetNextControlName(controlName);
        if (GUI.GetNameOfFocusedControl() == controlName)
        {
            m_SelectedSection = sectionIndex;
            m_SelectedText = 0;
        }
        GUILayout.Label("Description (first line is summary)", DocBrowser.styles.docSectionHeader);
        if (!readOnly)
            GUI.backgroundColor = GetColor(true, section.Summary != string.Empty);
        section.Summary = TextArea(section.Summary, DocBrowser.styles.description, readOnly);
        GUI.backgroundColor = Color.white;

        // Description and examples
        int i = 0;
        int removeExample = -1;
        int exampleCount = 0;
        foreach (TextBlock t in section.TextBlocks)
        {
            bool isExample = (t is ExampleBlock);

            controlName = "Section_" + sectionIndex + "_Text_" + i;
            GUI.SetNextControlName(controlName);
            if (GUI.GetNameOfFocusedControl() == controlName && !isExample)
            {
                m_SelectedSection = sectionIndex;
                m_SelectedText = i;
            }

            if (isExample)
            {
                exampleCount++;

                GUILayout.Space(4);

                EditorGUILayout.BeginHorizontal();
                var example = t as ExampleBlock;

                GUILayout.FlexibleSpace();

                using (new EditorGUI.DisabledScope(readOnly))
                {
                    example.IsConvertExample = GUILayout.Toggle(example.IsConvertExample, new GUIContent("Auto-Convert",
                        "auto-conversion will only happen if one of the languages is empty for this example"));
                    example.IsNoCheck = GUILayout.Toggle(example.IsNoCheck, new GUIContent("NoCheck",
                        "If this is on, the UnityScript example won't be verified for correctness (use only for unsupported languages, like PHP)"));

                    if (GUILayout.Button("Delete this example (all languages)", EditorStyles.miniButton))
                    {
                        removeExample = i;
                        ClearVerifyScriptExamples();
                    }
                }

                EditorGUILayout.EndHorizontal();

                EditorGUILayout.BeginHorizontal();
                GUILayout.Label("Example (C#)", DocBrowser.styles.exampleHeader);
                GUI.enabled = true;
                if (GUILayout.Button("Convert C# => UnityScript  (Preview)", EditorStyles.miniButton))
                    ConvertPreviewWindow.Init(example, ConvertPreviewWindow.SourceLang.CS, m_Browser);
                EditorGUILayout.EndHorizontal();

                // Check compile results for this script and apply a background color if we have run the script verification test.
                var oldColor = GUI.backgroundColor;
                bool hasErrors = false;
                if (!example.IsNoCheck && m_CompilationResult != CompilationResult.Unknown)
                {
                    if (m_CompilationResult == CompilationResult.Fail)
                        hasErrors = true;

                    GUI.backgroundColor = hasErrors ? new Color(0.9f, 0, 0) : Color.green;
                }

                example.CsExample = TextArea(example.CsExample, DocBrowser.styles.example, readOnly);

                if (hasErrors)
                {
                    GUI.backgroundColor = new Color(0.75f, 0, 0);
                    TextArea(m_CompilationErrorString, DocBrowser.styles.example, true);
                }
                GUI.backgroundColor = oldColor;

                EditorGUILayout.BeginHorizontal();
                GUILayout.Label("Example (UnityScript)", DocBrowser.styles.exampleHeader);
                if (GUILayout.Button("Convert UnityScript => C#", EditorStyles.miniButton))
                    ConvertPreviewWindow.Init(example, ConvertPreviewWindow.SourceLang.JS, m_Browser);
                EditorGUILayout.EndHorizontal();

                var exampleStyle = m_ConversionError ? DocBrowser.styles.exampleError : DocBrowser.styles.example;
                example.JsExample = TextArea(example.JsExample, exampleStyle, readOnly);
            }
            else
            {
                if (i > 0)
                    GUILayout.Label("Description (continued)", DocBrowser.styles.docSectionHeader);
                t.Text = TextArea(t.Text, DocBrowser.styles.description, readOnly);
            }

            i++;
        }

        if (removeExample >= 0)
        {
            section.TextBlocks.RemoveAt(removeExample);
            EditMember.Model.SanitizeForEditing();
        }
    }

    void ClearVerifyScriptExamples()
    {
        m_CompilationResult = CompilationResult.Unknown;
        m_CompilationErrorString = string.Empty;
    }

    void VerifyScriptExamples()
    {
        List<string> codeExamples = new List<string>();
        try
        {
            EditorUtility.DisplayProgressBar("Compiling Example Scripts", "Preparing", 0.0f);

            ClearVerifyScriptExamples();
            foreach (TextBlock textBlock in EditMember.Model.TextBlocks)
            {
                var exampleBlock = textBlock as ExampleBlock;
                if (exampleBlock != null && !exampleBlock.IsNoCheck)
                {
                    codeExamples.Add(exampleBlock.CsExample);
                }
            }

            EditorUtility.DisplayProgressBar("Compiling Example Scripts", "Testing", 0.5f);

            // Setup compiler
            var providerOptions = new Dictionary<string, string>
            {
                {"CompilerVersion", "v3.5"}
            };
            var domProvider = new CSharpCodeProvider(providerOptions);
            var tempFiles = new TempFileCollection() {KeepFiles = false};

            var compilerParams = new CompilerParameters
            {
                GenerateExecutable = false,
                GenerateInMemory = false,
                TreatWarningsAsErrors = false,
                CompilerOptions = "/unsafe",
                TempFiles = tempFiles,
            };
            var assemblies = AppDomain.CurrentDomain.GetAssemblies();
            foreach (var assembly in assemblies)
            {
                try
                {
                    compilerParams.ReferencedAssemblies.Add(assembly.Location);
                }
                catch (NotSupportedException)
                {
                    //we have some "dynamic" assmeblies that do not have a filename
                }
            }

            // Attempt to compile the scripts separately.
            bool failedToCompile = false;
            foreach (string script in codeExamples)
            {
                var compileResults = domProvider.CompileAssemblyFromSource(compilerParams, script);

                foreach (CompilerError error in compileResults.Errors)
                {
                    // In certain instances .HasErrors is set incorrectly because the compiler adds additional messages after warnings
                    // Example: /Unity.app/Contents/Managed/UnityEngine/UnityEngine.CoreModule.dll (Location of the symbol related to previous warning)
                    // This is not set as a warning, it seems like interally it may be causing HasErrors to be set.
                    // We'll check that the error has a valid error number as it seems more stable.

                    if (!error.IsWarning && !string.IsNullOrEmpty(error.ErrorNumber))
                    {
                        failedToCompile = true;
                        m_CompilationErrorString += string.Format("{0}({1},{2}): {3}\n", error.ErrorNumber, error.Line, error.Column, error.ErrorText);
                    }
                }
            }


            if (failedToCompile && codeExamples.Count > 1)
            {
                // Its possible that the scripts are all part of one example so we should compile them together and see if that works instead.
                var combinedResults = domProvider.CompileAssemblyFromSource(compilerParams, codeExamples.ToArray());
                if (!combinedResults.Errors.HasErrors)
                {
                    failedToCompile = false;
                    m_CompilationErrorString = string.Empty;
                }
            }

            tempFiles.Delete();

            m_CompilationResult = failedToCompile ? CompilationResult.Fail : CompilationResult.Pass;
        }
        finally
        {
            EditorUtility.ClearProgressBar();
        }
    }

    void RawTextDiffGUI()
    {
        EditorGUILayout.BeginVertical(DocBrowser.styles.frameWithMarginWhite, GUILayout.Height(150), GUILayout.ExpandHeight(false));

        if (EditMember != null && EditMember.Loaded)
        {
            Color red, green;
            if (EditorGUIUtility.isProSkin)
            {
                green = new Color(0.1f, 0.3f, 0.1f);
                red = new Color(0.35f, 0.1f, 0.1f);
            }
            else
            {
                green = new Color(0.8f, 1.0f, 0.8f);
                red = new Color(1.0f, 0.8f, 0.8f);
            }

            m_RawTextScroll = EditorGUILayout.BeginScrollView(m_RawTextScroll);

            foreach (StringDiff.DiffLine d in EditMember.Diff.diffLines)
            {
                if (d.status == 0)
                    GUI.backgroundColor = Color.clear;
                else if (d.status == 1)
                    GUI.backgroundColor = green;
                else if (d.status == -1)
                    GUI.backgroundColor = red;
                GUILayout.Label(d.text, DocBrowser.styles.dif);
            }
            GUI.backgroundColor = Color.white;

            EditorGUILayout.EndScrollView();
        }

        EditorGUILayout.EndHorizontal();
    }
}

[InitializeOnLoad]
public class MonoStartupFix
{
    static MonoStartupFix()
    {
        // Sometimes(mainly on OSX) the path may not contain the mono gmcs so add it to the path to prevent issues when verifying the scripts.
        var name = "PATH";
        var PATH = System.Environment.GetEnvironmentVariable(name);
        var MonoPath = Path.Combine(EditorApplication.applicationContentsPath, "Mono/bin");
        var value = PATH + ":" + MonoPath;
        var target = System.EnvironmentVariableTarget.Process;
        System.Environment.SetEnvironmentVariable(name, value, target);
    }
}
