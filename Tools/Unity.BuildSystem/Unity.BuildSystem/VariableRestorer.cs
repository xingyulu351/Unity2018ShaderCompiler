using System;
using System.Collections.Generic;

namespace JamSharp.Runtime
{
    public class VariableRestorer : IDisposable
    {
        readonly HashSet<string> m_BackedupNames = new HashSet<string>();
        readonly Dictionary<JamList, string[]> m_Backups = new Dictionary<JamList, string[]>();

        public void Dispose()
        {
            foreach (var kvp in m_Backups)
                kvp.Key.Assign(kvp.Value);
        }

        public void RestoreAfterFunction(string variableName)
        {
            if (m_BackedupNames.Contains(variableName))
                return;

            m_BackedupNames.Add(variableName);
            var jamList = GlobalVariables.Singleton[variableName];
            m_Backups[jamList] = jamList.Elements;
        }
    }
}
