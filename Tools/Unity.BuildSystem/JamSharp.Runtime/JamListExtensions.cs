using System;
using System.Collections.Generic;
using System.Linq;
using NiceIO;

namespace JamSharp.Runtime
{
    public static class JamListExtensions
    {
        public static JamList[] On(this string s, JamList targets)
        {
            return GlobalVariables.Singleton.GetOrCreateVariableOnTargetContext(targets, new JamList(s));
        }

        public static JamList Dereference(this string s)
        {
            return GlobalVariables.Singleton.DereferenceElements(new[] { s });
        }

        public static JamList[] On(this JamList variables, JamList targets)
        {
            return GlobalVariables.Singleton.GetOrCreateVariableOnTargetContext(targets, variables);
        }

        public static void Assign(this JamList[] jamlists)
        {
            foreach (var jamlist in jamlists)
                jamlist.Assign();
        }

        public static void Assign(this JamList[] jamlists, string value)
        {
            JamList jValue = new JamList(value);
            foreach (var jamlist in jamlists)
                jamlist.Assign(jValue);
        }

        public static void Assign(this JamList[] jamlists, JamList value)
        {
            foreach (var jamlist in jamlists)
                jamlist.Assign(value);
        }

        public static void Assign(this JamList[] jamlists, params string[] values)
        {
            var jamvalues = new JamList(values);
            foreach (var jamlist in jamlists)
                jamlist.Assign(jamvalues);
        }

        public static void Assign(this JamList[] jamlists, params JamList[] values)
        {
            foreach (var jamlist in jamlists)
                jamlist.Assign(values);
        }

        public static void Assign(this JamList[] jamlists, IEnumerable<JamTarget> values)
        {
            foreach (var jamlist in jamlists)
                jamlist.Assign(values.Select(v => v.JamTargetName));
        }

        public static void Append(this JamList[] jamlists, IEnumerable<string> values)
        {
            var valuesArray = values.ToArray();
            foreach (var jamlist in jamlists)
                jamlist.Append(valuesArray);
        }

        public static void Append(this JamList[] jamlists, IEnumerable<NPath> values)
        {
            foreach (var jamlist in jamlists)
                foreach (var value in values)
                    jamlist.Append(value);
        }

        public static void Append(this JamList[] jamlists, JamList value)
        {
            foreach (var jamlist in jamlists)
                jamlist.Append(value);
        }

        public static void Append(this JamList[] jamlists, params JamList[] values)
        {
            foreach (var jamlist in jamlists)
                jamlist.Append(values);
        }

        public static void Subtract(this JamList[] jamlists, params JamList[] values)
        {
            foreach (var jamlist in jamlists)
                jamlist.Subtract(values);
        }

        public static void AssignIfEmpty(this JamList[] jamlists, params string[] values)
        {
            foreach (var jamlist in jamlists)
                jamlist.AssignIfEmpty(values);
        }

        public static void AssignIfEmpty(this JamList[] jamlists, params JamList[] values)
        {
            foreach (var jamlist in jamlists)
                jamlist.AssignIfEmpty(values);
        }

        public static JamList Clone(this JamList list)
        {
            if (list == null)
                return new JamList();

            return new JamList(list.Elements);
        }

        public static string[][] ToLOL(this JamList[] jamlists)
        {
            return Array.ConvertAll(jamlists, i => i == null
                ? new string[0]
                : i.Elements).ToArray();
        }

        public static JamList ToJamList(this IEnumerable<NPath> self)
        {
            return new JamList(self);
        }
    }
}
