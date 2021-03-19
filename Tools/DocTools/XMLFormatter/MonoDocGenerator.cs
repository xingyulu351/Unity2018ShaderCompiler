using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Xml.Linq;
using Mono.Cecil;
using UnderlyingModel;
using UnderlyingModel.MemDoc;

namespace XMLFormatter
{
    public static class MonoDocConsts
    {
        public static string Paragraph = "para";
        public static string Param = "param";
        public static string Returns = "returns";
        public static string Members = "members";
        public static string Member = "member";
        public static string Summary = "summary";
        public static string Name = "name";
    }

    public class MonoDocGenerator
    {
        static readonly Regex SquareBracketsRegex = new Regex(@"\[\[([^\]]*)\]\]");
        static readonly Regex WikiRegex = new Regex(@"\s??\[\[wiki:.*\]\]");
        static readonly Regex RefRegex = new Regex(@"([^\w\d])::p??ref::([\w\d]+)");
        static readonly Regex ClassRefRegex = new Regex(@"([\w\d]*)::p??ref::([\w\d]+)");
        static readonly Regex BoldRegex = new Regex(@"__([^_]*)__");
        static readonly Regex ItalicRegex = new Regex(@"''([^']*)''");
        static readonly Regex VariableRegex = new Regex(@"/([^/]*)/");
        static readonly Regex MonoSpacedRegex = new Regex(@"@@([^@]*)@@");
        static readonly Regex BreakRegex = new Regex(@"\s??\\\\");
        static readonly Regex SeeAlsoRegex = new Regex(@"SA:");
        static readonly Regex ReadOnlyRegex = new Regex(@"\(RO\)");
        static readonly Regex ImgRegex = new Regex(@"\s??\{img.*\}");

        private readonly IMemberGetter m_MemberGetter;

        public MonoDocGenerator(IMemberGetter project)
        {
            m_MemberGetter = project;
        }

        public IMemberGetter MemberItemProject
        {
            get { return m_MemberGetter; }
        }

        private static string GetTypePrefix(AssemblyType ass)
        {
            switch (ass)
            {
                case AssemblyType.Class:
                case AssemblyType.Struct:
                case AssemblyType.Enum:
                case AssemblyType.Delegate:
                    return "T";
                case AssemblyType.Field:
                case AssemblyType.EnumValue:
                    return "F";
                case AssemblyType.Property:
                    return "P";
                case AssemblyType.Constructor:
                case AssemblyType.Method:
                    return "M";
                case AssemblyType.Assembly:
                    return "A";
                default:
                    return "?";
            }
        }

        public List<XElement> XmlContentList(MemberItem member)
        {
            var ret = new List<XElement>();
            for (int i = 0; i < member.DocModel.SubSections.Count; i++)
            {
                var elems = XmlContentForSection(member, i);
                ret.AddRange(elems);
            }
            return ret;
        }

        private string StripMarkup(string text)
        {
            text = WikiRegex.Replace(text, ""); // Remove [[wiki]] entries
            text = SquareBracketsRegex.Replace(text, "$1"); // Replace [[Foo.Bar]] with Foo.Bar
            text = RefRegex.Replace(text, "$1$2"); // Replace (p)ref::Foo with Foo
            text = ClassRefRegex.Replace(text, "$1.$2"); // Replace Foo::(p)ref::Bar with Foo.Bar
            text = BoldRegex.Replace(text, "$1"); // Replace __bold__ with bold
            text = ItalicRegex.Replace(text, "$1"); // Replace ''italic'' with italic
            text = VariableRegex.Replace(text, "$1"); // Replace /variable/ with variable
            text = MonoSpacedRegex.Replace(text, "$1"); // Replace @@monospaced@@ with monospaced
            text = BreakRegex.Replace(text, "\n"); // Replace \\ with \n
            text = SeeAlsoRegex.Replace(text, "See Also:");
            text = ReadOnlyRegex.Replace(text, "(Read Only)");
            text = ImgRegex.Replace(text, ""); // Remove {img} entries

            return text;
        }

        private List<XElement> XmlContentForSection(MemberItem member, int i)
        {
            var elemList = new List<XElement>();
            var thisSection = member.DocModel.SubSections[i];
            for (int j = 0; j < thisSection.SignatureList.Count; j++)
            {
                var memberElem = GetMemberNameXElement(member, i, j);


                var paraElem = new XElement(MonoDocConsts.Paragraph, StripMarkup(thisSection.Summary));
                var summaryElem = new XElement(MonoDocConsts.Summary, paraElem);

                memberElem.Add(summaryElem);

                foreach (var param in thisSection.Parameters)
                {
                    var paramElem = new XElement(MonoDocConsts.Param, new XAttribute(MonoDocConsts.Name, param.Name), StripMarkup(param.Doc));
                    memberElem.Add(paramElem);
                }
                var returnDoc = thisSection.ReturnDoc;
                if (returnDoc != null && returnDoc.HasDoc)
                {
                    var returnElem = new XElement(MonoDocConsts.Returns,
                        new XElement(MonoDocConsts.Paragraph, StripMarkup(returnDoc.Doc)));
                    memberElem.Add(returnElem);
                }
                elemList.Add(memberElem);
            }
            return elemList;
        }

        public XElement GetMemberNameXElement(MemberItem member, int i, int j)
        {
            var memberElem = new XElement("member");

            var fullName = GetMonoDocName(member, i, j);
            var memberMonoDocName = string.Format("{0}:{1}", GetTypePrefix(member.ItemType), fullName);
            var paramAppendix = GetTypeSignature(member, i, j);
            memberMonoDocName += paramAppendix;
            memberElem.SetAttributeValue("name", memberMonoDocName);
            return memberElem;
        }

        private static string GetMonoDocName(MemberItem member, int sectionIndex, int signatureIndex)
        {
            var fullName = member.ItemName;
            var names = member.GetNamesForMonoDoc();

            if (AssemblyTypeUtils.CanHaveChildren(member.ItemType))
            {
                if (sectionIndex > 0 && signatureIndex > 0)
                    fullName = member.DocModel.SubSections[sectionIndex].SignatureEntryList[signatureIndex].Asm.MemberCecilType.ToString();
                else if (member.FirstCecilType != null)
                {
                    fullName = member.FirstCecilType.FullName;
                    fullName = fullName.Replace('/', '.'); //nested classes / enums are denoted by slash in Mono.Cecil, but we want the . in the name
                }
            }
            else if (member.ItemType == AssemblyType.Constructor)
            {
                fullName = names.NamespaceAndClass + ".#ctor";
            }
            else
            {
                fullName = names.FullName;
            }
            return fullName;
        }

        private static string GetTypeSignature(MemberItem member, int i, int j)
        {
            SignatureEntry sig;

            if (member.DocModel.SubSections.Count == 0)
            {
                sig = member.Signatures[j];
            }
            else
            {
                sig = member.DocModel.SubSections[i].SignatureEntryList[j];
            }

            if (sig.Asm == null || sig.Asm.ParamList.Count <= 0)
                return null;

            var methodInfo = sig.Asm.MemberCecilType as MethodDefinition;
            var sb = new StringBuilder();
            if (methodInfo != null)
            {
                var sigParams = StringConvertUtils.SignatureToParamList(sig.Name);
                var sigParamCount = sigParams.Count;

                if (sigParamCount > 0)
                    sb.AppendFormat("({0})", string.Join(",", methodInfo.Parameters.ToList().GetRange(0, sigParamCount).Select(e => e.ParameterType.ToString().Replace("0...", "")).ToArray()));
                else
                    sb.Append("()");
            }
            return sb.ToString();
        }
    }
}
