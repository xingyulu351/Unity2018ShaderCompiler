using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;
using Mono.Cecil;
using UnderlyingModel;

namespace XMLFormatter
{
    internal class XMLFormatterAsmEntry : IXMLFormatter
    {
        private readonly AsmEntry m_AsmEntry;
        private readonly IMemberGetter m_Getter;
        private readonly string m_CurClass;

        public XMLFormatterAsmEntry(AsmEntry asmEntry, IMemberGetter getter, string curClass)
        {
            m_AsmEntry = asmEntry;
            m_Getter = getter;
            m_CurClass = curClass;
        }

        public XElement FormattedXML()
        {
            string typeName = AssemblyTypeUtils.AssemblyTypeNameForSignature(m_AsmEntry.EntryType);
            var attList = new List<XAttribute>
            {
                new XAttribute("name", m_AsmEntry.SignatureDeclarationName(m_CurClass)),
                new XAttribute("type", typeName)
            };
            var declaringType = m_AsmEntry.MemberCecilType as TypeReference;
            if (declaringType == null)
            {
                var fr = (m_AsmEntry.MemberCecilType as FieldReference);
                if (fr != null)
                    declaringType = fr.DeclaringType;
            }
            var ns = declaringType != null ? CecilHelpers.NamespaceFromDescendants(declaringType) : "";

            if (!ns.IsEmpty())
            {
                attList.Add(new XAttribute("namespace", ns));
            }

            var modifiersSb = new StringBuilder();

            modifiersSb.Append(m_AsmEntry.IsProtectedUnsealed || m_AsmEntry.IsProtected ? "protected" : "public");
            if (m_AsmEntry.IsStatic)
                modifiersSb.Append(" static");

            if (!modifiersSb.ToString().IsEmpty())
                attList.Add(new XAttribute("modifiers", modifiersSb.ToString()));
            var declElement = new XElement("Declaration", attList);

            var elementList = new List<XElement> {declElement};
            if (AssemblyTypeUtils.MultipleSignaturesPossibleForType(m_AsmEntry.EntryType) || m_AsmEntry.EntryType == AssemblyType.Delegate)
                elementList.AddRange(m_AsmEntry.ParamList.Select(m => new XMLFormatterParamElement(m, m_Getter).FormattedXML()));

            var returnType = new XElement("ReturnType", m_AsmEntry.ReturnType);

            //Obsolete attribute
            string obsolete;
            if (m_AsmEntry.ObsoleteInfo.ObsoleteText != "")
                obsolete = "Obsolete ";
            else
                obsolete = "";

            declElement.SetAttributeValue("Obsolete", obsolete);

            if (m_AsmEntry.ReturnType != null)
            {
                var returnTypeName = m_AsmEntry.ReturnType;
                var returnTypeMember = m_Getter.GetClosestNonObsoleteMember(returnTypeName);

                if (returnTypeMember != null && !returnTypeMember.IsUndoc)
                {
                    returnType = new XElement("ReturnType", returnTypeMember.ItemName);
                    if (returnTypeMember.ItemName != returnTypeMember.DisplayName)
                    {
                        returnType.SetAttributeValue("DisplayName", returnTypeMember.DisplayName);
                    }
                    if (returnTypeMember.AnyHaveDoc)
                        returnType.SetAttributeValue("hasLink", true);
                }
            }

            elementList.Add(returnType);
            return new XElement("Signature", elementList);
        }
    }
}
