using System;
using System.Linq;
using JamSharp.Runtime;
using NiceIO;
using Unity.BuildTools;
using System.Collections.Generic;
using System.IO;
using System.Reflection;
using Bee.TestHelpers;
using NUnit.Framework;

namespace Bee.Core.Tests
{
    //This is the baseclass for "I am a thing designed to run tests that code against Bee api's like NativeProgram and make them work using one of our backends".
    //You would expect the build method to take a single Action object that has to point to a static method,  that the implementing backend ends up invoking by loading
    //the assembly that static method lives in and invoking it.  It's a bit more complex than that, because in some cases, like NativeProgram integrationtests, the
    //"code that talks to bee api to test a certain thing", actually takes an argument. In the native program case, we make it take a toolchain object, so we can run
    //native program integrationtests not only against different backends, but also against different toolchains.
    //So we designed the API for this test backend very much geared towards that scenario, where instead of taking a single Action that points to the buildcode under test,
    //we take two.  One Func<T> that will be called first, and that will return something.  and one Action<T> that we call with whatever the first one returned.
    //At the time of this writing, the only usecase of this is the native program tests, and the T that is being passed around is the toolchain.  I've made it generic
    //though as this test class is Bee.Core.Tests, a very lowlevel library, that has no knowledge of what a NativeProgram is, and I'd like to keep it that way.
    //
    //when we start writing more integration tests for other bee api's than NativeProgram, we might have to revise this design a little bit, but we'll tackle that
    //when an actual need arises.

    public abstract class BeeAPITestBackend<T>
    {
        public NPath TestRoot { get; internal set; }
        public abstract string Name { get; }
        public abstract bool RunsOutOfProcess { get; }

        public void Start(string testName = null, int pathBudget = TempDirectory.DefaultPathBudget)
        {
            TestRoot = new TempDirectory(testName, pathBudget);
            TestContext.Out.WriteLine($"TestRoot: {TestRoot}");
        }

        public abstract Shell.ExecuteResult Build(Func<T> firstCallThis, Action<T> thenCallThis, bool throwOnFailure = true, string[] requestedTargets = null);
    }

    public static class BeeTestApiBackendsLocator
    {
        public static IEnumerable<BeeAPITestBackend<T>> AllBeeApiTestBackends<T>()
        {
            return AllGenericBackendTypes.Value.Select(t => (BeeAPITestBackend<T>)Activator.CreateInstance(t.MakeGenericType(typeof(T))));
        }

        static readonly Lazy<Type[]> AllGenericBackendTypes = new Lazy<Type[]>(() => FindAllGenericBackendTypes().ToArray());

        private static Type[] FindAllGenericBackendTypes()
        {
            var assembly = typeof(BeeAPITestBackend<>).Assembly;
            return new NPath(assembly.Location)
                .Parent
                .Files("*.dll")
                .Select(p => Assembly.LoadFrom(p.ToString(SlashMode.Native)))
                .Append(assembly)
                .Distinct()
                .SelectMany(t => t.GetTypes())
                .Where(t => IsAssignableToGenericType(t, typeof(BeeAPITestBackend<>)))
                .Where(t => !t.IsAbstract)
                .ToArray();
        }

        private static bool IsAssignableToGenericType(Type givenType, Type genericType)
        {
            var interfaceTypes = givenType.GetInterfaces();

            foreach (var it in interfaceTypes)
            {
                if (it.IsGenericType && it.GetGenericTypeDefinition() == genericType)
                    return true;
            }

            if (givenType.IsGenericType && givenType.GetGenericTypeDefinition() == genericType)
                return true;

            Type baseType = givenType.BaseType;
            if (baseType == null) return false;

            return IsAssignableToGenericType(baseType, genericType);
        }
    }
}
