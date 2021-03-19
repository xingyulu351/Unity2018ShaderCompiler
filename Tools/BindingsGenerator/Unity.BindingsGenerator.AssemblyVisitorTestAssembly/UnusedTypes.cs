using UnityEngine.Bindings;

namespace Unity.BindingsGenerator.AssemblyVisitorTestAssembly
{
    #pragma warning disable 169

    [NativeType(Header = "UnusedNativeEnum", CodegenOptions = CodegenOptions.Force)]
    enum UnusedNativeEnum
    {
        T = 1
    }

    [NativeType(Header = "UnusedNativeStruct", CodegenOptions = CodegenOptions.Force)]
    struct UnusedNativeStruct
    {
        private int i;
    }

    [NativeType(Header = "UnusedNativeType")]
    class UnusedNativeType
    {
        private string s;
    }
}
