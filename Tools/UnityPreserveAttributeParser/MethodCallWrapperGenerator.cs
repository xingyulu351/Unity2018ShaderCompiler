using System;
using Mono.Cecil;
using System.Collections.Generic;
using System.Linq;
using UnityPreserveAttributeParser.ManagedProxies;

namespace UnityPreserveAttributeParser
{
    public class MethodCallWrapperGenerator
    {
        public static string CppTypeForManagedType(TypeReference t, bool forReturnType = false)
        {
            switch (t.MetadataType)
            {
                case MetadataType.Int32:
                    return "int";
                case MetadataType.Int64:
                    return "SInt64";
                case MetadataType.UInt64:
                    return "UInt64";
                case MetadataType.Boolean:
                    return "bool";
                case MetadataType.String:
                    return "core::string";
                case MetadataType.Single:
                    return "float";
                case MetadataType.Double:
                    return "double";
                case MetadataType.IntPtr:
                    return "void*";
                default:
                    if (t.IsArray)
                        return "ScriptingArrayPtr";
                    if (t.Name == "Void")
                        return "void";
                    if (t.Resolve().IsEnum)
                        return "int";
                    if (t.IsValueType && !forReturnType)
                        return "const void*";
                    if (t.IsByReference)
                    {
                        var elementType = t.Resolve().GetElementType();
                        if (!elementType.IsValueType)
                            return "ScriptingObjectPtr*";
                        else if (elementType.IsPrimitive)
                            return CppTypeForManagedType(elementType) + "*";
                        else
                            return CppTypeForManagedType(elementType);
                    }
                    return "ScriptingObjectPtr";
            }
        }

        public enum GenerationOptions
        {
            None = 0,
            Implementation = 1 << 0,
            Invocation = 1 << 1,
            AddThisAsParameter = 1 << 2,
            InCommonScriptingClasses = 1 << 4,
            ForProxy = 1 << 5,
        }

        public static string MethodDefinitionToCppDeclaration(MethodDefinition method, string moduleName, GenerationOptions options)
        {
            var prototype = "";
            var cppName = Naming.CppNamefor(method);
            var cppNameUpper = Char.ToUpper(cppName[0]) + cppName.Substring(1);
            if ((options & GenerationOptions.Invocation) == 0)
                prototype += CppTypeForManagedType(method.ReturnType, true) + " ";
            if ((options & GenerationOptions.Implementation) != 0)
            {
                if ((options & GenerationOptions.InCommonScriptingClasses) != 0)
                    prototype += moduleName + "ScriptingClasses::";
                else
                    prototype += $"{CodeGenerator.CppNamespaceFor(method.DeclaringType.NestedNamespace(), false)}::{method.DeclaringType.CppName()}::";
            }
            prototype += cppNameUpper + "(";
            if (!method.IsStatic && (options & GenerationOptions.AddThisAsParameter) != 0)
            {
                if ((options & GenerationOptions.Invocation) != 0)
                    prototype += "*this, ";
                else
                    prototype += CppTypeForManagedType(method.DeclaringType) + " _this, ";
            }
            foreach (var param in method.Parameters)
            {
                var paramType = param.ParameterType;
                if ((options & GenerationOptions.ForProxy) != 0 && paramType.Resolve().NativeProxyType() != null && paramType.MetadataType != MetadataType.ByReference)
                {
                    prototype += CodeGenerator.ScriptingTypeNameFor(paramType);
                    if (paramType.IsValueType && !paramType.IsPrimitive)
                        prototype += "*";
                    prototype +=  " ";
                }
                else if ((options & GenerationOptions.Invocation) == 0)
                    prototype += CppTypeForManagedType(paramType) + " ";
                prototype += param.Name + ", ";
            }
            if ((options & GenerationOptions.Invocation) != 0)
                prototype += "outException";
            else
            {
                prototype += "ScriptingExceptionPtr *outException";
                if ((options & GenerationOptions.Implementation) == 0)
                    prototype += "=NULL";
            }
            prototype += ")";
            return prototype;
        }

        static string AddArgumentForType(TypeReference type)
        {
            switch (type.MetadataType)
            {
                case MetadataType.Int32:
                    return "AddInt";
                case MetadataType.Int64:
                    return "AddInt64";
                case MetadataType.UInt64:
                    return "AddUInt64";
                case MetadataType.Boolean:
                    return "AddBoolean";
                case MetadataType.String:
                    return "AddString";
                case MetadataType.Single:
                    return "AddFloat";
                case MetadataType.Double:
                    return "AddDouble";
                case MetadataType.IntPtr:
                    return "AddIntPtr";
                default:
                    if (type.IsArray)
                        return "AddArray";
                    else if (type.Resolve().IsEnum)
                        return "AddEnum";
                    else if (type.IsValueType)
                        return "AddStruct";
                    else if (type.IsByReference)
                    {
                        if (!type.Resolve().GetElementType().IsValueType)
                            return "AddOutObject";
                        else
                            return AddArgumentForType(type.Resolve().GetElementType());
                    }
                    else
                        return "AddObject";
            }
        }

        public static string MethodDefinitionToCppWrapper(MethodDefinition method, string moduleName, PreserveState state)
        {
            var wrapper = "";
            var cppName = Naming.CppNamefor(method);
            var cppNameUpper = Char.ToUpper(cppName[0]) + cppName.Substring(1);
            var cppReturnType = CppTypeForManagedType(method.ReturnType, true);
            var isVoidMethod = method.ReturnType.Name == "Void";
            wrapper += MethodDefinitionToCppDeclaration(method, moduleName, GenerationOptions.Implementation | GenerationOptions.ForProxy) + "\n";
            wrapper += "{\n";

            var fakingMacro = method.IsStatic ? "__FAKEABLE_STATIC_METHOD__" : "__FAKEABLE_METHOD__";
            var parameters = new List<string>(method.Parameters.Select(p => p.Name)) {"outException"};
            wrapper += $"\t{fakingMacro}(::{CodeGenerator.CppNamespaceFor(method.DeclaringType.NestedNamespace(), false)}::{method.DeclaringType.CppName()}, {cppNameUpper}, ({string.Join(", ", parameters)}));\n";

            wrapper += "\tScriptingMethodPtr method = Get" + moduleName + "ScriptingClasses()." + cppName + ";\n";

            if (state != PreserveState.Required)
            {
                wrapper += "\tif (method.IsNull())\n";
                if (isVoidMethod)
                    wrapper += "\t\treturn;\n";
                else
                {
                    wrapper += "\t{\n";
                    wrapper += "\t\tstatic " + cppReturnType + " defaultRetVal;";
                    wrapper += "\t\treturn defaultRetVal;";
                    wrapper += "\t}\n";
                }
            }
            if (!isVoidMethod)
                wrapper += $"\t{cppReturnType} retval;\n";

            wrapper += "\tScriptingInvocation invocation(";
            if (!method.IsStatic)
                wrapper += "*this, ";
            wrapper += "method);\n";
            foreach (var param in method.Parameters)
            {
                var type = param.ParameterType;
                wrapper += $"\tinvocation.{AddArgumentForType(type)}(";
                if (type.IsByReference && type.Resolve().GetElementType().IsPrimitive)
                    wrapper += "*";
                wrapper += $"{param.Name});\n";
            }
            wrapper += "\tif (outException != NULL)\n";
            wrapper += "\t{\n";
            wrapper += "\t\tinvocation.logException = false;\n";
            wrapper += "\t\t";
            if (!isVoidMethod)
                wrapper += "retval = ";
            wrapper += "invocation.Invoke<" + cppReturnType + ">(outException);\n";
            wrapper += "\t}\n";
            wrapper += "\telse\n";
            wrapper += "\t{\n";
            wrapper += "\t\t";
            if (!isVoidMethod)
                wrapper += "retval = ";
            wrapper += "invocation.Invoke<" + cppReturnType + ">();\n";
            wrapper += "\t}\n";
            int argumentIndex = 0;
            foreach (var param in method.Parameters)
            {
                var type = param.ParameterType;
                if (type.IsByReference && type.Resolve().GetElementType().IsPrimitive)
                {
                    wrapper += $"\t*{param.Name} = invocation.Arguments().";
                    switch (type.Resolve().GetElementType().MetadataType)
                    {
                        case MetadataType.Int32:
                            wrapper += "GetIntAt";
                            break;
                        case MetadataType.Int64:
                            wrapper += "GetInt64At";
                            break;
                        case MetadataType.UInt64:
                            wrapper += "GetUInt64At";
                            break;
                        case MetadataType.Boolean:
                            wrapper += "GetBooleanAt";
                            break;
                        case MetadataType.Single:
                            wrapper += "GetFloatAt";
                            break;
                        case MetadataType.Double:
                            wrapper += "GetDoubleAt";
                            break;
                        case MetadataType.IntPtr:
                            wrapper += "GetIntPtrAt";
                            break;
                        default:
                            throw new Exception($"Unsupported primitive out type: {type}.");
                    }
                    wrapper += $"({argumentIndex});\n";
                }
                argumentIndex++;
            }
            if (!isVoidMethod)
                wrapper += "\treturn retval;\n";
            wrapper += "}\n";
            return wrapper;
        }
    }
}
