using System;

namespace JamSharp.Runtime
{
    public class TemporaryTargetContext : IDisposable
    {
        readonly string m_Target;

        public TemporaryTargetContext() {}

        public TemporaryTargetContext(string target)
        {
            m_Target = target;

            GlobalVariables.Singleton.PushSettingsFor(m_Target);
        }

        public void Dispose()
        {
            if (m_Target != null)
                GlobalVariables.Singleton.PopSettingsFor(m_Target);
        }
    }
}
