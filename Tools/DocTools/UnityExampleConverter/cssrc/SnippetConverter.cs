using System;
using System.IO;
using Boo.Lang.Compiler.Ast;
using Boo.Lang.Compiler.Ast.Visitors;
using Boo.Lang.Compiler.IO;

namespace UnityExampleConverter
{
    public class SnippetConverter
    {
        public readonly bool m_ConvertExamples;

        [ThreadStatic] static UnityScriptConverter s_PerThreadConverter;

        public SnippetConverter(bool convertExamples)
        {
            m_ConvertExamples = convertExamples;
        }

        public static UnityScriptConverter PerThreadConverter
        {
            get { return s_PerThreadConverter ?? (s_PerThreadConverter = CreateUnityScriptConverter()); }
        }

        private static UnityScriptConverter CreateUnityScriptConverter()
        {
            var converter = new UnityScriptConverter();
            var assembliesBasePath = Path.GetFullPath("../../build/CombinedAssemblies/");
            var unityEngineAssembly = System.Reflection.Assembly.LoadFile(Path.Combine(assembliesBasePath, "UnityEngine.dll"));

            foreach (var reference in unityEngineAssembly.GetReferencedAssemblies())
            {
                var path = Path.Combine(assembliesBasePath, reference.Name + ".dll");
                if (File.Exists(path))
                    converter.References.Add(System.Reflection.Assembly.LoadFile(path));
            }
            converter.References.Add(System.Reflection.Assembly.LoadFile(Path.Combine(assembliesBasePath, "UnityEditor.dll")));
            return converter;
        }

        public ConversionResult Convert(string javaScriptText)
        {
            PerThreadConverter.Input.Add(new StringInput("ExampleClass", javaScriptText));
            try
            {
                var convertedCode = PerThreadConverter.Run();
                return new ConversionResult(CSharpCodeFor(convertedCode), BooCodeFor(convertedCode));
            }
            finally
            {
                PerThreadConverter.Input.Clear();
            }
        }

        public struct ConversionResult
        {
            public readonly bool m_IsConverted;
            public readonly string CSharpCode;
            public readonly string BooCode;

            public ConversionResult(bool converted)
            {
                m_IsConverted = converted;
                CSharpCode = "";
                BooCode = "";
            }

            public ConversionResult(string cSharpCode, string booCode)
            {
                m_IsConverted = true;
                CSharpCode = cSharpCode;
                BooCode = booCode;
            }
        }

        private static string CSharpCodeFor(CompileUnit[] convertedCode)
        {
            var csharp = new StringWriter();
            convertedCode[0].Accept(new CSharpPrinter(csharp));
            return csharp.ToString();
        }

        private static string BooCodeFor(CompileUnit[] convertedCode)
        {
            var boo = new StringWriter();
            convertedCode[0].Accept(new BooPrinterVisitor(boo));
            return boo.ToString();
        }
    }
}
