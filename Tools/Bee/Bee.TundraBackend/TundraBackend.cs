using System;
using System.Collections.Generic;
using System.ComponentModel.Design;
using System.Configuration;
using System.IO;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Security.Policy;
using System.Text.RegularExpressions;
using Bee.Core;
using Newtonsoft.Json.Linq;
using NiceIO;
using Unity.BuildTools;

namespace Bee.TundraBackend
{
    public class TundraBackend : Backend, IBackend
    {
        public NPath TundraFilesRoot { get; }

        private class ActionObject : JObject {}

        private readonly bool _checkForMissingInputs;
        private JObject Json { get; }
        internal JArray ActionsArray { get; } = new JArray();
        private JArray ScannersArray { get; } = new JArray();
        private JObject NamedNodes { get; } = new JObject();
        private JArray DefaultNodes { get; } = new JArray();
        private JArray FileSignatures { get; } = new JArray();
        private JArray GlobSignatures { get; } = new JArray();
        private JArray ContentDigestExtensions { get; } = new JArray() {".rsp", ".dll", ".exe"};
        readonly Dictionary<NPath, int> _outputFileToActionIndex = new Dictionary<NPath, int>();
        private readonly List<NPath[]> _scannerPaths = new List<NPath[]>();
        private readonly Dictionary<string, int> _aliasToActions = new Dictionary<string, int>();
        private Dictionary<NPath, int> _directoriesCausingImplicitDependencies = new Dictionary<NPath, int>();
        private int DefaultDefaultNodeIndex { get; }
        public NPath DigestCacheFile => "tundra.digestcache";

        public TundraBackend(NPath tundraFilesRoot, string buildTitle = null, bool checkForMissingInputs = false)
        {
            TundraFilesRoot = tundraFilesRoot;
            _checkForMissingInputs = checkForMissingInputs;
            Json = new JObject
            {
                ["Nodes"] = ActionsArray,
                ["FileSignatures"] = FileSignatures,
                ["GlobSignatures"] = GlobSignatures,
                ["ContentDigestExtensions"] = ContentDigestExtensions,
                ["StructuredLogFileName"] = StructuredLogPath.ToString(),
                ["StateFileName"] = $"{tundraFilesRoot}/TundraBuildState.state",
                ["StateFileNameTmp"] = $"{tundraFilesRoot}/TundraBuildState.state.tmp",
                ["ScanCacheFileName"] =  $"{tundraFilesRoot}/tundra.scancache",
                ["ScanCacheFileNameTmp"] =  $"{tundraFilesRoot}/tundra.scancache.tmp",
                ["DigestCacheFileName"] =  $"{tundraFilesRoot}/{DigestCacheFile}",
                ["DigestCacheFileNameTmp"] =  $"{tundraFilesRoot}/{DigestCacheFile}.tmp",
                ["Passes"] = new JArray() {"0"},
                ["Scanners"] = ScannersArray,
                ["Setup"] = new JObject
                {
                    ["DefaultBuildTuple"] = new JObject
                    {
                        ["VariantIndex"] = 0,
                        ["ConfigIndex"] = 0,
                        ["SubVariantIndex"] = 0
                    },
                    ["SubVariants"] = new JArray
                    {
                        "default"
                    },
                    ["Configs"] = new JArray
                    {
                        "default"
                    },
                    ["Variants"] = new JArray
                    {
                        "default"
                    },
                    ["BuildTuples"] = new JArray
                    {
                        new JObject
                        {
                            ["VariantIndex"] = 0,
                            ["DefaultNodes"] = DefaultNodes,
                            ["NamedNodes"] = NamedNodes,
                            ["ConfigIndex"] = 0,
                            ["SubVariantIndex"] = 0,
                            ["AlwaysNodes"] = new JArray()
                        }
                    }
                }
            };

            if (buildTitle != null)
                Json["BuildTitle"] = buildTitle;

            //set to -1, so that when we create the DefaultDefaultNode, we realize it's the very first node, and it shouldn't
            //Bee added to its own dependencies.
            DefaultDefaultNodeIndex = -1;
            DefaultDefaultNodeIndex = AliasIndexFor(DefaultDefaultNode);
        }

        public NPath StructuredLogPath => $"{TundraFilesRoot}/tundra.log.json";

        public void Write(NPath target)
        {
            Json["Identifier"] = target.ToString();
            if (DefaultNodes.Count == 0)
                DefaultNodes.Add(AliasIndexFor(DefaultDefaultNode));
            if (ActionsArray.Count == 0)
                SetForceDagRebuild(true);
            target.MakeAbsolute().WriteAllText(Json.ToString());
        }

        public void AddDefaultNode(NPath path)
        {
            int index;
            if (!_outputFileToActionIndex.TryGetValue(path, out index))
                throw new ArgumentException($"No actionIndex for {path}");
            DefaultNodes.Add(index);
        }

        public void AddDefaultAliasNode(string node)
        {
            DefaultNodes.Add(AliasIndexFor(node));
        }

        public void SetForceDagRebuild(bool value)
        {
            Json["ForceDagRebuild"] = value ? 1 : 0;
        }

        public void AddWriteTextAction(NPath file, string contents, string actionName = "WriteText")
        {
            var writeTextFileActionObject = CreateWriteTextFileActionObject(file, contents, $"{actionName} {file}");

            if (_outputFileToActionIndex.TryGetValue(file, out var alreadyRegisteredIndex))
            {
                var alreadyRegistered = ActionsArray[alreadyRegisteredIndex];
                if (((JObject)alreadyRegistered).TryGetValue("WriteTextFilePayload", out var existingPayload))
                {
                    if (alreadyRegistered["WriteTextFilePayload"].ToString() == contents)
                        return;
                    throw new ArgumentException($"{file} has already been registered with different file contents");
                }

                throw new ArgumentException($"{file} is already registered as a non writetextfile action");
            }
            var index = RegisterActionObject(writeTextFileActionObject);
            _outputFileToActionIndex.Add(file, index);
        }

        public void AddDependency(NPath @from, NPath to)
        {
            AddToDepsArray(ActionIndexFor(@from), ActionIndexFor(to));
        }

        internal int ActionIndexFor(NPath @from)
        {
            int fromIndex;
            if (!_outputFileToActionIndex.TryGetValue(@from, out fromIndex))
                throw new ArgumentException($"No actionIndex for {@from}");
            return fromIndex;
        }

        private void AddToDepsArray(int fromIndex, int toIndex)
        {
            var actionObject = ActionsArray[fromIndex];

            if ((int)actionObject["DebugActionIndex"] == toIndex)
            {
                Console.WriteLine($"You're making action {actionObject["Annotation"]} depend on itself");
                return;
            }

            ((JArray)actionObject["Deps"]).Add(toIndex);
        }

        public void AddDependency(NPath @from, IEnumerable<NPath> to)
        {
            foreach (var t in to)
                AddDependency(@from, t);
        }

        public void AddIncludes()
        {
            throw new NotImplementedException();
        }

        public void ScanCppFileForHeaders(NPath objectFile, NPath cppFile, NPath[] includeDirectories)
        {
            var index = _outputFileToActionIndex[objectFile];
            ActionsArray[index]["ScannerIndex"] = ScannerIndexFor(includeDirectories);

            foreach (var dir in includeDirectories)
            {
                foreach (var kvp in _directoriesCausingImplicitDependencies)
                    if (dir.IsChildOf(kvp.Key))
                        ((JArray)ActionsArray[index]["Deps"]).Add(kvp.Value);
            }
        }

        private int ScannerIndexFor(NPath[] includeDirectories)
        {
            for (int i = 0; i != _scannerPaths.Count; i++)
            {
                if (_scannerPaths[i].SequenceEqual(includeDirectories))
                    return i;
            }
            _scannerPaths.Add(includeDirectories);
            ScannersArray.Add(new JObject
            {
                ["Kind"] = "cpp",
                ["IncludePaths"] = new JArray(includeDirectories.Select(d => d.ToString()))
            });
            return _scannerPaths.Count - 1;
        }

        public void AddAction(string actionName,
            NPath[] targetFiles,
            NPath[] inputs,
            string executableStringFor,
            string[] commandLineArguments,
            bool supportResponseFile = false,
            Dictionary<string, string> environmentVariables = null,
            NPath[] rspAndBatchFiles = null,
            bool allowUnexpectedOutput = true,
            bool allowUnwrittenOutputFiles = false,
            string[] allowedOutputSubstrings = null)
        {
            rspAndBatchFiles = rspAndBatchFiles ?? Array.Empty<NPath>();

            var annotation = $"{actionName} {targetFiles.First()}";

            if (supportResponseFile)
            {
                var rspContents = commandLineArguments.SeparateWithSpace();
                var knuth = new KnuthHash();
                knuth.Add(annotation);
                NPath rspPath = $"artifacts/rsp/{knuth.Value}.rsp";
                AddWriteTextAction(rspPath, rspContents, "WriteResponseFile");

                commandLineArguments = new[] {$"@{rspPath.InQuotes(SlashMode.Native)}"};
                rspAndBatchFiles = rspAndBatchFiles.Append(rspPath).ToArray();
                inputs = inputs.Append(rspPath).ToArray();
            }

            var action = $"{executableStringFor} {commandLineArguments.SeparateWithSpace()}";

            if (_checkForMissingInputs)
                CheckForMissingInputs(action, rspAndBatchFiles, inputs, targetFiles, actionName);


            var targetFilesList = new List<NPath>(targetFiles);
            var actionObject = CreateActionObject(targetFiles: targetFiles,
                inputs: inputs,
                annotation: annotation,
                action: action,
                allowUnexpectedOutput: allowUnexpectedOutput,
                allowedOutputSubstrings: allowedOutputSubstrings,
                allowUnwrittenOutputFiles: allowUnwrittenOutputFiles,
                rspAndBatchFiles: rspAndBatchFiles,
                environmentVariables: environmentVariables);
            foreach (var targetFile in targetFiles)
            {
                var actionIndex = 0;
                if (_outputFileToActionIndex.TryGetValue(targetFile, out actionIndex))
                {
                    var existingActionObject = CreateComparableActionObject(ActionsArray[actionIndex]);
                    if (!JToken.DeepEquals(actionObject, existingActionObject))
                        throw new ArgumentException(
                            $"Another action is already registered for {targetFile}\nNew:{actionObject}\nExisting:{existingActionObject}");
                    targetFilesList.Remove(targetFile);
                }
            }
            if (!targetFilesList.Any())
                return;

            var index = RegisterActionObject(actionObject);
            foreach (var targetFile in targetFilesList)
                _outputFileToActionIndex.Add(targetFile, index);
        }

        private JObject CreateComparableActionObject(JToken actionObject)
        {
            var result = new JObject
            {
                ["Annotation"] = actionObject["Annotation"],
                ["Action"] = actionObject["Action"],
                ["Inputs"] = actionObject["Inputs"],
                ["Outputs"] = actionObject["Outputs"],
                ["PassIndex"] = actionObject["PassIndex"],
                ["Deps"] = actionObject["Deps"],
                ["OverwriteOutputs"] = actionObject["OverwriteOutputs"],
                ["FrontendResponseFiles"] = actionObject["FrontendResponseFiles"],
                ["AllowUnexpectedOutput"] = actionObject["AllowUnexpectedOutput"],
                ["AllowUnwrittenOutputFiles"] = actionObject["AllowUnwrittenOutputFiles"],
                ["AllowedOutputSubstrings"] = actionObject["AllowedOutputSubstrings"],
            };
            if (actionObject["Env"] != null)
                result["Env"] = actionObject["Env"];
            return result;
        }

        private ActionObject CreateActionObject(NPath[] targetFiles,
            NPath[] inputs,
            string annotation,
            string action,
            bool allowUnexpectedOutput,
            string[] allowedOutputSubstrings,
            bool allowUnwrittenOutputFiles,
            NPath[] rspAndBatchFiles = null,
            Dictionary<string, string> environmentVariables = null)
        {
            var deps = new JArray();
            foreach (var input in inputs)
            {
                int result = 0;
                if (_outputFileToActionIndex.TryGetValue(input, out result))
                    deps.Add(result);
                foreach (var kvp in _directoriesCausingImplicitDependencies.Where(kvp => input.IsChildOf(kvp.Key)))
                    deps.Add(kvp.Value);
            }

            var actionObject = new ActionObject
            {
                ["Annotation"] = annotation,
                ["Action"] = action,
                ["Inputs"] = new JArray(inputs.Concat(rspAndBatchFiles ?? Array.Empty<NPath>()).Select(i => i.ToString()).Distinct()),
                ["Outputs"] = new JArray(targetFiles.Select(i => i.ToString())),
                ["PassIndex"] = 0,
                ["Deps"] = deps,
                ["OverwriteOutputs"] = true,
                ["FrontendResponseFiles"] = new JArray((rspAndBatchFiles ?? Array.Empty<NPath>()).Select(i => i.ToString())),
                ["AllowUnexpectedOutput"] = allowUnexpectedOutput,
                ["AllowUnwrittenOutputFiles"] = allowUnwrittenOutputFiles,
                ["AllowedOutputSubstrings"] = new JArray(allowedOutputSubstrings ?? Array.Empty<string>())
            };
            if (environmentVariables != null)
                actionObject["Env"] =
                    new JArray(environmentVariables.Select(kvp => new JObject() {["Key"] = kvp.Key, ["Value"] = kvp.Value}));
            return actionObject;
        }

        private ActionObject CreateWriteTextFileActionObject(NPath targetFile, string payload, string annotation)
        {
            return new ActionObject
            {
                ["Annotation"] = annotation,
                ["WriteTextFilePayload"] = payload,
                ["Inputs"] = new JArray(),
                ["Outputs"] = new JArray(targetFile.ToString()),
                ["PassIndex"] = 0,
                ["Deps"] = new JArray(),
                ["OverwriteOutputs"] = true,
                ["AllowUnwrittenOutputFiles"] = true
            };
        }

        private int RegisterActionObject(ActionObject action)
        {
            int actionIndex = ActionsArray.Count;
            action["DebugActionIndex"] = actionIndex;
            ActionsArray.Add(action);

            if (DefaultDefaultNodeIndex >= 0)
                ((JArray)ActionsArray[DefaultDefaultNodeIndex]["Deps"]).Add(actionIndex);
            return actionIndex;
        }

        private static string DefaultDefaultNode => "all_tundra_nodes";

        private static Regex[] s_Regex = CheckExtensions.Select(e => new Regex($"[a-zA-Z0-9/\\\\_\\.\\-\\+]+\\.{e}[ \\\"\\n]", RegexOptions.Compiled)).ToArray();
        private static string[] CheckExtensions => new[] {"c", "cpp", "cc", "xml", "cs", "js"};

        private void CheckForMissingInputs(string action, NPath[] rspAndBatchFiles, NPath[] inputs, NPath[] outputs, string actionName)
        {
            var filesFoundInCommandLine = ExtractAllFilesFromAction(action, rspAndBatchFiles).Select(s => new NPath(s).MakeAbsolute()).ToArray();

            var absoluteInputsAndOutputs = inputs.Concat(outputs).Select(f => f.MakeAbsolute()).ToArray();

            foreach (var found in filesFoundInCommandLine.Where(f => !absoluteInputsAndOutputs.Contains(f)))
            {
                var totalCommand = rspAndBatchFiles.Select(f => f.MakeAbsolute().ReadAllText()).Append(action).SeparateWithSpace() + " ";
                Console.WriteLine(
                    $"Found {found} in commandline, but not in inputs or outputs.  ActionName: {actionName} Target: {outputs.First()} CommandLine: {action} TotalCommand: {totalCommand}");
            }
        }

        private static List<string> ExtractAllFilesFromAction(string action, NPath[] rspAndBatchFiles)
        {
            var result = new List<string>();
            var totalCommand = " " + rspAndBatchFiles.Select(f => f.MakeAbsolute().ReadAllText()).Append(action).SeparateWithSpace() + " ";
            foreach (var regex in s_Regex)
            {
                foreach (Match match in regex.Matches(totalCommand))
                {
                    result.Add(match.Value.TrimEnd(' ', '\"'));
                }
            }
            return result;
        }

        public void AddAliasDependency(string fromAlias, NPath to)
        {
            if (fromAlias == "all")
                throw new ArgumentException("You're setting up a dependency for alias all, but that is no longer required");
            int aliasIndex = AliasIndexFor(fromAlias);

            int toIndex;
            if (!_outputFileToActionIndex.TryGetValue(to, out toIndex))
                throw new ArgumentException($"No actionIndex for {to}");

            ((JArray)ActionsArray[aliasIndex]["Deps"]).Add(toIndex);
        }

        public void RegisterGlobInfluencingGraph(NPath directory)
        {
            AddGlobSignature(directory);
        }

        public void RegisterFileInfluencingGraph(params NPath[] files)
        {
            AddFileSignature(files);
        }

        public void AddAliasToAliasDependency(string fromAlias, string toAlias)
        {
            int aliasIndex = AliasIndexFor(fromAlias);
            int toAliasIndex = AliasIndexFor(toAlias);

            ((JArray)ActionsArray[aliasIndex]["Deps"]).Add(toAliasIndex);
        }

        public bool CanHandleAnsiColors => true;

        public void MightNotUpdate(NPath path)
        {
        }

        public int AliasIndexFor(string aliasName)
        {
            int result;
            if (_aliasToActions.TryGetValue(aliasName, out result))
                return result;
            result = RegisterActionObject(CreateActionObject(Array.Empty<NPath>(),
                Array.Empty<NPath>(),
                aliasName,
                String.Empty,
                true,
                Array.Empty<string>(),
                false,
                null));
            _aliasToActions.Add(aliasName, result);
            NamedNodes.Add(aliasName, result);
            return result;
        }

        public void AddDependencyToAlias(NPath fromPath, string toAlias)
        {
            int toAliasIndex = AliasIndexFor(toAlias);

            int fromIndex;
            if (!_outputFileToActionIndex.TryGetValue(fromPath, out fromIndex))
                throw new ArgumentException($"No actionIndex for {fromPath}");

            ((JArray)ActionsArray[fromIndex]["Deps"]).Add(toAliasIndex);
        }

        public void AddExtensionToBeScannedByHashInsteadOfTimeStamp(params string[] extensions)
        {
            foreach (var extension in extensions)
                ContentDigestExtensions.Add(extension.StartsWith(".") ? extension : "." + extension);
        }

        public void AddFileSignature(params NPath[] paths)
        {
            foreach (var path in paths.Select(p => p.RelativeTo(NPath.CurrentDirectory)))
            {
                if (_alreadyAddedFileSignatures.Add(path))
                    FileSignatures.Add(new JObject() {["File"] = path.ToString()});
            }
        }

        readonly HashSet<NPath> _alreadyAddedGlobSignatures = new HashSet<NPath>();
        readonly HashSet<NPath> _alreadyAddedFileSignatures = new HashSet<NPath>();

        public void AddGlobSignature(NPath directory)
        {
            directory = directory.RelativeTo(NPath.CurrentDirectory);
            if (!_alreadyAddedGlobSignatures.Add(directory))
                return;
            GlobSignatures.Add(new JObject {["Path"] = directory.ToString()});
        }

        public void AddImplicitDependenciesToAllActionsWithInputsThatLiveIn(NPath directory, NPath makeDependOnThis)
        {
            if (!_outputFileToActionIndex.TryGetValue(makeDependOnThis, out var dependencyIndex))
                throw new ArgumentException($"No actionIndex for {makeDependOnThis}");

            if (_directoriesCausingImplicitDependencies.TryGetValue(directory, out int existingDependencyIndex))
            {
                if (existingDependencyIndex == dependencyIndex) return; // OK, have it already
                throw new ArgumentException($"Another implicit dependency already configured for directory '{directory}'");
            }

            _directoriesCausingImplicitDependencies.Add(directory, dependencyIndex);
        }

        public IEnumerable<NPath> AllDirectoriesProducingImplicitInputs =>
            _directoriesCausingImplicitDependencies.Keys;

        public void AddFileAndGlobSignaturesFromOtherTundraFile(NPath tundraFile)
        {
            var json = JObject.Parse(tundraFile.MakeAbsolute().ReadAllText());
            foreach (var file in json["FileSignatures"])
                AddFileSignature(new NPath(file["File"].ToString()));
            foreach (var dir in json["GlobSignatures"])
                AddGlobSignature(new NPath(dir["Path"].ToString()));
        }
    }
}
