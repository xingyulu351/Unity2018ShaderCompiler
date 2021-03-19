using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using Bee.TundraBackend;
using JamSharp.JamState;
using JamSharp.Runtime;
using NiceIO;
using Unity.BuildSystem.Common;
using Unity.BuildTools;
using Unity.TinyProfiling;
using JamTarget = JamSharp.JamState.JamTarget;

namespace Unity.BuildSystem.JamStateToTundraConverter
{
    public class Converter
    {
        public void Convert(JamState jamState, string[] requestedTargets, NPath tundraDagJsonFile,
            JamBackend jamBackend, NPath[] filesInfluencingGraph = null,
            NPath[] tundraFilesToCopyFileAndGlobSignaturesFrom = null)
        {
            using (TinyProfiler.Section("TundraConverter"))
            {
                var requested = requestedTargets == null
                    ? jamState.Targets
                    : jamState.Targets.Where(t => requestedTargets.Contains(t.targetName));

                if (!requested.Any() && requestedTargets != null && requestedTargets.Length > 0)
                {
                    Errors.Exit("Unknown target: " + requestedTargets.First());
                }

                var directoryGeneratingTargets = jamState.Targets.Where(t => jamBackend.ImplicitDependenciesToAddForDirectories.Any(kvp => kvp.Value == t.targetName));

                var targetsToEmit = requested.Concat(directoryGeneratingTargets).ToArray();

                bool useAllNodes = false;
                var allJamNodes = useAllNodes ? jamState.Targets.ToArray() : FindAllJamNodesFor(targetsToEmit).ToArray();

                var backend = new TundraBackend(new NPath("artifacts/tundra"));

                foreach (var file in tundraFilesToCopyFileAndGlobSignaturesFrom ?? Array.Empty<NPath>())
                    backend.AddFileAndGlobSignaturesFromOtherTundraFile(file);

                var nodesWithActions = allJamNodes.Where(n => n.actions.Any());

                using (TinyProfiler.Section(nameof(EmitActions)))
                    EmitActions(nodesWithActions, backend, jamBackend);
                using (TinyProfiler.Section(nameof(EmitDependencies)))
                    EmitDependencies(allJamNodes, backend);

                var convertedjamtargets = "convertedjamtargets";
                backend.AliasIndexFor("convertedjamtargets");
                foreach (var jamTarget in jamState.Targets.Where(t => (requestedTargets?.Contains(t.targetName) ?? true)))
                {
                    if (IsJamNodeFileNode(jamTarget))
                    {
                        if (jamTarget.actions.Any())
                            backend.AddAliasDependency(convertedjamtargets, BackendPathFor(jamTarget));
                    }
                    else
                        backend.AddAliasToAliasDependency(convertedjamtargets, jamTarget.targetName);
                }

                using (TinyProfiler.Section(nameof(RegisterGlobs)))
                    RegisterGlobs(backend, jamBackend);
                using (TinyProfiler.Section(nameof(RegisterFileSignatures)))
                    RegisterFileSignatures(backend, jamBackend.GetAllRegisteredFilesInfluencingGraph());

                RegisterFileSignatures(backend, filesInfluencingGraph ?? Array.Empty<NPath>());
                //unforuntately our editor resources build action actually writes to one of its input files.
                //it writes the same content that was there before, but this still causes tundra to do endless rebuilds.
                //since it overwrites it with the same contents, lets not use timestamps but sha1 for guiskins.
                backend.AddExtensionToBeScannedByHashInsteadOfTimeStamp(".guiskin");
                backend.AddExtensionToBeScannedByHashInsteadOfTimeStamp(".GUISkin");
                backend.AddExtensionToBeScannedByHashInsteadOfTimeStamp(".cpp");
                backend.AddExtensionToBeScannedByHashInsteadOfTimeStamp(".c");
                backend.AddExtensionToBeScannedByHashInsteadOfTimeStamp(".h");
                backend.AddExtensionToBeScannedByHashInsteadOfTimeStamp(".hpp");

                backend.Write(tundraDagJsonFile);
            }
        }

        private void RegisterFileSignatures(TundraBackend backend, IEnumerable<NPath> filesInfluencingBuildProgram)
        {
            foreach (var file in filesInfluencingBuildProgram)
                backend.AddFileSignature(file);
        }

        private void RegisterGlobs(TundraBackend backend, JamBackend jamBackend)
        {
            foreach (var glob in jamBackend.GetAllRegisteredGlobsInfluencingGraph())
            {
                if (glob == new NPath("Runtime/Resource"))
                    throw new ArgumentException("unexpected");
                if (glob.FileName.Contains("NonRedistributable"))
                    throw new ArgumentException("unexpceted");
                backend.RegisterGlobInfluencingGraph(glob);
            }
        }

        private void EmitDependencies(JamTarget[] allJamNodes, TundraBackend backend)
        {
            foreach (var node in allJamNodes)
            {
                if (node.targetName == "all")
                    continue;

                bool isNodeFile = IsJamNodeFileNode(node);

                foreach (var dep in NodeDependenciesNeedsAndIncludes(node))
                {
                    bool isDepFile = IsJamNodeFileNode(dep);
                    bool isDepFileWithAction = isDepFile && dep.actions.Any();

                    if (isNodeFile && isDepFileWithAction)
                    {
                        var nodePath = BackendPathFor(node);
                        var depPath = BackendPathFor(dep);
                        if (depPath != nodePath)
                            backend.AddDependency(nodePath, depPath);
                    }
                    if (isNodeFile && !isDepFileWithAction)
                    {
                        if (!isDepFile || dep.dependencies.Any() || dep.needs.Any())
                            backend.AddDependencyToAlias(BackendPathFor(node), dep.targetName);
                    }
                    if (!isNodeFile && isDepFileWithAction)
                        backend.AddAliasDependency(node.targetName, BackendPathFor(dep));
                    if (!isNodeFile && !isDepFile)
                        backend.AddAliasToAliasDependency(node.targetName, dep.targetName);
                }
            }
        }

        private IEnumerable<ActionInvocation> ValidActionsOf(JamTarget depTarget)
        {
            return depTarget.actions.Where(a => !a.Action.body.Contains("^^($(1)|$(CONTENTS))"));
        }

        private void EmitActions(IEnumerable<JamTarget> nodesWithActions, TundraBackend backend, JamBackend jamBackend)
        {
            var nodesProcessedAsSideEffect = new HashSet<JamTarget>();

            var implicitDependencies = new List<KeyValuePair<NPath, NPath>>();
            foreach (var nodeWithAction in nodesWithActions)
            {
                if (nodesProcessedAsSideEffect.Contains(nodeWithAction))
                    continue;

                using (TinyProfiler.Section(nodeWithAction.targetName))
                {
                    var allTargetsOfAllActionsOfThisNode =
                        ValidActionsOf(nodeWithAction).SelectMany(a => a.Targets).Distinct().ToArray();

                    foreach (var t in allTargetsOfAllActionsOfThisNode)
                        nodesProcessedAsSideEffect.Add(t);

                    if (!allTargetsOfAllActionsOfThisNode.All(j =>
                        j.actions.SequenceEqual(allTargetsOfAllActionsOfThisNode.First().actions)))
                        throw new ArgumentException($"Node {nodeWithAction} has actions that have different targets");

                    var beeAction = MakeBeeAction2(nodeWithAction);

                    WriteFileAction writeFileAction = beeAction as WriteFileAction;
                    if (writeFileAction != null)
                    {
                        backend.AddWriteTextAction(writeFileAction.m_TargetFile, writeFileAction.Contents);
                        continue;
                    }

                    ShellAction shellAction = beeAction as ShellAction;
                    if (shellAction != null)
                    {
                        var targetNameFileSystemSafe =
                            BackendPathFor(allTargetsOfAllActionsOfThisNode.First()).ToString().Replace("/", "_");
                        var simplifiedCommand = SimplifiedCommandFor(shellAction, targetNameFileSystemSafe);

                        string backendCommand = null;
                        FileDependency batchFile = null;
                        switch (simplifiedCommand.InvocationMode)
                        {
                            case InvocationMode.BatchFile:
                                if (HostPlatform.IsWindows)
                                {
                                    var batchFileContents = simplifiedCommand.Commands.Prepend("@echo off").SeparateWith("\n");
                                    batchFile = FileDependencyForBatchFile(batchFileContents, targetNameFileSystemSafe);
                                    backendCommand = $"cmd.exe /c {batchFile.Path.InQuotes(SlashMode.Native)}";
                                }
                                else
                                {
                                    batchFile = FileDependencyForBatchFile(simplifiedCommand.Commands.SeparateWith("\n"), targetNameFileSystemSafe);
                                    backendCommand = $"/bin/sh {batchFile.Path.InQuotes(SlashMode.Native)}";
                                }

                                break;
                            case InvocationMode.CmdExe:
                                backendCommand = $"cmd.exe /S /c \"{simplifiedCommand.Commands.Single()}\"";
                                break;
                            case InvocationMode.CreateProcess:
                                backendCommand = String.Join(" && ", simplifiedCommand.Commands);
                                break;
                        }

                        var inputTargets = ReconstructInputs(allTargetsOfAllActionsOfThisNode);
                        var targetFiles = allTargetsOfAllActionsOfThisNode.Select(BackendPathFor).Distinct().ToArray();
                        var targetFilesSet = new HashSet<NPath>(targetFiles);

                        var inputFiles = inputTargets
                            .Where(IsJamNodeFileNode)
                            .Select(BackendPathFor)
                            //there's a bug in v1 depgraph creation code that makes the .obj file that gets created as part of generating a .pch file, depend on that pch file, making it both an input and an output.
                            //let's filter for that right here, as fixing this in v1 is far from trivial.
                            .Where(f => !targetFilesSet.Contains(f))
                            .ToArray();

                        var rspFiles = batchFile == null
                            ? simplifiedCommand.RspFiles
                            : simplifiedCommand.RspFiles.Append(batchFile).ToArray();

                        var targetVars =
                            GlobalVariables.Singleton.Storage.AllOnTargetVariablesOn(nodeWithAction.targetName);
                        bool disAllowOutput = targetVars.TryGetValue("ALLOW_UNEXPECTED_OUTPUT", out JamList value);
                        if (disAllowOutput && value != "0")
                            throw new ArgumentException("You can only set ALLOW_UNEXPECTED_OUTPUT to 0");
                        targetVars.TryGetValue("ALLOWED_OUTPUT_SUBSTRINGS", out JamList allowedSubstrings);

                        backend.AddAction(
                            actionName: shellAction.TypeString,
                            targetFiles: targetFiles,
                            inputs: inputFiles,
                            executableStringFor: backendCommand,
                            commandLineArguments: Array.Empty<string>(),
                            supportResponseFile: false,
                            allowUnexpectedOutput: !disAllowOutput,
                            allowUnwrittenOutputFiles: shellAction.AllowUnwrittenOutputFiles,
                            allowedOutputSubstrings: allowedSubstrings?.Elements ?? Array.Empty<string>(),
                            rspAndBatchFiles: rspFiles.Select(rsp => rsp.Path).ToArray()
                        );

                        foreach (var rspFile in rspFiles)
                        {
                            backend.AddWriteTextAction(rspFile.Path, rspFile.Contents);
                            backend.AddDependency(targetFiles[0], rspFile.Path);
                        }

                        IEnumerable<NPath> filesToScanForImplicitDependencies = inputFiles;

                        var includeDirectories = IncludeDirectoriesFor(allTargetsOfAllActionsOfThisNode);
                        if (includeDirectories.Any())
                        {
                            var relativeIncludeDirs = includeDirectories.ToArray();//.Select(i => i.MakeAbsolute().RelativeTo(Paths.UnityRoot)).ToArray();
                            backend.ScanCppFileForHeaders(BackendPathFor(allTargetsOfAllActionsOfThisNode.First()), null, relativeIncludeDirs);
                            filesToScanForImplicitDependencies = filesToScanForImplicitDependencies.Concat(relativeIncludeDirs);
                        }

                        foreach (var input in filesToScanForImplicitDependencies)
                        {
                            var foundKvp = jamBackend.ImplicitDependenciesToAddForDirectories.FirstOrDefault(kvp => input.IsChildOf(kvp.Key));
                            if (foundKvp.Key == null)
                                continue;
                            implicitDependencies.Add(new KeyValuePair<NPath, NPath>(targetFiles.First(), foundKvp.Value));
                        }
                    }
                }
            }

            foreach (var implicitDep in implicitDependencies)
            {
                backend.AddDependency(implicitDep.Key, implicitDep.Value);
            }
        }

        private static NPath[] IncludeDirectoriesFor(JamTarget[] allTargetsOfAllActionsOfThisNode)
        {
            var allOnTargetVariablesOn = GlobalVariables.Singleton.Storage.AllOnTargetVariablesOn(allTargetsOfAllActionsOfThisNode.First().targetName);

            if (allOnTargetVariablesOn.TryGetValue("TUNDRA_INCLUDEDIRS", out var headers))
                return headers.ToNPaths();

            return Array.Empty<NPath>();
        }

        private static FileDependency FileDependencyForBatchFile(string batchFileContents, string targetNameFileSystemSafe)
        {
            return new FileDependency
            {
                Path = new NPath($"artifacts/tundra/batchfiles/bat{HashValueFor(batchFileContents)}{targetNameFileSystemSafe}.{(HostPlatform.IsWindows ? "bat" : "sh")}"),
                Contents = batchFileContents
            };
        }

        private IEnumerable<JamTarget> ReconstructInputs(JamTarget[] allTargetsOfAllActionsOfThisNode)
        {
            var result = new List<JamTarget>();
            var directDependencies = allTargetsOfAllActionsOfThisNode.SelectMany(a => a.dependencies.Concat(a.includes).Select(d => d.Target)).Distinct().ToArray();
            foreach (var directDependency in directDependencies)
            {
                if (IsJamNodeFileNode(directDependency))
                {
                    result.Add(directDependency);
                    continue;
                }

                foreach (var dep in directDependency.dependencies.Select(d => d.Target).Where(IsJamNodeFileNode))
                {
                    //Console.WriteLine("Reconstructing input through indirect dependency: "+dep.BoundPath);
                    result.Add(dep);
                }
            }
            return result;
        }

        JamTarget[] NodeDependenciesNeedsAndIncludes(JamTarget jamNode)
        {
            var beeDependenciesForJamNodeDepends = jamNode.dependencies.Concat(jamNode.needs)
                .Select(d => d.Target)
                .ToArray();

            //note that we only have to process the includes of files that we depend on. our own includes are not a part of our dependencies, they only influence other nodes that depend on us.
            var dependenciesForIncludesOfJamNodeDepends =
                jamNode.dependencies.Concat(jamNode.needs)
                    .SelectMany(d => RecursiveIncludesOf(d.Target))
                    .Distinct()
                    .ToArray();

            return beeDependenciesForJamNodeDepends.Concat(dependenciesForIncludesOfJamNodeDepends).Exclude(jamNode).ToArray();
        }

        IEnumerable<JamTarget> RecursiveIncludesOf(JamTarget target, HashSet<JamTarget> processed = null)
        {
            processed = processed ?? new HashSet<JamTarget>();

            foreach (var inc in target.includes)
            {
                if (processed.Contains(inc.Target))
                    continue;

                processed.Add(inc.Target);
                yield return inc.Target;

                foreach (var subinc in RecursiveIncludesOf(inc.Target, processed))
                    yield return subinc;
            }
        }

        bool IsJamNodeFileNode(JamTarget jamNode)
        {
            if (jamNode.actions.Count > 0)
                return true;
            if (jamNode.dependencies.Count > 0)
                return false;
            if (jamNode.needs.Count > 0)
                return false;
            if (jamNode.targetName.Contains("DependenciesForAllObjectFiles"))
                return false;
            return true;
        }

        public enum InvocationMode
        {
            BatchFile,
            CmdExe,
            CreateProcess
        }

        private static readonly char[] AllNewLines = {'\n', '\r'};

        static InvocationMode RequiredInvocationThroughCmdExe(IEnumerable<ShellCommand> commands)
        {
            if (commands.Count() > 1)
                return InvocationMode.BatchFile;

            var command = commands.Single();

            var lines = command.Command.Split(AllNewLines, StringSplitOptions.RemoveEmptyEntries);

            if (lines.Length > 1)
                return InvocationMode.BatchFile;

            if (!HostPlatform.IsWindows)
                return InvocationMode.CreateProcess;

            var binary = new NPath(command.Command.Split(' ').ToArray()[0]);
            if (binary.HasExtension("bat"))
                return InvocationMode.CmdExe;
            if (!binary.FileExists())
                return InvocationMode.CmdExe;
            return InvocationMode.CreateProcess;
        }

        class FileDependency
        {
            public NPath Path;
            public string Contents;
        }

        class SimplifiedCommand
        {
            public string[] Commands;
            public InvocationMode InvocationMode;
            public FileDependency[] RspFiles;
        }

        private static string AddNewLinesIfNeeded(string input)
        {
            if (input.Length < 100000)
                return input;

            if (input.StartsWith("CREATE", StringComparison.OrdinalIgnoreCase))
            {
                //we have a few exotic cases where switch buildcode uses a responsefile to actually store a linker command script.
                //the linker command script has things like CREATE addmod etc, and if you add newlines to the script, it will no
                //longer work.
                return input;
            }

            var sb = new StringBuilder();
            bool inQuotes = false;
            foreach (var c in input)
            {
                if (c == '"') inQuotes = !inQuotes;
                if (!inQuotes && c == ' ') sb.Append(Environment.NewLine);
                else sb.Append(c);
            }
            return sb.ToString();
        }

        private static SimplifiedCommand SimplifiedCommandFor(ShellAction shellAction, string targetNameFileSystemSafe)
        {
            var commands = new List<string>();

            var invocationMode = RequiredInvocationThroughCmdExe(shellAction.ShellCommands);

            var rspFiles = new List<FileDependency>();
            foreach (var shellCommand in shellAction.ShellCommands)
            {
                var command = shellCommand.Command.Trim();
                foreach (var replacement in shellCommand.Replacements)
                {
                    if (replacement.IsResponseReplacement)
                    {
                        var contents =
                            AddNewLinesIfNeeded(replacement.ReplacementContents);
                        var nPath = new NPath($"artifacts/tundra/rsp/{HashValueFor(shellAction.TypeString + " " + targetNameFileSystemSafe + "." + rspFiles.Count)}.rsp");
                        rspFiles.Add(new FileDependency
                        {
                            Path = nPath,
                            Contents = contents
                        });
                        command = command.Replace(replacement.StringToReplace, nPath.ToString(SlashMode.Native));
                        continue;
                    }

                    command = command.Replace(replacement.StringToReplace, replacement.ReplacementContents);
                }
                commands.AddRange(command.Split(new[] {'\n'}, StringSplitOptions.RemoveEmptyEntries));
            }

            return new SimplifiedCommand()
            {
                Commands = commands.ToArray(),
                InvocationMode = invocationMode,

                //revert the rsp files, so they appear in the original order as they were written
                //so that the backend will also display them like that.
                RspFiles = ((IEnumerable<FileDependency>)rspFiles).Reverse().ToArray()
            };
        }

        private static ulong HashValueFor(string rspContents)
        {
            var knuthHash = new KnuthHash();
            knuthHash.Add(rspContents);
            return knuthHash.Value;
        }

        NPath BackendPathFor(JamTarget jamTarget)
        {
            var npath = new NPath(jamTarget.BoundPath);
            return npath.IsRelative ? npath : npath.RelativeTo(Paths.UnityRoot);
        }

        private static IEnumerable<JamTarget> FindAllJamNodesFor(JamTarget[] targets)
        {
            using (TinyProfiler.Section("FindAllJamNodesForTargets"))
            {
                var processed = new HashSet<JamTarget>(targets);
                var queue = new Queue<JamTarget>(targets);
                while (queue.Count > 0)
                {
                    var t = queue.Dequeue();
                    yield return t;

                    foreach (var d in t.actions.SelectMany(a => a.Targets)
                             .Concat(t.dependencies.Concat(t.needs).Concat(t.includes).Select(d => d.Target)))
                        if (processed.Add(d))
                            queue.Enqueue(d);
                }
            }
        }

        private static NPath BeePathFor(string pathString)
        {
            var npath = new NPath(pathString);
            return npath.IsRelative ? npath : npath.RelativeTo(Paths.UnityRoot);
        }

        private static IEnumerable<ActionInvocation> MergeTogetherActions(IEnumerable<ActionInvocation> inputInvocations)
        {
            var groups = inputInvocations.GroupBy(ai => ai.Action.name);
            foreach (var group in groups)
            {
                var actionInvocation = @group.First();
                var actionInfo = actionInvocation.Action;

                if (@group.Count() <= 1)
                {
                    yield return @group.Single();
                    continue;
                }

                if (!actionInfo.actionFlags.HasFlag(ActionsFlags.Together))
                {
                    ConverterWarning($"Warning: Multiple invocations of {actionInfo.name} on {actionInvocation.Targets.Select(t=>t.targetName).SeparateWithComma()} while that action does not have together flag");
                    foreach (var ai in @group)
                        yield return ai;
                    continue;
                }

                //lets merge the together action invocations in a single one, concatenating all the second arguments, and Distinct()-ing them, which matches the behaviour of the jam backend.
                yield return new ActionInvocation()
                {
                    Action = actionInfo,
                    Callstack = "",
                    SecondArgument =
                        new JamList(@group.SelectMany(ai => ai.SecondArgument.Elements).Distinct()),

                    //todo: assert all targets are identical
                    Targets = actionInvocation.Targets
                };
            }
        }

        private static void ConverterWarning(string warning)
        {
        }

        static Action MakeBeeAction2(JamTarget jamNode)
        {
            using (new TemporaryTargetContext(jamNode.targetName))
            {
                var actionBodyReplacer = new ActionBodyReplacer();

                var globalVariables = GlobalVariables.Singleton;

                var actions = jamNode.actions;

                var mergeTogetherActions = MergeTogetherActions(actions);

                if (mergeTogetherActions.Count() > 4)
                {
                    Console.WriteLine($"A lot of actions for {jamNode.targetName}");
                    foreach (var actionInvocation in mergeTogetherActions)
                        Console.WriteLine(actionInvocation.Action.name);
                    Console.WriteLine();
                }

                var shellCommands = new List<ShellCommand>();
                var usedActions = new List<ActionInvocation>();
                foreach (var actionInvocation in mergeTogetherActions)
                {
                    var regex = @"\^\^\((.*)\|(.*)\)";
                    var match = Regex.Match(actionInvocation.Action.body, regex);
                    if (match.Success)
                    {
                        /*
                        if (jamNode.actions.Count(a => !(a.Action is WriteFileAction)) > 1)
                            throw new ArgumentException(
                                $"Found a WriteFile action on {jamNode.targetName} that also has other actions.");
*/
                        var contentsExpression = new JamSharp.Runtime.Parser.Parser(match.Groups[2].Value).ParseExpression();
                        var contentsValue = RuntimeExpressionEvaluation.Evaluate(contentsExpression, globalVariables)
                            .ToString();

                        return new WriteFileAction(contentsValue, BeePathFor(jamNode.BoundPath));
                    }

                    var targetsBoundPath = actionInvocation.Targets.Select(t => t.BoundPath).ToArray();
                    globalVariables["<"].Assign(targetsBoundPath);
                    globalVariables["1"].Assign(targetsBoundPath);

                    var boundSecondArguments = actionInvocation.SecondArgument != null
                        ? actionInvocation.SecondArgument.GetBoundPath()
                        : new JamList();
                    globalVariables[">"].Assign(boundSecondArguments);
                    globalVariables["2"].Assign(boundSecondArguments);


                    var shellCommand = actionBodyReplacer.Replace(actionInvocation.Action.body, globalVariables);
                    if (shellCommands.Any())
                    {
                        if (shellCommand.CommandWithNonResponseCommandsFlattened ==
                            shellCommands.Last().CommandWithNonResponseCommandsFlattened &&
                            !shellCommand.Replacements.Any(r => r.IsResponseReplacement) &&
                            !shellCommands.Last().Replacements.Any(r => r.IsResponseReplacement))
                        {
                            //this is a complete dupe, let's skip it. this will happens sometimes if in the jam code,
                            //a certain action is setup twice. it happens especially with copying files.
                            continue;
                        }
                    }

                    usedActions.Add(actionInvocation);
                    shellCommands.Add(shellCommand);
                }
                return new ShellAction(shellCommands.ToArray(),
                    usedActions.Select(a => a.Action?.name).SeparateWith("_"),
                    usedActions.Any(a => a.Action.actionFlags.HasFlag(ActionsFlags.AllowUnwrittenOutputFiles)));
            }
        }
    }
}
