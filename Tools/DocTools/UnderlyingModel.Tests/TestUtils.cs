using System.IO;
using NUnit.Framework;

namespace UnderlyingModel.Tests
{
    public class TestUtils
    {
        public static IMemberGetter CreateMemberGetterFromTestAssembly()
        {
            if (m_MemberGetter != null)
                return m_MemberGetter;

            var a = new EmptyClass();
            string st = a.GetType().Assembly.CodeBase;
            st = st.Replace("file:///", "");
            if (System.Environment.OSVersion.Platform == System.PlatformID.Unix)
                st = "/" + st;
            st = Path.GetDirectoryName(st);
            var dirs = new MemberItemDirectories
            {
                Assemblies = st,
                Mem2files = Path.Combine(DirectoryCalculator.RootDirName, "Tools/DocTools/UnderlyingModel.TestAssemblySource/TestDocs")
            };
            var newDataItemProject = new NewDataItemProject(dirs, false);
            string[] assemblies = Directory.GetFiles(dirs.Assemblies, "Test*.dll", SearchOption.AllDirectories);
            newDataItemProject.PopulateDllsList(assemblies);
            newDataItemProject.ReloadAllProjectData();

            Assert.GreaterOrEqual(newDataItemProject.ItemCount, 0, "Project should not be empty, did you load the right Dlls?");
            Assert.LessOrEqual(newDataItemProject.ItemCount, 1000, "Project contains too many symbols, did you load the right Dlls?");
            m_MemberGetter = new MemberGetter(newDataItemProject);
            return m_MemberGetter;
        }

        private static IMemberGetter m_MemberGetter = null;

        public static IMemberGetter CreateNewDataItemProjectFromLegacyDlls()
        {
            if (m_ProjectFixedDlls != null)
                return m_ProjectFixedDlls;

            var dirs = DirectoryCalculator.GetMemberItemDirectoriesAsmXml(
                "Tools/DocTools/TestDlls",
                "Tools/DocTools/TestXml");
            var project = new NewDataItemProject(dirs, false);
            project.ReloadAllProjectData();
            m_ProjectFixedDlls = new MemberGetter(project);
            return m_ProjectFixedDlls;
        }

        private static IMemberGetter m_ProjectFixedDlls = null;
    }
}
