using System.Runtime.CompilerServices;
using UnityEngine.Bindings;

namespace Unity.BindingsGenerator.AssemblyVisitorTestAssembly
{
    [NativeType(Header = "EmptyNativeTypeStruct")]
    struct EmptyNativeTypeStruct
    {
        void SomeMethod(EmptyNativeTypeStruct arg)
        {
        }
    }

    [NativeType(Header = "NativeTypeStructWithStaticMethod")]
    struct NativeTypeStructWithStaticMethod
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void ExternStaticMethod();
    }

    [NativeType(Header = "NativeTypeBlittableStructWithInstanceMethod")]
    struct NativeTypeBlittableStructWithInstanceMethod
    {
        #pragma warning disable 169
        int a;

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void ExternInstanceMethod();
    }
}
