using System.Collections.Generic;

namespace JamSharp.JamState
{
    public class JamState
    {
        readonly Dictionary<string, JamTarget> m_TargetInfos = new Dictionary<string, JamTarget>(50000);
        readonly Dictionary<string, ActionInfo> m_NamesToActions = new Dictionary<string, ActionInfo>();

        public IEnumerable<JamTarget> Targets => m_TargetInfos.Values;

        public JamState()
        {
        }

        internal JamState(IEnumerable<JamTarget> targets, IEnumerable<ActionInfo> actions)
        {
            foreach (var target in targets)
                m_TargetInfos.Add(target.targetName, target);
            foreach (var action in actions)
                m_NamesToActions.Add(action.name, action);
        }

        public JamTarget GetTarget(string targetName)
        {
            JamTarget result;
            m_TargetInfos.TryGetValue(targetName, out result);
            return result;
        }
    }
}
