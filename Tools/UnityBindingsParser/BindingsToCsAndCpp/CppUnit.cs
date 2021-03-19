using System;
using System.Collections.Generic;
using System.Linq;
using ICSharpCode.NRefactory.CSharp;

namespace BindingsToCsAndCpp
{
    public abstract class CppUnit : Unit
    {
        public Unit Base;

        public string ParamString;

        protected readonly Dictionary<string, string> _typeMap;
        protected readonly Dictionary<string, string> _retTypeMap;
        public List<ParameterDeclaration> CallParameters = new List<ParameterDeclaration>();

        public List<string> ParameterSignatureWithSelf = new List<string>();
        public List<string> ParameterSignature = new List<string>();
        public List<string> ParameterCall = new List<string>();

        string _mappedReturnType;
        public string MappedReturnType
        {
            get { return _mappedReturnType ?? base.ReturnType; }
            set { _mappedReturnType = value; }
        }
        public override bool IsSync { get; set; }
        public override bool IsThreadSafe
        {
            get { return Base.IsThreadSafe; }
            set { value = false; }
        }

        public override string Namespace
        {
            get { return Base.Namespace; }
            set {}
        }

        public string SyncString
        {
            get
            {
                return IsSync ? " self->SyncJobs();" : "";
            }
        }

        public string CustomName;
        public List<Tuple<string, string>> MappedParameterTypes = new List<Tuple<string, string>>();
        public Dictionary<string, string> FixedUpParameterNames = new Dictionary<string, string>();

        protected CppUnit(Dictionary<string, string> typeMap, Dictionary<string, string> retTypeMap)
        {
            _typeMap = typeMap;
            _retTypeMap = retTypeMap;
        }

        protected string MapTypeForReturn(AstType type)
        {
            var typeName = RemapReturnType(MapTypeForReturnGeneric(type));
            if (!IsTargetMetro)
                return typeName;
            return typeName;
        }

        private string RemapReturnType(string typeName)
        {
            if (typeName == "void*")
                return "ICallType_IntPtr_Return";

            if (typeName == "ScriptingExceptionPtr" || typeName == "ScriptingBackendNativeExceptionPtr")
                return "ICallType_Exception_Return";

            if (typeName == "ScriptingStringPtr" || typeName == "ScriptingBackendNativeStringPtr")
                return "ICallType_String_Return";

            if (typeName == "ScriptingArrayPtr" || typeName == "ScriptingBackendNativeArrayPtr")
                return "ICallType_Array_Return";

            if (typeName == "ScriptingSystemTypeObjectPtr")
                return "ICallType_SystemTypeObject_Return";

            if (IsReferenceType(typeName))
                return "ICallType_Object_Return";

            return typeName;
        }

        protected string MapTypeForReturnGeneric(AstType type)
        {
            if (type is ComposedType && ((ComposedType)type).ArraySpecifiers.Any())
                return "ScriptingBackendNativeArrayPtr";

            var cppname = type.ToString();
            if (_typeMap.TryGetValue(cppname, out cppname))
            {
                if (!cppname.Contains("ObjectOfType"))
                {
                    if (Base is CSAutoMethod || Base is CSAutoProperty)
                        return Base.IsPointer ? "ScriptingBackendNativeObjectPtr" : cppname;

                    string rettype = null;
                    if (_retTypeMap.TryGetValue(cppname, out rettype))
                        return Base.IsPointer ? "ScriptingBackendNativeObjectPtr" : rettype;

                    if (cppname.Contains("RefCounted") || cppname.Contains("ScriptingObjectWithIntPtrField"))
                        return "ScriptingBackendNativeObjectPtr";

                    return Base.IsPointer ? "ScriptingBackendNativeObjectPtr" : cppname;
                }
            }

            if (cppname != null)
            {
                var idx = cppname.IndexOf("<");
                if (idx > 0)
                {
                    string val;
                    if (_retTypeMap.TryGetValue(cppname.Substring(0, idx), out val))
                    {
                        return Base.IsPointer ? "ScriptingBackendNativeObjectPtr" : val;
                    }
                }

                if (type is PrimitiveType)
                    return cppname;
            }

            if (Base.IsPointer && type.ToString() != "void")
                return "ScriptingBackendNativeObjectPtr";
            return type.ToString();
        }

        string QueryTypeMap(ParameterDeclaration parameter, string typeString)
        {
            var hasWritableAttribute = parameter.Attributes.Any(x => x.Attributes.FirstOrDefault(a => a.Type.ToString() == "Writable") != null);
            var isWritable = hasWritableAttribute;
            var typeMapKey = isWritable ? "[Writable]" + typeString : typeString;
            string commonIncludeValue;
            if (_typeMap.TryGetValue(typeMapKey, out commonIncludeValue))
                return commonIncludeValue;

            if (typeString.StartsWith(Base.ClassName + "."))
                return null;

            return QueryTypeMap(parameter, Base.ClassName + "." + typeString);
        }

        class TypeInfo
        {
            public string ForArgument;
            public string ForLocal;

            public static TypeInfo WithBothValues(string value)
            {
                return new TypeInfo { ForArgument = value, ForLocal = value };
            }
        }

        TypeInfo TypeInfoFor(ParameterDeclaration parameter)
        {
            var type = parameter.Type;
            var typeString = type.ToString();
            var postFix = parameter.IsRef() ? "&" : parameter.IsOut() ? "*" : "";

            var typeDefPostfix = parameter.IsOut() ? "_Out" : parameter.IsRef() ? "_Ref" : "";

            if (type is ComposedType && ((ComposedType)type).ArraySpecifiers.Any())
            {
                var baseTypeName = ((ComposedType)type).BaseType.ToString();
                baseTypeName = QueryTypeMap(parameter, baseTypeName) ?? baseTypeName;
                return new TypeInfo
                {
                    ForArgument = "ICallType_Array_Argument" + typeDefPostfix + "_Generic(" + baseTypeName + ")",
                    ForLocal = "ICallType_Array_Local" + typeDefPostfix + "_Generic(" + baseTypeName + ")"
                };
            }

            if (type.IsString())
            {
                return new TypeInfo
                {
                    ForLocal = "ICallType_String_Local" + typeDefPostfix,
                    ForArgument = "ICallType_String_Argument" + typeDefPostfix
                };
            }

            var commonIncludeValue = QueryTypeMap(parameter, typeString);

            if (type.IsStruct())
            {
                var cppTypeName = commonIncludeValue ?? typeString;

                if (parameter.IsOut() || parameter.IsRef())
                    return TypeInfo.WithBothValues(cppTypeName + postFix);

                return TypeInfo.WithBothValues("const " + cppTypeName + "&");
            }

            if (type.IsIntPtr())
                return new TypeInfo { ForArgument = "ICallType_IntPtr_Argument" + typeDefPostfix, ForLocal = "void*" + postFix };

            if (commonIncludeValue != null)
            {
                bool isReferenceType = IsReferenceType(commonIncludeValue);

                if ((parameter.IsOut() || parameter.IsRef()) && isReferenceType)
                    return new TypeInfo { ForArgument = "ICallType_Object_Argument" + typeDefPostfix, ForLocal = "ICallType_Object_Local" + typeDefPostfix };

                return new TypeInfo
                {
                    ForLocal = RemapCommonInclude(commonIncludeValue + postFix),
                    ForArgument = ArgumentTypeForCommonIncludeValue(commonIncludeValue) + (isReferenceType ? typeDefPostfix : postFix),
                };
            }

            if (parameter.Name == "self" || parameter.Type.ToString() == ClassName)
                return new TypeInfo { ForArgument = "ICallType_ReadOnlyUnityEngineObject_Argument" + typeDefPostfix, ForLocal = "ReadOnlyScriptingObjectOfType<" + typeString + ">" };

            return new TypeInfo { ForArgument = typeString + postFix, ForLocal = typeString + postFix };
        }

        static string RemapCommonInclude(string commonIncludeValue)
        {
            if (commonIncludeValue == "ScriptingObjectPtr")
                return "ICallType_Object_Local";

            if (commonIncludeValue == "ScriptingStringPtr")
                return "ICallType_String_Local";

            if (commonIncludeValue == "ScriptingArrayPtr")
                return "ICallType_Array_Local";

            if (commonIncludeValue == "ScriptingExceptionPtr")
                return "ICallType_Exception_Local";

            return commonIncludeValue;
        }

        static bool IsReferenceType(string commonIncludeValue)
        {
            return (
                commonIncludeValue.StartsWith("ScriptingObjectOfType")
                || commonIncludeValue.StartsWith("Scripting::")
                || commonIncludeValue.StartsWith("ReadOnlyScriptingObjectOfType")
                || commonIncludeValue.StartsWith("ScriptingObjectWithIntPtrField")
                || commonIncludeValue == "ScriptingSystemTypeObjectPtr"
                || commonIncludeValue == "ScriptingObjectPtr"
                || commonIncludeValue == "ScriptingExceptionPtr"
                || commonIncludeValue == "ScriptingBackendNativeSystemTypeObjectPtr"
                || commonIncludeValue == "ScriptingBackendNativeObjectPtr"
                || commonIncludeValue == "ScriptingBackendNativeExceptionPtr"
            );
        }

        static string ArgumentTypeForCommonIncludeValue(string commonIncludeValue)
        {
            if (commonIncludeValue == "void*")
                return "ICallType_IntPtr_Argument";

            if (commonIncludeValue == "ScriptingExceptionPtr" || commonIncludeValue == "ScriptingBackendNativeExceptionPtr")
                return "ICallType_Exception_Argument";

            if (commonIncludeValue == "ScriptingSystemTypeObjectPtr" || commonIncludeValue == "ScriptingBackendNativeSystemTypeObjectPtr")
                return "ICallType_SystemTypeObject_Argument";

            if (commonIncludeValue == "ScriptingStringPtr" || commonIncludeValue == "ScriptingBackendNativeStringPtr")
                return "ICallType_String_Argument";

            if (commonIncludeValue == "ScriptingArrayPtr" || commonIncludeValue == "ScriptingBackendNativeArrayPtr")
                return "ICallType_Array_Argument";

            if (commonIncludeValue.StartsWith("ReadOnlyScriptingObjectOfType"))
                return "ICallType_ReadOnlyUnityEngineObject_Argument";

            if (IsReferenceType(commonIncludeValue))
                return "ICallType_Object_Argument";

            return commonIncludeValue;
        }

        protected void BuildParamSignature()
        {
            foreach (var parameter in Parameters)
            {
                var typeInfo = TypeInfoFor(parameter);

                if (typeInfo.ForLocal != typeInfo.ForArgument)
                    MappedParameterTypes.Add(Tuple.Create(parameter.Name, typeInfo.ForLocal));

                if (typeInfo.ForArgument != typeInfo.ForLocal)
                {
                    FixedUpParameterNames.Add(parameter.Name + "_", parameter.Name);
                    parameter.Name += "_";
                }

                ParameterSignatureWithSelf.Add(typeInfo.ForArgument + " " + parameter.Name);
                if (parameter.Type.ToString() == ClassName && IsSelfParameter(parameter))
                    continue;
                ParameterSignature.Add(typeInfo.ForArgument + " " + parameter.Name);
            }
        }

        protected void BuildParamCall()
        {
            foreach (var parameter in Parameters)
            {
                if (!IsStatic && parameter.Type.ToString() == ClassName && IsSelfParameter(parameter))
                    continue;

                var mapped = MapParameterForCall(parameter);
                ParameterCall.Add(mapped);
            }
        }

        public bool IsTargetMetro
        {
            get
            {
                return string.Equals(TargetPlatform, "metro", StringComparison.CurrentCultureIgnoreCase)
                    || string.Equals(TargetPlatform, "wp8", StringComparison.CurrentCultureIgnoreCase);
            }
        }

        private bool IsParameterNamed(ParameterDeclaration parameter, string name)
        {
            return parameter.Name == name || parameter.Name == name + "_";
        }

        protected bool IsSelfParameter(ParameterDeclaration parameter)
        {
            return (parameter.Type.ToString() == ClassName /*&& Base is CSProperty*/) &&
                IsParameterNamed(parameter, "self");
        }

        private string MapParameterForCall(ParameterDeclaration parameter)
        {
            var ret = "";
            if (IsSelfParameter(parameter))
                return ret;
            return FixedUpParameterNames.TryGetValue(parameter.Name, out ret) ? ret : parameter.Name;
        }
    }

    class CppText : CppUnit
    {
        public CppText(Dictionary<string, string> typeMap, Dictionary<string, string> retTypeMap, string text) : base(typeMap, retTypeMap)
        {
            OriginalContent = text;
        }
    }
}
