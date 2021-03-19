using System.Runtime.CompilerServices;
using UnityEngine.Bindings;

namespace Unity.BindingsGenerator.AssemblyVisitorTestAssembly
{
    #pragma warning disable 169

    // We only check argument usage here

    [NativeType(Header = "NativeEnumUsedAsArgument", CodegenOptions = CodegenOptions.Force)]
    enum NativeEnumUsedAsArgument
    {
        Hello = 1
    }

    [NativeType(Header = "NativeNonGeneratingEnumUsedAsArgument")]
    enum NativeNonGeneratingEnumUsedAsArgument
    {
        World = 2
    }

    [NativeType(Header = "PodNativeStructUsedAsArgument")]
    struct PodNativeStructUsedAsArgument
    {
        private int hello;
    }

    [NativeType(Header = "NonPodNativeStructUsedAsArgument")]
    struct NonPodNativeStructUsedAsArgument
    {
        private string hello;
    }

    [NativeType(Header = "ClassUsedOnlyAsReturn")]
    class ClassUsedAsReturn
    {
        private int hello;
    }


    [NativeType(Header = "OtherTypeUser")]
    class OtherTypeUser : UnityEngine.Object
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void EnumUser(NativeEnumUsedAsArgument arg1, NativeNonGeneratingEnumUsedAsArgument arg2);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void PodStructUser(PodNativeStructUsedAsArgument arg);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void NonPodStructUser(NonPodNativeStructUsedAsArgument arg);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern ClassUsedAsReturn ClassReturner();
    }
}
