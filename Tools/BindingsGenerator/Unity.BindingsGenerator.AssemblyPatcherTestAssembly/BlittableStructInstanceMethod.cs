using System.Runtime.CompilerServices;
using UnityEngine;

namespace Unity.BindingsGenerator.AssemblyPatcherTestAssembly
{
    #pragma warning disable 169
    internal struct BlittableStructWithExternInstanceMethod
    {
        int field;

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void ExternInstanceMethod();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void ExternInstanceMethodWithWritable([Writable] UnityEngine.Object param);
    }

    struct BlittableStructWithExternInstanceMethodExpected
    {
        int field;

        public void ExternInstanceMethod()
        {
            ExternInstanceMethod_Injected(ref this);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void ExternInstanceMethod_Injected(ref BlittableStructWithExternInstanceMethodExpected _unity_self);

        public void ExternInstanceMethodWithWritable([Writable] UnityEngine.Object param)
        {
            ExternInstanceMethodWithWritable_Injected(ref this, param);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void ExternInstanceMethodWithWritable_Injected(ref BlittableStructWithExternInstanceMethodExpected _unity_self, [Writable] UnityEngine.Object param);
    }
}
