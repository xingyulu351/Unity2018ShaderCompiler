using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using Newtonsoft.Json.Linq;
using NiceIO;

namespace Bee.Why
{
    internal class TundraExplainer
    {
        private readonly BuiltNodeInfo[] _lines;
        private readonly string[] _roots;
        private readonly TundraBuildGraph _graph;

        private readonly NPath _dagFilePath;
        private readonly NPath structuredLogPath;

        public TundraExplainer(TundraBackend.TundraBackend backend)
        {
            if (!backend.StructuredLogPath.Exists())
                throw new FileNotFoundException(
                    "Couldn't find the structured log file from Tundra; explanations are not available.");

            structuredLogPath = backend.StructuredLogPath.MakeAbsolute();
            var lines = structuredLogPath.ReadAllLines().Select(JObject.Parse).ToList();
            if (lines.Count == 0)
                throw new InvalidDataException("Failed to parse any entries from the structured log.");

            var initMsg = lines[0];
            if ((string)initMsg["msg"] != "init")
                throw new InvalidDataException("The first message in the structured log is not the init message");

            _roots = initMsg["targets"].Select(t => t.ToString()).ToArray();
            _dagFilePath = new NPath(initMsg["dagFile"] + ".json").MakeAbsolute();

            _graph = TundraBuildGraph.CreateFromJsonDagFile(_dagFilePath);
            if (_graph == null)
                throw new InvalidDataException($"Failed to deserialize build DAG from {_dagFilePath}.");

            if (_roots.Length == 0)
            {
                // If no target was specified, Tundra will use the 'default configuration' from the setup object
                // at the end of the dagfile
                _roots = _graph.Setup.BuildTuples[_graph.Setup.DefaultBuildTuple.ConfigIndex].DefaultNodes
                    .Select(i => _graph.Nodes[i].Annotation)
                    .ToArray();
            }


            lines.RemoveAll(obj =>
                !obj.TryGetValue("msg", out JToken msg) ||
                !obj.TryGetValue("annotation", out JToken _) ||
                !((string)msg == "newNode" || (string)msg == "inputSignatureChanged" || (string)msg == "nodeOutputsMissing" || (string)msg == "nodeRetryBuild"));

            _lines = lines.Select(l => new BuiltNodeInfo((string)l["annotation"], (int)l["index"], GetRebuildReasons(l))).ToArray();

            PostprocessRebuildReasons();
        }

        public IEnumerable<BuiltNodeInfo> FindNodesByAnnotation(string annotation)
        {
            if (string.IsNullOrEmpty(annotation))
                throw new ArgumentNullException(nameof(annotation));
            return _lines.Where(l => l.Annotation.Contains(annotation));
        }

        public IEnumerable<BuiltNodeInfo> FindNodesByInput(string inputFragment)
        {
            var inputNodeIndex = _graph.FindIndexByInput(inputFragment);
            return _lines.Where(l => l.OriginalDagIndex == inputNodeIndex);
        }

        public BuiltNodeInfo FindNodeByOutput(NPath outputPath)
        {
            var outputNodeIndex = _graph.FindIndexByOutput(outputPath);
            return _lines.FirstOrDefault(l => l.OriginalDagIndex == outputNodeIndex);
        }

        public Explanation Explain(string softSearch)
        {
            var node = FindNodeByOutput(softSearch)
                ?? FindNodesByAnnotation(softSearch).FirstOrDefault()
                ?? FindNodesByInput(softSearch).FirstOrDefault();

            if (node == null)
                throw new KeyNotFoundException($"Couldn't find any node by output, annotation, or input, containing the string \"{softSearch}\".");

            return Explain(node);
        }

        public Explanation Explain(BuiltNodeInfo o)
        {
            if (o == null)
                throw new ArgumentNullException(nameof(o));

            return new Explanation(o, _graph.GetDependencyChainFor(o.OriginalDagIndex, _roots));
        }

        public static IEnumerable<RebuildReason> GetRebuildReasons(JObject evt)
        {
            if ((string)evt["msg"] == "newNode")
            {
                yield return new NotBuiltBeforeReason();
                yield break;
            }

            if ((string)evt["msg"] == "nodeOutputsMissing")
            {
                yield return new OutputsMissingReason(evt["files"].Select(jv => new NPath(jv.ToString())));
                yield break;
            }

            if ((string)evt["msg"] == "nodeRetryBuild")
            {
                yield return new PreviousBuildFailedReason();
                yield break;
            }

            foreach (var jToken in evt["changes"])
            {
                var change = (JObject)jToken;
                switch ((string)change["key"])
                {
                    case "Action":
                    {
                        yield return new ActionChangedReason((string)change["oldvalue"], (string)change["value"]);
                        break;
                    }
                    case "InputFileList":
                    {
                        yield return new InputListChangedReason((string)change["dependency"],
                            change["oldvalue"].Select(t => new NPath((string)t)),
                            change["value"].Select(t => new NPath((string)t)));
                        break;
                    }
                    case "InputFileDigest":
                    {
                        yield return new InputHashChangedReason(new NPath((string)change["path"]));
                        break;
                    }
                    case "InputFileTimestamp":
                    {
                        yield return new InputTimeStampChangedReason(new NPath((string)change["path"]));
                        break;
                    }
                }
            }
        }

        private void PostprocessRebuildReasons()
        {
            foreach (var line in _lines)
            {
                if (line.RebuildReasons.Count == 0)
                {
                    line.RebuildReasons.Add(new NoGoodReason());
                    continue;
                }

                for (int i = 0; i < line.RebuildReasons.Count; ++i)
                {
                    if (!(line.RebuildReasons[i] is InputChangedReason inputChange))
                        continue;

                    if (inputChange.File.HasExtension("rsp"))
                    {
                        var rspNode = FindNodeByOutput(inputChange.File);
                        if (rspNode == null)
                        {
                            Console.WriteLine($"WARNING: {line.Annotation} changed because it depends on {inputChange.File.InQuotes()}.");
                            Console.WriteLine($"         This looks like a response file (.rsp), so it should have been created by the build process.");
                            Console.WriteLine($"         However, the node that outputs it could not be found in the DAG {_dagFilePath.RelativeTo(NPath.CurrentDirectory).InQuotes()}");
                            Console.WriteLine($"         As a result, response file detail will not be available for this change.");
                            Console.WriteLine();
                            continue;
                        }

                        var explanationForRsp = Explain(rspNode);
                        var reason = explanationForRsp.RebuildReasons.OfType<ActionChangedReason>().Single();
                        line.RebuildReasons[i] = new ResponseFileChangedReason(reason.OldAction, reason.NewAction);
                        continue;
                    }

                    var dependency = FindNodeByOutput(inputChange.File);
                    if (dependency != null)
                        line.RebuildReasons[i] = new DependencyChanged(dependency, inputChange);
                }
            }
        }
    }
}
