using System.Runtime.CompilerServices;
using UnityEngine.Bindings;

namespace Unity.BindingsGenerator.AssemblyVisitorTestAssembly
{
    [NativeType(Header = "EmptyNativeTypeClass")]
    class EmptyNativeTypeClass
    {
        void SomeMethod(EmptyNativeTypeClass arg)
        {
        }
    }

    [NativeType(Header = "NativeTypeClassWithMemberMethod")]
    class NativeTypeClassWithMemberMethod : UnityEngine.Object
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void ExternMemberMethod();
    }

    [NativeType(Header = "NativeTypeClassWithStaticMethod")]
    class NativeTypeClassWithStaticMethod
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void ExternStaticMethod();
    }

    [NativeType(Header = "NativeTypeClassWithBindingsParserMethods")]
    class NativeTypeClassWithBindingsParserMethods
    {
        [UnityEngine.Scripting.GeneratedByOldBindingsGeneratorAttribute]
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void ExternStaticMethod();

        public static extern int ExternStaticProperty
        {
            [UnityEngine.Scripting.GeneratedByOldBindingsGeneratorAttribute]
            [MethodImpl(MethodImplOptions.InternalCall)]
            get;

            [UnityEngine.Scripting.GeneratedByOldBindingsGeneratorAttribute]
            [MethodImpl(MethodImplOptions.InternalCall)]
            set;
        }
    }

    class ParentOfNestedWithStaticMethod
    {
        class NestedWithStaticMethod
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void ExternStaticMethod();
        }
    }
}
