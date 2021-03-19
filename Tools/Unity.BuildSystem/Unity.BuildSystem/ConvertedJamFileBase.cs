using System;
using System.Diagnostics;
using System.Linq;
using System.Reflection;
using Action = System.Action;

namespace JamSharp.Runtime
{
    public class ConvertedJamFileBase
    {
        public static void InitializeAllConvertedJamFiles()
        {
            if (!RuntimeManager.Initialized)
                RuntimeManager.Initialize();
        }

        public static void RegisterRuleWithReturnValue(string rulename, Func<JamList> callback)
        {
            RegisterRule(rulename, callback.Method);
        }

        public static void RegisterRuleWithReturnValue(string rulename, Func<JamList, JamList> callback)
        {
            RegisterRule(rulename, callback.Method);
        }

        public static void RegisterRuleWithReturnValue(string rulename, Func<JamList, JamList, JamList> callback)
        {
            RegisterRule(rulename, callback.Method);
        }

        public static void RegisterRuleWithReturnValue(string rulename, Func<JamList, JamList, JamList, JamList> callback)
        {
            RegisterRule(rulename, callback.Method);
        }

        public static void RegisterRuleWithReturnValue(string rulename, Func<JamList, JamList, JamList, JamList, JamList> callback)
        {
            RegisterRule(rulename, callback.Method);
        }

        public static void RegisterRuleWithReturnValue(string rulename, Func<JamList, JamList, JamList, JamList, JamList, JamList> callback)
        {
            RegisterRule(rulename, callback.Method);
        }

        public static void RegisterRuleWithReturnValue(string rulename, Func<JamList, JamList, JamList, JamList, JamList, JamList, JamList> callback)
        {
            RegisterRule(rulename, callback.Method);
        }

        public static void RegisterRuleWithReturnValue(string rulename, Func<JamList, JamList, JamList, JamList, JamList, JamList, JamList, JamList> callback)
        {
            RegisterRule(rulename, callback.Method);
        }

        public static void RegisterRuleWithReturnValue(string rulename, Func<JamList, JamList, JamList, JamList, JamList, JamList, JamList, JamList, JamList> callback)
        {
            RegisterRule(rulename, callback.Method);
        }

        public static void RegisterRuleWithReturnValue(string rulename, Func<JamList, JamList, JamList, JamList, JamList, JamList, JamList, JamList, JamList, JamList> callback)
        {
            RegisterRule(rulename, callback.Method);
        }

        public static void RegisterRuleWithReturnValue(string rulename, Func<JamList, JamList, JamList, JamList, JamList, JamList, JamList, JamList, JamList, JamList, JamList> callback)
        {
            RegisterRule(rulename, callback.Method);
        }

        public static void RegisterRule(string rulename, Action callback)
        {
            RegisterRule(rulename, callback.Method);
        }

        public static void RegisterRule(string rulename, Action<JamList> callback)
        {
            RegisterRule(rulename, callback.Method);
        }

        public static void RegisterRule(string rulename, Action<JamList, JamList> callback)
        {
            RegisterRule(rulename, callback.Method);
        }

        public static void RegisterRule(string rulename, Action<JamList, JamList, JamList> callback)
        {
            RegisterRule(rulename, callback.Method);
        }

        public static void RegisterRule(string rulename, Action<JamList, JamList, JamList, JamList> callback)
        {
            RegisterRule(rulename, callback.Method);
        }

        public static void RegisterRule(string rulename, Action<JamList, JamList, JamList, JamList, JamList> callback)
        {
            RegisterRule(rulename, callback.Method);
        }

        public static void RegisterRule(string rulename, Action<JamList, JamList, JamList, JamList, JamList, JamList> callback)
        {
            RegisterRule(rulename, callback.Method);
        }

        public static void RegisterRule(string rulename, Action<JamList, JamList, JamList, JamList, JamList, JamList, JamList> callback)
        {
            RegisterRule(rulename, callback.Method);
        }

        public static void RegisterRule(string rulename, Action<JamList, JamList, JamList, JamList, JamList, JamList, JamList, JamList> callback)
        {
            RegisterRule(rulename, callback.Method);
        }

        public static void RegisterRule(string rulename, Action<JamList, JamList, JamList, JamList, JamList, JamList, JamList, JamList, JamList> callback)
        {
            RegisterRule(rulename, callback.Method);
        }

        public static void RegisterRule(string rulename, Action<JamList, JamList, JamList, JamList, JamList, JamList, JamList, JamList, JamList, JamList> callback)
        {
            RegisterRule(rulename, callback.Method);
        }

        public static void RegisterRule(string rulename, MethodInfo callback)
        {
            Func<string[][], string[]> d = jamLists =>
            {
                if (RuntimeManager.Finished)
                    throw new Exception($"Rule {rulename} invoked while we were finished");

                var targetArguments = new object[callback.GetParameters().Length];
                for (int i = 0; i != targetArguments.Length; i++)
                {
                    targetArguments[i] = jamLists.Length > i
                        ? new JamList(jamLists[i])
                        : new JamList();
                }

                object result = callback.Invoke(null, targetArguments);

                if (result == null)
                    return new string[0];
                return ((JamList)result).Elements.ToArray();
            };

            JamCore.RegisterRule(rulename, d);
        }

        [DebuggerStepThrough]
        public static JamList JamList(params JamList[] elements)
        {
            return new JamList(elements);
        }

        [DebuggerStepThrough]
        public static JamList JamList(string singleElement)
        {
            return new JamList(singleElement);
        }

        [DebuggerStepThrough]
        public static JamList JamList()
        {
            return new JamList();
        }

        public static string SwitchTokenFor(JamList input)
        {
            return input.Elements.FirstOrDefault() ?? "";
        }

        public static IDisposable OnTargetContext(JamList targetName)
        {
            return targetName.Elements.Any()
                ? new TemporaryTargetContext(targetName.Elements.First())
                : new TemporaryTargetContext();
        }

        public static T OnTarget<T>(JamList targetName, Func<T> action)
        {
            using (OnTargetContext(targetName))
                return action();
        }

        public static void OnTarget(JamList targetName, Action action)
        {
            using (OnTargetContext(targetName))
                action();
        }
    }
}
