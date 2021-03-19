using System.Linq;
using Mono.Cecil;
using System.Collections.Generic;

namespace UnityPreserveAttributeParser.ManagedProxies
{
    public sealed class HeaderGenerator : CodeGenerator
    {
        public HeaderGenerator(TypeDefinition typeDefinition, string moduleName, List<MethodReference> methods) : base(typeDefinition, moduleName, methods)
        {
        }

        public string GenerateForwardDefinition()
        {
            GenerateNamespaceHeader();
            Builder.AppendFormat("class {0};\n", TypeDefinition.CppName());
            GenerateNamespaceFooter();

            return Builder.ToString();
        }

        public string Generate()
        {
            GenerateScriptingTraits();

            GenerateNamespaceHeader();
            GenerateProxyDeclaration();
            GenerateNamespaceFooter();

            if (TypeDefinition.RequiresManagedProxyGeneration())
            {
                GenerateProxyRegistration();

                if (TypeDefinition.NativeProxyType() != null)
                    Builder.Append($"BIND_MANAGED_TYPE_NAME({TypeDefinition.NativeProxyType()}, {TypeDefinition.Namespace.Replace('.', '_')}_{TypeDefinition.Name});");
            }

            return Builder.ToString();
        }

        private void GenerateScriptingTraits()
        {
            var isBlittable = TypeDefinition.IsBlittable();


            Builder.AppendLine("\ntemplate<>");
            Builder.AppendFormat("struct ScriptingTraits<{0}::{1}> {{\n", CppNamespace(), TypeDefinition.CppName());
            Builder.AppendFormat("\tstatic const bool IsScriptingObjectPtr = {0};\n", FormatBool(!TypeDefinition.IsValueType));
            Builder.AppendFormat("\tstatic const bool IsManagedProxy = {0};\n", FormatBool(true));
            Builder.AppendFormat("\tstatic const bool Blittable = {0};\n", FormatBool(isBlittable));
            Builder.AppendFormat("\tstatic const bool ReferenceType = {0};\n", FormatBool(!TypeDefinition.IsValueType));
            Builder.AppendFormat("\tstatic const bool ValueType = {0};\n", FormatBool(TypeDefinition.IsValueType));
            Builder.AppendFormat("\tstatic const bool ArrayElementRead = {0};\n", FormatBool(true));
            Builder.AppendFormat("\tstatic const bool ArrayElementWrite = {0};\n", FormatBool(true));
            Builder.AppendFormat("\tstatic const bool FieldsOfTypeCanBeRead = {0};\n", FormatBool(true));
            Builder.AppendFormat("\tstatic const bool FieldsOfTypeCanBeWritten = {0};\n", FormatBool(true));
            Builder.AppendFormat("\tstatic const bool BarrierOnFieldOfTypeWrite = {0};\n", FormatBool(!isBlittable));
            Builder.AppendFormat("\tstatic const bool BarrierOnArrayElementWrite = {0};\n", FormatBool(!isBlittable));
            Builder.Append("#if ENABLE_DOTNET\n");
            Builder.AppendFormat("\tstatic {0}::{1} GetterMethod(ManagedObjectHandle instance, void* getterMethodPtr);\n", CppNamespace(), TypeDefinition.CppName());
            Builder.AppendFormat("\tstatic void SetterMethod(ManagedObjectHandle instance, {0}::{1} value, void* setterMethodPtr);\n", CppNamespace(), TypeDefinition.CppName());
            Builder.Append("#endif\n");
            Builder.AppendLine("};");
        }

        private static string FormatBool(bool value)
        {
            return value ? "true" : "false";
        }

        private void GenerateNativeStructAssignment(string nativeType)
        {
            Builder.AppendFormat("\t{0} (const ::{1}& native);\n", TypeDefinition.CppName(), nativeType);
            Builder.AppendFormat("\t{0}& operator = (const ::{1}& native);\n", TypeDefinition.CppName(), nativeType);
            Builder.AppendFormat("\toperator ::{0} () const;\n", nativeType);
        }

        private void GenerateNativeObjectAccesorts(string nativeType)
        {
            Builder.AppendFormat("\t{1} (::{0}* value);\n", nativeType, TypeDefinition.CppName());
            Builder.AppendFormat("\t{1} (PPtr< ::{0}> value);\n", nativeType, TypeDefinition.CppName());

            Builder.AppendFormat("\tinline operator ::{0}* () {{\n", nativeType);
            Builder.AppendFormat("\t\treturn IsValid() ? (::{0}*)get_CachedPtr() : NULL;\n", nativeType);
            Builder.AppendFormat("\t}}\n");

            Builder.AppendFormat("\tinline ::{0}* operator ->() {{\n", nativeType);
            Builder.AppendFormat("\t\treturn IsValid() ? (::{0}*)get_CachedPtr() : NULL;\n", nativeType);
            Builder.AppendFormat("\t}}\n");

            Builder.AppendFormat("\tinline ::{0}& operator *() {{\n", nativeType);
            Builder.AppendFormat("\t\treturn *(::{0}*)get_CachedPtr();\n", nativeType);
            Builder.AppendFormat("\t}}\n");
        }

        private void GenerateUnityObjectAccessors()
        {
            if (TypeDefinition.NativeProxyType() != null)
            {
                var nativeType = TypeDefinition.NativeProxyType();
                if (!TypeDefinition.IsValueType && !TypeDefinition.NativeAsStruct())
                {
                    if (TypeDefinition.IsUnityEngineObject())
                        GenerateNativeObjectAccesorts(nativeType);
                }
                else
                    GenerateNativeStructAssignment(nativeType);
            }
        }

        private void GenerateProxyDeclaration()
        {
            if (TypeDefinition.IsValueType)
                Builder.AppendFormat("class {0}\n{{\n", TypeDefinition.CppName(), BaseTypeCppFullName());
            else
                Builder.AppendFormat("class {0} : public {1}\n{{\n", TypeDefinition.CppName(), BaseTypeCppFullName());
            Builder.AppendLine("public:");
            Builder.AppendLine("\tstatic ScriptingClassPtr s_ScriptingClass;");
            Builder.AppendLine("\tstatic ScriptingClassPtr GetScriptingClass() { return s_ScriptingClass; }");
            Builder.AppendFormat("\tstatic ScriptingGCHandle s_EmptyArray;\n\n");
            if (TypeDefinition.NativeProxyType() != null)
                Builder.AppendFormat("\ttypedef ::{0} NativeType;\n\n", TypeDefinition.NativeProxyType());
            else
                Builder.AppendFormat("\ttypedef {0} NativeType;\n\n", TypeDefinition.CppName());
            Builder.AppendLine("\tstatic void InitProxy ();");
            Builder.AppendLine("\tstatic void CleanupProxy ();\n");
            if (!TypeDefinition.IsValueType)
            {
                Builder.AppendFormat("\tinline {0} () : {1} () {{ }}\n", TypeDefinition.CppName(), BaseTypeCppFullName());
                Builder.AppendFormat("\tinline {0} (ScriptingObjectPtr _value) {{ m_Target = _value; }}\n", TypeDefinition.CppName());
                Builder.Append("#if ENABLE_DOTNET\n");
                Builder.AppendFormat("\tinline {0} (ManagedObjectHandle _value) {{ m_Target = ScriptingObjectPtr(_value); }}\n", TypeDefinition.CppName());
                Builder.Append("#endif\n");
                Builder.Append("\tinline bool IsValid() const { return m_Target != ScriptingObjectNull(); }\n");

                Builder.AppendFormat("\tinline void operator= (const ScriptingObjectPtr& other) {{\n");
                Builder.AppendFormat("\t\t//write barrier here\n");
                Builder.AppendFormat("\t\tScriptingObjectPtr::operator=(other);\n");
                Builder.AppendFormat("\t}}\n");
            }
            else
                Builder.AppendFormat("\tinline {0} () {{ }}\n", TypeDefinition.CppName());
            GenerateUnityObjectAccessors();
            if (TypeDefinition.RequiresManagedProxyGeneration())
            {
                GenerateFieldAccessors();
                if (!TypeDefinition.IsValueType)
                    Builder.AppendFormat("\tstatic {0} New ();\n", TypeDefinition.CppName());
            }
            GenerateMethodAccessors();

            Builder.Append("};\n");
        }

        private void GenerateFieldAccessors()
        {
            foreach (var fieldDefinition in TypeDefinition.Fields.Where(Extensions.CanHaveNativeFieldAccessor))
            {
                switch (fieldDefinition.FieldType.MetadataType)
                {
                    case MetadataType.String:
                        GenerateAccessorForStringField(fieldDefinition);
                        break;

                    default:
                        GenerateDefaultFieldAccessor(fieldDefinition);
                        break;
                }
            }

            Builder.AppendLine();
        }

        private void GenerateMethodAccessors()
        {
            if (Methods != null)
            {
                foreach (var method in Methods)
                {
                    // We never call constructors explicitly from native code, and will have naming conflicts if we add them, so skip them here.
                    if (method.Name == ".ctor")
                        return;

                    if (method.Resolve().NameIsAmbiguous())
                    {
                        Builder.AppendLine("\t" + method.Resolve().WarningMessageForAmbiguousName());
                        continue;
                    }

                    var methodDefinition = method.Resolve();
                    Builder.Append("\t");
                    if (methodDefinition.IsStatic)
                        Builder.Append("static ");

                    Builder.AppendLine(MethodCallWrapperGenerator.MethodDefinitionToCppDeclaration(method.Resolve(), ModuleName, MethodCallWrapperGenerator.GenerationOptions.ForProxy) + ";");
                }
            }

            Builder.AppendLine();
        }

        private void GenerateDefaultFieldAccessor(FieldDefinition fieldDefinition)
        {
            if (TypeDefinition.IsValueType)
            {
                Builder.AppendFormat(@"
private:
    {1} {2};
public:
    inline {1} get_{2} () const {{ return {2}; }}
    inline void set_{2} ({1} const & _value) {{ {2} = _value; }};
", TypeDefinition.CppName(), ScriptingTypeNameFor(fieldDefinition.FieldType), fieldDefinition.FieldName());
            }
            else
            {
                Builder.AppendFormat(@"
    static Scripting::detail::FieldAccessInfo s_AccessInfo_{2};
    inline {1} get_{2} () const {{
        DETAIL__SCRIPTING_VALIDATE_FIELD_ACCESSOR({0}, {2});
#if !ENABLE_DOTNET
        return *(Scripting::detail::GetFieldValueAt<{1}>(*this, s_AccessInfo_{2}));
#else
        return ScriptingTraits<{3}>::GetterMethod(*this, s_AccessInfo_{2}.fieldPtr.GetBackendPtr()->getterMethodPtr);
#endif
    }}
    inline void set_{2} ({1} const & _value) {{
        DETAIL__SCRIPTING_VALIDATE_FIELD_ACCESSOR({0}, {2});
#if !ENABLE_DOTNET
        Scripting::detail::SetFieldValueAt<{1}> (*this, s_AccessInfo_{2}, _value);
#else
        ScriptingTraits<{3}>::SetterMethod(*this, _value, s_AccessInfo_{2}.fieldPtr.GetBackendPtr()->setterMethodPtr);
#endif
    }}", TypeDefinition.CppName(), ScriptingTypeNameFor(fieldDefinition.FieldType), fieldDefinition.FieldName(), fieldDefinition.FieldType.IsArray ? "ScriptingArrayPtr" : ScriptingTypeNameFor(fieldDefinition.FieldType));
            }
        }

        private void GenerateAccessorForStringField(FieldDefinition fieldDefinition)
        {
            GenerateDefaultFieldAccessor(fieldDefinition);
            if (TypeDefinition.IsValueType)
            {
                Builder.AppendFormat(@"
    inline void set_{2} (const core::string& _value) {{
        {2} = scripting_string_new(_value);
    }}", TypeDefinition.CppName(), ScriptingTypeNameFor(fieldDefinition.FieldType), fieldDefinition.FieldName());
            }
            else
            {
                Builder.AppendFormat(@"
    inline void set_{2} (const core::string& _value) {{
        DETAIL__SCRIPTING_VALIDATE_FIELD_ACCESSOR({0}, {2});
#if !ENABLE_DOTNET
        Scripting::detail::SetFieldValueAt<{1}> (*this, s_AccessInfo_{2}, _value);
#else
        ScriptingTraits<{1}>::SetterMethod(*this, scripting_string_new(_value), s_AccessInfo_{2}.fieldPtr.GetBackendPtr()->setterMethodPtr);
#endif
    }}", TypeDefinition.CppName(), ScriptingTypeNameFor(fieldDefinition.FieldType), fieldDefinition.FieldName());
            }
        }

        private int BaseIndex()
        {
            return BaseIndexFor(TypeDefinition.BaseType.Resolve());
        }

        private static int BaseIndexFor(TypeDefinition typeDefinition)
        {
            if (!Naming.IsUnityEngineType(typeDefinition))
                return 0;

            return BaseIndexFor(typeDefinition.BaseType.Resolve()) + typeDefinition.Fields.Count;
        }

        private void GenerateProxyRegistration()
        {
            Builder.AppendFormat(
                "SCRIPTING_MANAGED_OBJECT_PROXY_DECLARE({0}, {1});\n", CppNamespace(), TypeDefinition.CppName());
        }
    }
}
