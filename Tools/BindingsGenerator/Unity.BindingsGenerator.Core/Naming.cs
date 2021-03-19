using Mono.Cecil;

namespace Unity.BindingsGenerator.Core
{
    public static class Naming
    {
        public static readonly string Self = "_unity_self";
        public static readonly string VoidType = "void";
        public static readonly string ReturnVar = "ret";
        public static readonly string PropertyValue = "_unity_value";
        public static readonly string DeclSpec = "SCRIPT_BINDINGS_EXPORT_DECL";
        public static readonly string CallingConvention = "SCRIPT_CALL_CONVENTION";
        public static readonly string EtwEntryMacro = "SCRIPTINGAPI_ETW_ENTRY";
        public static readonly string StackCheckMacro = "SCRIPTINGAPI_STACK_CHECK";
        public static readonly string ThreadSafeMacro = "SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK";
        public static readonly string ThreadUnsafeAndThrowMacro = "SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK_THROW_EXCEPTION"; // TODO(rb): This is temporary, remove when all threading errors throw
        public static readonly string ScriptingClassPtrType = "ScriptingClassPtr";
        public static readonly string ScriptingClassFunction = "GetScriptingClass";
        public static readonly string MarshallingNamespace = "Marshalling";
        public static readonly string ScriptingExceptionType = "ScriptingExceptionPtr";
        public static readonly string ExceptionVariableName  = "exception";
        public static readonly string ScriptingRaiseExceptionFunction = "scripting_raise_exception";
        public static readonly string HandleExceptionLabel = "handle_exception";
        public static readonly string ScriptingNull = "SCRIPTING_NULL";
        public static readonly string BindingsTypePrefix = "ICallType_Generated_";
        public static readonly string BindManagedTypeMacroName = "BIND_MANAGED_TYPE_NAME";
        public static readonly string RegistrationChecksNamespace = "RegistrationChecks";
        public static readonly string StructSizeChecksNamespace = "StructSizeChecks";
        public static readonly string MarshalFunctionName = "Marshal";
        public static readonly string UnmarshalFunctionName = "Unmarshal";
        public static readonly string RequiresMarshalingField = "RequiresMarshaling";
        public static readonly string MarshalInfo = "MarshalInfo";
        public static readonly string MarshalInfoDeclarationMacro = "MARSHAL_INFO";
        public static readonly string MarshalInfoDeclarationEndMacro = "END_MARSHAL_INFO";

        public static string AddressOf(string expr)
        {
            return $"&{expr}";
        }

        public static string Marshalled(this string variable)
        {
            return $"{variable}_marshalled";
        }

        public static string Unmarshalled(this string variable)
        {
            return $"{variable}_unmarshalled";
        }

        public static string Pointer(this string variable)
        {
            return $"{variable}*";
        }

        public static string ConstPointer(this string variable)
        {
            return $"const {variable}*";
        }

        public static string Reference(string expr)
        {
            return $"{expr}&";
        }

        public static string ConstReference(string expr)
        {
            return $"const {expr}&";
        }

        public static string IntermediateTypeName(string typeName)
        {
            return $"{typeName.WithoutNativeNamespaces()}__";
        }

        public static string ByRefInjectedMethod(string name)
        {
            return name + "_Injected";
        }

        public static string WithTemplateArgument(this string name, string templateArgument)
        {
            if (templateArgument.EndsWith(">"))
                templateArgument += " ";

            return $"{name}<{templateArgument}>";
        }

        public static string CppNameFor(TypeDefinition typeDefinition)
        {
            return CleanName(ExtractNameFrom(typeDefinition));
        }

        private static string ExtractNameFrom(TypeDefinition typeDefinition)
        {
            if (!typeDefinition.IsNested)
                return typeDefinition.Name;

            return ExtractNameFrom(typeDefinition.DeclaringType) + typeDefinition.Name;
        }

        public static string RegisteredNameFor(string name)
        {
            return BindingsTypePrefix + name;
        }

        internal static string CleanName(string name)
        {
            var cppName = name;
            for (var i = 0; i < name.Length && char.IsUpper(name[i]) && (i == name.Length - 1 || char.IsUpper(name[i + 1]) || i == 0); i++)
                cppName = cppName.Substring(0, i) + char.ToLower(cppName[i]) + cppName.Substring(i + 1);
            return cppName;
        }

        public static string WithoutNativeNamespaces(this string name)
        {
            var lastIndex = name.LastIndexOf(":");
            return lastIndex == -1 ? name : name.Substring(lastIndex + 1);
        }

        public static string VerifyBindingNameFor(string name)
        {
            return $"Verify_{BindManagedTypeMacroName}_On_{name}";
        }
    }
}
