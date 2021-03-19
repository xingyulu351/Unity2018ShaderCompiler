using System;
using System.Runtime.CompilerServices;
using NUnit.Framework;
using Unity.BindingsGenerator.Core;
using Unity.BindingsGenerator.TestFramework;

namespace Unity.BindingsGenerator.Tests
{
    [TestFixture]
    class ThreadAttributeTests : BindingsGeneratorTestsBase
    {
        class ThreadSafeAttributeTestClass
        {
            [UnityEngine.ThreadAndSerializationSafe()]
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void SomeMethod();

            public static extern int SomeProperty
            {
                [UnityEngine.ThreadAndSerializationSafe()]
                [MethodImpl(MethodImplOptions.InternalCall)]
                get;

                [UnityEngine.ThreadAndSerializationSafe()]
                [MethodImpl(MethodImplOptions.InternalCall)]
                set;
            }
        };

        [TestCase(typeof(ThreadSafeAttributeTestClass), "SomeMethod")]
        [TestCase(typeof(ThreadSafeAttributeTestClass), "get_SomeProperty")]
        [TestCase(typeof(ThreadSafeAttributeTestClass), "set_SomeProperty")]
        public void ThreadSafeAttributeSupressesThreadCheckMacro(Type klass, string methodName)
        {
            var functionNode = FunctionNodeForMethod(klass, methodName);
            var functionNodeSource = new CppGenerator().Generate(functionNode);
            Assert.IsFalse(functionNodeSource.Contains(Naming.ThreadSafeMacro));
        }
    }
}
