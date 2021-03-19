using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Text;
using System.Xml.Linq;
using ScriptRefBase;
using UnderlyingModel;
using UnderlyingModel.MemDoc;
using UnityExampleConverter;

namespace XMLFormatter
{
    internal class XMLFormatterExampleBlock : IXMLFormatter
    {
        private readonly ExampleBlock m_Block;
        private readonly LinkResolverWithMemSubstitutions m_LinkResolver;
        private readonly SnippetConverter m_SnippetConverter;
        private const string DummyCsText = "no example available in C#";
        private const string DummyJsText = "no example available in JavaScript";

        public XMLFormatterExampleBlock(ExampleBlock block, SnippetConverter sc, LinkResolverWithMemSubstitutions linkResolver)
        {
            m_Block = block;
            m_SnippetConverter = sc;
            m_LinkResolver = linkResolver;
        }

        struct ExamplesIR
        {
            public MemDocElementList JavaScriptIR;
            public MemDocElementList CSharpIR;
        }

        ExamplesIR GetIRFromExample()
        {
            var ir = new ExamplesIR();
            string javaScriptText = DummyJsText;
            string cSharpScriptText = DummyCsText;
            //both languages are present, don't convert
            switch (m_Block.DetectedSourceLang)
            {
                case CompLang.Dual:
                    javaScriptText = m_Block.JsExample;
                    cSharpScriptText = m_Block.CsExample;
                    break;
                case CompLang.JavaScript:
                    javaScriptText = m_Block.JsExample;

                    //if JS and convertExample => convert
                    if (m_SnippetConverter.m_ConvertExamples && m_Block.IsConvertExample)
                    {
                        try
                        {
                            cSharpScriptText = ConvertJsToCs(javaScriptText);
                        }
                        catch (Exception ex)
                        {
                            Console.WriteLine(ex);
                            //some examples fail to convert, so we lave cSharpScriptText as is
                        }
                    }
                    break;
                case CompLang.CSharp:
                    cSharpScriptText = m_Block.CsExample;

                    if (m_SnippetConverter.m_ConvertExamples && m_Block.IsConvertExample)
                    {
                        try
                        {
                            javaScriptText = ConvertCs2Us(cSharpScriptText);
                        }
                        catch (Exception ex)
                        {
                            Console.WriteLine(ex);
                        }
                    }

                    break;
            }
            if (cSharpScriptText == null || cSharpScriptText.Length == 0 || cSharpScriptText == "\n")
                cSharpScriptText = DummyCsText;
            if (javaScriptText == null || javaScriptText.Length == 0 || javaScriptText == "\n")
                javaScriptText = DummyJsText;
            ir.JavaScriptIR = m_LinkResolver.ParseDescriptionWithMemSubstitutions(javaScriptText);
            ir.CSharpIR = m_LinkResolver.ParseDescriptionWithMemSubstitutions(cSharpScriptText);

            return ir;
        }

        public XElement FormattedXML()
        {
            XElement returnElement;

            //javascript example not explicitly marked for conversion
            if (m_Block.DetectedSourceLang == CompLang.JavaScript && (!m_SnippetConverter.m_ConvertExamples || !m_Block.IsConvertExample))
            {
                var unconvertedIR = m_LinkResolver.ParseDescriptionWithMemSubstitutions(m_Block.JsExample);
                var unconvertedXML = MemDocOutputterXML.GetXML("Unconverted", unconvertedIR);
                returnElement = new XElement("Example",
                    new XAttribute("nocheck", m_Block.IsNoCheck),
                    new XAttribute("convertexample", m_Block.IsConvertExample),
                    unconvertedXML);
            }
            else
            {
                var ir = GetIRFromExample();
                var exampleList = new List<XElement>
                {
                    MemDocOutputterXML.GetXML("JavaScript", ir.JavaScriptIR),
                    MemDocOutputterXML.GetXML("CSharp", ir.CSharpIR)
                };

                returnElement = new XElement("Example",
                    new XAttribute("nocheck", m_Block.IsNoCheck),
                    new XAttribute("convertexample", m_Block.IsConvertExample),
                    exampleList);
            }
            return returnElement;
        }

        private string ConvertJsToCs(string javaScriptText)
        {
            //might throw an exception, catch it in the caller
            var result = m_SnippetConverter.Convert(javaScriptText);
            return result.CSharpCode;
        }

        private string ConvertCs2Us(string csScriptText)
        {
            var tempPath = Path.GetTempPath();
            var tempCsExamplePath = Path.Combine(tempPath, "example.cs");
            var tempJsExamplePath = Path.Combine(tempPath, "example.js");
            File.WriteAllText(tempCsExamplePath, csScriptText);
            var psi = ProcessUtil.GetStartInfo("Tools/DocTools/Cs2us/bin/Debug/cs2us.exe",
                tempCsExamplePath + " \"" + tempJsExamplePath + "\"");
            psi.RedirectStandardOutput = true;
            psi.RedirectStandardError = true;

            var p = new Process { StartInfo = psi };
            var sb = new StringBuilder();
            var sbError = new StringBuilder();
            p.OutputDataReceived += (sender, eventargs) => sb.AppendLine(eventargs.Data);
            p.ErrorDataReceived += (sender, eventargs) => sbError.AppendLine(eventargs.Data);
            try
            {
                p.Start();
            }
            catch (Exception)
            {
                Console.WriteLine("Unable to execute converter at dir={0}, file={1}", psi.WorkingDirectory, psi.FileName);
                return "";
            }
            p.BeginErrorReadLine();
            p.BeginOutputReadLine();

            if (!p.WaitForExit(10000))
            {
                Console.WriteLine("the converter process didn't finish within 10s, likely a conversion error");
                return "";
            }
            if (p.ExitCode != 0)
            {
                var convertedText = "converter returned error " + p.ExitCode + "\noutput = " + sb + "\nError = " + sbError;
                Console.WriteLine("converter exited with code {0}, error= '{1}'", p.ExitCode, convertedText);
                return "";
            }
            var usResult = File.ReadAllText(tempJsExamplePath);
            return usResult;
        }
    }
}
