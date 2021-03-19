using System.Collections.Generic;
using System.Linq;
using Mono.Cecil;

namespace UnderlyingModel
{
    public class AsmEntry
    {
        public string Name { get; private set; }
        public AssemblyType EntryType { get; private set; }
        public bool Private { get; internal set; }
        public List<ParamElement> ParamList { get; internal set; }
        public List<string> GenericParamList { get; internal set; }

        public string ReturnType { get; internal set; }
        public bool IsStatic { get; internal set; }
        public bool IsProtectedUnsealed { get; set; }
        public bool IsOverride { get; internal set; }
        public bool IsProtected { get; internal set; }
        public TypeDefinition ParentCecilType { get; private set; }
        public ICustomAttributeProvider MemberCecilType { get; private set; }

        public ObsoleteInfoType ObsoleteInfo { get; private set; }

        public AsmEntry(string name, AssemblyType type, bool isStatic, TypeDefinition parentCecilType, ICustomAttributeProvider memberCecilType)
        {
            Name = CecilHelpers.Backticks2Underscores(name);
            EntryType = type;
            ParamList = new List<ParamElement>();
            GenericParamList = new List<string>();
            IsStatic = isStatic;
            ParentCecilType = parentCecilType;
            MemberCecilType = memberCecilType;
            ObsoleteInfo = CecilHelpers.GetObsoleteInfo(memberCecilType);

            //descendants of obsolete enums/classes/structs are still obsolete even if not explicitly marked as such
            while (parentCecilType != null && !ObsoleteInfo.IsObsolete)
            {
                var parentObsoleteInfo = CecilHelpers.GetObsoleteInfo(parentCecilType);
                if (parentObsoleteInfo.IsObsolete)
                    ObsoleteInfo = parentObsoleteInfo;
                parentCecilType = parentCecilType.DeclaringType;
            }

            IsProtectedUnsealed = false;
            IsOverride = false;
        }

        public string Formatted { get { return GetFormatted(false); } }
        public string FormattedHTML { get { return GetFormatted(true); } }

        public string GetFormatted(bool useHTML)
        {
            string begin = useHTML ? "<b>" : "";
            string end = useHTML ? "</b>" : "";
            string str = AssemblyTypeUtils.AssemblyTypeNameForSignature(EntryType) + " " + begin + Name + end;
            if (EntryType == AssemblyType.Method || EntryType == AssemblyType.Constructor)
            {
                if (GenericParamList.Count > 0)
                {
                    str += begin + "<";
                    str += string.Join(", ", GenericParamList.ToArray());
                    str += ">" + end;
                }

                str += begin + " (" + end;
                string[] paramList = ParamList.Select(p => (useHTML ? p.FormattedHTML() : p.Formatted())).ToArray();
                str += string.Join(begin + ", " + end, paramList);
                str += begin + ")" + end;
            }
            if (ReturnType != null)
                str += begin + " : " + end + ReturnType;
            return str;
        }

        public string SignatureDeclarationName(string curClass)
        {
            return MemberItem.SignatureDisplayName(Name, EntryType, curClass);
        }

        internal void PopulateParametersAndReturnType(MethodDefinition method)
        {
            foreach (var param in method.Parameters)
            {
                string paramType = param.ParameterType.ToString().SimplifyTypes();
                string paramModifiers =
                    CecilHelpers.IsParams(param) ? "params"
                    : CecilHelpers.IsOut(param) ? "out"
                    : CecilHelpers.IsRef(param) ? "ref"
                    : "";
                var paramElement = new ParamElement(param.Name, paramType, paramModifiers,
                    CecilHelpers.GetParameterDefaultValue(param) ?? "", param.IsOptional);
                ParamList.Add(paramElement);
            }
            foreach (GenericParameter param in method.GenericParameters)
            {
                string paramType = param.GetElementType().ToString().SimplifyTypes();
                GenericParamList.Add(paramType);
            }
            ReturnType = method.ReturnType.ToString().SimplifyTypes();
        }
    }

    public class SignatureEntry
    {
        public string Name { get; private set; }
        public AsmEntry Asm { get; set; }
        public bool InAsm { get { return Asm != null; } }
        public bool InDoc { get; private set; }
        public bool InBothAsmAndDoc { get { return InAsm && InDoc; } }
        public bool InAsmOrDoc { get { return InAsm || InDoc; } }

        public bool IsOverload { get; set; }

        public SignatureEntry(string name)
        {
            Name = name;
            IsOverload = false;
        }

        public string Formatted
        {
            get
            {
                return InAsm ? Asm.Formatted : Name.Replace("(", " (").Replace(",", ", ");
            }
        }

        public string FormattedHTML
        {
            get
            {
                return InAsm ? Asm.FormattedHTML : Name.Replace("(", " (").Replace(",", ", ");
            }
        }

        public void SetInDoc(bool b)
        {
            InDoc = b;
        }
    }
}
