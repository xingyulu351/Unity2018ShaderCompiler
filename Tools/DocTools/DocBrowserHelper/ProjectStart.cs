using UnityEngine;
using UnityEditor;
using DocBrowserHelper;

[InitializeOnLoad]
public class ProjectStart
{
    static ProjectStart()
    {
        DocBrowserUtility.InitialUnderlyingModel();
    }
}
