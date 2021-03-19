using System;

namespace Unity.CommonTools
{
    public class LoggedScope : IDisposable
    {
        private readonly ILog m_Log;
        private readonly string m_ScopeName;

        public LoggedScope(string scopeName)
        {
            m_Log = new ConsoleLog();
            m_ScopeName = scopeName;
            m_Log.Info(string.Format("[blockOpened name='{0}']", m_ScopeName));
        }

        public void Dispose()
        {
            m_Log.Info(string.Format("[blockClosed name='{0}']", m_ScopeName));
        }
    }
}
