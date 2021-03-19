using System;
using System.Linq;
using External.Jamplus.builds.bin;
using External.Jamplus.builds.bin.modules;
using JamSharp.Runtime;
using NiceIO;
using Unity.BuildTools;
using static JamSharp.Runtime.BuiltinRules;

namespace Tools.BugReporterV2.jam_modules
{
    class Qt : ConvertedJamFile
    {
        private static string sharedLibPrefix = "";
        private static string sharedLibSuffix = "";

        internal static NPath qtRoot;

        internal static void TopLevel()
        {
            var qtBase = $"{Paths.UnityRoot}/External/Qt";

            if (Vars.TargetPlatformIsWindows)
            {
                qtRoot = $"{qtBase}/win/{Vars.PLATFORM}-vs2015/builds";
            }
            else if (Vars.TargetPlatformIsMac)
            {
                qtRoot = $"{qtBase}/mac/{Vars.PLATFORM}/builds";
            }
            else if (Vars.TargetPlatformIsLinux64)
            {
                qtRoot = $"{qtBase}/linux/{Vars.PLATFORM}/builds";
            }
            else
            {
                Errors.PrintError($"Qt build scripts should be only set up on PC platforms; whereas current platform is '{Vars.PLATFORM}'");
                throw new Exception("Unknown platorm for setting up Qt");
            }

            sharedLibPrefix = "";

            if (Vars.TargetPlatformIsMac || Vars.TargetPlatformIsLinux64)
                sharedLibPrefix = "lib";

            if (Vars.TargetPlatformIsWindows)
                sharedLibSuffix = ".dll";

            if (Vars.TargetPlatformIsMac)
                sharedLibSuffix = ".dylib";

            if (Vars.TargetPlatformIsLinux64)
                sharedLibSuffix = ".so";

            if (Vars.TargetPlatformIsMac)
                JamCore.RegisterAction("InstallName", @"install_name_tool $(INSTALLNAMEARGS) $(1:C)");

            JamCore.RegisterAction("Qt.Moc", $"{qtRoot}/bin/moc $(2) -o $(1)");
            JamCore.RegisterAction("Qt.Rcc", $"{qtRoot}/bin/rcc -name $(RCC_NAME) $(2) -o $(1)");
            JamCore.RegisterAction("Qt.Uic", $"{qtRoot}/bin/uic $(2) -o $(1)");
        }

        static string LibName(string name)
        {
            var prefix = "Qt5";
            var suffix = "";

            if (Vars.TargetPlatformIsWindows && Vars.CONFIG == "debug")
            {
                suffix = "d";
            }

            if (Vars.TargetPlatformIsMac)
            {
                if (Vars.CONFIG == "debug")
                    suffix = "_debug";
                suffix += ".5.3.3";
            }

            return $"{prefix}{name}{suffix}";
        }

        static string PluginName(string name)
        {
            var prefix = "q";
            var suffix = "";

            if (Vars.TargetPlatformIsWindows && Vars.CONFIG == "debug")
            {
                suffix = "d";
            }

            if (Vars.TargetPlatformIsMac && Vars.CONFIG == "debug")
            {
                suffix = "_debug";
            }

            return $"{prefix}{name}{suffix}";
        }

        static void LinkLibrary(string component)
        {
            C.C_LinkDirectories(JamList(), $"{qtRoot}/lib");
            C.C_LinkPrebuiltLibraries(JamList(), LibName(component));

            if (Vars.TargetPlatformIsLinux)
                C.C_LinkFlags(JamList(), "-Wl,-rpath='$ORIGIN',-rpath='$ORIGIN/platforms'");
        }

        static string ComponentBinaryName(string component)
        {
            var libName = LibName(component);
            return $"{sharedLibPrefix}{libName}{sharedLibSuffix}";
        }

        static NPath ComponentBinaryPath(string component)
        {
            return $"{qtRoot}/{(Vars.TargetPlatformIsWindows ? "bin" : "lib")}/{ComponentBinaryName(component)}";
        }

        private static string PluginBinaryName(string plugin)
        {
            string pluginName = PluginName(plugin);
            return $"{sharedLibPrefix}{pluginName}{sharedLibSuffix}";
        }

        static void InstallName(JamList file, JamList args)
        {
            if (Vars.TargetPlatformIsMac)
            {
                "INSTALLNAMEARGS".On(file).Assign(args);
                InvokeJamAction("InstallName", file);
            }
        }

        static JamList ComponentDependencies(string component)
        {
            switch (component.ToLowerInvariant())
            {
                case "core": return JamList();
                case "gui": return "Core";
                case "widgets": return JamList("Core", "Gui");
                case "network": return JamList("Core");
                case "xml": return JamList("Core");
                case "printsupport": return JamList("Core", "Gui", "Widgets");
                case "plugin.platforms.windows": return JamList("Core", "Gui", "Widgets", "PrintSupport");
                case "plugin.platforms.cocoa": return JamList("Core", "Gui", "Widgets", "PrintSupport");
                case "plugin.platforms.linux": return JamList("Core", "Gui", "Widgets", "PrintSupport", "DBus", "xcb");
                case "plugin.imageformats.gif": return JamList("Core", "Gui");
                case "plugin.imageformats.ico": return JamList("Core", "Gui");
                default: return JamList();
            }
        }

        static void CopyComponent(JamList target, string component, JamList directory)
        {
            JamList targetPath = directory.Clone();
            targetPath.AssignIfEmpty(C.C__retrieveOutputPath(target));
            var binary = ComponentBinaryName(component);
            var sourcePath = ComponentBinaryPath(component);
            var destinationPath = $"{targetPath}/{binary}";

            if (Vars.PLATFORM.IsIn("linux32", "linux64"))
                copyfile.CopyFile(target, $"{destinationPath}.5", sourcePath);
            else
                copyfile.CopyFile(target, destinationPath, sourcePath);

            JamList args = JamList("-id", binary);
            foreach (var comp in ComponentDependencies(component).Elements)
            {
                var name = ComponentBinaryName(comp);
                var oldName = ComponentBinaryPath(comp);
                var newName = $"@executable_path/{name}";
                args.Append("-change", oldName, newName);
            }
            InstallName(destinationPath, args);
        }

        internal static void CopyComponents(JamList target, JamList components, JamList directory = null)
        {
            directory = directory ?? new JamList();
            foreach (var component in components.Elements)
            {
                CopyComponent(target, component, directory);
            }

            if (Vars.PLATFORM.IsIn("linux32", "linux64"))
            {
                JamList targetPath = directory.Clone();
                targetPath.AssignIfEmpty(C.C__retrieveOutputPath(target));
                copyfile.CopyFile(target, $"{targetPath}/libpng12.so.0", $"{qtRoot}/lib/libpng12.so.0");
            }
        }

        internal static void PatchApplication(JamList target, JamList components, JamList binary)
        {
            target = target.Clone();
            target.Assign(Jambase._retrieveActiveTargetName(target));
            JamList binaryPath = binary.Clone();
            JamList args = JamList();
            foreach (var comp in components.Elements)
            {
                var name = ComponentBinaryName(comp);
                var oldName = ComponentBinaryPath(comp);
                var newName = $"@executable_path/{name}";
                args.Append("-change", oldName, newName);
            }
            InstallName(binaryPath, args);
        }

        internal static void Application(JamList target, JamList sources, JamList components, JamList options = null)
        {
            target = target.Clone();
            options = options ?? new JamList();
            target.Assign(Jambase._retrieveActiveTargetName(target));
            CopyComponents(target, JamList(components, "PrintSupport"));

            if (Vars.TargetPlatformIsWindows)
                CopyPlugin(target, "platforms", "windows");

            if (Vars.TargetPlatformIsMac)
                CopyPlugin(target, "platforms", "cocoa");

            if (Vars.TargetPlatformIsLinux64)
            {
                CopyPlugin(target, "platforms", "xcb");
                var destDir = C.C__retrieveOutputPath(target);
                copyfile.CopyFile(target, $"{destDir}/libpng12.so.0", $"{qtRoot}/lib/libpng12.so.0");
            }

            CopyPlugin(target, "imageformats", "gif");
            CopyPlugin(target, "imageformats", "ico");
            WriteConfig(target, JamList());
            JamList binary = C.C_Application(target, sources, options);
            PatchApplication(target, components, binary);
        }

        internal static void CopyPlugin(JamList target, string type, string plugin, JamList directory = null)
        {
            directory = directory ?? new JamList();
            JamList targetPath = directory.Clone();
            targetPath.AssignIfEmpty(C.C__retrieveOutputPath(target));
            var binary = PluginBinaryName(plugin);
            var sourcePath = $"{qtRoot}/plugins/{type}/{binary}";
            var destinationPath = $"{targetPath}/{type}/{binary}";
            copyfile.CopyFile(target, destinationPath, sourcePath);
            JamList args = JamList("-id", binary);
            foreach (var component in ComponentDependencies($"plugin.{type}.{plugin}").Elements)
            {
                var name = ComponentBinaryName(component);
                var oldName = ComponentBinaryPath(component);
                var newName = $"@executable_path/{name}";
                args.Append("-change", oldName, newName);
            }
            InstallName(destinationPath, args);
        }

        internal static void WriteConfig(JamList target, JamList content = null, JamList directory = null)
        {
            target = target.Clone();
            content = content ?? new JamList();
            directory = directory ?? new JamList();

            target.Assign(Jambase._retrieveActiveTargetName(target));
            JamList config = JamList("qt.conf").SetGrist(target.SetGrist(""));
            JamList targetPath = directory.Clone();
            targetPath.AssignIfEmpty(C.C__retrieveOutputPath(target));
            Jambase.MakeLocate(config, targetPath);
            "CONTENTS".On(config).Assign(content.JoinWithValue(Vars.NEWLINE));
            Jambase.Action_WriteFile(config);
            Depends(target, config);
        }

        internal static void Use(JamList components, bool link = false)
        {
            C.C_IncludeDirectories(JamList(), $"{qtRoot}/include");
            foreach (var el in components.Elements)
            {
                C.C_IncludeDirectories(JamList(), $"{qtRoot}/include/Qt{el}");
            }

            if (link)
            {
                foreach (var c in components.Elements)
                    LinkLibrary(c);
            }
        }

        internal static JamList Moc(JamList parents, JamList sources, JamList targetToDependOnAllGeneratedFiles)
        {
            using (VariableRestorer variableRestorer = new VariableRestorer())
            {
                parents = parents.Clone();

                parents.Assign(Jambase._retrieveActiveTargetName(parents));
                variableRestorer.RestoreAfterFunction("grist");
                Vars.grist.Assign(parents);
                JamList returnSources = JamList();
                foreach (JamList source in sources.ElementsAsJamLists)
                {
                    source.Assign(source.SetGrist(Vars.grist));
                    variableRestorer.RestoreAfterFunction("ext");
                    Vars.ext.Assign(source.GetSuffix());
                    variableRestorer.RestoreAfterFunction("target");
                    Vars.target.Assign();

                    if (Vars.ext.IsIn(".h", ".hpp"))
                    {
                        Vars.target.Assign(JamList($"moc_{source.GetFileNameBase ()}.cpp").SetGrist(Vars.grist));
                    }
                    else if (Vars.ext == ".cpp")
                    {
                        Vars.target.Assign(JamList($"{source.GetFileNameBase ()}.moc").SetGrist(Vars.grist));
                        source.Assign($"{source.SetGrist (Vars.grist)}|source");
                        // Avoid complaints of a circular dependency: file.cpp -> file.moc -> file.cpp
                        InvokeJamRule("ForceCare", Vars.target);
                        C.C_ObjectIncludeDirectories(parents, source, Vars.LOCATE_SOURCE);
                    }

                    Jambase.SearchSource(source, Vars.SEARCH_SOURCE);
                    returnSources.Append(Vars.target);
                    Jambase.MakeLocate(Vars.target, Vars.LOCATE_SOURCE);

                    Depends(Vars.target, source);
                    Depends(targetToDependOnAllGeneratedFiles, Vars.target);

                    InvokeJamAction("Qt.Moc", Vars.target, source);
                }

                return returnSources;
            }
        }

        internal static JamList Rcc(JamList parents, JamList sources, JamList name)
        {
            using (VariableRestorer variableRestorer = new VariableRestorer())
            {
                parents = parents.Clone();

                parents.Assign(Jambase._retrieveActiveTargetName(parents));
                variableRestorer.RestoreAfterFunction("grist");
                Vars.grist.Assign(parents);
                Jambase.SearchSource(sources, Vars.SEARCH_SOURCE);
                JamList targets = JamList(Combine("qrc_", sources.GetFileNameBase(), ".cpp")).SetGrist(Vars.grist);
                Jambase.MakeLocate(targets, Vars.LOCATE_SOURCE);
                Depends(targets, sources);

                "RCC_NAME".On(targets).Assign(name);
                InvokeJamAction("Qt.Rcc", targets, sources);

                return targets;
            }
        }

        internal static void Uic(JamList parents, JamList sources, JamList generatedName, JamList targetToDependOnAllGeneratedFiles)
        {
            using (VariableRestorer variableRestorer = new VariableRestorer())
            {
                parents = parents.Clone();
                parents.Assign(Jambase._retrieveActiveTargetName(parents));
                variableRestorer.RestoreAfterFunction("grist");
                Vars.grist.Assign(parents);
                Jambase.SearchSource(sources, Vars.SEARCH_SOURCE);
                JamList targets = JamList();

                if (Vars.PLATFORM.IsIn("linux32", "linux64"))
                    targets.Assign(JamList(Combine("ui_", generatedName, ".h")).SetGrist(Vars.grist));
                else
                    targets.Assign(JamList(Combine("ui_", sources.GetFileNameBase(), ".h")).SetGrist(Vars.grist));

                Jambase.MakeLocate(targets, Vars.LOCATE_SOURCE);
                Depends(targets, sources);
                Depends(targetToDependOnAllGeneratedFiles, targets);
                InvokeJamAction("Qt.Uic", targets, sources);
            }
        }
    }
}
