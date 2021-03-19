using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using Newtonsoft.Json;
using NiceIO;

namespace Bee.Why
{
    internal class TundraBuildGraph
    {
        public class Node
        {
            public string Annotation = null;
            public NPath[] Inputs = null;
            public NPath[] Outputs = null;
            public int[] Deps = null;
        }

        public Node[] Nodes = null;

        public class SetupInfo
        {
            public class DefaultBuildTupleInfo
            {
                public int ConfigIndex = 0;
            }

            public DefaultBuildTupleInfo DefaultBuildTuple = null;

            public class BuildTuple
            {
                public int[] DefaultNodes = null;
            }

            public BuildTuple[] BuildTuples = null;
        }

        public SetupInfo Setup = null;

        class JsonNPathConverter : JsonConverter
        {
            public override void WriteJson(JsonWriter writer, object value, JsonSerializer serializer)
            {
                writer.WriteValue(value.ToString());
            }

            public override object ReadJson(JsonReader reader, Type objectType, object existingValue, JsonSerializer serializer)
            {
                return new NPath((string)reader.Value);
            }

            public override bool CanConvert(Type objectType)
            {
                return objectType == typeof(NPath);
            }
        }

        public static TundraBuildGraph CreateFromJsonDagFile(NPath sourceFile)
        {
            var jsonContent = sourceFile.ReadAllText();
            return JsonConvert.DeserializeObject<TundraBuildGraph>(jsonContent, new JsonNPathConverter());
        }

        public DependencyChain GetDependencyChainFor(int targetNodeIndex, string[] targets)
        {
            return GetDependencyChainFor(targetNodeIndex,
                targets.Select(t => FindIndexByOutputOrAnnotation(t)).ToArray());
        }

        public int FindIndexByOutput(NPath path)
        {
            var p = path.ToString();
            return Array.FindIndex(Nodes, n => n.Outputs.Contains(p));
        }

        public int FindIndexByOutputOrAnnotation(string fragment)
        {
            int findByOutput = FindIndexByOutput(new NPath(fragment));
            if (findByOutput != -1)
                return findByOutput;

            return Array.FindIndex(Nodes, n => n.Annotation.Contains(fragment));
        }

        public int FindIndexByInput(string fragment)
        {
            return Array.FindIndex(Nodes, n => n.Inputs.Any(i => i.ToString().Contains(fragment)));
        }

        public DependencyChain GetDependencyChainFor(int targetNodeIndex, int[] targetIndices)
        {
            List<HashSet<int>> potentialChain = new List<HashSet<int>> {new HashSet<int> {targetNodeIndex}};

            while (true)
            {
                var depOn = potentialChain.Last();

                if (depOn.Overlaps(targetIndices))
                    break;

                var newDepOn = new HashSet<int>();
                for (int i = 0; i < Nodes.Length; ++i)
                {
                    if (depOn.Overlaps(Nodes[i].Deps))
                        newDepOn.Add(i);
                }

                if (newDepOn.Count == 0)
                    // Couldn't find any nodes to add to the dep chain
                    throw new InvalidDataException(
                        $"None of the nodes at the head of the dependency chain ({string.Join(", ", depOn.Select(d => d.ToString()).ToArray())}) appeared to have any nodes that required them, but they're also not targets.");

                potentialChain.Add(newDepOn);
            }

            List<int> resultNodeIndices = new List<int> {potentialChain.Last().Intersect(targetIndices).First()};
            potentialChain.RemoveAt(potentialChain.Count - 1);

            while (potentialChain.Count > 0)
            {
                resultNodeIndices.Add(potentialChain.Last().Intersect(Nodes[resultNodeIndices.Last()].Deps).First());
                potentialChain.RemoveAt(potentialChain.Count - 1);
            }

            return new DependencyChain(resultNodeIndices.Select((nodeIndex, chainIndex) =>
            {
                var node = Nodes[nodeIndex];
                var nextNode = (chainIndex < resultNodeIndices.Count - 1)
                    ? Nodes[resultNodeIndices[chainIndex + 1]]
                    : null;
                var depKind = DependencyChain.Kind.None;
                if (nextNode != null)
                {
                    depKind = node.Inputs.Intersect(nextNode.Outputs).Any()
                        ? DependencyChain.Kind.Uses
                        : DependencyChain.Kind.Depends;
                }

                return new Tuple<BuiltNodeInfo, DependencyChain.Kind>(new BuiltNodeInfo(node.Annotation, nodeIndex, null), depKind);
            }));
        }
    }
}
