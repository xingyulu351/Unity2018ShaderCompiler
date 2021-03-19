using System;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using Unity.BuildTools;

namespace JamSharp.Runtime
{
    [Flags]
    public enum ActionsFlags
    {
        None = 0,
        Updated = 1 << 0,
        Together = 1 << 1,
        Quietly = 1 << 3,
        Piecemeal = 1 << 4,
        Existing = 1 << 5,
        Response = 1 << 6,
        MaxTargets = 1 << 9,
        RemoveEmptyDirs = 1 << 12,
        AllowUnwrittenOutputFiles = 1 << 13
    }

    public class JamCore
    {
        public static string[] InvokeRule(string rulename, string[][] param, int dummy = 0)
        {
            if (param == null)
                throw new ArgumentNullException(nameof(param));

            if (Rules.HasRule(rulename))
            {
                JamList res = Rules.Invoke(rulename, param.Select(p => new JamList(p)).ToArray());
                return res.Elements;
            }

            if (rulename == "MightNotUpdate" || rulename == "NoCare")
                return Array.Empty<string>();

            throw new NotSupportedException($"There are no native rules anymore: {rulename}");
        }

        public static void RegisterAction(string name, string actions, ActionsFlags flags = ActionsFlags.None, int maxTargets = 0, int maxLines = 0)
        {
            //jam seems to tack on a newline at beginning and end.   lets do the same so it's easy to compare
            actions = Environment.NewLine + actions + Environment.NewLine;

            RuntimeManager.ParallelJamStateBuilder?.RegisterAction(name, actions, flags);
        }

        public static void ClearTimestampCaches() => throw new NotImplementedException();
        public static  void SetVar(string name, Operator @operator, string[] value) => throw new NotImplementedException();
        public static void SetVarBatch(string[] names, Operator @operator, string[][] values) => throw new NotImplementedException();
        public static string[] GetVar(string name) => throw new NotImplementedException();
        public static string[][] GetVarBatch(string[] names) => throw new NotImplementedException();
        public static void SetSettingBatch(StringTableEntry[][] names, StringTableEntry[] targets, Operator @operator, StringTableEntry[][][] values, string[] stringTableStringArray) => throw new NotImplementedException();
        static void RegisterRuleInternal(string name, object callback) => throw new NotImplementedException();
        public static  string[] Expand(string name) => throw new NotImplementedException();

        public static string[] RulesRegisteredOnNativeSide =
        {
            "JamSharpRuntime_FinishedJamExecution",
            "JamSharpRuntime_FinishedBindingPhase",
            "JamSharpRuntime_BuildWithBee",
            "JamSharpIncludeNotification",
            "Always",
            "Depends",
            "ExpandFileList",
            "Includes",
            "Leaves",
            "ForceCare",
            "NoCare",
            "NotFile",
            "NoUpdate",
            "Temporary",
            "UseDepCache",
            "UseCommandLine",
            "MightNotUpdate",
            "Needs",
            "Split",
            "DependsList",
            "RuleExists",
            "GetAllVariableNames",
            "GetAllVariablesOnTarget",
        };

        public static void RegisterRule(string name, Func<string[][], string[]> callback)
        {
            if (!RulesRegisteredOnNativeSide.Contains(name))
            {
                Func<string[][], string[]> managed = args =>
                {
                    return callback(args);
                };
                Rules.Register(name, args => new JamList(managed(args.ToLOL())));
            }
        }
    }

    [StructLayout(LayoutKind.Explicit)]
    public struct StringTableEntry
    {
        [FieldOffset(0)]
        internal int index;
    }

    public enum Operator
    {
        VAR_SET = 0, /* override previous value */
        VAR_APPEND = 1, /* append to previous value */
    }
}
