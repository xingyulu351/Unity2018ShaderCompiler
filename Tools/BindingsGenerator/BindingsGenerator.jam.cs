using System.Collections.Generic;
using System.Linq;
using JamSharp.Runtime;
using static JamSharp.Runtime.BuiltinRules;
using External.Jamplus.builds.bin;
using NiceIO;
using Unity.BuildSystem;

namespace Tools.BindingsGenerator
{
    internal class BindingsGenerator : ConvertedJamFile
    {
        private static Dictionary<string, JamList> s_IDEBindingsCsFiles = new Dictionary<string, JamList>();

        internal static JamList BindingsGenerator_UnprocessedAssemblyTargetFor(JamList target, JamList targetdll)
        {
            var unprocessedTarget = Combine(target, "_UnprocessedAssembly");
            if (!targetdll.IsEmpty)
            {
                var relativeTargetDir = new NPath(targetdll.GetDirectory().Single());
                if (!relativeTargetDir.IsRelative)
                    relativeTargetDir = relativeTargetDir.RelativeTo(Vars.TOP.Single());
                var uprocessedTargetDir = Combine(Vars.TOP, "/artifacts/UnprocessedAssemblies/", relativeTargetDir.ToString());
                var unprocessedTargetDll = targetdll.SetDirectory(uprocessedTargetDir);

                "BINDING".On(unprocessedTarget).Assign(unprocessedTargetDll);
            }

            return unprocessedTarget;
        }

        internal static JamList BindingsGenerator_Files(JamList parent, JamList target, string module = "Core", JamList parentTargetName = null)
        {
            List<NPath> generatedHeaderPaths;
            return BindingsGenerator_FilesForProfile(parent, target, module, parentTargetName, "unity", out generatedHeaderPaths);
        }

        internal static JamList BindingsGenerator_FilesForProfile(JamList parent,
            JamList targets,
            string module,
            JamList parentTargetName,
            JamList profile,
            out List<NPath> generatedHeaderPaths)
        {
            var allGenPaths = JamList();
            generatedHeaderPaths = new List<NPath>();

            foreach (var target in targets.ElementsAsJamLists)
            {
                JamList outdir = $"artifacts/{parentTargetName??parent}/modules/{module}/bindings_{Vars.SCRIPTING_BACKEND}";
                var unprocessedTarget = BindingsGenerator_UnprocessedAssemblyTargetFor(target, Vars.targetdll);
                if (ProjectFiles.IsRequested)
                {
                    // When generating IDE project files, we skip all the complicated "actual bindings generation"
                    // logic, but need to emit generated C++ source files into the projects.
                    JamList csFiles;
                    if (s_IDEBindingsCsFiles.TryGetValue(unprocessedTarget.SingleOrDefault(), out csFiles))
                    {
                        var cppFiles = csFiles.Select(s => $"{outdir}/{new NPath(s).FileNameWithoutExtension}.gen.cpp");
                        var hFiles = csFiles.Select(s => $"{outdir}/{new NPath(s).FileNameWithoutExtension}.gen.h");
                        allGenPaths.Append(cppFiles);
                        allGenPaths.Append(hFiles);
                    }
                    continue;
                }

                var references = JamList();
                using (OnTargetContext(target))
                {
                    references.Assign(Vars.assemblyReferences);
                }

                var genPaths = JamList();
                var bindingCsDefines = JamList();
                var bindingCsFiles = JamList();
                using (OnTargetContext(unprocessedTarget))
                {
                    bindingCsDefines.Assign(Vars.CS_DEFINES);
                    bindingCsFiles.Assign(Vars.CS_FILES.Include(".bindings.cs"));
                }

                // Sort the list of bindings .cs files so that the generated file suffixes in case of a collision are stable-ish
                bindingCsFiles = bindingCsFiles.Sorted();
                var seenFileNames = new HashSet<string>();

                foreach (var bindingCsFile in bindingCsFiles.ElementsAsJamLists)
                {
                    NPath csFilePath = bindingCsFile.Single();
                    var csFileName = csFilePath.FileNameWithoutExtension;

                    // If we have seen this bindings filename in some different folder already, then make the generated
                    // filename have an additional counter suffix, i.e. SomeFile1.gen.cpp, SomeFile2.gen.cpp etc.
                    if (seenFileNames.Contains(csFileName))
                    {
                        var counter = 0;
                        var newFileName = csFileName;
                        while (seenFileNames.Contains(newFileName))
                        {
                            newFileName = csFileName + counter;
                            ++counter;
                        }
                        csFileName = newFileName;
                    }

                    seenFileNames.Add(csFileName);
                    var outpath = $"{outdir}/{csFileName}.gen.cpp".ToNPath();
                    var outheader = $"{outdir}/{csFileName}.gen.h".ToNPath();

                    var genPath = JamList(outpath).SetGrist(parent);

                    BindingsGeneratorTool.Instance().SetupInvocation(
                        outputCppFile: outpath,
                        outputHeaderFile: outheader,
                        inputAssembly: unprocessedTarget,
                        inputFile: bindingCsFile,
                        codeGenReferences: references.Select(p => new JamTarget(p)).ToArray(),
                        defines: bindingCsDefines.Elements,
                        scriptingBackend: GlobalScriptingBackendValue,
                        profile: profile);

                    Depends(genPath, outpath);
                    genPaths.Append(genPath);
                    generatedHeaderPaths.Add(outheader);
                }

                Depends(parent, genPaths);
                allGenPaths.Append(genPaths);
            }

            return allGenPaths;
        }

        internal static JamList BindingsGenerator_BuildAssemblyAndPostprocessBindings(JamList target,
            JamList profile,
            JamList targetdll,
            JamList csfiles,
            JamList defines,
            JamList systemreferences,
            JamList references,
            JamList responsefiles = null,
            JamList extraflags = null,
            JamList scriptingBackend = null)
        {
            target = target.Clone();
            csfiles = csfiles.Clone();
            references = references.Clone();
            responsefiles = responsefiles ?? new JamList();
            extraflags = extraflags ?? new JamList();
            ScriptingBackend actualScriptingBackend = scriptingBackend == null ?
                GlobalScriptingBackendValue : ScriptingBackendFromJamList(scriptingBackend);

            "BINDING".On(target).Assign(targetdll);

            var winrtLegacyDll = JamList();
            using (OnTargetContext(target))
            {
                winrtLegacyDll.Assign(Vars.winRTLegacyDll);
            }

            target.AssignIfEmpty(Jambase._retrieveActiveTargetName(target));
            var unprocessedTarget = BindingsGenerator_UnprocessedAssemblyTargetFor(target, targetdll);

            if (ProjectFiles.IsRequested)
            {
                // When generating IDE project files, we skip all the complicated "actual bindings generation"
                // logic, but need to record which bindings files got assigned for each assembly target
                var bindingCsFiles = csfiles.Include(".bindings.cs");
                var name = unprocessedTarget.SingleOrDefault();
                if (s_IDEBindingsCsFiles.ContainsKey(name))
                    s_IDEBindingsCsFiles[name].Append(bindingCsFiles);
                else
                    s_IDEBindingsCsFiles.Add(name, bindingCsFiles);

                Jamrules.Action_NullAction(target);
                return targetdll.Clone();
            }


            // I don't like adding this here. Would be nicer to add this in the module setup for SharedInternals itself,
            // but we need to have the unprocessed target in the grist, which we don't know at that point.
            if (target.Single().Contains("SharedInternals"))
                csfiles.Append(Projects.Jam.SetupRuntimeModules2.BuildModuleFriendsAttributesFile(unprocessedTarget.Single()));

            Jamrules.GenDepends(unprocessedTarget, csfiles.Include("\\.gen\\.cs$"));

            if (profile == "uap")
            {
                Depends(unprocessedTarget, winrtLegacyDll);
                references.Append(winrtLegacyDll.BackSlashify().BackSlashify().Escape());
            }

            Jamrules.BuildAssembly(
                unprocessedTarget,
                profile,
                unprocessedTarget.GetBoundPath(),
                csfiles,
                defines,
                systemreferences,
                references,
                responsefiles,
                extraflags);
            Depends(target, unprocessedTarget);

            //later invocations of BindingsGeneratorTool.SetupInvocation still rely on being
            //able to figure out which references its input assembly have been built with
            "assemblyReferences".On(target).Assign(references);

            using (OnTargetContext(target))
            {
                if (Vars.mergeNativeArray)
                {
                    var unmergedTarget = unprocessedTarget.GetBoundPath().WithSuffix(".buffertemp.dll");

                    BindingsGeneratorTool.Instance().SetupPatchInvocation(
                        outputFile: unmergedTarget,
                        inputAssembly: unprocessedTarget,
                        defines: defines.Elements,
                        patcherReferences: references.AsJamTargets(),
                        scriptingBackend: actualScriptingBackend,
                        profile: profile);

                    UnsafeUtilityPatcherTool.Instance.SetupInvocation(target, unmergedTarget);
                }
                else
                {
                    BindingsGeneratorTool.Instance().SetupPatchInvocation(
                        outputFile: target,
                        inputAssembly: unprocessedTarget,
                        defines: defines.Elements,
                        patcherReferences: references.AsJamTargets(),
                        scriptingBackend: actualScriptingBackend,
                        profile: profile);
                }
            }

            Depends(targetdll, target);

            return targetdll.Clone();
        }

        static ScriptingBackend GlobalScriptingBackendValue
        {
            get
            {
                var scriptingBackend = Vars.SCRIPTING_BACKEND;
                return ScriptingBackendFromJamList(scriptingBackend);
            }
        }

        private static ScriptingBackend ScriptingBackendFromJamList(JamList scriptingBackend)
        {
            switch (scriptingBackend.Single().ToUpperInvariant())
            {
                case "IL2CPP":
                    return ScriptingBackend.IL2CPP;
                case "DOTNET":
                    return ScriptingBackend.DotNet;
                case "MONO":
                default:
                    return ScriptingBackend.Mono;
            }
        }
    }
}
