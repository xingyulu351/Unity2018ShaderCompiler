using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using JamSharp.Runtime;
using Newtonsoft.Json;
using NiceIO;
using Unity.BuildTools;

namespace JamSharp.JamState
{
    public class JamStateHtmlWriter
    {
        Dictionary<JamTarget, int> m_TargetToId;
        JamTarget[] m_TargetsToEmit;
        Dictionary<JamTarget, HashSet<JamTarget>> m_InverseDependencies;
        readonly Dictionary<JamTarget, int> m_RecursiveChildrenCache = new Dictionary<JamTarget, int>();
        Dictionary<string, int> m_StringPrefixes = new Dictionary<string, int>();
        readonly bool _slowMode = GlobalVariables.Singleton["WRITE_DEPGRAPH_SLOW"] == "1";

        public void WriteFor(JamState jamState)
        {
            var requestedTargets = GlobalVariables.Singleton["JAM_COMMAND_LINE_TARGETS"].Elements.Select(e =>
            {
                var t = jamState.GetTarget(e);
                if (t == null)
                    throw new Exception($"Unknown target '{e}' while writing build dependency graph.");
                return t;
            }).ToArray();

            WriteFor(requestedTargets);
        }

        public void WriteFor(JamTarget[] requestedTargets, NPath reportDir = null)
        {
            m_TargetsToEmit = FindTargetsToEmit(requestedTargets).ToArray();
            m_InverseDependencies = BuildInverseDependenciesMap();
            var template =
                Paths.UnityRoot.Combine("Tools/Unity.BuildSystem/JamSharp.JamState/depgraph_template.html")
                    .ReadAllText();

            var targetName = requestedTargets[0].targetName;
            targetName = SanitizeTargetName(targetName);
            var fileName = reportDir ?? new NPath($"artifacts/depgraph_{targetName}.html");

            Console.WriteLine($"Writing dependency graph of '{targetName}' into {fileName}...");
            using (var sw = new StreamWriter(fileName.ToString()))
            {
                var replaceMe = "'$REPLACE_ME$'";
                var index = template.IndexOf(replaceMe, StringComparison.Ordinal);
                sw.Write(template.Substring(0, index));

                // Serialize directly into the file, instead of in-memory string or Json.Linq objects.
                // For large dependency graphs the memory usage might be massive, and would make us crash
                // with out of memory (e.g. depgraph for mac editor is about 1GB of JSON text).
                Serialize(sw, requestedTargets);

                sw.Write(template.Substring(index + replaceMe.Length));
            }
            Console.ForegroundColor = ConsoleColor.DarkGreen;
            Console.Write("[depgraph] ");
            Console.ResetColor();

            if (HostPlatform.IsWindows)
            {
                Console.WriteLine("Wrote dependency graph, opening it in the browser");
                Process.Start(fileName.ToString(SlashMode.Backward));
            }
            else
            {
                Console.WriteLine("Wrote dependency graph");
                // Note: doing Process.Start for "open" with the pathname argument makes the actual
                // build that follows this start failing mysteriously. So don't do that for now :(
            }
        }

        private void Serialize(StreamWriter sw, JamTarget[] requestedTargets)
        {
            var json = new JsonTextWriter(sw)
            {
                StringEscapeHandling = StringEscapeHandling.EscapeNonAscii,
                Formatting = Formatting.Indented,
                Indentation = 1,
                QuoteName = false
            };
            json.WriteStartObject();

            json.WritePropertyName("nodes");
            json.WriteStartArray();
            foreach (var t in m_TargetsToEmit)
                SerializeTarget(json, t);
            json.WriteEnd();

            json.WritePropertyName("requestednodes");
            json.WriteStartArray();
            foreach (var t in requestedTargets)
                json.WriteValue(m_TargetToId[t]);
            json.WriteEndArray();

            json.WritePropertyName("prefixes");
            json.WriteStartArray();
            foreach (var s in m_StringPrefixes.OrderBy(p => p.Value))
                json.WriteValue(s.Key);
            json.WriteEndArray();

            json.WritePropertyName("top");
            json.WriteValue(Paths.UnityRoot.ToString());

            json.WriteEndObject();
        }

        private static string SanitizeTargetName(string name)
        {
            name = name.Replace('/', '_');
            name = name.Replace('\\', '_');
            name = name.Replace('<', '_');
            name = name.Replace('>', '_');
            name = name.Replace('!', '_');
            return name;
        }

        private static readonly char[] prefixSeparators = {'/', '\\'};

        // A lot of strings we store in the graph dump are path-like; with common
        // prefixes that occur very often. To reduce amount of storage & memory use
        // in the final html/js, store a table of common prefixes and markup
        // parts of strings with "@index@".
        //
        // This is a simple non-optimal algorithm that just gets "folder" part of
        // any name and uses that as a prefix; works pretty well.
        string StringWithPrefix(string s)
        {
            int splitPos = s.LastIndexOfAny(prefixSeparators);
            if (splitPos < 0)
                return s;

            string prefix = s.Substring(0, splitPos);
            int index;
            if (m_StringPrefixes.TryGetValue(prefix, out index))
            {
                // have prefix in the table already
                return $"@{index}@{s.Substring(splitPos)}";
            }

            // add new prefix to the table
            index = m_StringPrefixes.Count;
            m_StringPrefixes.Add(prefix, index);
            return $"@{index}@{s.Substring(splitPos)}";
        }

        Dictionary<JamTarget, HashSet<JamTarget>> BuildInverseDependenciesMap()
        {
            var inverseDeps = new Dictionary<JamTarget, HashSet<JamTarget>>();
            m_TargetToId = new Dictionary<JamTarget, int>();
            for (int i = 0; i != m_TargetsToEmit.Length; i++)
            {
                var target = m_TargetsToEmit[i];
                m_TargetToId[target] = i;
                foreach (var subDep in target.dependencies.Concat(target.includes).Concat(target.needs))
                {
                    HashSet<JamTarget> set;
                    if (!inverseDeps.TryGetValue(subDep.Target, out set))
                    {
                        set = new HashSet<JamTarget>();
                        inverseDeps[subDep.Target] = set;
                    }
                    set.Add(target);
                }
            }
            return inverseDeps;
        }

        IEnumerable<JamTarget> FindTargetsToEmit(JamTarget[] targets)
        {
            var processed = new HashSet<JamTarget>();
            return targets.Concat(targets.SelectMany(t => CrawlNode(t, processed)));
        }

        IEnumerable<JamTarget> CrawlNode(JamTarget target,
            HashSet<JamTarget> processed)
        {
            return CrawlDepList(target.dependencies, processed)
                .Concat(CrawlDepList(target.includes, processed))
                .Concat(CrawlDepList(target.needs, processed));
        }

        IEnumerable<JamTarget> CrawlDepList(List<Dependency> depList, HashSet<JamTarget> processed)
        {
            foreach (var dep in depList)
            {
                var depTarget = dep.Target;
                if (processed.Contains(depTarget))
                    continue;
                yield return depTarget;
                processed.Add(depTarget);
                foreach (var subDep in CrawlNode(depTarget, processed))
                    yield return subDep;
            }
        }

        void SerializeTarget(JsonTextWriter json, JamTarget targetInfo)
        {
            json.WriteStartObject();
            json.WritePropertyName("id"); json.WriteValue(m_TargetToId[targetInfo]);
            json.WritePropertyName("name"); json.WriteValue(StringWithPrefix(targetInfo.targetName));
            json.WritePropertyName("boundName"); json.WriteValue(StringWithPrefix(targetInfo.BoundPath));

            if (_slowMode)
            {
                var storage = GlobalVariables.Singleton.Storage;
                var allVariablesOnTarget = storage.AllVariablesOnTarget(targetInfo.targetName);
                if (allVariablesOnTarget.Any())
                {
                    json.WritePropertyName("targetvariables");
                    json.WriteStartArray();
                    foreach (var v in allVariablesOnTarget)
                    {
                        json.WriteStartObject();
                        json.WritePropertyName("name"); json.WriteValue(v);
                        json.WritePropertyName("value");
                        json.WriteStartArray();
                        foreach (var el in storage.VariableOnTarget(v, targetInfo.targetName).Elements)
                            json.WriteValue(el);
                        json.WriteEnd();
                        json.WriteEnd();
                    }
                    json.WriteEnd();
                }
            }
            if (targetInfo.actions.Any())
            {
                json.WritePropertyName("actions");
                json.WriteStartArray();
                foreach (var a in targetInfo.actions)
                    SerializeAction(json, targetInfo, a);
                json.WriteEnd();
            }
            if (targetInfo.dependencies.Any())
            {
                json.WritePropertyName("deps");
                SerializeDeplist(json, targetInfo.dependencies);
            }
            if (targetInfo.includes.Any())
            {
                json.WritePropertyName("inc");
                SerializeDeplist(json, targetInfo.includes);
            }
            if (targetInfo.needs.Any())
            {
                json.WritePropertyName("needs");
                SerializeDeplist(json, targetInfo.needs);
            }

            HashSet<JamTarget> inverseDeps;
            if (m_InverseDependencies.TryGetValue(targetInfo, out inverseDeps))
            {
                json.WritePropertyName("refby");
                json.WriteStartArray();
                foreach (var d in inverseDeps)
                {
                    json.WriteStartObject();
                    json.WritePropertyName("id");
                    json.WriteValue(m_TargetToId[d]);
                    json.WriteEnd();
                }
                json.WriteEnd();
            }

            json.WriteEnd();
        }

        static readonly Dictionary<ActionInvocation, JamTarget> s_SeenActions = new Dictionary<ActionInvocation, JamTarget>();

        void SerializeAction(JsonTextWriter json, JamTarget targetInfo, ActionInvocation arg)
        {
            json.WriteStartObject();
            json.WritePropertyName("name"); json.WriteValue(StringWithPrefix(arg.Action.name));

            // Some actions (e.g. FastCopyDirectory) have massive targets list, and when dumping JSON we keep on outputting
            // the same list over and over again for every target it contains. Making resulting HTML be gigabytes in size,
            // and browsers don't appreciate that. So detect if we have a "long enough" targets list, and we have
            // already seen in, and then in the JSON dump just show that info. Gets something like EditorTempProject
            // dump from 2GB to 7MB.
            var largeTargetsList = arg.Targets.Length > 20;
            var seenAction = s_SeenActions.ContainsKey(arg);

            if (largeTargetsList && seenAction)
            {
                json.WritePropertyName("largeTargetList");
                json.WriteStartObject();
                json.WritePropertyName("seenInID"); json.WriteValue(m_TargetToId[s_SeenActions[arg]]);
                json.WritePropertyName("seenInName"); json.WriteValue(StringWithPrefix(s_SeenActions[arg].targetName));
                json.WritePropertyName("targetListLength"); json.WriteValue(arg.Targets.Length);
                json.WriteEnd();
            }
            else
            {
                if (!seenAction)
                    s_SeenActions.Add(arg, targetInfo);

                json.WritePropertyName("targets");
                json.WriteStartArray();
                foreach (var t in arg.Targets)
                    json.WriteValue(StringWithPrefix(t.targetName));
                json.WriteEnd();
                json.WritePropertyName("flags");
                json.WriteValue(arg.Action.actionFlags.ToString());
                if (_slowMode)
                {
                    json.WritePropertyName("body");
                    json.WriteValue(arg.Action.body);
                }

                if (arg.Callstack.Length > 0)
                {
                    json.WritePropertyName("callstack");
                    json.WriteValue(arg.Callstack);
                }
                if (arg.SecondArgument.Elements.Any())
                {
                    json.WritePropertyName("secondargument");
                    json.WriteStartArray();
                    foreach (var s in arg.SecondArgument.Elements)
                        json.WriteValue(StringWithPrefix(s.Replace("<", "").Replace(">", "")));
                    json.WriteEnd();
                }
            }
            json.WriteEnd();
        }

        void SerializeDeplist(JsonTextWriter json, IEnumerable<Dependency> depList)
        {
            json.WriteStartArray();
            foreach (var d in depList.OrderByDescending(d => RecursiveChildrenOf(d.Target)))
            {
                json.WriteStartObject();
                json.WritePropertyName("id"); json.WriteValue(m_TargetToId[d.Target]);
                if (d.Callstack.Length > 0)
                {
                    json.WritePropertyName("callstack"); json.WriteValue(d.Callstack);
                }
                json.WriteEnd();
            }
            json.WriteEnd();
        }

        int RecursiveChildrenOf(JamTarget target)
        {
            int result = 0;
            if (m_RecursiveChildrenCache.TryGetValue(target, out result))
                return result;
            m_RecursiveChildrenCache[target] = 0;
            result = target.dependencies.Concat(target.includes).Concat(target.needs).Sum(d => RecursiveChildrenOf(d.Target));
            m_RecursiveChildrenCache[target] = result;
            return result;
        }
    }
}
