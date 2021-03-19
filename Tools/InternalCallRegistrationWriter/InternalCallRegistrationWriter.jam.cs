using JamSharp.Runtime;
using static JamSharp.Runtime.BuiltinRules;
using External.Jamplus.builds.bin.modules;
using External.Jamplus.builds.bin;
using Unity.BuildSystem;

namespace Tools.InternalCallRegistrationWriter
{
    class InternalCallRegistrationWriter : ConvertedJamFile
    {
        internal static void TopLevel()
        {
            Jambase.SubDir(JamList("TOP", "Tools", "InternalCallRegistrationWriter"));
            JamList srcdir = $"{Vars.TOP}/Tools/InternalCallRegistrationWriter/";
            JamList refs = Cecil.Current40AllRefs;
            Jamrules.BuildAssemblyFromFolderWithCSFiles(
                "InternalCallRegistrationWriter",
                "4.0",
                $"{Vars.TOP}/build/bin/InternalCallRegistrationWriter.exe",
                srcdir,
                refs);
        }

        internal static void BuildInternalCallRegistrationWriterCopyTo(JamList project, JamList targetdir)
        {
            copyfile.CopyFile(
                project,
                Combine(
                    targetdir,
                    "/",
                    JamList(
                        "InternalCallRegistrationWriter").GetBoundPath().BaseAndSuffix()),
                "InternalCallRegistrationWriter");

            foreach (var reference in Cecil.Current40AllRefs)
                copyfile.CopyFile(project, $"{targetdir}/{reference.FileName}", new JamList(reference));

            Depends(project, "InternalCallRegistrationWriter");
        }
    }
}
