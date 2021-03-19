using System;
using System.Collections.Generic;
using System.IO;
using ICSharpCode.NRefactory.CSharp;
using Unity.CommonTools;

namespace BindingsToCsAndCpp.Tests
{
    //[TestFixture]
    public class CppGenerationTests
    {
        //[Test]
        public void Generate()
        {
            var typeMap = MapBuilder.BuildFrom<Dictionary<string, string>>(File.ReadAllLines(Workspace.BasePath + "/Runtime/Export/common_include"), MapBuilder.MapType.Type);
            var retTypeMap = ParsingHelpers.RetTypeMap;

            var method = new CSAutoMethod("MyClass", "void EnsureQuaternionContinuity(bool test);");
            var cppmethod = new CppMethod(typeMap, retTypeMap, method);

            var export = CppExports.Prepare("FILENAME", new List<CppUnit> {cppmethod}, "");
            export.Generate();
            Console.WriteLine(export.Output);

            const string str = @"
#if TRUE
void EnsureQuaternionContinuity(MyClass test) {}
#endif
";
            var ret = CSharpSignatureAnalyzer.Analyze("MyClass", str);
            var result = ret.ParseCSRawMethods();
            foreach (var m in result)
            {
                if (m.Item2 == null)
                    continue;
                var rawmethod = new CSRawMethod("MyClass", m.Item1, m.Item2 as MethodDeclaration);
                var cpprawmethod = new CppMethod(typeMap, retTypeMap, rawmethod);
            }
        }
    }
}
