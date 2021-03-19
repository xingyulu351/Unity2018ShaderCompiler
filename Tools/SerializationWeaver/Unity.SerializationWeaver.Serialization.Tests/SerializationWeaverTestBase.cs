using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using Mono.Cecil;
using Moq;
using NUnit.Framework;
using UnityEngine;

namespace Unity.SerializationWeaver.Serialization.Tests
{
    public class SerializationWeaverTestBase
    {
        [SetUp]
        public void SetupFixture()
        {
            AppDomain.CurrentDomain.AssemblyResolve += ResolveEventHandler;
        }

        [TearDown]
        public void TearDown()
        {
            AppDomain.CurrentDomain.AssemblyResolve -= ResolveEventHandler;
        }

        private static Assembly ResolveEventHandler(object sender, ResolveEventArgs args)
        {
            throw new NotImplementedException();
        }

        private static string UnityEngineDLLPath
        {
            get { return typeof(IUnitySerializable).Assembly.Location; }
        }

        protected static bool Is<T>(object value)
        {
            return value != null && value.GetType().FullName == typeof(T).FullName;
        }

        protected static void RunTest<TFixtureType, TFixtureInterface, TMockInterface, TInstanceHolder>(Action<TypeDefinition> actions, Func<Type, MethodInfo> testMethodBuilder, Action<TFixtureInterface> setupFixtureInstance, Action<Mock<TMockInterface>, TFixtureInterface> setupMock)
            where TFixtureType : class
            where TMockInterface : class
        {
            var tempAssembly = ApplyActionsTo<TFixtureType>(actions);

            var fixtureType = tempAssembly.GetType(typeof(TFixtureType).FullName);
            var fixtureInstance = Activator.CreateInstance(fixtureType);

            InitializeFixtureInstance(fixtureInstance, setupFixtureInstance);

            var mock = MockFor<TMockInterface>();

            setupMock(mock, (TFixtureInterface)fixtureInstance);

            InitializeStaticInstanceWith<TMockInterface, TInstanceHolder>(mock.Object);
            InvokeTestMethod(testMethodBuilder, fixtureType, fixtureInstance);

            mock.VerifyAll();
        }

        protected static void RunTest<TFixtureType, TMockInterface, TInstanceHolder>(Action<TypeDefinition> actions, Func<Type, MethodInfo> testMethodBuilder, Func<Dictionary<string, object>> getInitialFieldValues, Action<Mock<TMockInterface>, object> setupMock)
            where TMockInterface : class
        {
            var tempAssembly = ApplyActionsTo<TFixtureType>(actions);

            var fixtureType = tempAssembly.GetType(typeof(TFixtureType).FullName);
            var fixtureInstance = Activator.CreateInstance(fixtureType);

            InitializeFixtureInstance<TFixtureType>(fixtureInstance, fixtureType, getInitialFieldValues);

            var mock = MockFor<TMockInterface>();

            setupMock(mock, fixtureInstance);

            InitializeStaticInstanceWith<TMockInterface, TInstanceHolder>(mock.Object);
            InvokeTestMethod(testMethodBuilder, fixtureType, fixtureInstance);

            mock.VerifyAll();
        }

        private static void InitializeFixtureInstance<TFixtureInterface>(object fixtureInstance, Action<TFixtureInterface> setupFixtureInstance)
        {
            setupFixtureInstance((TFixtureInterface)fixtureInstance);
        }

        private static void InitializeFixtureInstance<TFixtureType>(object fixtureInstance, Type fixtureType, Func<Dictionary<string, object>> getInitialFieldValues)
        {
            foreach (var fieldValue in getInitialFieldValues())
            {
                var fieldInfo = fixtureType.GetField(fieldValue.Key, BindingFlags.Public | BindingFlags.Static | BindingFlags.NonPublic | BindingFlags.Instance);
                if (fieldInfo == null)
                    throw new MissingFieldException("Expected " + fieldValue.Key + " field not found in " + typeof(TFixtureType).FullName);

                fieldInfo.SetValue(fixtureInstance, fieldValue.Value);
            }
        }

        private static Mock<TMockInterface> MockFor<TMockInterface>() where TMockInterface : class
        {
            return new Mock<TMockInterface>(MockBehavior.Strict);
        }

        private static void InvokeTestMethod(Func<Type, MethodInfo> testMethodBuilder, Type fixtureType, object fixtureInstance)
        {
            var testMethod = testMethodBuilder(fixtureType);
            if (testMethod == null)
                throw new NotSupportedException("Invalid fixture type. Expecting a Unity_LivenessAnalysis method to be defined!");

            if (testMethod.GetParameters().Length == 1)
                testMethod.Invoke(fixtureInstance, new object[1] { 1 });
            else
                testMethod.Invoke(fixtureInstance, new object[2] { 1, false });
        }

        private static void InitializeStaticInstanceWith<TMockInterface, TInstanceHolder>(TMockInterface value) where TMockInterface : class
        {
            var instanceField = typeof(TInstanceHolder).GetField("Instance", BindingFlags.Static | BindingFlags.Public);
            if (instanceField == null)
                throw new NotSupportedException(typeof(TInstanceHolder).FullName + " doesn't have a public Instance static field!");

            instanceField.SetValue(null, value);
        }

        private static Assembly ApplyActionsTo<TFixtureType>(Action<TypeDefinition> actions)
        {
            var assemblyDefinition = LoadFixturesAssembly<TFixtureType>();

            // collect all first, as otherwise we get exception after the first item in a loop (state change)
            var types = new List<TypeDefinition>(assemblyDefinition.MainModule.Types.Where(t => t.FullName == typeof(TFixtureType).FullName));
            foreach (var typeDefinition in types)
                actions(typeDefinition);

            var tempAssemblyDir = TempAssemblyPath();
            var tempAssemblyPath = Path.Combine(tempAssemblyDir, "Temp.dll");

            // Note: a random name will allow us to skip the assembly cache if we run multiple tests
            assemblyDefinition.Name = NewRandomAssemblyName();
            assemblyDefinition.Write(tempAssemblyPath);

            var tempAssembly = Assembly.LoadFrom(tempAssemblyPath);
            return tempAssembly;
        }

        private static AssemblyNameDefinition NewRandomAssemblyName()
        {
            return new AssemblyNameDefinition(Path.GetFileNameWithoutExtension(Path.GetTempFileName()), new Version());
        }

        protected static AssemblyDefinition UnityEngineDLLAssembly()
        {
            return LoadAssembly(UnityEngineDLLPath);
        }

        private static AssemblyDefinition LoadFixturesAssembly<T>()
        {
            return LoadAssembly(typeof(T).Assembly.Location, Path.GetDirectoryName(UnityEngineDLLPath));
        }

        private static AssemblyDefinition LoadAssembly(string path, params string[] assemblySearchDir)
        {
            var resolver = new DefaultAssemblyResolver();
            resolver.AddSearchDirectory(Path.GetDirectoryName(path));

            foreach (var dir in assemblySearchDir)
                resolver.AddSearchDirectory(dir);

            return AssemblyDefinition.ReadAssembly(
                path,
                new ReaderParameters
                {
                    AssemblyResolver = resolver
                });
        }

        private static string TempAssemblyPath()
        {
            var tempFile = Path.GetTempFileName();
            File.Delete(tempFile);

            var tempDirectoryName = Path.GetFileNameWithoutExtension(tempFile);
            if (tempDirectoryName == null)
                throw new NullReferenceException("invalid temp directory name!");

            var tempPath = Path.Combine(Path.GetTempPath(), tempDirectoryName);
            Directory.CreateDirectory(tempPath);

            return tempPath;
        }
    }
}
