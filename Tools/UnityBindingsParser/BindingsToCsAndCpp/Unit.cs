using System.Collections.Generic;
using System.Linq;
using ICSharpCode.NRefactory.CSharp;

namespace BindingsToCsAndCpp
{
    public class Unit
    {
        public string TargetPlatform;
        public virtual Unit Parent { get; set; }

        public AstNode AstNode;
        public AstType AstReturnType;
        public string Name;
        public virtual string Namespace { get; set; }

        string _internalName;
        public string InternalName
        {
            get { return _internalName ?? Name; }
            set { _internalName = value; }
        }

        public virtual string ReturnType
        {
            get { return AstReturnType.IsNull ? "void" : AstReturnType.ToString(); }
        }

        public string Visibility;
        public bool IsStatic;
        public bool IsOverride;
        public bool IsNew;
        public string Signature;
        public string ClassName;
        public virtual bool IsThreadSafe { get; set; }
        public bool IsObsolete;
        public string ObsoleteKind;
        public string ObsoleteText;
        public bool ParentIsStruct;
        public virtual bool IsSync { get; set; }
        public List<ParameterDeclaration> Parameters;

        public virtual string StaticModifier { get { return IsStatic ? "static" : ""; } }

        protected IndentedWriter output = new IndentedWriter();
        public string Body;
        public bool NeedsInternal;
        public List<string> Conditions = new List<string>();
        public string OriginalContent;
        protected const string SelfFormat = "{0}{1} self";

        public string GenerateOutput(int indentationLevel)
        {
            output.Clear();
            output.CurrentIndentationLevel = indentationLevel;
            Generate();
            return output.ToString();
        }

        public string Output
        {
            get
            {
                output.Clear();
                Generate();
                return output.ToString();
            }
        }

        public virtual bool IsPointer { get; set; }

        public virtual void Generate()
        {
        }

        protected string GetDirectionForSignature(ParameterDeclaration parameter)
        {
            return parameter.GetDirection();
        }

        protected void Init(string className, EntityDeclaration declaration, string defaultVisibility = "public")
        {
            Name = declaration.GetName();
            ClassName = className;
            AstReturnType = declaration.GetReturnType();
            Visibility = declaration.GetVisibility(defaultVisibility);
            IsStatic = declaration.IsStatic();
            IsOverride = declaration.HasModifier(Modifiers.Override);
            IsNew = declaration.HasModifier(Modifiers.New);

            AstNode = declaration;
            Parameters = declaration.GetParameters().ToList();
        }
    }
}
