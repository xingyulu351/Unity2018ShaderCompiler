using System.Collections.Generic;
using System.Linq;
using System.Text;
using JamSharp.Runtime;

namespace JamSharp.JamState
{
    public class JamTarget
    {
        public string targetName;
        private string _boundPath;
        public string BoundPath
        {
            get
            {
                if (_boundPath != null)
                    return _boundPath;
                _boundPath = new JamList(targetName).GetBoundPath().Elements.Single();
                return _boundPath;
            }
        }

        public bool isFile = true;
        public List<ActionInvocation> actions = new List<ActionInvocation>();
        public readonly List<Dependency> dependencies = new List<Dependency>();
        public readonly List<Dependency> includes = new List<Dependency>();
        public readonly List<Dependency> needs = new List<Dependency>();

        public JamTarget(string target)
        {
            targetName = target;
        }

        public override string ToString()
        {
            return ToString(0, "");
        }

        public string ToString(int depth, string indent)
        {
            var sb = new StringBuilder();

            sb.AppendLine($"{indent}targetName: {targetName}, action: {actions.FirstOrDefault()?.Action?.name} secondArg: {actions.FirstOrDefault()?.SecondArgument}");
            if (depth >= 3)
                return sb.ToString();

            var newIndent = "";
            for (int i = 0; i != depth; i++)
                newIndent = newIndent + " ";

            foreach (var dep in dependencies.Take(5))
                sb.Append(dep.Target.ToString(depth + 1, newIndent + "D "));

            foreach (var need in needs.Take(5))
                sb.Append(need.Target.ToString(depth + 1, newIndent + "N "));

            return sb.ToString();
        }
    }

    public struct Dependency
    {
        public JamTarget Target;
        public string Callstack;
    }

    public class ActionInvocation
    {
        public ActionInfo Action;
        public JamTarget[] Targets;
        public JamList SecondArgument;
        public string Callstack;
    }

    public class ActionInfo
    {
        public string body;
        public string name;
        public ActionsFlags actionFlags;
    }
}
