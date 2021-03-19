using System.Runtime.CompilerServices;
using UnityEngine.Bindings;

namespace Unity.BindingsGenerator.AssemblyVisitorTestAssembly
{
    // This class should be handled by the bindings generator, because it has extern methods
    // which are not marked as GeneratedByOldBindingsGeneratorAttribute
    [NativeHeader(Header = "ClassWithNoNativeTypeAttribute")]
    internal class ClassWithNoNativeTypeAttribute : UnityEngine.Object
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void MemberMethod();
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void StaticMethodWithClassArgument(ClassWithNoNativeTypeAttribute arg);
    }
}
