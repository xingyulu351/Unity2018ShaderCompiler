using System;
using System.IO;

namespace UnderlyingModel
{
    [Serializable]
    public class MemberItemDirectories
    {
        public string Memfiles;
        public string Mem2files;
        public string Mem2TranslatedRoot;
        public string Assemblies;
        public string StaticFilesSource;
        public string XSLTSource;
        public string XmlOutputRoot;
        public string HtmlOutputRoot;
        public void PrependRootDirectory(string root)
        {
            if (Memfiles != null) Memfiles = Path.Combine(root, Memfiles);
            if (Mem2files != null) Mem2files = Path.Combine(root, Mem2files);
            if (Mem2TranslatedRoot != null) Mem2TranslatedRoot = Path.Combine(root, Mem2TranslatedRoot);
            if (Assemblies != null) Assemblies = Path.Combine(root, Assemblies);
            if (StaticFilesSource != null) StaticFilesSource = Path.Combine(root, StaticFilesSource);
            if (XSLTSource != null) XSLTSource = Path.Combine(root, XSLTSource);
            if (XmlOutputRoot != null) XmlOutputRoot = Path.Combine(root, XmlOutputRoot);
            if (HtmlOutputRoot != null) HtmlOutputRoot = Path.Combine(root, HtmlOutputRoot);
        }
    }
}
