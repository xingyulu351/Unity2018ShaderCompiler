using System;
using System.Runtime.InteropServices;

namespace UnityEngineTestDLL.Proxies
{
    [NativeHeader("NativeHeader.h")]
    [NativeClass("Test::NativeStruct")]
    [RequiredByNativeCode(GenerateProxy = true)]
    [StructLayout(LayoutKind.Sequential)]
    public struct TestStruct
    {
        [NativeName("NativeFloatField")]
        public float FloatField;

        public TestClass ObjectField;

        public TestClass[] ArrayField;
    }

    [RequiredByNativeCode]
    [StructLayout(LayoutKind.Sequential)]
    public class BaseClass
    {
        [RequiredByNativeCode(GenerateProxy = true)]
        [StructLayout(LayoutKind.Sequential)]
        public class NestedClass
        {
        }
    }

    [RequiredByNativeCode(GenerateProxy = true)]
    [NativeClass("Test::NativeClass", "struct NativeClassCustomForwarderDeclaration;")]
    [StructLayout(LayoutKind.Sequential)]
    public class TestClass : BaseClass
    {
        public float FloatField;
    }
}
