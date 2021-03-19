using System;
using System.Collections.Generic;
using System.Linq;
using Unity.BuildTools;

namespace JamSharp.Runtime
{
    public static class Rules
    {
        static Dictionary<string, Func<JamList[], JamList>> s_Rules = new Dictionary<string, Func<JamList[], JamList>>();
        static List<string> s_Actions = new List<string>();

        public static string[] OriginalJamplusBuiltinRules = new string[]
        {
            "Always",
            "Depends",
            "Includes",
            "Leaves",
            "Match",
            "ForceCare",
            "NoCare",
            "NotFile",
            "NoUpdate",
            "Subst",
            "Temporary",
            "W32_GETREG",
            "UseDepCache",
            "UseCommandLine",
            "MightNotUpdate",
            "Needs",
            "Split",
            "ExpandFileList",
            "DependsList",
            "RuleExists",
            "GetAllVariableNames",
            "GetAllVariablesOnTarget",
        };

        static readonly HashSet<string> s_InvalidlyDynamicallyInvokedRules = new HashSet<string>();

        public static bool HasRule(string name)
        {
            return s_Rules.ContainsKey(name) || s_Actions.Contains(name);
        }

        public static void Register(string ruleName, Func<JamList[], JamList> rule)
        {
            s_Rules[ruleName] = rule;
        }

        public static void Register(string ruleName, Action<JamList[]> rule)
        {
            s_Rules[ruleName] = args => { rule(args); return null; };
        }

        public static JamList Invoke(string ruleName, JamList[] args)
        {
            if (HasRule(ruleName))
            {
                if (s_Rules.TryGetValue(ruleName, out var rule))
                {
                    if (!IsValidDynamicRule(ruleName))
                    {
                        s_InvalidlyDynamicallyInvokedRules.Add(ruleName);
                    }

                    return rule(args) ?? new JamList();
                }
                throw new ArgumentException($"Unknown rule: {ruleName}");
            }

            throw new Exception($"Rule '{ruleName}' not found.");
        }

        static string[] s_ValidDynamicRules = new string[]
        {
            "C.AutoDetect",
            "C.FDefines",
            "C.FIncludes",
            "C.FSysIncludes",
            "C.LinkFrameworks",
            "C.ObjectCFlags",
            "ConvertFile",
            "C.FLibraryPaths",
            "C.ObjectC++Flags",
            "C.IntermediatePath",
            "C.Link",
            "C.WholeLibrary",
            "C.WholeLibraryFromObjectsAndLibs",
            "C.CreateMRIResponseFile",
            "MainJamFileStarting",
        };

        public static bool IsValidDynamicRule(string ruleName)
        {
            if (s_ValidDynamicRules.Contains(ruleName) || ruleName.Contains("_Report") || ruleName.Contains("_OverrideModule") || ruleName.StartsWith("C._"))
                return true;
            return false;
        }

        public static void Cleanup()
        {
            if (s_InvalidlyDynamicallyInvokedRules.Any())
                throw new Exception("You are trying to dynamically invoke the following jam rules. This is not the end of the world, however in order for them to not fail the build," +
                    "You need to inform the jamsharp converter about them, by adding them to s_ValidDynamicRules in Tools/Build/JamSharp/JamSharp.Runtime/Rules.cs The following rules were illegally dynamically invoked: " + s_InvalidlyDynamicallyInvokedRules.SeparateWithSpace());
        }

        /*
        static readonly string[] s_AllowedDuplicateActions =
        {
            //investigate:
            "BuildUnityProject",
            "Bindings",
            "libtoolEverythingTogether",
            "MkDir1",
            "_RmTemps",
            "Test",
            "InstallName",
            "InfoPList",
        };
        */

        static void RegisterBuiltins()
        {
            Register("Match", args => BuiltinRules.Match(args[0], args[1]));
            Register("W32_GETREG", args => BuiltinRules.W32_GETREG(args[0]));
        }

        static Rules()
        {
            RegisterBuiltins();
        }
    }
}
