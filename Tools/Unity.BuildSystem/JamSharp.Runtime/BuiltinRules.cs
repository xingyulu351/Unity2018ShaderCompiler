using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using Bee.Tools;
using Microsoft.Win32;
using NiceIO;
using Unity.TinyProfiling;

namespace JamSharp.Runtime
{
    public static class BuiltinRules
    {
        static readonly StringBuilder k_StringBuilder = new StringBuilder();
        static readonly byte[] s_Md5ItemSeparator = new byte[] { 0 };


        public static void Echo(params JamList[] values)
        {
            if (values.Length > 0)
                Console.Write(values[0].ToString());
            Console.Write('\n');
        }

        public static void Depends(params JamList[] things)
        {
            RuntimeManager.ParallelJamStateBuilder?.Depends(things);
        }

        public static void Needs(params JamList[] things)
        {
            RuntimeManager.ParallelJamStateBuilder?.Needs(things);
        }

        public static void Includes(JamList theseThings, JamList includeTheseThings)
        {
            RuntimeManager.ParallelJamStateBuilder?.Includes(theseThings, includeTheseThings);
        }

        public static void IncludesParallelJamStateOnly(JamList theseThings, JamList includeTheseThings)
        {
            RuntimeManager.ParallelJamStateBuilder?.Includes(theseThings, includeTheseThings);
        }

        public static void NotFile(JamList targets)
        {
            RuntimeManager.ParallelJamStateBuilder?.NotFile(targets);
        }

        public static void NotFile(params string[] targets)
        {
            NotFile(new JamList(targets));
        }

        public static void NoCare(JamList targets)
        {
            var mappedTargets = new JamList(targets.Elements.Select(e => e.EndsWith(".jam") ? e + ".cs" : e));
            InvokeJamRule("NoCare", mappedTargets);
        }

        public static JamList W32_GETREG(JamList key)
        {
            JamList result;
            var path = String.Join(@"\", key.Elements.Take(key.Elements.Length - 1));
            var name = key.Elements.Last();
            var value = Registry.GetValue(path, name, null);

            if (value == null)
                result = new JamList();
            else if (value is string[])
                result = new JamList();
            else if (value is byte[])
                result = new JamList();
            else
                result = new JamList(value.ToString());

            return result;
        }

        public static JamList MD5(JamList args)
        {
            var md5 = System.Security.Cryptography.MD5.Create();
            var addItemSeparator = false;

            foreach (var arg in args)
            {
                if (addItemSeparator)
                    md5.TransformBlock(s_Md5ItemSeparator, 0, s_Md5ItemSeparator.Length, null, 0);
                var data = Encoding.UTF8.GetBytes(arg);
                md5.TransformBlock(data, 0, data.Length, null, 0);
                addItemSeparator = true;
            }
            md5.TransformFinalBlock(Array.Empty<byte>(), 0, 0);
            return md5.Hash.ToHexString();
        }

        public static JamList RuleExists(JamList input)
        {
            if (input.Elements.Length > 0)
            {
                var ruleName = input.Elements[0];
                if (Rules.HasRule(ruleName) || JamCore.RulesRegisteredOnNativeSide.Contains(ruleName))
                    return new JamList("true");

                if (Rules.OriginalJamplusBuiltinRules.Contains(ruleName))
                    throw new Exception("Not registered builtin: " + ruleName);
            }

            return new JamList();
        }

        public static JamList Match(JamList REGEXPS, JamList LIST)
        {
            using (TinyProfiler.Section("Match", LIST.ToString()))
            {
                var result = new List<string>();
                foreach (var pattern in REGEXPS)
                {
                    var regex = new Regex(JamList.ConvertJamRegexToDotNet(pattern));
                    foreach (var entry in LIST)
                    {
                        foreach (Match match in regex.Matches(entry))
                            foreach (Group group in match.Groups.Cast<Group>().Skip(1))
                                if (@group.Value != "")
                                    result.Add(@group.Value);
                    }
                }

                return new JamList(result);
            }
        }

        public static JamList ExpandFileList(JamList files)
        {
            var searchSourceVar = GlobalVariables.Singleton.Storage.GlobalVariable("SEARCH_SOURCE");
            var searchSource = new NPath(searchSourceVar.Count > 0 ? searchSourceVar.Single() : "");

            var result = new List<string>();
            foreach (var file in files)
            {
                if (file.Contains('*') || file.Contains('?'))
                {
                    var globbed = Glob.Search(file.TrimEnd(new char[] { '/', '\\' }));
                    result.AddRange(globbed.Select(g => new NPath(g).RelativeTo(searchSource).ToString()));
                }
                else
                {
                    result.Add(file);
                }
            }

            return new JamList(result);
        }

        public static JamList InvokeJamRule(JamList rules, params JamList[] values)
        {
            RuntimeManager.ParallelJamStateBuilder?.InvokeJamRule(rules, values);


            var jamListArrayToLol = values.ToLOL();
            return new JamList(rules.Elements.SelectMany(rule => JamCore.InvokeRule(rule, jamListArrayToLol)));
        }

        public static void InvokeJamAction(
            string actionName,
            JamList targets,
            JamList sources = null,
            bool allowUnwrittenOutputFiles = false)
        {
            RuntimeManager.ParallelJamStateBuilder?.InvokeAction(
                actionName,
                targets,
                sources,
                allowUnwrittenOutputFiles: allowUnwrittenOutputFiles);
        }

        [DebuggerStepThrough]
        public static JamList Combine(params JamList[] values)
        {
            string[][] valueElements = values.Select(v => v.Elements).ToArray();

            k_StringBuilder.Clear();

            foreach (var inputList in valueElements)
            {
                if (inputList.Length != 1)
                    return new JamList(valueElements.CartesianProduct().Select(inputs => String.Join(String.Empty, inputs)).ToArray());
                k_StringBuilder.Append(inputList[0]);
            }
            return new JamList(k_StringBuilder.ToString());
        }

        public static void SetAllowedOutputSubstringsFor(JamList targets, string[] allowedOutputSubstrings)
        {
            "ALLOWED_OUTPUT_SUBSTRINGS".On(targets).Assign(allowedOutputSubstrings);
        }

        public static void DisallowUnexpectedOutputFor(JamList targets)
        {
            "ALLOW_UNEXPECTED_OUTPUT".On(targets).Assign("0");
        }
    }
}
