using System.Linq;
using Mono.Cecil;
using System.Collections.Generic;

namespace UnityPreserveAttributeParser.ManagedProxies
{
    public sealed class SourceGenerator : CodeGenerator
    {
        public SourceGenerator(TypeDefinition typeDefinition, string moduleName, List<MethodReference> methods) : base(typeDefinition, moduleName, methods)
        {
        }

        public string Generate()
        {
            GenerateNamespaceHeader();
            GenerateProxyDefinition();

            GenerateUnityObjectAccessors();

            GenerateProxyRegistration();

            GenerateNamespaceFooter();

            GenerateScriptingTraits();

            return Builder.ToString();
        }

        private void GenerateScriptingTraits()
        {
            if (!TypeDefinition.IsValueType)
            {
                Builder.Append("#if ENABLE_DOTNET\n");
                Builder.AppendFormat("\t{0}::{1} ScriptingTraits<{0}::{1}>::GetterMethod(ManagedObjectHandle instance, void* getterMethodPtr)\n", CppNamespace(), TypeDefinition.CppName());
                Builder.AppendLine("\t{");
                Builder.AppendLine("\t\treturn ScriptingTraits<ScriptingObjectPtr>::GetterMethod(instance, getterMethodPtr);\n");
                Builder.AppendLine("\t}");
                Builder.AppendFormat("\tvoid ScriptingTraits<{0}::{1}>::SetterMethod(ManagedObjectHandle instance, {0}::{1} value, void* setterMethodPtr)\n", CppNamespace(), TypeDefinition.CppName());
                Builder.AppendLine("\t{");
                Builder.AppendLine("\tScriptingTraits<ScriptingObjectPtr>::SetterMethod(instance, value, setterMethodPtr);\n");
                Builder.AppendLine("\t}");
                Builder.Append("#endif\n");
            }
        }

        private void GenerateNativeStructAssignment(string nativeType)
        {
            Builder.AppendFormat("{0}::{0} (const ::{1}& native) {{\n", TypeDefinition.CppName(), nativeType);
            if (!TypeDefinition.IsValueType)
                Builder.AppendFormat("\tm_Target = New();\n");

            foreach (var fieldDefinition in TypeDefinition.Fields.Where(Extensions.CanHaveNativeFieldAccessor))
            {
                var fieldName = fieldDefinition.FieldName();
                var nativeFieldName = fieldDefinition.NativeFieldName();
                Builder.AppendFormat("\tset_{0}(native.{1});\n", fieldName, nativeFieldName);
            }
            Builder.AppendFormat("}}\n\n");

            Builder.AppendFormat("{0}& {0}::operator = (const ::{1}& native) {{\n", TypeDefinition.CppName(), nativeType);
            if (!TypeDefinition.IsValueType)
            {
                Builder.AppendFormat("\tif (!m_Target)\n");
                Builder.AppendFormat("\t\tm_Target = New();\n");
            }
            foreach (var fieldDefinition in TypeDefinition.Fields.Where(Extensions.CanHaveNativeFieldAccessor))
            {
                var fieldName = fieldDefinition.FieldName();
                var nativeFieldName = fieldDefinition.NativeFieldName();
                Builder.AppendFormat("\tset_{0}(native.{1});\n", fieldName, nativeFieldName);
            }
            Builder.AppendFormat("\treturn *this;\n");
            Builder.AppendFormat("}}\n\n");


            Builder.AppendFormat("{0}::operator ::{1} () const {{\n", TypeDefinition.CppName(), nativeType);
            Builder.AppendFormat("\t::{0} native;\n", nativeType);
            foreach (var fieldDefinition in TypeDefinition.Fields.Where(Extensions.CanHaveNativeFieldAccessor))
            {
                var fieldName = fieldDefinition.FieldName();
                var nativeFieldName = fieldDefinition.NativeFieldName();
                if (fieldDefinition.FieldType.MetadataType == MetadataType.String)
                {
                    Builder.AppendFormat("\tMarshalling::StringMarshaller {0}Marshaller;\n", fieldName);
                    Builder.AppendFormat("\t{0}Marshaller = (ICallType_String_Argument)get_{0}();\n", fieldName);
                    Builder.AppendFormat("\tnative.{1} = {0}Marshaller;\n", fieldName, nativeFieldName);
                }
                else
                    Builder.AppendFormat("\tnative.{1} = get_{0}();\n", fieldName, nativeFieldName);
            }
            Builder.AppendFormat("\treturn native;\n");
            Builder.AppendFormat("}}\n\n");
        }

        private void GenerateUnityObjectAccessors()
        {
            if (TypeDefinition.NativeProxyType() != null)
            {
                var nativeType = TypeDefinition.NativeProxyType();
                if (TypeDefinition.IsValueType || TypeDefinition.NativeAsStruct())
                    GenerateNativeStructAssignment(nativeType);
            }
        }

        private void GenerateProxyRegistration()
        {
            Builder.AppendFormat(
                "SCRIPTING_MANAGED_OBJECT_PROXY_DEFINE({0}, {1});\n", CppNamespace(), TypeDefinition.CppName());
        }

        private void GenerateProxyDefinition()
        {
            Builder.AppendFormat("ScriptingClassPtr {0}::s_ScriptingClass;\n", TypeDefinition.CppName());
            Builder.AppendFormat("ScriptingGCHandle {0}::s_EmptyArray;\n\n", TypeDefinition.CppName());

            if (TypeDefinition.RequiresManagedProxyGeneration())
                GenerateFieldsStaticCacheDecl();

            Builder.AppendLine("");

            Builder.AppendFormat("void {0}::InitProxy ()\n", TypeDefinition.CppName());
            Builder.AppendLine("{");
            GenerateStaticCacheInit();
            Builder.AppendLine("}\n");

            Builder.AppendFormat("void {0}::CleanupProxy ()\n", TypeDefinition.CppName());
            Builder.AppendLine("{");
            GenerateStaticCacheCleanup();
            Builder.AppendLine("}\n");

            if (TypeDefinition.RequiresManagedProxyGeneration())
            {
                if (!TypeDefinition.IsValueType)
                {
                    Builder.AppendFormat("{0} {0}::New ()\n", TypeDefinition.CppName());
                    Builder.AppendLine("{");
                    Builder.AppendFormat("\treturn {0} (scripting_object_new (s_ScriptingClass));\n", TypeDefinition.CppName());
                    Builder.AppendLine("}\n");
                }
            }

            if (TypeDefinition.NativeProxyType() != null)
            {
                var nativeType = TypeDefinition.NativeProxyType();
                if (!TypeDefinition.IsValueType && !TypeDefinition.NativeAsStruct())
                {
                    if (TypeDefinition.IsUnityEngineObject())
                    {
                        Builder.AppendFormat("{1}::{1} (::{0}* value) {{\n", nativeType, TypeDefinition.CppName());
                        Builder.AppendFormat("\tm_Target = ::Scripting::ScriptingWrapperFor((::Object*)value);\n");
                        Builder.AppendFormat("}}\n");

                        Builder.AppendFormat("{1}::{1} (PPtr< ::{0}> value) {{\n", nativeType, TypeDefinition.CppName());
                        Builder.AppendFormat("\tif (value.GetInstanceID() == 0)\n");
                        Builder.AppendFormat("\t\tm_Target = SCRIPTING_NULL;\n");
                        Builder.AppendFormat("\telse\n");
                        Builder.AppendFormat("\t\tm_Target = ::Scripting::ScriptingWrapperFor(&**(PPtr< ::Object>*)&value);\n");
                        Builder.AppendFormat("}}\n");
                    }
                }
            }
        }

        private void GenerateStaticCacheInit()
        {
            Builder.AppendFormat("\t{0}::s_ScriptingClass = OptionalType(\"{1}.dll\", \"{2}\", \"{3}\");\n", TypeDefinition.CppName(), TypeDefinition.Module.Assembly.Name.Name, TypeDefinition.Namespace, TypeDefinition.Name);
            Builder.AppendFormat("\tif ({0}::s_ScriptingClass != SCRIPTING_NULL)\n", TypeDefinition.CppName(), TypeDefinition.Module.Assembly.Name.Name, TypeDefinition.Namespace);
            Builder.AppendLine("\t{");
            Builder.AppendFormat("\t\t{0}::s_EmptyArray.AcquireStrong(scripting_array_new({0}::s_ScriptingClass, sizeof({0}), 0));\n", TypeDefinition.CppName());

            if (TypeDefinition.RequiresManagedProxyGeneration() && !TypeDefinition.IsValueType)
            {
                foreach (var fieldDefinition in TypeDefinition.Fields.Where(Extensions.CanHaveNativeFieldAccessor))
                {
                    Builder.AppendFormat("\t\t{0}::s_AccessInfo_{1}.fieldPtr = scripting_class_get_field_from_name ({0}::s_ScriptingClass, \"{2}\");\n", TypeDefinition.CppName(), fieldDefinition.FieldName(), fieldDefinition.Name);
                    Builder.AppendLine("#if !ENABLE_DOTNET");
                    Builder.AppendFormat("\t\t{0}::s_AccessInfo_{1}.offset = scripting_field_get_offset ({0}::s_AccessInfo_{1}.fieldPtr);\n", TypeDefinition.CppName(), fieldDefinition.FieldName());
                    Builder.AppendLine("#endif");
                }
            }
            Builder.AppendLine("\t}");
        }

        private void GenerateStaticCacheCleanup()
        {
            Builder.AppendFormat("\t{0}::s_ScriptingClass = SCRIPTING_NULL;\n\n", TypeDefinition.CppName());
            Builder.AppendFormat("\t{0}::s_EmptyArray.ReleaseAndClear();\n", TypeDefinition.CppName());

            if (TypeDefinition.RequiresManagedProxyGeneration() && !TypeDefinition.IsValueType)
            {
                foreach (var fieldDefinition in TypeDefinition.Fields.Where(Extensions.CanHaveNativeFieldAccessor))
                {
                    Builder.AppendFormat("\t{0}::s_AccessInfo_{1}.fieldPtr = SCRIPTING_NULL;\n", TypeDefinition.CppName(), fieldDefinition.FieldName());
                    Builder.AppendLine("#if !ENABLE_DOTNET");
                    Builder.AppendFormat("\t{0}::s_AccessInfo_{1}.offset = -1;\n", TypeDefinition.CppName(), fieldDefinition.FieldName());
                    Builder.AppendLine("#endif");
                }
            }
        }

        private void GenerateFieldsStaticCacheDecl()
        {
            if (!TypeDefinition.IsValueType)
            {
                foreach (var fieldDefinition in TypeDefinition.Fields.Where(Extensions.CanHaveNativeFieldAccessor))
                {
                    Builder.AppendFormat("Scripting::detail::FieldAccessInfo {0}::s_AccessInfo_{1} (\"{1}\");\n", TypeDefinition.CppName(), fieldDefinition.FieldName());
                }
            }

            Builder.AppendLine();
        }
    }
}
