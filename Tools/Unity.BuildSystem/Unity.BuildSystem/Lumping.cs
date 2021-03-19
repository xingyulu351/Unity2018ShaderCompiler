using System.Collections.Generic;
using System.Linq;
using System.Text;
using Bee.Core;
using Modules.SketchUpEditor;
using External.Jamplus.builds.bin;
using JamSharp.Runtime;
using NiceIO;
using Projects.Jam;
using Unity.BuildSystem.Common;
using Unity.BuildTools;
using Unity.TinyProfiling;

namespace Unity.BuildSystem
{
    internal class Lumping : ConvertedJamFile
    {
        static bool s_Lump = Vars.OS != "LINUX";

        static Lumping()
        {
            if (GlobalVariables.Singleton["LUMP"] == "0")
                s_Lump = false;
        }

        public static bool Enabled => s_Lump;

        //lumping is complicated. SetupLumps will go through your sources, and figure out which ones are eligable for being put into a single lump.
        //it will then remove those files from your sources list, and write out small .cpp files that say "#include yourcppfile.cpp".  those lumpfiles
        //then get added to your sources variable.
        //SetupLumps is smart in that it will check per file compile flags and include dirs and defines to determine eligability. it is important however
        //that you call SetupLumps _after_ you have setup all per-file defines and includepaths.

        static HashSet<string> RelevantCustomCompileSettings { get; } =
            new HashSet<string>(PermutationsOf("DEFINES")
                .Concat(PermutationsOf("C++FLAGS"))
                .Concat(PermutationsOf("HDRS"))
                .Append("FORCE_INCLUDES"));

        static IEnumerable<string> PermutationsOf(string something)
        {
            return new[]
            {
                $"{something}.*.*",
                $"{something}.{Vars.CONFIG}.*",
                $"{something}.*.{Vars.PLATFORM}",
                $"{something}.{Vars.CONFIG}.{Vars.PLATFORM}"
            };
        }

        static readonly StringBuilder _compileSettingsStringBuilder = new StringBuilder();
        private static HashSet<string> _hashSetWithPch = new HashSet<string> {"PCH"};

        static string GetCustomCompileSettings(JamList target, JamList src)
        {
            _compileSettingsStringBuilder.Clear();

            var gristed = src.SetGrist(target);
            JamList gristedWithObjSuffix = gristed.WithSuffix(Vars.SUFOBJ);

            var onTargetVars = GlobalVariables.Singleton.Storage.AllOnTargetVariablesOn(gristedWithObjSuffix.Single());

            foreach (var kvp in onTargetVars.OrderBy(kvp => kvp.Key))
            {
                if (!RelevantCustomCompileSettings.Contains(kvp.Key))
                    continue;
                _compileSettingsStringBuilder.Append(kvp.Value.ToString());
            }

            var onTargetVars2 = GlobalVariables.Singleton.Storage.AllOnTargetVariablesOn(gristed.Single());
            JamList value2;
            if (onTargetVars2.TryGetValue("PCH", out value2))
                _compileSettingsStringBuilder.Append(value2.ToString());

            return _compileSettingsStringBuilder.ToString();
        }

        static void CopyVarsFromAtoB(string @from, string to, HashSet<string> vars)
        {
            var fromVariables = GlobalVariables.Singleton.Storage.AllOnTargetVariablesOn(@from);

            foreach (var kvp in fromVariables)
            {
                if (!vars.Contains(kvp.Key)) continue;
                GlobalVariables.Singleton.GetOrCreateVariableOnTargetContext(to, kvp.Key).Assign(kvp.Value);
            }

            foreach (var v in vars)
            {
                JamList value;
                if (fromVariables.TryGetValue(v, out value))
                    GlobalVariables.Singleton.GetOrCreateVariableOnTargetContext(to, v).Assign(value);
            }
        }

        static string LumpContentsFor(string precompiledHeader, string[] filesToLump)
        {
            var contents = new StringBuilder($"/* This lump file is a generated file.  Do not modify. */");
            contents.AppendLine();

            if (precompiledHeader != null)
                contents.AppendLine($"#include \"{precompiledHeader}\"");


            foreach (var includeLine in filesToLump.Select(boundPath => $"#include \"{boundPath}\""))
                contents.AppendLine(includeLine);

            contents.AppendLine();
            return contents.ToString();
        }

        static bool ShouldExcludeFromBuild(JamList file, string grist)
        {
            JamList exclude;
            if (GlobalVariables.Singleton.Storage.AllOnTargetVariablesOn(file.SetGrist(grist).Single()).TryGetValue("EXCLUDED_FROM_BUILD", out exclude))
            {
                if (exclude == "true")
                    return true;
            }
            return false;
        }

        public static void SetupLumps(JamList target, JamList srcsVarname, string precompiledHeader = null)
        {
            using (TinyProfiler.Section("SetupLumps"))
            {
                target = target.Clone();
                target.Assign(Jambase._retrieveActiveTargetName(target));

                var srcs = GlobalVariables.Singleton.DereferenceElementsNonFlat(srcsVarname).Single();
                srcs.Assign(SetupLumpsReturnResult(target, srcs.SetGrist(""), precompiledHeader));
            }
        }

        public static JamList SetupLumpsReturnResult(JamList target, JamList inputSources, string pchSettings)
        {
            using (TinyProfiler.Section("SetupLumpsReturnResult"))
            {
                target = target.Clone();

                // SUFOBJ migth end up being not assigned if we there are no C compilers assigned
                if (ProjectFiles.IsRequested || !s_Lump || !Vars.SUFOBJ)
                    return inputSources.Clone();

                target.Assign(Jambase._retrieveActiveTargetName(target));

                var lumpable_files = new List<string>();
                var nonlumpable_files = new List<string>();
                var targetName = target.Single();
                foreach (var f in inputSources)
                {
                    if (ShouldExcludeFromBuild(f, targetName))
                        continue;
                    if (NonLumpableFiles.IsLumpableFile(f))
                        lumpable_files.Add(f);
                    else
                        nonlumpable_files.Add(f);
                }

                var byDirectory = lumpable_files.GroupBy(f => new NPath(f).Parent);

                var filesToCompile = new JamList(nonlumpable_files);
                foreach (var directoryGroup in byDirectory)
                {
                    var bySettings = directoryGroup.GroupBy(f => GetCustomCompileSettings(target, f).ToString());
                    var varsSufobj = Vars.SUFOBJ.Single();

                    int lumpNumber = 0;
                    foreach (var uniqeSettingsGroup in bySettings)
                    {
                        if (uniqeSettingsGroup.Count() <= 1)
                        {
                            filesToCompile.Append(uniqeSettingsGroup.Single());
                            continue;
                        }

                        foreach (var chunk in uniqeSettingsGroup.OrderBy(e => e).Chunked(8))
                        {
                            var chunkArray = chunk.ToArray();
                            var npath = new NPath(chunkArray.First()).Parent;
                            var dirName = npath.ToString(SlashMode.Forward).Replace("/", "_");
                            var lumpedFileTarget = $"<{target.Single()}>{dirName.ToLower()}_{lumpNumber++}.cpp";

                            var lumpContents = LumpContentsFor(pchSettings, chunkArray);
                            Jambase.MakeLocate(lumpedFileTarget, Vars.LOCATE_TARGET);

                            var gristedChunk = new JamList(chunkArray).SetGrist(target);
                            "LUMPSOURCES".On(lumpedFileTarget).Assign(gristedChunk);
                            "SEARCH".On(gristedChunk).Assign(Vars.TOP);
                            BuiltinRules.Includes(lumpedFileTarget, gristedChunk);

                            // Lumped file target only includes the filename, and not the artifacts folder,
                            // so it is the same between different architectures/configs.
                            ((JamBackend)Backend.Current).AddWriteTextAction(lumpedFileTarget, lumpContents, "MakeLump");

                            JamList src = chunkArray[0];

                            JamList objTarget = src.SetGrist(target).WithSuffix(varsSufobj);
                            var gristedLumpTarget = ((JamList)lumpedFileTarget).SetGrist(target);
                            JamList dstTarget = gristedLumpTarget.WithSuffix(varsSufobj);

                            CopyVarsFromAtoB(objTarget.Single(), dstTarget.Single(), RelevantCustomCompileSettings);
                            CopyVarsFromAtoB(src.SetGrist(target).Single(), gristedLumpTarget.Single(), _hashSetWithPch);

                            filesToCompile.Append(lumpedFileTarget);
                        }
                    }
                }
                return filesToCompile;
            }
        }
    }
}
