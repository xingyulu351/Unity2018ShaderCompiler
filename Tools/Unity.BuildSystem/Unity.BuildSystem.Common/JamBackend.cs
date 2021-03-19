using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Bee.Core;
using JamSharp.Runtime;
using NiceIO;
using Unity.BuildTools;

namespace Unity.BuildSystem.Common
{
    public class JamBackend : Backend, IJamBackend
    {
        readonly HashSet<string> s_FileWritingActions = new HashSet<string>();

        public readonly string MagicJamWriteTextFileContents = "^^($(1)|$(CONTENTS))";

        public void EnsureFileWritingActionIsSetupWithName(string actionName)
        {
            if (s_FileWritingActions.Contains(actionName))
                return;

            s_FileWritingActions.Add(actionName);

            JamCore.RegisterAction(actionName, MagicJamWriteTextFileContents);
        }

        public void AddWriteTextAction(NPath file, string contents, string actionName)
        {
            var path = file;
            "CONTENTS".On(path).Assign(contents);

            var actionNameToUse = actionName ?? "WriteFile";
            EnsureFileWritingActionIsSetupWithName(actionNameToUse);

            // Jam treats all arguments of all rules as possible targets to find/build; however
            // the file contents we write out are not a file path.
            BuiltinRules.NotFile(new JamList(contents));
            BuiltinRules.Depends("all", file);
            BuiltinRules.InvokeJamAction(actionNameToUse, path);
        }

        public void AddDependency(NPath @from, NPath to)
        {
            BuiltinRules.Needs(@from, to);
        }

        public void AddDependency(NPath @from, IEnumerable<NPath> to)
        {
            BuiltinRules.Needs(@from, new JamList(to));
        }

        public void AddIncludes(NPath thing, NPath includesThisThing)
        {
            BuiltinRules.Includes(thing, includesThisThing);
        }

        public void ScanCppFileForHeaders(NPath objectFile, NPath cppFile, NPath[] includeDirectories)
        {
            var cppGristedToOurIncludeDirs =
                $"<{Hash(includeDirectories)}>{cppFile.ToString(SlashMode.Forward)}";

            "TUNDRA_INCLUDEDIRS".On(objectFile).Assign(includeDirectories);

            BuiltinRules.Depends(objectFile, cppGristedToOurIncludeDirs);

            //tricky business. if we're compiling a .cpp file that is a generated file, and it's not yet
            //generated from a previous run, the gristed cpp file, will fail to be bound.  we deal with
            //this situation by making the gristed cpp file depend on the non gristed one that has the action
            BuiltinRules.Depends(cppGristedToOurIncludeDirs, cppFile);
        }

        readonly HashSet<NPath> _setupTargets = new HashSet<NPath>();

        readonly Dictionary<string, string> _setupActions = new Dictionary<string, string>();
        private readonly Dictionary<NPath, NPath> _implicitDependenciesToAddForDirectories = new Dictionary<NPath, NPath>();

        string ActionBodyFor(bool supportsResponseFile) => supportsResponseFile
        ? "$(ENVIRONMENTSETUPPREFIX)$(PROGRAM) @@($(ARGS:N))"
        : "$(ENVIRONMENTSETUPPREFIX)$(PROGRAM) $(ARGS)";

        public void AddAction(
            string actionName,
            NPath[] targetFiles,
            NPath[] inputs,
            string executableString,
            string[] commandLineArguments,
            bool supportResponseFile = false,
            Dictionary<string, string> environmentVariables = null,
            NPath[] rspAndBatchFiles = null,
            bool allowUnexpectedOutput = true,
            bool allowUnwrittenOutputFiles = true,
            string[] allowedOutputSubstrings = null)
        {
            if (actionName == null)
                throw new ArgumentNullException(nameof(actionName));
            if (executableString == null)
                throw new ArgumentNullException(nameof(executableString));

            var targetName = targetFiles.First();

            if (!_setupTargets.Add(targetName))
                throw new ArgumentException($"A target is being setup more than once, without ActionsFlags.Together: {targetName}");

            if (!_setupActions.ContainsKey(actionName))
            {
                var actionBody = ActionBodyFor(supportResponseFile);
                var flags = ActionsFlags.Response;
                if (allowUnwrittenOutputFiles)
                    flags |= ActionsFlags.AllowUnwrittenOutputFiles;
                JamCore.RegisterAction(actionName, actionBody, flags);
                _setupActions.Add(actionName, actionBody);
            }

            var environmentSetupPrefix = new StringBuilder();

            if (environmentVariables != null && environmentVariables.Any())
            {
                if (HostPlatform.IsWindows)
                    environmentSetupPrefix.Append($"perl {Paths.UnityRoot}/Tools/Build/EnvironmentHelper.pl ");
                foreach (var en in environmentVariables)
                    environmentSetupPrefix.Append($"{en.Key}={en.Value} ");
            }

            "ENVIRONMENTSETUPPREFIX".On(targetFiles).Assign(environmentSetupPrefix.ToString());
            "ARGS".On(targetFiles)
                .Assign(
                    supportResponseFile && commandLineArguments.Length > 500
                    ? commandLineArguments.SeparateWith("\n")
                    : commandLineArguments.SeparateWithSpace());
            "PROGRAM".On(targetFiles).Assign(executableString);

            BuiltinRules.Depends("all", targetFiles, inputs);

            BuiltinRules.InvokeJamAction(actionName, targetFiles.ToJamList());

            if (!allowUnexpectedOutput)
                BuiltinRules.DisallowUnexpectedOutputFor(targetName);
            if (allowedOutputSubstrings != null && allowedOutputSubstrings.Length > 0)
                BuiltinRules.SetAllowedOutputSubstringsFor(targetName, allowedOutputSubstrings);
        }

        public void AddAliasDependency(string alias, NPath path)
        {
            BuiltinRules.Depends(alias, path);
        }

        public IEnumerable<KeyValuePair<NPath, NPath>> ImplicitDependenciesToAddForDirectories => _implicitDependenciesToAddForDirectories;
        public void AddImplicitDependenciesToAllActionsWithInputsThatLiveIn(NPath directory, NPath makeDependOnThis)
        {
            if (_implicitDependenciesToAddForDirectories.TryGetValue(directory, out var existingValue) &&
                existingValue != makeDependOnThis)
                throw new ArgumentException();

            _implicitDependenciesToAddForDirectories[directory] = makeDependOnThis;
        }

        public IEnumerable<NPath> AllDirectoriesProducingImplicitInputs => _implicitDependenciesToAddForDirectories.Keys;

        public void RegisterGlobInfluencingGraph(NPath directory)
        {
            _registeredGlobsInfluencingGraph.Add(directory);
        }

        readonly HashSet<NPath> _registeredGlobsInfluencingGraph = new HashSet<NPath>();
        readonly HashSet<NPath> _registeredFilesInfluencingGraph = new HashSet<NPath>();

        public IEnumerable<NPath> GetAllRegisteredGlobsInfluencingGraph()
        {
            return _registeredGlobsInfluencingGraph.OrderBy(p => p);
        }

        public void RegisterFileInfluencingGraph(params NPath[] files)
        {
            foreach (var file in files)
            {
                _registeredFilesInfluencingGraph.Add(file);
            }
        }

        public IEnumerable<NPath> GetAllRegisteredFilesInfluencingGraph()
        {
            return _registeredFilesInfluencingGraph.OrderBy(p => p);
        }

        public void AddAliasToAliasDependency(string alias, string otherAlias)
        {
            BuiltinRules.Depends(alias, otherAlias);
        }

        public void NotifyOfLump(NPath lumpCpp, NPath[] inputFiles)
        {
            BuiltinRules.Includes(lumpCpp, inputFiles);
        }

        //if tundra is enabled, we always can handle colors, as tundra will strip them out if we end up outputing in a non interactive environment
        public bool CanHandleAnsiColors => true;

        static string Hash(IEnumerable<NPath> includeDirectories)
        {
            if (!includeDirectories.Any())
                return "00000";

            return Math.Abs(includeDirectories.Select(i => i.GetHashCode()).Aggregate(CombineHashCodes)).ToString();
        }

        private static int CombineHashCodes(int h1, int h2)
        {
            return (((h1 << 5) + h1) ^ h2);
        }
    }
}
