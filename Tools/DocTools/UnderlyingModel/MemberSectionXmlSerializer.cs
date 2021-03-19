using System.Collections.Generic;
using System.Linq;
using System.Xml;
using System.Xml.Linq;

namespace UnderlyingModel.MemDoc.Serializers
{
    public class MemberSectionXmlSerializer
    {
        public uint Version { private get; set; }
        private const bool UseCData = false;

        public XElement Serialize(MemberSubSection section, bool includeSignatureDeclaration)
        {
            if (section.IsEmpty() && !section.SignatureList.Any())
                return null;

            var sigElement = new XElement(XmlTags.Section);
            if (includeSignatureDeclaration)
            {
                var sigList = section.SignatureList.Select(
                    sig => CreatePlainTextElement(XmlTags.Signature, sig)).ToList();
                sigElement.Add(sigList);
            }
            if (section.IsDocOnly)
                sigElement.SetAttributeValue(XmlTags.DocOnly, "true");

            if (section.IsUndoc)
                sigElement.SetAttributeValue(XmlTags.Undoc, "true");

            if (!section.Summary.IsEmpty())
                sigElement.Add(CreatePlainTextElement(XmlTags.Summary, section.Summary));

            //param and return handling
            foreach (var param in section.Parameters)
                if (param.HasDoc)
                {
                    var paramElement = CreatePlainTextElement(XmlTags.Param, param.Doc);
                    paramElement.SetAttributeValue(XmlTags.ID, param.Name);
                    sigElement.Add(paramElement);
                }

            var retDoc = section.ReturnDoc;
            if (retDoc != null && retDoc.HasDoc)
                sigElement.Add(CreatePlainTextElement(XmlTags.Return, retDoc.Doc));

            foreach (var textOrExample in section.TextBlocks)
            {
                if (textOrExample is DescriptionBlock)
                {
                    if (textOrExample.Text != "")
                        sigElement.Add(new XElement(XmlTags.Description, textOrExample.Text));
                }
                if (textOrExample is ExampleBlock)
                {
                    //examples don't necessarily follow descriptions
                    var exampleBlock = textOrExample as ExampleBlock;
                    var exampleElement = SerializeExampleBlock(exampleBlock);
                    sigElement.Add(exampleElement);
                }
            }

            return sigElement;
        }

        private XElement SerializeExampleBlock(ExampleBlock exampleBlock)
        {
            var exampleElement = new XElement(XmlTags.Example, "");
            if (Version == 1)
            {
                exampleElement = CreatePlainTextElement(XmlTags.Example, exampleBlock.PureText());
            }
            else if (Version == 2)
            {
                if (exampleBlock.JsExample != "")
                {
                    var jsElement = CreatePlainTextElement(XmlTags.JS, exampleBlock.JsExample.TrimEnd());
                    exampleElement.Add(jsElement);
                }
                if (exampleBlock.CsExample != "")
                {
                    var csElement = CreatePlainTextElement(XmlTags.CS, exampleBlock.CsExample.TrimEnd());
                    exampleElement.Add(csElement);
                }
            }
            else if (Version == 3)
            {
                var exampleXElementList = new List<XElement>();
                if (exampleBlock.JsExample != "")
                {
                    //TODO: re-enable when the new trunk format is in effect
                    //var jsElement = CreatePlainTextElement(XmlTags.Code, PrepareForAutoFormatting(exampleBlock.JsExample));
                    var jsElement = CreatePlainTextElement(XmlTags.Code, exampleBlock.JsExample);
                    jsElement.SetAttributeValue(XmlTags.Lang, XmlTags.JS);
                    exampleXElementList.Add(jsElement);
                }
                if (exampleBlock.CsExample != "")
                {
                    //TODO: re-enable when the new trunk format is in effect
                    //var csElement = CreatePlainTextElement(XmlTags.Code, PrepareForAutoFormatting(exampleBlock.CsExample));
                    var csElement = CreatePlainTextElement(XmlTags.Code, exampleBlock.CsExample);
                    csElement.SetAttributeValue(XmlTags.Lang, XmlTags.CS);
                    exampleXElementList.Add(csElement);
                }
                exampleElement = new XElement(XmlTags.Example, exampleXElementList);
            }
            if (exampleBlock.IsConvertExample)
                exampleElement.SetAttributeValue(XmlTags.ConvertExample, "true");
            if (exampleBlock.IsNoCheck)
                exampleElement.SetAttributeValue(XmlTags.NoCheck, "true");
            return exampleElement;
        }

        public MemberSubSection Deserialize(XmlElement sectionElement)
        {
            var subsection = new MemberSubSection();
            var summaryEl = sectionElement.GetElementsByTagName(XmlTags.Summary).Item(0) as XmlElement;
            if (summaryEl != null)
                subsection.Summary = ParsePlainTextElement(XmlTags.Summary, summaryEl);
            if (sectionElement.HasAttribute(XmlTags.Undoc))
                subsection.IsUndoc = true;
            if (sectionElement.HasAttribute(XmlTags.DocOnly))
                subsection.IsDocOnly = true;
            var sigNodes = sectionElement.SelectNodes(XmlTags.Signature);
            if (sigNodes != null)
                foreach (var node in sigNodes)
                {
                    var sig = ParsePlainTextElement(XmlTags.Signature, (XmlNode)node);
                    subsection.SignatureList.Add(sig);
                }

            var paramElements = sectionElement.SelectNodes(XmlTags.Param);
            foreach (var param in paramElements)
            {
                var paramXml = (XmlElement)param;
                subsection.Parameters.Add(new ParameterWithDoc(paramXml.GetAttribute(XmlTags.ParamName))
                {
                    Doc = ParsePlainTextElement(XmlTags.Param, paramXml)
                });
            }

            var retElement = sectionElement.SelectNodes(XmlTags.Return).Item(0);
            if (retElement != null)
            {
                var returnDoc = ParsePlainTextElement(XmlTags.Return, retElement);

                subsection.ReturnDoc = new ReturnWithDoc(returnDoc);
            }

            var xPathDescriptionOrExample = string.Format("{0}|{1}", XmlTags.Description, XmlTags.Example);
            var textBlocksXml = sectionElement.SelectNodes(xPathDescriptionOrExample);
            foreach (var block in textBlocksXml)
            {
                var blockElement = block as XmlElement;
                if (blockElement.Name == XmlTags.Description)
                {
                    var plainTextElement = ParsePlainTextElement(XmlTags.Description, blockElement);
                    subsection.TextBlocks.Add(new DescriptionBlock(plainTextElement));
                }
                else if (blockElement.Name == XmlTags.Example)
                {
                    DeserializeExampleBlock(blockElement, subsection);
                }
            }
            return subsection;
        }

        private void DeserializeExampleBlock(XmlElement blockElement, MemberSubSection subsection)
        {
            var exampleBlock = new ExampleBlock("dummy");
            if (Version == 1)
                exampleBlock = new ExampleBlock(ParsePlainTextElement(XmlTags.Example, blockElement));
            else if (Version == 2)
            {
                var jsNode = blockElement.SelectSingleNode(XmlTags.JS);

                var csNode = blockElement.SelectSingleNode(XmlTags.CS);
                var jsText = (jsNode != null) ? ParsePlainTextElement(XmlTags.JS, jsNode) : "";
                var csText = (csNode != null) ? ParsePlainTextElement(XmlTags.CS, csNode) : "";
                exampleBlock = new ExampleBlock(jsText, csText);
            }
            else if (Version == 3)
            {
                var xPathJs = string.Format("{0}[@{1}='{2}']", XmlTags.Code, XmlTags.Lang, XmlTags.JS);
                var xPathCs = string.Format("{0}[@{1}='{2}']", XmlTags.Code, XmlTags.Lang, XmlTags.CS);
                var jsNode = blockElement.SelectSingleNode(xPathJs);
                var csNode = blockElement.SelectSingleNode(xPathCs);
                var jsText = (jsNode != null) ? PostprocessAutoFormatting(ParsePlainTextElement(XmlTags.Code, jsNode)) : "";
                var csText = (csNode != null) ? PostprocessAutoFormatting(ParsePlainTextElement(XmlTags.Code, csNode)) : "";
                exampleBlock = new ExampleBlock(jsText, csText);
            }
            if (blockElement.HasAttribute(XmlTags.ConvertExample))
                exampleBlock.IsConvertExample = true;
            if (blockElement.HasAttribute(XmlTags.NoCheck))
                exampleBlock.IsNoCheck = true;
            subsection.TextBlocks.Add(exampleBlock);
        }

        private static string PrepareForAutoFormatting(string original)
        {
            if (!original.Contains("\n"))
            {
                return original;
            }
            var ret = original;
            if (!ret.StartsWith("\n"))
            {
                ret = "\n" + ret;
            }
            if (!ret.EndsWith("\n"))
            {
                ret += "\n";
            }
            return ret;
        }

        private static string PostprocessAutoFormatting(string original)
        {
            if (!original.Contains("\n"))
            {
                return original.Trim();
            }
            var ret = original.TrimEnd();
            if (ret != original)
            {
                ret += "\n";
            }
            return ret;
        }

        private static bool UseCDataForThisType(string xmltag)
        {
            return xmltag == XmlTags.Signature || xmltag == XmlTags.Example
                || xmltag == XmlTags.CS || xmltag == XmlTags.JS || xmltag == XmlTags.Code;
        }

        private XElement CreatePlainTextElement(string xmltag, string text)
        {
            object ret;
            if (UseCData || UseCDataForThisType(xmltag))
                ret = new XCData(text);
            else
                ret = text; //XmlUtils.XmlEscape(text);
            return new XElement(xmltag, ret);
        }

        private string ParsePlainTextElement(string xmltag, XmlNode x)
        {
            if (UseCData || UseCDataForThisType(xmltag))
                return x.InnerText;
            else
                return XmlUtils.XmlUnescape(x.InnerXml);
        }
    }
}
