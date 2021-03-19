using System;
using System.Runtime.CompilerServices;
using UnityEngine.Bindings;

namespace Unity.BindingsGenerator.AssemblyPatcherTestAssembly
{
    public class SomeAttribute : Attribute
    {
    }

    [SomeAttribute]
    [NativeHeader]
    [NativeHeader]
    [NativeType]
    [NativeConditional("SOME_CONDITION")]
    [Obsolete]
    public class ClassWithAttributes
    {
        [SomeAttribute]
        [NativeHeader]
        [NativeHeader]
        [NativeType]
        [Obsolete]
        private class NestedClass
        {
            [SomeAttribute]
            [NativeMethod]
            [NativeWritableSelf]
            [MethodImpl(MethodImplOptions.InternalCall)]
            public extern void NestedMethod();

            [SomeAttribute]
            [NativeMethod]
            public extern int NestedProp
            {
                [SomeAttribute][NativeMethod][NativeWritableSelf][MethodImpl(MethodImplOptions.InternalCall)] get;
                [SomeAttribute][NativeMethod][NativeWritableSelf][MethodImpl(MethodImplOptions.InternalCall)] set;
            }

            [SomeAttribute]
            [NativeType]
            [Obsolete]
            private enum NestedEnum
            {
            }
        }

        [SomeAttribute]
        [NativeMethod]
        [NativeWritableSelf]
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SomeMethod();

        [SomeAttribute]
        [NativeMethod]
        public extern int SomeProp
        {
            [SomeAttribute][NativeMethod][NativeWritableSelf][MethodImpl(MethodImplOptions.InternalCall)] get;
            [SomeAttribute][NativeMethod][NativeWritableSelf][MethodImpl(MethodImplOptions.InternalCall)] set;
        }

        [SomeAttribute]
        [NativeType]
        [Obsolete]
        private enum SomeEnum
        {
        }

        [SomeAttribute]
        [NativeType]
        [Obsolete]
        private struct SomeStruct
        {
        }
    }
}
