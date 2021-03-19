using JamSharp.Runtime;
using External.Jamplus.builds.bin;

namespace Tools.DocTools.DocBrowserHelper
{
    class DocBrowserHelper : ConvertedJamFile
    {
        internal static void TopLevel()
        {
            Jambase.SubDir(JamList("DocTools", "DocBrowserHelper"));
            JamList extDeps = JamList("DefaultEditorDll", "DefaultEngineDll");
            Jamrules.BuildAssemblyFromFolderWithCSFiles(
                "DocBrowserHelper",
                "3.5",
                $"{Vars.TOP}/Tools/DocTools/UnityDocBrowser/Assets/DocBrowserHelper.dll",
                $"{Vars.TOP}/Tools/DocTools/DocBrowserHelper",
                extDeps);
        }
    }
}
