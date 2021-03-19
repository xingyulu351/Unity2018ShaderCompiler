using UnityEngine;
using UnityEditor;
using UnderlyingModel;
using UnderlyingModel.MemDoc;
using UnderlyingModel.ItemSerializers;

[System.Serializable]
internal class MemberSession
{
    public NewDataItemProject DocProject;
    public MemberItem Member = null;
    public MemDocModel DummyModel { get; private set; }
    private bool m_IsDummy = false;

    public MemDocModel Model
    {
        get
        {
            if (DummyModel != null)
                return DummyModel;
            return Member.DocModel;
        }
    }
    public string TextOrig = "";
    public string TextCurrent = "";
    public bool Dirty = false;
    public bool DirtyAutoChanges = false;
    public StringDiff Diff;
    internal readonly MemberItemIOMem2 m_ItemIO;

    public MemberSession(NewDataItemProject docProject, MemberItem member)
    {
        DocProject = docProject;
        Member = member;
        m_ItemIO = new MemberItemIOMem2(DocProject);
        Load();
    }

    public MemberSession(NewDataItemProject docProject, MemberItem member, string oldMemberText)
    {
        DocProject = docProject;
        Member = member;

        m_IsDummy = true;
        try
        {
            DummyModel = member.LoadDoc(oldMemberText);
        }
        catch
        {
            Debug.Log("illegal load operation");
        }
        Model.EnforcePunctuation();
        Model.SanitizeForEditing();

        TextOrig = Model.ToString();
        TextCurrent = TextOrig;
    }

    public void OnEnable(MemberItem member)
    {
        if (member == null)
            return;
        if (m_IsDummy)
        {
            Member = member;
            DummyModel = member.LoadDoc(TextOrig);
            DummyModel.EnforcePunctuation();
            DummyModel.SanitizeForEditing();
        }
        else
        {
            Member = member;
            if (m_ItemIO == null)
                return;
            m_ItemIO.LoadDoc(member);
            Debug.Log("loading TextCurrent=" + TextCurrent);
            member.LoadDoc(TextCurrent);
            Model.EnforcePunctuation();
            Model.SanitizeForEditing();
            if (Diff == null)
                Diff = new StringDiff();
            Diff.Compare(TextOrig, TextCurrent);
        }
    }

    public bool Loaded { get { return Member != null && Model != null && Diff != null; } }

    public void Load()
    {
        if (m_IsDummy)
            return;

        if (Member == null)
        {
            Debug.LogError("Cannot load member since it is null.");
            return;
        }

        try
        {
            var parent = DocProject.GetParentMember(Member);
            m_ItemIO.LoadDoc(parent); //don't forget to fill in the relevant child
        }
        catch (System.Exception ex)
        {
            //Debug.LogError("error loading " + Member.ItemName + ";" + ex.Message );
        }
        Model.EnforcePunctuation();
        Model.SanitizeForEditing();

        TextOrig = Model.ToString();
        TextCurrent = TextOrig;
        if (Diff == null)
            Diff = new StringDiff();
        Diff.Compare(TextOrig, TextCurrent);

        Dirty = false;
        DirtyAutoChanges = false;
    }

    public void SaveAtPath(string path)
    {
        if (m_IsDummy)
            return;

        var parent = DocProject.GetParentMember(Member);

        m_ItemIO.SaveDocAtPath(parent, path);
        Load();
    }

    public void Save()
    {
        if (m_IsDummy)
            return;

        var parent = DocProject.GetParentMember(Member);

        var path = DirectoryCalculator.GetFullPathFromMember(parent);
        m_ItemIO.SaveDocAtPath(parent, path);
        Load();
    }

    //this will throw if the Move fails
    internal void Move(string oldPlatformName, string newPlatformName)
    {
        var parent = DocProject.GetParentMember(Member);
        m_ItemIO.MoveDoc(parent, oldPlatformName, newPlatformName);
    }

    public bool LeaveWithPermission()
    {
        if (m_IsDummy || !Loaded || !Dirty)
        {
            if (DirtyAutoChanges)
                Load();
            return true;
        }
        int choice = EditorUtility.DisplayDialogComplex(
            "Save Documentation for Member",
            "Do you want to save the documentation for " + Member.ItemName + "?",
            "Save",     // 0
            "Don't Save",     // 1
            "Cancel"     // 2
        );
        if (choice == 2)
            return false;
        if (choice == 0)
            Save();
        else
            Load();
        return true;
    }

    public void LeaveForced()
    {
        if (m_IsDummy || !Dirty)
        {
            if (DirtyAutoChanges)
                Load();
            return;
        }
        if (EditorUtility.DisplayDialog(
            "Save Documentation for Member",
            "Do you want to save the documentation for " + Member.ItemName + "?",
            "Save",
            "Don't Save"
            ))
            Save();
        else
            Load();
    }

    public void OnModelEdited(bool reload = false)
    {
        TextCurrent = Model.ToString();
        Dirty = true;
        if (reload)
            Member.LoadDoc(TextCurrent);
        Diff.Compare(TextOrig, TextCurrent);
    }
}
