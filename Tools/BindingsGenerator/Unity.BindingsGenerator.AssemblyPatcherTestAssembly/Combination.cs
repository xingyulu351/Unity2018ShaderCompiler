using System.Runtime.CompilerServices;
using UnityEngine.Bindings;

namespace Unity.BindingsGenerator.AssemblyPatcherTestAssembly
{
    class SimpleCombinationTest
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        [NativeMethod]
        public extern SomeStruct SomeMethod(int a, out SomeStruct doNotTouchMe, SomeStruct touchMe, int b = 19);
    }

    class SimpleCombinationTestExpected
    {
        public SomeStruct SomeMethod(int a, out SomeStruct doNotTouchMe, SomeStruct touchMe)
        {
            return SomeMethod(a, out doNotTouchMe, touchMe, 19);
        }

        public SomeStruct SomeMethod(int a, out SomeStruct doNotTouchMe, SomeStruct touchMe, int b)
        {
            SomeStruct ret;
            SomeMethod_Injected(a, out doNotTouchMe, ref touchMe, b, out ret);
            return ret;
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void SomeMethod_Injected(int a, out SomeStruct doNotTouchMe, ref SomeStruct touchMe, int b, out SomeStruct ret);
    }
}
