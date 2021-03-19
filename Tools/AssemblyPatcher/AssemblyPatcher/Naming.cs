using Mono.Cecil;

namespace AssemblyPatcher
{
    class Naming
    {
        public static string ForUnsupportedMethod(MethodReference method)
        {
            return ForUnsupportedMethodWithReturnType(method.ReturnType);
        }

        public static string ForUnsupportedMethodWithReturnType(TypeReference returnType)
        {
            return AssemblyPatcherConstants.UnsupportedMemberMethodName + "_" + returnType.Name;
        }
    }
}
