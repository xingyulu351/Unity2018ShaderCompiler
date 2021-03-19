using System;
using System.Reflection;
using Bee.Core;
using Bee.Tools;
using NiceIO;
using Unity.BuildTools;

namespace Bee.Tests
{
    public static class BeeBackend_build
    {
        public static void Main()
        {
            var args = new NPath("serialized_function_of_buildcode_under_test").MakeAbsolute().ReadAllLines();

            var toolChain = Invoke(GetMethodFromNames(args[0], args[1], args[2]));
            Invoke(GetMethodFromNames(args[3], args[4], args[5]), toolChain);

            Backend.Current.RegisterFileInfluencingGraph(args[0]);
        }

        private static object Invoke(MethodInfo testBuildCode, params object[] parameters)
        {
            var instance2 = testBuildCode.IsStatic ? null : System.Activator.CreateInstance(testBuildCode.DeclaringType);
            return testBuildCode.Invoke(instance2, parameters);
        }

        private static MethodInfo GetMethodFromNames(NPath assembly, string typeName, string staticMethod)
        {
            var a = Assembly.LoadFile(assembly.ToString(SlashMode.Native));

            var type = a.GetType(typeName);
            if (type == null)
                throw new ArgumentException($"Cannot find {typeName}");

            var method = type.GetMethod(staticMethod, BindingFlags.NonPublic | BindingFlags.Static | BindingFlags.Instance);
            if (method == null)
                throw new ArgumentException($"Cannot find method {staticMethod}");
            return method;
        }
    }
}
