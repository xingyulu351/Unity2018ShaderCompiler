using System.Collections.Generic;
using System.Linq;
using System.Text.RegularExpressions;
using ICSharpCode.NRefactory.CSharp;

namespace BindingsToCsAndCpp
{
    public class CppMethod : CppUnit
    {
        public override bool IsSync
        {
            get
            {
                return Base.IsSync;
            }
        }

        private Unit _parent;
        public override Unit Parent
        {
            get
            {
                if (_parent == null)
                    _parent = Base.Parent;
                return _parent;
            }
            set { _parent = value; }
        }

        public CppMethod(Dictionary<string, string> typeMap, Dictionary<string, string> retTypeMap, CSAutoMethod method) :
            this(typeMap, retTypeMap, method as CSBaseMethod)
        {
            BuildParamCall();
            SetBody(method);
        }

        public CppMethod(Dictionary<string, string> typeMap, Dictionary<string, string> retTypeMap, CSConstructor method) :
            this(typeMap, retTypeMap, method as CSBaseMethod)
        {
            AstReturnType = new PrimitiveType("void");
            MappedReturnType = "void";
            BuildParamCall();
            SetBody(method);
            if (!method.NeedsInternal)
                InternalName = ".ctor";
        }

        public CppMethod(Dictionary<string, string> typeMap, Dictionary<string, string> retTypeMap, CSRawMethod method) :
            this(typeMap, retTypeMap, method as CSBaseMethod)
        {
        }

        public CppMethod(Dictionary<string, string> typeMap, Dictionary<string, string> retTypeMap, CSMethod method) :
            this(typeMap, retTypeMap, method as CSBaseMethod)
        {
            SetBody(method);
        }

        CppMethod(Dictionary<string, string> typeMap, Dictionary<string, string> retTypeMap, CSBaseMethod method) : base(typeMap, retTypeMap)
        {
            Base = method;
            Init(method.ClassName, method.AstNode as EntityDeclaration);
            Init(method);
        }

        void Init(CSBaseMethod unit)
        {
            TargetPlatform = unit.TargetPlatform;
            ParamString = unit.ParamString;
            InternalName = unit.InternalName;
            CustomName = "CUSTOM_" + InternalName;

            if (!IsStatic)
            {
                var parameterModifier = ParentIsStruct ? ParameterModifier.Ref : ParameterModifier.None;
                Parameters = Parameters.Prepend(new ParameterDeclaration(new SimpleType(ClassName), "self", parameterModifier)).ToList();
            }

            BuildParamSignature();
            MappedReturnType = MapTypeForReturn(AstReturnType);

            if (AstReturnType.ShouldRewriteReturnValueAsParameterByReference())
            {
                ParameterSignatureWithSelf.Add(MappedReturnType + "& returnValue");
                ParameterSignature.Add(MappedReturnType + "& returnValue");
                MappedReturnType = "void";
            }

            Conditions = unit.Conditions;
        }

        private void SetBody(CSAutoMethod method)
        {
            const string sig = "{0}{1}{2}({3});";
            var ret = AstReturnType.ShouldRewriteReturnValueAsParameterByReference() ? "returnValue = " : "return "; // why is cspreprocess always returning even in void methods?
            var self = "";
            if (!IsStatic && Parameters.FirstOrDefault(x => x.Type.ToString() == ClassName && IsSelfParameter(x)) != null)
                self = "self->";
            Body = string.Format(sig, ret, self, Name, ParameterCall.CommaSeparatedList());
        }

        private void SetBody(CSMethod method)
        {
            Body = ParsingHelpers.RemoveComments(method.Body.Split('\n'));

            if (AstReturnType.ShouldRewriteReturnValueAsParameterByReference())
            {
                Body = Regex.Replace(Body, @"return (.*);", "{ returnValue =($1); return; }");
            }
        }
    }
}
