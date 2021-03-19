using System.Linq;
using External.Jamplus.builds.bin;
using External.Jamplus.builds.bin.modules;
using JamSharp.Runtime;
using Tools.BugReporterV2.jam_modules;
using Unity.BuildTools;
using static JamSharp.Runtime.BuiltinRules;

namespace Tools.BugReporterV2
{
    class BugReporterRules : ConvertedJamFile
    {
        internal static void TopLevel()
        {
            JamCore.RegisterAction("Test", @"$(COMMAND)", ActionsFlags.AllowUnwrittenOutputFiles);
            Qt.TopLevel();
            JamCore.RegisterAction("InfoPList", @"$(PROPERTY_LIST_COMMANDS)");
        }

        internal static void SubProject(JamList target)
        {
            c_autodetect.ActiveProject(target);
            C.C_RuntimeType(target, "dynamic");
            C.C_Defines(target, JamList("WINDOWS", "WIN64", "UNICODE", "_UNICODE", "_CRT_SECURE_NO_WARNINGS"), JamList(), "win64");
            C.C_Defines(target, JamList("MACOSX", "MACX64"), JamList(), "macosx64");
            C.C_Defines(target, JamList("LINUX", "_GLIBCXX_USE_CXX11_ABI=0"), JamList(), "linux64");

            C.C_IncludeDirectories(
                target,
                JamList(
                    BugReporter.Base.Combine("common"),
                    BugReporter.Base.Combine("include")));
            C.C_CPlusPlusFlags(
                target,
                JamList("-std=c++11", "-stdlib=libc++", "-Wno-inconsistent-missing-override"),
                JamList(),
                JamList("macosx64"));
            C.C_CPlusPlusFlags(target, JamList("-std=c++11", "-fPIC", "-fpermissive", "-Wno-deprecated-declarations"), JamList(), "linux64");
            C.C_Flags(
                JamList("CC", "C++", "MM", "LINK"),
                target,
                JamList("-stdlib=libc++", "-mmacosx-version-min=10.7"),
                JamList(),
                JamList("macosx64"));
            C.C_OutputPostfixClear(target);
            C.C_OutputPostfix(target, ".debug", "debug");
            C.C_OutputPostfix(target, ".release", "release");
        }

        internal static JamList InfoPList(JamList target, JamList values)
        {
            JamList plistFile = JamList("Info.plist").SetGrist(target.SetGrist(""));
            Jambase.MakeLocate(plistFile, Paths.UnityRoot.Combine("artifacts/bugreporter/tmp"));
            JamList commands = JamList();
            foreach (JamList value in values.ElementsAsJamLists)
            {
                JamList record = new JamList(value.Single().Split('='));

                JamList elements =
                    JamList("defaults", "write", plistFile.GetBoundPath(), record.IndexedBy(1), "-string", Combine("'", record.IndexedBy(2), "'"));
                commands.Append(elements.JoinWithValue(Vars.SPACE));
            }
            commands.Append(Combine("chmod 644 ", plistFile.GetBoundPath(), ""));
            "PROPERTY_LIST_COMMANDS".On(plistFile).Assign(commands.JoinWithValue(";"));
            InvokeJamAction("InfoPList", plistFile);
            Depends(target, plistFile);

            return plistFile;
        }

        internal static JamList TestApplication(JamList sources)
        {
            JamList target = Vars.ACTIVE_PROJECT.Clone();
            C.C_IncludeDirectories(target, $"{Paths.UnityRoot}/External/UnitTest++/src");
            C.C_LinkLibraries(target, JamList("testing", "bugreporterunittest++"));
            C.C_OutputPath(target, $"{BugReporter.ArtifactsPath}/{target}");
            // Order matters here!
            JamList app = C.C_Application(target, sources);
            JamList testTarget = Combine("test:", target.SetGrist(""));
            Depends(testTarget, target);
            Depends("test", testTarget);
            "COMMAND".On(testTarget).Assign(app.First());
            InvokeJamAction("Test", testTarget);

            return app;
        }

        internal static void TestQtApplication(JamList sources, JamList components)
        {
            JamList target = Vars.ACTIVE_PROJECT.Clone();
            JamList binary = TestApplication(sources);
            JamList destFolder = C.C__retrieveOutputPath(target);

            if (Vars.TargetPlatformIsWindows)
            {
                Qt.CopyComponents(target, JamList(components.ToLower(), "PrintSupport"), destFolder);
                Qt.CopyPlugin(target, "platforms", "windows", destFolder);
            }

            if (Vars.TargetPlatformIsMac)
            {
                Qt.CopyComponents(target, JamList(components.ToLower(), "PrintSupport"), destFolder);
                Qt.CopyPlugin(target, "platforms", "cocoa", destFolder);
            }

            if (Vars.TargetPlatformIsLinux64)
            {
                Qt.CopyComponents(target, JamList(components, "DBus", "PrintSupport"), destFolder);
                Qt.CopyPlugin(target, "platforms", "xcb", destFolder);
                copyfile.CopyFile(
                    target,
                    $"{destFolder}/libpng12.so.0",
                    $"{Qt.qtRoot}/lib/libpng12.so.0");
            }

            Qt.WriteConfig(target, JamList(), destFolder);
            Qt.PatchApplication(target, components, binary);
        }
    }
}
