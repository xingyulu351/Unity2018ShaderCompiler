using System.Linq;
using External.Jamplus.builds.bin.modules;
using External.Jamplus.builds.bin.modules.c;
using JamSharp.Runtime;
using NiceIO;
using Projects.Jam;
using Tools.BugReporterV2.jam_modules;
using Unity.BuildTools;
using static JamSharp.Runtime.BuiltinRules;

namespace Tools.BugReporterV2
{
    class BugReporter : ConvertedJamFile
    {
        public static NPath Base { get; } = "Tools/BugReporterV2";
        public static NPath ArtifactsPath { get; } = Paths.UnityRoot.Combine($"artifacts/BugReporter/{Vars.PLATFORM}-{Vars.CONFIG}");
        private static NPath BridgeBinary = "";

        internal static void TopLevel()
        {
            SubIncludeRunner.RunOnce(BugReporterRules.TopLevel);

            Vars.ALL_SUBDIR_TOKENS.Assign();
            Vars.LOCATE_TARGET.Assign(ArtifactsPath);
            Vars.LOCATE_SOURCE.Assign(ArtifactsPath);
            Vars.SEARCH_SOURCE.Assign(Paths.UnityRoot.Combine(Base));

            SetupCommonTests();
            SetupSettingsLib();
            SetupSysinfoLib();
            SetupQtComponents();
            SetupTesting();
            SetupLongTermOperation();
            SetupSystemInterplay();
            SetupFileSystem();
            SetupAttachment();
            if (Vars.TargetPlatformIsWindows)
            {
                SetupUnityBridge();
            }
            SetupLauncher();
            SetupReporter();
            SetupUnitTestPlusPlus();
            SetupSender();
            SetupExporter();
            SetupQtFace();
            SetupUnityCollectors();
            SetupSystemCollectors();
            SetupZipPacker();
            SetupUnityVersion();
            SetupTestUI();
            SetupProgression();
            SetupFeedback();
            SetupSearchIntegration();

            if (Vars.TargetPlatformIsWindows)
            {
                SetupWinCollectors();
            }

            if (Vars.TargetPlatformIsMac)
                SetupMacCollectors();

            NotFile("bugreporter");

            Depends("bugreporter_test", "test");

            var deployDir = EditorFiles.TargetPath.Combine("BugReporter");
            if (Vars.TargetPlatformIsMac)
            {
                deployDir = EditorFiles.DataPath.Combine("BugReporter/Unity Bug Reporter.app/Contents/MacOS");
            }

            JamList launcherPath = $"{ArtifactsPath}/launcher.exe";
            Depends(launcherPath, "bugreporter.launcher");
            Qt.CopyComponents(
                "bugreporter",
                JamList(
                    "Core",
                    "Gui",
                    "Widgets",
                    "Network",
                    "PrintSupport",
                    "Xml"),
                deployDir);

            if (Vars.TargetPlatformIsWindows)
            {
                Qt.CopyPlugin("bugreporter", "platforms", "windows", deployDir);
                copyfile.CopyFile("bugreporter", $"{deployDir}/unity.bugreporter.exe", launcherPath);
                Depends($"{deployDir}/{BridgeBinary.FileName}", BridgeBinary, "unity_bridge");
                copyfile.CopyFile("bugreporter", $"{deployDir}/{BridgeBinary.FileName}", BridgeBinary);
            }

            if (Vars.TargetPlatformIsMac)
            {
                Qt.CopyPlugin("bugreporter", "platforms", "cocoa", deployDir);
                copyfile.CopyFile("bugreporter", $"{deployDir}/unity.bugreporter.exe", launcherPath);
            }

            if (Vars.TargetPlatformIsLinux)
            {
                Qt.CopyComponents("bugreporter", "DBus", deployDir);
                Qt.CopyPlugin("bugreporter", "platforms", "xcb", deployDir);
                copyfile.CopyFile("bugreporter", $"{deployDir}/unity.bugreporter", launcherPath);
            }

            Qt.CopyPlugin("bugreporter", "imageformats", "gif", deployDir);
            Qt.CopyPlugin("bugreporter", "imageformats", "ico", deployDir);
            Qt.WriteConfig("bugreporter", JamList(), deployDir);

            if (Vars.TargetPlatformIsMac)
            {
                var shortVersion = UnityVersion.Version;
                var fullVersion = UnityVersion.Exact;
                var revision = GetConfigurationProperty("Version/Revision");
                var copyright = GetConfigurationProperty("Copyright");
                var infoString = $"Unity Bug Reporter {fullVersion} ({revision}) {copyright}";
                JamList propertyList =
                    BugReporterRules.InfoPList(
                        "bugreporter.plist",
                        JamList(
                            "CFBundleIdentifier=com.unity3d.bugreporter",
                            "CFBundlePackageType=APPL",
                            "CFBundleSignature=UNBR",
                            "CFBundleName=Unity Bug Reporter",
                            "CFBundleSpokenName=Bug Reporter",
                            "CFBundleDisplayName=Unity Bug Reporter",
                            "CFBundleIconFile=AppIcon",
                            "CFBundleExecutable=unity.bugreporter.exe",
                            $"CFBundleVersion={fullVersion}",
                            $"CFBundleShortVersionString={shortVersion}",
                            $"CFBundleGetInfoString={infoString}",
                            "NSPrincipalClass=NSApplication",
                            "NSHighResolutionCapable=True",
                            $"UnityBuildNumber={revision}"));
                copyfile.CopyFile("bugreporter", deployDir.Parent.Combine("Info.plist"), propertyList);
                copyfile.CopyFile("bugreporter", deployDir.Parent.Combine("Resources/AppIcon.icns"), Base.Combine("bundle/resources/AppIcon.icns"));
            }

            copydirectory.CopyDirectory("bugreporter", $"{deployDir}/localization", Base.Combine("qt_face/localization/qm"));
        }

        static string GetConfigurationProperty(NPath path)
        {
            path = $"{Vars.TOP}/artifacts/generated/Configuration/Property/{path}";

            if (!path.FileExists())
                return "";

            return path.ReadAllText();
        }

        static void SetupCommonTests()
        {
            BugReporterRules.SubProject("common_tests");
            var src = Base.Combine("common/tests/src").Files("*.cpp");
            C.C_IncludeDirectories(JamList(), $"{Base}/common/lib/src");
            BugReporterRules.TestApplication(src);
        }

        static void SetupSettingsLib()
        {
            BugReporterRules.SubProject("settings");
            var src = Base.Combine("settings/lib/src").Files("*.cpp");
            C.C_IncludeDirectories(JamList(), JamList($"{Base}/settings/lib/src", $"{Base}/include"));
            Qt.Use("Core", true);
            C.C_Library(JamList(), src);
        }

        static void SetupSysinfoLib()
        {
            BugReporterRules.SubProject("sysinfo");
            JamList src = Base.Combine("sysinfo/lib/src").Files("*.cpp");

            if (Vars.TargetPlatformIsWindows)
                src.Append(Base.Combine("sysinfo/lib/src/windows").Files("*.cpp"));

            if (Vars.TargetPlatformIsMac)
                src.Append(Base.Combine("sysinfo/lib/src/macosx").Files("*.cpp"));

            if (Vars.TargetPlatformIsLinux64)
            {
                Qt.Use("Gui");
                src.Append(Base.Combine("sysinfo/lib/src/linux").Files("*.cpp"));
            }

            C.C_IncludeDirectories(
                JamList(),
                JamList(
                    $"{Base}/sysinfo/lib/src",
                    Base.Combine("include"),
                    Base.Combine("common"),
                    Base.Combine("unity_bridge/main/include")));
            C.C_Library(JamList(), src);

            /* Bug reporter people say: sysinfo/tests temporarily turned off; plan to bring them back
            BugReporterRules.SubProject("sysinfo_tests");
            JamList tests = Base.Combine("sysinfo/tests/src").Files("*.cpp");
            C.C_IncludeDirectories(JamList(), JamList($"{Base}/sysinfo/tests/src", $"{Base}/include"));
            JamList qtComponents = JamList("Core", "Gui", "Widgets");
            Qt.Use(qtComponents, true);

            if (Vars.TargetPlatformIsLinux64)
            {
                Qt.Use("OpenGL", true);
                qtComponents.Append("OpenGL");
            }

            C.C_LinkPrebuiltLibraries(
                JamList(),
                JamList("Iphlpapi", "d3d9", "Shell32", "Wbemuuid", "Gdi32", "Ole32", "OleAut32", "Opengl32", "User32", "Advapi32", "comsuppw"),
                JamList(),
                JamList("win64"));
            C.C_LinkLibraries(JamList(), JamList("sysinfo", "system_interplay"));
            BugReporterRules.TestApplication(JamList(), tests);
            Qt.CopyComponents("sysinfo_tests", qtComponents);
            */
        }

        static void SetupQtComponents()
        {
            BugReporterRules.SubProject("qtcomponents");
            NotFile("qtcomponents");
            Qt.CopyComponents("qtcomponents", JamList("Core", "Gui", "Widgets", "Network", "PrintSupport"));

            if (Vars.TargetPlatformIsWindows)
                Qt.CopyPlugin("qtcomponents", "platforms", "windows");

            if (Vars.TargetPlatformIsMac)
                Qt.CopyPlugin("qtcomponents", "platforms", "cocoa");

            if (Vars.TargetPlatformIsLinux64)
                Qt.CopyPlugin("qtcomponents", "platforms", "xcb");

            Qt.WriteConfig("qtcomponents");
        }

        static void SetupTesting()
        {
            BugReporterRules.SubProject("testing");
            var src = Base.Combine("testing/lib/src").Files("*.cpp");
            C.C_IncludeDirectories(JamList(), $"{Paths.UnityRoot}/External/UnitTest++/src");
            C.C_Library(JamList(), src);

            BugReporterRules.SubProject("testing_tests");
            var tests = Base.Combine("testing/tests/src").Files("*.cpp");
            C.C_IncludeDirectories(JamList(), Base.Combine("testing/lib/src"));
            BugReporterRules.TestApplication(tests);
        }

        static void SetupLongTermOperation()
        {
            BugReporterRules.SubProject("long_term_operation");
            var src = Base.Combine("long_term_operation/lib/src").Files("*.cpp");
            Qt.Use("Core", true);
            C.C_Library(JamList(), src);

            BugReporterRules.SubProject("long_term_operation_tests");
            var tests = Base.Combine("long_term_operation/tests/src").Files("*.cpp");
            C.C_LinkLibraries(JamList(), "long_term_operation");
            BugReporterRules.TestApplication(tests);
        }

        static void SetupSystemInterplay()
        {
            BugReporterRules.SubProject("system_interplay");
            Qt.Use("Core", true);
            var dir = $"{Base}/system_interplay/lib/src";
            JamList src = JamList();
            if (Vars.TargetPlatformIsWindows)
                src.Append($"{dir}/WindowsGate.cpp", $"{dir}/WindowsFileSystem.cpp");

            if (Vars.TargetPlatformIsMac)
                src.Append($"{dir}/PosixGate.cpp", $"{dir}/MacxFileSystem.cpp");

            if (Vars.TargetPlatformIsLinux64)
                src.Append($"{dir}/PosixGate.cpp", $"{dir}/LinuxFileSystem.cpp");

            C.C_Library(JamList(), src);

            BugReporterRules.SubProject("system_interplay_tests");
            var testsDir = $"{Base}/system_interplay/tests/src";
            JamList tests = JamList();

            if (Vars.TargetPlatformIsWindows)
                tests.Append($"{testsDir}/WindowsShellTest.cpp");

            if (Vars.TargetPlatformIsMac)
                tests.Append($"{testsDir}/MacxShellTest.cpp");

            C.C_IncludeDirectories(JamList(), dir);
            C.C_LinkLibraries(JamList(), "system_interplay");
            BugReporterRules.TestApplication(tests);
        }

        static void SetupFileSystem()
        {
            BugReporterRules.SubProject("file_system");
            var dir = $"{Base}/file_system/lib/src";
            JamList src = JamList($"{dir}/BasicFileGateway.cpp", $"{dir}/SystemFileGateway.cpp",
                $"{dir}/QtBasedFileSystem.cpp", $"{dir}/FileSystem.cpp");
            Qt.Use("Core", true);
            C.C_Library(JamList(), src);

            BugReporterRules.SubProject("file_system_tests");
            var testsDir = $"{Base}/file_system/tests/src";
            JamList tests = JamList(
                $"{testsDir}/BasicFileGatewayTest.cpp",
                $"{testsDir}/FileEntryTest.cpp",
                $"{testsDir}/DirEntryTest.cpp",
                $"{testsDir}/ProjectDirEntryTest.cpp",
                $"{testsDir}/UnityProjectIdentifierTest.cpp");
            C.C_IncludeDirectories(JamList(), JamList(dir, testsDir));
            Qt.Use("Core", true);
            C.C_LinkLibraries(JamList(), JamList("file_system", "system_interplay"));
            BugReporterRules.TestApplication(tests);
            Qt.CopyComponents("file_system_tests", "Core");
        }

        static void SetupAttachment()
        {
            BugReporterRules.SubProject("attachment");
            var src = Base.Combine("attachment/lib/src").Files("*.cpp");
            C.C_Library(JamList(), src);

            BugReporterRules.SubProject("attachment-tests");
            Qt.Use("Core", true);
            var tests = Base.Combine("attachment/tests/src").Files("*.cpp");
            C.C_IncludeDirectories(JamList(), Base.Combine("attachment/lib/src"));
            C.C_LinkLibraries(JamList(), JamList("attachment", "file_system", "system_interplay"));
            BugReporterRules.TestApplication(tests);
            Qt.CopyComponents("attachment-tests", "Core");
        }

        static void SetupLauncher()
        {
            var dir = $"{Base}/launcher/app";
            BugReporterRules.SubProject("bugreporter.launcher");
            C.C_OutputPostfix("bugreporter.launcher", JamList(), JamList("release", "debug"));
            C.C_OutputName("bugreporter.launcher", "launcher");
            C.C_OutputSuffix("bugreporter.launcher", "exe");
            JamList src = JamList(
                $"{dir}/src/main.cpp",
                $"{dir}/src/Args.cpp",
                $"{dir}/src/ReportBuilder.cpp",
                $"{dir}/src/ExternalAttachmentFactory.cpp",
                $"{dir}/src/UnityPlatformTraits.cpp",
                Qt.Rcc(JamList(), $"{Base}/qt_face/lib/src/resedit.qrc", "ResourcesInit"));

            if (Vars.TargetPlatformIsWindows)
            {
                win32resource.C_RcIncludeDirectories(
                    "bugreporter.launcher",
                    $"{dir}/src/Launcher.rc",
                    $"{Paths.UnityRoot}/artifacts/generated/Configuration");
                src.Append(
                    $"{dir}/src/WindowsEntry.cpp",
                    win32resource.C_ResourceCompiler("bugreporter.launcher", $"{dir}/src/Launcher.rc"));
            }

            if (Vars.TargetPlatformIsMac || Vars.TargetPlatformIsLinux64)
                src.Append($"{dir}/src/PosixEntry.cpp");

            Qt.Use(JamList("Core", "Gui", "Widgets", "Network", "Xml"), true);
            C.C_LinkPrebuiltLibraries(
                JamList(),
                JamList("Iphlpapi", "d3d9", "Shell32", "Wbemuuid", "Gdi32", "Ole32", "OleAut32", "Opengl32", "User32",
                    "Advapi32", "comsuppw"),
                JamList(),
                JamList("win64"));
            C.C_LinkLibraries(
                JamList(),
                JamList(
                    "attachment",
                    "system_interplay",
                    "qt_sender",
                    "qt_exporter",
                    "zip_packer",
                    "system_collectors",
                    "unity_collectors",
                    "settings",
                    "long_term_operation",
                    "unity_version",
                    "file_system",
                    "qt_face",
                    "sysinfo",
                    "reporter",
                    "progression",
                    "search_integration",
                    "feedback"));

            if (Vars.TargetPlatformIsWindows)
                C.C_LinkLibraries(JamList(), JamList("win_collectors", "unity_bridge"));

            if (Vars.TargetPlatformIsMac)
            {
                C.C_LinkLibraries(JamList(), "macx_collectors");
                InvokeJamRule("C.LinkFrameworks", JamList(), "Foundation");
            }

            if (Vars.TargetPlatformIsLinux64)
                Qt.Use("OpenGL", true);

            Depends("bugreporter.launcher", Base.Combine("launcher/app/src/Launcher.manifest"));
            Qt.Application(
                "bugreporter.launcher",
                src,
                JamList(
                    "Core",
                    "Gui",
                    "Widgets",
                    "Network",
                    "Xml"),
                JamList("windows", "nomanifest"));
            Depends("bugreporter.launcher", "qtcomponents");

            BugReporterRules.SubProject("launcher_tests");
            var testsDir = $"{Base}/launcher/tests";
            JamList tests =
                JamList(
                    $"{Base}/launcher/app/src/Args.cpp",
                    $"{Base}/launcher/app/src/ReportBuilder.cpp",
                    $"{testsDir}/src/ArgUtilsTest.cpp",
                    $"{testsDir}/src/ArgsTest.cpp",
                    $"{testsDir}/src/CollabReportModifierTests.cpp",
                    $"{testsDir}/src/UnityPackageManagerReportModifierTests.cpp",
                    $"{testsDir}/src/ReportBuilderTest.cpp",
                    $"{testsDir}/src/ReportSettingsLoaderTests.cpp");

            if (Vars.TargetPlatformIsWindows)
                tests.Append($"{testsDir}/src/WindowsArgUtilsTest.cpp");

            C.C_IncludeDirectories(JamList(), JamList("src", Base.Combine("launcher/app/src")));
            C.C_LinkPrebuiltLibraries(
                JamList(),
                JamList("Iphlpapi", "d3d9", "Shell32", "Wbemuuid", "Gdi32", "Ole32", "OleAut32", "Opengl32", "User32",
                    "Advapi32", "comsuppw"),
                JamList(),
                JamList("win64"));
            C.C_LinkLibraries(JamList(), "reporter");
            BugReporterRules.TestApplication(tests);
        }

        static void SetupReporter()
        {
            BugReporterRules.SubProject("reporter");
            var src = Base.Combine("reporter/lib/src").Files("*.cpp");
            C.C_LinkLibraries(JamList(), JamList("file_system", "settings"));
            C.C_Library(JamList(), src);

            BugReporterRules.SubProject("reporter_tests");
            var tests = Base.Combine("reporter/tests/src").Files("*.cpp");
            C.C_IncludeDirectories(JamList(), $"{Base}/reporter/lib/src");
            C.C_LinkLibraries(JamList(), JamList("reporter", "file_system"));
            BugReporterRules.TestApplication(tests);
        }

        static void SetupUnitTestPlusPlus()
        {
            BugReporterRules.SubProject("bugreporterunittest++");
            JamList src = new NPath("External/UnitTest++/src").Files("*.cpp");
            if (Vars.TargetPlatformIsMac || Vars.TargetPlatformIsLinux)
                src.Append(new NPath("External/UnitTest++/src/Posix").Files("*.cpp"));

            C.C_Library(JamList(), src);
        }

        static void SetupSender()
        {
            BugReporterRules.SubProject("qt_sender");
            JamList src = Base.Combine("sender/lib/src").Files("*.cpp");
            var dependAllTargetsOn = new JamList("qt_sender_generated_files");
            src.Append(Qt.Moc(JamList(), $"{Base}/sender/lib/src/NetworkReplyProgressListener.h", dependAllTargetsOn));
            Qt.Use(JamList("Core", "Network"), true);
            C.C_MakeAllObjectFilesOfTargetDependOn(JamList(), dependAllTargetsOn);
            C.C_Library(JamList(), src);

            BugReporterRules.SubProject("qt_sender_tests");
            var tests = Base.Combine("sender/tests/src").Files("*.cpp");
            C.C_IncludeDirectories(JamList(), $"{Base}/sender/lib/src");
            Qt.Use(JamList("Core", "Network"), true);
            BugReporterRules.TestQtApplication(tests, JamList("Core", "Network"));
            Depends(JamList(), "qtcomponents");
        }

        static void SetupExporter()
        {
            BugReporterRules.SubProject("qt_exporter");
            JamList src = Base.Combine("exporter/lib/src").Files("*.cpp");
            Qt.Use(JamList("Core"), true);
            C.C_Library(JamList(), src);
            C.C_IncludeDirectories(JamList(), $"{Base}/exporter/lib/src");
            Depends(JamList(), "qtcomponents");
        }

        static void SetupQtFace()
        {
            var dir = $"{Base}/qt_face/lib/src";
            BugReporterRules.SubProject("qt_face");
            JamList src = Base.Combine("qt_face/lib/src").Files("*.cpp");

            var generatedFilesTarget = new JamList("qtface_generatedfiles");

            Qt.Uic(JamList(), $"{dir}/mainwindow.ui", "MainWindow", generatedFilesTarget);
            src.Append(Qt.Moc(JamList(), $"{dir}/MainWindow.h", generatedFilesTarget));
            Qt.Uic(JamList(), $"{dir}/previewdialog.ui", "PreviewDialog", generatedFilesTarget);
            src.Append(Qt.Moc(JamList(), $"{dir}/PreviewDialog.h", generatedFilesTarget));
            Qt.Uic(JamList(), $"{dir}/crashsolutioncheckdialog.ui", "CrashSolutionCheckDialog", generatedFilesTarget);

            src.Append(Qt.Moc(JamList(), $"{dir}/CrashSolutionCheckDialog.h", generatedFilesTarget));
            src.Append(Qt.Moc(JamList(), $"{dir}/ProgressDialogToLongTermOperConnector.h", generatedFilesTarget));
            src.Append(Qt.Moc(JamList(), $"{dir}/ReportActionThread.h", generatedFilesTarget));
            src.Append(Qt.Moc(JamList(), $"{dir}/AttachmentsTableModel.h", generatedFilesTarget));

            if (Vars.TargetPlatformIsWindows)
            {
                src.Append(Base.Combine("qt_face/lib/src/windows").Files("*.cpp"));
            }

            if (Vars.TargetPlatformIsMac)
            {
                src.Append(Base.Combine("qt_face/lib/src/macosx").Files("*.cpp"));
            }

            if (Vars.TargetPlatformIsLinux64)
            {
                src.Append(Base.Combine("qt_face/lib/src/linux").Files("*.cpp"));
            }

            Qt.Use(JamList("Core", "Gui", "Widgets", "Xml", "Network"));
            C.C_IncludeDirectories(JamList(), JamList(ArtifactsPath, Paths.UnityRoot));
            C.C_MakeAllObjectFilesOfTargetDependOn(JamList(), generatedFilesTarget);
            C.C_Library(JamList(), src);

            BugReporterRules.SubProject("qt_face_tests");
            JamList tests = Base.Combine("qt_face/tests/src").Files("*.cpp");
            C.C_IncludeDirectories(JamList(), JamList($"{Base}/qt_face/lib", $"{Base}/qt_face/lib/src"));
            Qt.Use(JamList("Core", "Gui", "Widgets", "Xml", "Network"), true);
            C.C_LinkLibraries(JamList(), JamList("qt_face", "long_term_operation", "progression", "settings", "search_integration", "feedback"));

            if (!(Vars.TargetPlatformIsLinux64 && Vars.UNITY_THISISABUILDMACHINE == "1"))
            {
                // Linux build agents are headless
                BugReporterRules.TestQtApplication(tests, JamList("Core", "Gui", "Widgets", "Xml", "Network"));
            }

            Depends("qt_face_tests", "qtcomponents");
        }

        static void SetupUnityCollectors()
        {
            BugReporterRules.SubProject("unity_collectors");
            JamList src = Base.Combine("unity_collectors/lib/src").Files("*.cpp");

            if (Vars.TargetPlatformIsWindows)
            {
                src.Append(Base.Combine("unity_collectors/lib/src/windows").Files());
            }

            if (Vars.TargetPlatformIsMac)
            {
                src.Append(Base.Combine("unity_collectors/lib/src/macosx").Files());
            }

            if (Vars.TargetPlatformIsLinux64)
            {
                src.Append(Base.Combine("unity_collectors/lib/src/linux").Files());
            }

            C.C_IncludeDirectories(JamList(), Paths.UnityRoot);
            Qt.Use("Core", true);
            C.C_LinkLibraries(JamList(), JamList("system_interplay", "file_system", "zip_packer", "unity_version"));
            C.C_Library(JamList(), src);

            BugReporterRules.SubProject("unity_collectors_tests");
            var tests = Base.Combine("unity_collectors/tests/src").Files("*.cpp");
            C.C_IncludeDirectories(JamList(), $"{Base}/unity_collectors/lib/src");
            C.C_LinkLibraries(JamList(), JamList("unity_collectors", "reporter", "file_system"));
            BugReporterRules.TestApplication(tests);
        }

        static void SetupSystemCollectors()
        {
            BugReporterRules.SubProject("system_collectors");
            JamList src = $"{Base}/system_collectors/lib/src/Module.cpp";
            C.C_LinkLibraries(JamList(), "sysinfo");
            C.C_Library(JamList(), src);

            BugReporterRules.SubProject("system_collectors_tests");
            JamList tests = JamList($"{Base}/system_collectors/tests/src/OSInfoCollectorTest.cpp", $"{Base}/system_collectors/tests/src/HardwareInfoCollectorTest.cpp");
            C.C_IncludeDirectories(JamList(), $"{Base}/system_collectors/lib/src");
            C.C_LinkLibraries(JamList(), "system_collectors");
            BugReporterRules.TestApplication(tests);
        }

        static void SetupZipPacker()
        {
            BugReporterRules.SubProject("zip_packer");
            var miniz = $"{Paths.UnityRoot}/External/miniz/src";
            JamList src = Base.Combine("zip_packer/lib/src").Files("*.cpp");

            if (Vars.TargetPlatformIsWindows)
            {
                src.Append(Base.Combine("zip_packer/lib/src/windows").Files());
            }
            else
            {
                src.Append(Base.Combine("zip_packer/lib/src/posix").Files());
                C.C_CFlags(JamList(), JamList("-x", "c"));
            }

            C.C_IncludeDirectories(JamList(), JamList($"{Base}/zip_packer/lib/src", miniz));
            C.C_Library(JamList(), src);

            BugReporterRules.SubProject("zip_packer_tests");
            var tests = Base.Combine("zip_packer/tests/src").Files("*.cpp");
            C.C_IncludeDirectories(JamList(), JamList($"{Base}/zip_packer/lib/src", miniz));
            C.C_LinkLibraries(JamList(), "zip_packer");
            BugReporterRules.TestApplication(tests);
        }

        static void SetupUnityVersion()
        {
            BugReporterRules.SubProject("unity_version");
            JamList src = $"{Base}/unity_version/lib/src/UnityVersion.cpp";
            C.C_IncludeDirectories(JamList(), Paths.UnityRoot);
            C.C_Library(JamList(), src);
        }

        static void SetupTestUI()
        {
            BugReporterRules.SubProject("bugreporter.test_ui");
            C.C_OutputName("bugreporter.test_ui", "test_ui");
            JamList src = $"{Base}/test_ui/app/src/main.cpp";
            src.Append(Qt.Rcc(JamList(), Base.Combine("qt_face/lib/src/resedit.qrc"), "ResourcesInit"));

            if (Vars.TargetPlatformIsWindows)
                src.Append($"{Base}/test_ui/app/src/WindowsSleep.cpp");

            if (Vars.TargetPlatformIsMac)
                src.Append($"{Base}/test_ui/app/src/MacxSleep.cpp");

            Qt.Use(JamList("Core", "Gui", "Widgets", "Xml"), true);
            Depends("bugreporter.test_ui", "qtcomponents");
            C.C_IncludeDirectories(JamList(), Base.Combine("qt_face/lib/src"));
            C.C_LinkLibraries(JamList(), JamList("qt_face", "long_term_operation", "progression", "settings", "search_integration", "feedback"));
            Qt.Application(JamList(), src, JamList("Core", "Gui", "Widgets", "Xml"));
        }

        static void SetupProgression()
        {
            BugReporterRules.SubProject("progression");
            JamList src = $"{Base}/progression/lib/src/Progression.cpp";
            C.C_Library(JamList(), src);

            BugReporterRules.SubProject("progression_tests");
            JamList tests = $"{Base}/progression/tests/src/ProgressionTest.cpp";
            C.C_LinkLibraries(JamList(), "progression");
            BugReporterRules.TestApplication(tests);
        }

        static void SetupFeedback()
        {
            BugReporterRules.SubProject("feedback");
            JamList src = Base.Combine("feedback/lib/src").Files("*.cpp");
            var allGeneratedFiles = new JamList("feedback_generatedfiles");
            src.Append(Qt.Moc(JamList(), $"{Base}/feedback/lib/src/FeedbackServiceCommunicator.h", allGeneratedFiles));
            src.Append(Qt.Moc(JamList(), $"{Base}/feedback/lib/src/FeedbackCollection.h", allGeneratedFiles));
            src.Append(Qt.Moc(JamList(), $"{Base}/feedback/lib/src/BasicFeedbackManager.h", allGeneratedFiles));
            C.C_IncludeDirectories(JamList(), $"{Paths.UnityRoot}");
            Qt.Use(JamList("Core", "Network"));
            C.C_LinkLibraries(JamList(), "settings");
            C.C_MakeAllObjectFilesOfTargetDependOn(JamList(), allGeneratedFiles);
            C.C_Library(JamList(), src);

            BugReporterRules.SubProject("feedback_tests");
            JamList tests = Base.Combine("feedback/tests/src").Files("*.cpp");
            C.C_IncludeDirectories(
                JamList(),
                JamList($"{Base}/feedback/lib/src", $"{Paths.UnityRoot}/External"));
            Qt.Use(JamList("Core", "Network", "Gui", "Widgets"), true);
            C.C_LinkLibraries(JamList(), JamList("feedback", "settings"));
            BugReporterRules.TestQtApplication(tests, JamList("Core", "Network", "Gui", "Widgets"));
        }

        static void SetupSearchIntegration()
        {
            BugReporterRules.SubProject("search_integration");
            JamList src = Base.Combine("search_integration/lib/src").Files("*.cpp");
            var allGeneratedFiles = new JamList("search_integration_generatedfiles");
            src.Append(Qt.Moc(JamList(), $"{Base}/search_integration/lib/src/GoogleSearch.h", allGeneratedFiles));
            C.C_IncludeDirectories(JamList(), $"{Paths.UnityRoot}");
            Qt.Use(JamList("Core", "Network"));
            C.C_MakeAllObjectFilesOfTargetDependOn(JamList(), allGeneratedFiles);
            C.C_Library(JamList(), src);

            BugReporterRules.SubProject("search_integration_tests");
            var tests = Base.Combine("search_integration/tests/src").Files("*.cpp");
            C.C_IncludeDirectories(
                JamList(),
                JamList(
                    $"{Base}/search_integration/lib/src",
                    $"{Paths.UnityRoot}"));
            Qt.Use(JamList("Core", "Network"), true);
            C.C_LinkLibraries(JamList(), "search_integration");
            BugReporterRules.TestQtApplication(tests, JamList("Core", "Network"));
        }

        static void SetupUnityBridge()
        {
            BugReporterRules.SubProject("unity_bridge");
            JamList winInfoDir = "PlatformDependent/Win/";
            JamList winInfoSources = JamList("SystemInfo.cpp", "VersionHelpers.cpp", "Registry.cpp", "GetFormattedErrorString.cpp", "WinDriverUtils.cpp", "FileInformation.cpp");
            JamList src = JamList(
                Base.Combine("unity_bridge/main/src").Files("*.cpp"),
                Combine(winInfoDir, winInfoSources),
                "Runtime/Allocator/MemoryMacros.cpp",
                "Runtime/Allocator/MemoryManager.cpp");
            C.C_ObjectDefines("unity_bridge", src, JamList("UNITY_EXTERNAL_TOOL"));
            C.C_IncludeDirectories(
                "unity_bridge",
                JamList(
                    $"{Paths.UnityRoot}/Projects/PrecompiledHeaders",
                    $"{Paths.UnityRoot}/PlatformDependent/Win",
                    Paths.UnityRoot));
            C.C_LinkPrebuiltLibraries(
                "unity_bridge",
                JamList("Advapi32", "Iphlpapi", "Ole32", "OleAut32", "Shell32", "User32", "Wbemuuid"),
                JamList(),
                JamList("win64"));
            BridgeBinary = C.C_Library("unity_bridge", src, JamList("shared", "nomanifest")).IndexedBy(1).GetBoundPath().Single();
        }

        static void SetupWinCollectors()
        {
            BugReporterRules.SubProject("win_collectors");
            JamList src = $"{Base}/win_collectors/lib/src/Module.cpp";
            C.C_IncludeDirectories(JamList(), Base.Combine("unity_bridge/main/include"));
            C.C_Library(JamList(), src);

            BugReporterRules.SubProject("win_collectors_tests");
            JamList tests = Base.Combine("win_collectors/tests/src").Files("*.cpp");
            C.C_IncludeDirectories(
                JamList(),
                JamList(
                    $"{Paths.UnityRoot}/PlatformDependent/Win",
                    Paths.UnityRoot,
                    Base.Combine("win_collectors/lib/src"),
                    Base.Combine("unity_bridge/main/include")));
            C.C_LinkPrebuiltLibraries(
                JamList(),
                JamList("Iphlpapi", "d3d9", "Shell32", "Wbemuuid", "Gdi32", "Ole32", "OleAut32", "Opengl32", "User32", "Advapi32", "comsuppw"),
                JamList(),
                JamList("win64"));
            C.C_LinkLibraries(JamList(), JamList("reporter", "attachment", "file_system", "system_interplay", "win_collectors", "unity_bridge"));
            BugReporterRules.TestApplication(tests);
            var destFolder = C.C__retrieveOutputPath("win_collectors_tests");
            Depends($"{destFolder}/{BridgeBinary.FileName}", BridgeBinary, "unity_bridge");
            copyfile.CopyFile("win_collectors_tests", $"{destFolder}/{BridgeBinary.FileName}", BridgeBinary);
        }

        static void SetupMacCollectors()
        {
            var dir = $"{Base}/macx_collectors/lib/";
            BugReporterRules.SubProject("macx_collectors");
            JamList sources = JamList("src/Module.cpp", "src/CrashReportsProvider.cpp", "src/ShellSystemProfile.cpp");
            C.C_Library(JamList(), Combine(dir, sources));

            var dir2 = $"{Base}/macx_collectors/tests/";
            BugReporterRules.SubProject("macx_collectors_tests");
            JamList sources2 = JamList(
                "src/CrashReportTest.cpp",
                "src/CrashReportsProviderTest.cpp",
                "src/UnityCrashLogsCollectorTest.cpp",
                "src/SystemProfileCollectorTest.cpp",
                "src/ShellSystemProfileTest.cpp",
                "src/LogAttacherTest.cpp");
            C.C_IncludeDirectories(JamList(), Base.Combine("macx_collectors/lib/src"));
            C.C_LinkLibraries(JamList(), JamList("reporter", "macx_collectors"));
            BugReporterRules.TestApplication(Combine(dir2, sources2));
        }
    }
}
