using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using JamSharp.Runtime;
using NiceIO;
using Unity.BuildSystem.Common;
using Unity.BuildTools;

namespace JamSharp.JamState
{
    public class JamStateBuilder : IJamStateBuilder
    {
        readonly Dictionary<string, JamTarget> m_NamesToTargets = new Dictionary<string, JamTarget>(50000);
        readonly Dictionary<string, ActionInfo> m_NamesToActions = new Dictionary<string, ActionInfo>();
        readonly string m_CallStackFilter = GlobalVariables.Singleton["CALLSTACKFILTER"].FirstOrDefault();

        public void Depends(params JamList[] values)
        {
            AddDependenciesOrIncludes(values, s => TargetInfoFor(s).dependencies);
        }

        public void Needs(params JamList[] values)
        {
            AddDependenciesOrIncludes(values, s => TargetInfoFor(s).needs);
        }

        public void NotFile(JamList values)
        {
            foreach (var t in TargetInfosFor(values))
                t.isFile = false;
        }

        public void Includes(params JamList[] values)
        {
            AddDependenciesOrIncludes(values, s => TargetInfoFor(s).includes);
        }

        public void InvokeAction(
            string actionName,
            JamList targets,
            JamList secondArguments = null,
            bool allowUnwrittenOutputFiles = false)
        {
            if (!m_NamesToActions.ContainsKey(actionName))
                RegisterAction(actionName, "NOTHING!!!", ActionsFlags.None);

            var actionInfo = m_NamesToActions[actionName];
            if (allowUnwrittenOutputFiles)
                actionInfo.actionFlags |= ActionsFlags.AllowUnwrittenOutputFiles;

            secondArguments = secondArguments ?? new JamList();

            var interestingValues = new[] {targets, secondArguments};
            var callstackContext = interestingValues.SelectMany(j => j.Elements).Append(actionInfo.name);

            var actionInvocation = new ActionInvocation()
            {
                Action = actionInfo,
                SecondArgument = secondArguments.Clone(),
                Targets = targets.Elements.Select(TargetInfoFor).ToArray(),
                Callstack = GetStackTrace(callstackContext),
            };
            foreach (var t in TargetInfosFor(targets))
            {
                t.actions.Add(actionInvocation);
            }
        }

        string GetStackTrace(IEnumerable<string> context)
        {
            if (m_CallStackFilter == null || !context.Any(c => c.Contains(m_CallStackFilter)))
                return string.Empty;

            var trace = new StackTrace(true);

            var summaries = trace.GetFrames().Select(f =>
            {
                // beautify stack trace file paths:
                // - strip unity root,
                // - for other paths (usually from system C# libs) only keep filename,
                // - don't print file/line info for frames that don't have any info
                NPath filePath = f.GetFileName() ?? "";
                if (!filePath.IsRelative && filePath.IsChildOf(Paths.UnityRoot))
                    filePath = filePath.RelativeTo(Paths.UnityRoot);
                else if (filePath.Depth > 0)
                    filePath = filePath.FileName;
                return new StackFrameSummary()
                {
                    MethodDescription =
                        $"{f.GetMethod().DeclaringType}.{f.GetMethod().Name}({f.GetMethod().GetParameters().Select(p => p.ParameterType.Name).SeparateWithComma()})",
                    FileDescription = filePath.Depth > 0 ? $"{filePath}:{f.GetFileLineNumber()}" : ""
                };
            }).ToArray();


            var longest = summaries.Max(s => s.MethodDescription.Length);
            var sb = new StringBuilder();
            foreach (var summary in summaries)
            {
                sb.Append(summary.MethodDescription);
                if (!string.IsNullOrEmpty(summary.FileDescription))
                {
                    for (int i = summary.MethodDescription.Length; i != longest + 10; i++)
                        sb.Append(' ');
                    sb.AppendLine(summary.FileDescription);
                }
                else
                    sb.AppendLine();
            }
            return sb.ToString();
        }

        struct StackFrameSummary
        {
            public string MethodDescription;
            public string FileDescription;
        }

        public void RegisterAction(string actionName, string actionBody, ActionsFlags actionFlags)
        {
            m_NamesToActions[actionName] = new ActionInfo() {body = actionBody, name = actionName, actionFlags = actionFlags};
        }

        void AddDependenciesOrIncludes(JamList[] values, Func<string, List<Dependency>> listSelector)
        {
            var callstack = GetStackTrace(values.SelectMany(v => v.Elements));

            for (int i = 0; i != values.Length - 1; i++)
            {
                var one = values[i];
                var net = values[i + 1];

                foreach (var o in one.Elements)
                    listSelector(o).AddRange(TargetInfosFor(net).Select(t => new Dependency() { Target = t, Callstack = callstack}));
            }
        }

        IEnumerable<JamTarget> TargetInfosFor(JamList value)
        {
            return value.Elements.Select(TargetInfoFor);
        }

        JamTarget TargetInfoFor(string target)
        {
            JamTarget result;
            if (m_NamesToTargets.TryGetValue(target, out result))
                return result;
            result = new JamTarget(target);
            m_NamesToTargets[target] = result;
            return result;
        }

        public void InvokeJamRule(JamList rules, JamList[] values)
        {
            var potentialAction = rules.Elements.FirstOrDefault();
            if (potentialAction == null)
                return;

            if (m_NamesToActions.ContainsKey(potentialAction))
                throw new ArgumentException($"The following action: {potentialAction} is being invoked through {nameof(InvokeJamRule)} instead of {nameof(BuiltinRules.InvokeJamAction)}, please fix");
        }

        public JamState Build()
        {
            return new JamState(m_NamesToTargets.Values, m_NamesToActions.Values);
        }
    }
}
