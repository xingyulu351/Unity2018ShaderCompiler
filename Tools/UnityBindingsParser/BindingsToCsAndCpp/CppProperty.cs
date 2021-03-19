using System.Collections.Generic;
using System.Linq;
using System.Text.RegularExpressions;
using ICSharpCode.NRefactory.CSharp;

namespace BindingsToCsAndCpp
{
    public class CppProperty : CppUnit
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
            get { return _parent ?? (_parent = Base.Parent); }
            set { _parent = value; }
        }

        public static IEnumerable<CppUnit> ParseProperty(Dictionary<string, string> typeMap, Dictionary<string, string> retTypeMap, CSBaseProperty unit)
        {
            var units = new List<CppProperty>();
            units.Add(new CppProperty(typeMap, retTypeMap, unit));
            if (unit.HasSetter)
                units.Add(new CppProperty(typeMap, retTypeMap, unit, true));
            return units;
        }

        public CppProperty(Dictionary<string, string> typeMap, Dictionary<string, string> retTypeMap, CSBaseProperty property, bool setter = false) : base(typeMap, retTypeMap)
        {
            Base = property;
            Init(property.ClassName, property.AstNode as EntityDeclaration);
            Init(property, setter);
        }

        void Init(CSBaseProperty unit, bool setter)
        {
            Conditions = unit.Conditions;
            TargetPlatform = unit.TargetPlatform;
            var capitalizedName = unit.Name[0].ToString().ToUpper() + unit.Name.Substring(1);

            if (setter)
            {
                if (unit.NeedsInternal)
                    CustomName = "CUSTOM_" + unit.SetterInternalName;
                else
                    CustomName = "Set_Custom_Prop" + unit.Name[0].ToString().ToUpper() + unit.Name.Substring(1);
                Name = "set_" + unit.Name;
                InternalName = unit.SetterInternalName;
            }
            else
            {
                if (unit.NeedsInternal)
                    CustomName = "CUSTOM_" + unit.GetterInternalName;
                else
                    CustomName = "Get_Custom_Prop" + capitalizedName;
                Name = "get_" + unit.Name;
                InternalName = unit.GetterInternalName;
            }

            if (!unit.IsStatic)
                Parameters = Parameters.Prepend(new ParameterDeclaration(new SimpleType(ClassName), "self")).ToList();

            if (setter)
            {
                if (unit.IsPointer)
                    Parameters.Add(new ParameterDeclaration(AstReturnType.Clone(), "val"));
                else
                    Parameters.Add(new ParameterDeclaration(AstReturnType.Clone(), "value"));
                AstReturnType = new PrimitiveType("void");
            }
            else if (AstReturnType.ShouldRewriteReturnValueAsParameterByReference() && unit.NeedsInternal)
            {
                Parameters.Add(new ParameterDeclaration(AstReturnType.Clone(), "returnValue", ParameterModifier.Out));
                AstReturnType = new PrimitiveType("void");

                if (unit is CSAutoProperty)
                {
                    var parts = ParsingHelpers.ParseAutoProp(unit.OriginalContent);
                    Body = "*returnValue = self->" + parts.Item2 + "();\n";
                }
                else
                {
                    Body = Regex.Replace(unit.Getter, @"^\s*return (.*)\s*;$", "{ *returnValue =($1); return;};", RegexOptions.Multiline);
                }
            }

            BuildParamSignature();
            MappedReturnType = MapTypeForReturn(AstReturnType);
            if (Body == null)
                Body = setter ? unit.Setter : unit.Getter;
        }
    }
}
