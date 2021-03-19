using System;
using UnityEditor;
using UnityEngine;

public class DocBrowserLegendWindow : EditorWindow
{
    Vector2 scroll;

    public DocBrowserLegendWindow()
    {
        this.titleContent = new GUIContent("Legend");
    }

    string HelpText
    {
        get
        {
            return @"Supported Hyperlinks

    [[ClassName]]                               <a href=""ClassName.html"">ClassName</a>
    ::ref::propName                             <a href=""CurrentClass-propName.html"">propName</a>
    ::ref::MethodName                               <a href=""CurrentClass-propName.html"">propName</a>
    ClassName::ref::propName or
    [[ClassName.propName]]                          <a href=""ClassName-propName.html"">ClassName.propName</a>
    ClassName::ref::MethodName or
    [[ClassName.MethodName]]                            <a href=""ClassName.MethodName.html"">ClassName.MethodName</a>
    ClassName::pref::propName                           <a href=""ClassName-propName.html"">propName</a>
    ClassName::pref::MethodName                         <a href=""ClassName.MethodName.html"">MethodName</a>
    [[wiki:manual_page|Link Text]]                           <a href=""../Manual|Components/manual_page.html"">Link Text</a>
    [[wiki:comp:manual_page|Link Text]]                      <a href=""../Components/manual_page.html"">Link Text</a>
    [[wiki:man:manual_page|Link Text]]                       <a href=""../Manual/manual_page.html"">Link Text</a>


Formatting

    __x__               Makes x bold
    ''x''               Makes x italic
    @@x@@               Makes x monospaced
    /x/             Makes x italic (no whitespaces around x)
    \\              Inserts a line break (<br/>).
    SA:             Makes a 'See Also' line in a specific style.
    (RO)                Expands to (Read Only)
    {img foo.png}           --> <img class='refimg' src='ScriptRefImages/foo.png' />";
        }
    }

    void OnGUI()
    {
        if (GUILayout.Button("Open in Confluence", GUILayout.Width(150)))
        {
            Application.OpenURL("http://confluence.hq.unity3d.com/display/DEV/Documenting+Scripting+API");
        }

        scroll = EditorGUILayout.BeginScrollView(scroll);
        EditorGUILayout.TextArea(HelpText, GUILayout.Height(position.height - 30));
        EditorGUILayout.EndScrollView();
    }
}
