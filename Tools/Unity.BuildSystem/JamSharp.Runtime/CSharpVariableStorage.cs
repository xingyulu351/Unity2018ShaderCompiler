using System;
using System.Collections.Generic;
using System.Linq;

namespace JamSharp.Runtime
{
    public class CSharpVariableStorage
    {
        readonly Stack<Dictionary<string, JamList>> m_Backups = new Stack<Dictionary<string, JamList>>();
        readonly Dictionary<string, JamList> m_Globals = new Dictionary<string, JamList>(1000);
        readonly Dictionary<string, Dictionary<string, JamList>> m_TargetVariables = new Dictionary<string, Dictionary<string, JamList>>(80000);

        public string[] AllGlobalVariables => m_Globals.Keys.ToArray();
        public string[] AllTargets => m_TargetVariables.Keys.ToArray();

        public string[] AllVariablesOnTarget(string target)
        {
            Dictionary<string, JamList> variables;
            if (!m_TargetVariables.TryGetValue(target, out variables))
                return new string[0];

            return variables.Keys.ToArray();
        }

        public JamList GlobalVariable(string name)
        {
            return GetValueFromVariables(m_Globals, name);
        }

        public Dictionary<string, JamList> AllOnTargetVariablesOn(string target)
        {
            Dictionary<string, JamList> variables;
            if (m_TargetVariables.TryGetValue(target, out variables))
                return variables;
            variables = new Dictionary<string, JamList>();
            m_TargetVariables.Add(target, variables);
            return variables;
        }

        static JamList GetValueFromVariables(Dictionary<string, JamList> variables, string name)
        {
            JamList result;
            if (variables.TryGetValue(name, out result))
                return result;

            result = new JamList();
            variables.Add(name, result);
            return result;
        }

        public JamList VariableOnTarget(string name, string target)
        {
            return GetValueFromVariables(AllOnTargetVariablesOn(target), name);
        }

        public void PushSettingsFor(string target)
        {
            Dictionary<string, JamList> targetVariables;
            m_TargetVariables.TryGetValue(target, out targetVariables);

            var backup = new Dictionary<string, JamList>();
            m_Backups.Push(backup);

            if (targetVariables == null)
                return;

            foreach (var varName in targetVariables.Keys)
            {
                JamList l;
                if (!m_Globals.TryGetValue(varName, out l))
                    l = new JamList();

                backup.Add(varName, new JamList(l.Elements));

                GlobalVariable(varName).Assign(targetVariables[varName]);
            }
        }

        public void PopSettingsFor(string target)
        {
            var variables = m_Backups.Pop();

            foreach (var varName in variables.Keys)
            {
                m_Globals[varName].Assign(variables[varName]);
            }
        }

        public string[][] GetGlobalVariables(string[] names)
        {
            var values = new string[names.Length][];

            for (var i = 0; i < names.Length; i++)
            {
                values[i] = GlobalVariable(names[i]).Elements;
            }

            return values;
        }

        public void SetGlobalVariables(string[] names, string[][] values)
        {
            if (names.Length != values.Length)
            {
                throw new Exception("Names and Values must have the same length!!");
            }

            for (var i = 0; i < names.Length; i++)
            {
                GlobalVariable(names[i]).Assign(new JamList(values[i]));
            }
        }
    }
}
