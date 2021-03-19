using JamSharp.Runtime;
using static JamSharp.Runtime.BuiltinRules;
using External.Jamplus.builds.bin.modules;
using External.Jamplus.builds.bin;
using Unity.BuildSystem;

namespace Tools.AssemblyPatcher
{
    class AssemblyPatcher : ConvertedJamFile
    {
        internal static void TopLevel()
        {
            Jambase.SubDir("TOP");
            Vars.wsa_patched_unityscript.Assign($"{Vars.TOP}/build/temp/patched_unityscript/wsa");
            Vars.uap_patched_unityscript.Assign($"{Vars.TOP}/build/temp/patched_unityscript/uap");
            NotFile("PatchBooLang");
            Depends("PatchBooLang", "AssemblyPatcher");
            Vars.assembly_patcher_targetdir.Assign($"{Vars.TOP}/build/temp/AssemblyPatcher");
            Vars.assembly_patcher_deps.Assign(
                $"{Vars.TOP}/Tools/AssemblyPatcher/packages/log4net.2.0.0/lib/net40-full/log4net.dll",
                $"{Vars.TOP}/Tools/AssemblyPatcher/packages/CommandLineParser.1.9.71/lib/net40/CommandLine.dll");
            Vars.assembly_patcher_deps.Append(Cecil.Current40AllRefs);
            Vars.assembly_patcher_config.Assign($"{Vars.assembly_patcher_targetdir}/AssemblyPatcher.exe.config");
            copyfile.CopyFile(JamList(), Vars.assembly_patcher_config, $"{Vars.TOP}/Tools/AssemblyPatcher/AssemblyPatcher/app.config");
            Jamrules.BuildAssemblyFromFolderWithCSFiles(
                "AssemblyPatcher",
                "4.0",
                $"{Vars.assembly_patcher_targetdir}/AssemblyPatcher.exe",
                $"{Vars.TOP}/Tools/AssemblyPatcher/AssemblyPatcher",
                Vars.assembly_patcher_deps);
            PatchAssemblyForWinRT(
                Combine(
                    Vars.wsa_patched_unityscript,
                    "/Boo.Lang.dll"),
                $"{Vars.TOP}/External/Mono/builds/monodistribution/lib/mono/bare-minimum/wsa/Boo.Lang.dll",
                "WSA");
            PatchAssemblyForWinRT(
                Combine(
                    Vars.wsa_patched_unityscript,
                    "/UnityScript.Lang.dll"),
                $"{Vars.TOP}/External/Mono/builds/monodistribution/lib/mono/bare-minimum/UnityScript.Lang.dll",
                "WSA");
            PatchAssemblyForWinRT(
                Combine(
                    Vars.uap_patched_unityscript,
                    "/Boo.Lang.dll"),
                $"{Vars.TOP}/External/Mono/builds/monodistribution/lib/mono/bare-minimum/wsa/Boo.Lang.dll",
                "WSA");
            PatchAssemblyForWinRT(
                Combine(
                    Vars.uap_patched_unityscript,
                    "/UnityScript.Lang.dll"),
                $"{Vars.TOP}/External/Mono/builds/monodistribution/lib/mono/bare-minimum/UnityScript.Lang.dll",
                "WSA");
            foreach (JamList @ref in Vars.assembly_patcher_deps.ElementsAsJamLists)
            {
                copyfile.CopyFile("AssemblyPatcher", $"{Vars.assembly_patcher_targetdir}/{@ref.GetBoundPath ().BaseAndSuffix ()}", @ref);
            }
            Depends("Editor", "AssemblyPatcher");
            Depends("MacEditor", "AssemblyPatcher");
        }

        internal static void PatchAssemblyForWinRT(JamList TARGET, JamList assembly, JamList platform)
        {
            JamList winrtLegacyFolder = JamList();
            JamList platformConfig = JamList();
            platformConfig.Assign($"{Vars.TOP}/Tools/AssemblyPatcher/AssemblyPatcher/config_wsa.txt");

            platformConfig.Assign($"{Vars.TOP}/Tools/AssemblyPatcher/AssemblyPatcher/config_uap.txt");
            winrtLegacyFolder.Assign($"{Vars.TOP}/build/MetroSupport/Managed/UAP");

            JamList winrtLegacy = $"{winrtLegacyFolder}/WinRTLegacy.dll";
            Depends(TARGET, winrtLegacy);
            Depends(TARGET, "AssemblyPatcher");
            Depends(TARGET, platformConfig);
            Depends(TARGET, Vars.assembly_patcher_config);
            "assemblypatcher_cfg".On(TARGET).Assign(platformConfig);
            "opts".On(TARGET).Assign("-s", winrtLegacyFolder, "-p", platform);
            Jamrules.Action_RunAssemblyPatcher(TARGET, assembly);
        }
    }
}
