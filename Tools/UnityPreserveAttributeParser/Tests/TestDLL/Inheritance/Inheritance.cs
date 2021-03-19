using System.Runtime.InteropServices;

namespace UnityEngineTestDLL.Inheritance
{
    [RequiredByNativeCode(GenerateProxy = true)]
    [StructLayout(LayoutKind.Sequential)]
    public class TheBaseClassOne
    {
    }

    [RequiredByNativeCode(GenerateProxy = true)]
    [StructLayout(LayoutKind.Sequential)]
    public class TheOtherBaseClassOne : TheBaseClassOne
    {
    }

    [RequiredByNativeCode(GenerateProxy = true)]
    [StructLayout(LayoutKind.Sequential)]
    public class TheOtherParentClassOne : TheBaseClassOne
    {
    }

    [RequiredByNativeCode(GenerateProxy = true)]
    [StructLayout(LayoutKind.Sequential)]
    public class TheParentClassOne : TheOtherBaseClassOne
    {
    }
}
