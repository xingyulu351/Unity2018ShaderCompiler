using System;
using JamSharp.Runtime;
using static JamSharp.Runtime.BuiltinRules;
using External.Jamplus.builds.bin.modules;
using External.Jamplus.builds.bin;
using Unity.BuildSystem;

namespace Tools.UnityPreserveAttributeParser
{
    class UnityPreserveAttributeParser : ConvertedJamFile
    {
        public static Lazy<JamList> Target = new Lazy<JamList>(
            () =>
            {
                using (new SubIncludeGlobalStateBackup())
                {
                    Jambase.SubDir("TOP");
                    JamList targetdir = $"{Vars.TOP}/artifacts/UnityPreserveAttributeParser";
                    JamList dlls = JamList($"{targetdir}/Mono.Options.dll");
                    dlls.Append(Cecil.Current35WithSymbolRefs);
                    foreach (JamList file in dlls.ElementsAsJamLists)
                    {
                        JamList dest = file.SetDirectory(targetdir);

                        if (dest.NotJamEquals(file))
                            copyfile.CopyFile(JamList(), dest, file);

                        Needs("UnityPreserveAttributeParser", dest);
                    }
                    Depends($"{targetdir}/Mono.Options.dll", "MonoOptions");
                    Jamrules.BuildAssembly(
                        "MonoOptions",
                        "4.0",
                        $"{targetdir}/Mono.Options.dll",
                        $"{Vars.TOP}/External/Mono.Options/Options.cs",
                        JamList());

                    return Jamrules.BuildAssemblyFromFolderWithCSFiles(
                        "UnityPreserveAttributeParser",
                        "4.0",
                        $"{targetdir}/UnityPreserveAttributeParser.exe",
                        $"{Vars.TOP}/Tools/UnityPreserveAttributeParser",
                        dlls);
                }
            });
    }
}
