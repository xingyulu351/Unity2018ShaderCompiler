using System;
using System.Collections.Generic;
using System.Linq;
using NUnit.Framework;
using nrefact = ICSharpCode.NRefactory.CSharp;

namespace BindingsToCsAndCpp
{
    [TestFixture]
    class CSharpSignatureAnalyzerTest
    {
        [Test]
        public void InstanceMethod()
        {
            var method = CSharpSignatureAnalyzer.AnalyzeMethod("MyClass", "public void MyMethod()");
            Assert.IsNotNull(method);
            Assert.IsFalse(method.HasModifier(nrefact.Modifiers.Static));
        }

        [Test]
        public void Constructor()
        {
            var method = CSharpSignatureAnalyzer.AnalyzeMethod("MyClass", "public MyClass()");
            Assert.IsNotNull(method);
            Assert.IsFalse(method.HasModifier(nrefact.Modifiers.Static));
        }

        [Test]
        public void StaticMethod()
        {
            var method = CSharpSignatureAnalyzer.AnalyzeMethod("MyClass", "public static void MyMethod()");
            Assert.IsNotNull(method);
            Assert.IsTrue(method.HasModifier(nrefact.Modifiers.Static));
        }

        [Test]
        public void WithIntegerArgument()
        {
            var method = CSharpSignatureAnalyzer.AnalyzeMethod("MyClass", "public static void MyMethod(int a)");
            Assert.IsNotNull(method);
            var parameterDeclaration = method.GetParameters().First();
            Assert.AreEqual("a", parameterDeclaration.Name);
            Assert.AreEqual("int", parameterDeclaration.Type.ToString());
        }

        [Test]
        public void WithArrayArgument()
        {
            var method = CSharpSignatureAnalyzer.AnalyzeMethod("MyClass", "public static void MyMethod(SomeNameSpace.SomeType[] myvariablename)");
            var parameterDeclaration = method.GetParameters().First();
            Assert.AreEqual("myvariablename", parameterDeclaration.Name);

            var composedType = (nrefact.ComposedType)parameterDeclaration.Type;

            Assert.AreEqual("SomeNameSpace.SomeType", composedType.BaseType.ToString());
            Assert.AreEqual(1, composedType.ArraySpecifiers.Count());
        }

        [Test]
        public void InvalidConstStaticMethod()
        {
            var method = CSharpSignatureAnalyzer.AnalyzeMethod("MyClass", "public static const MyClass MyMethod()");
            Assert.IsNull(method);
        }

        [Test]
        public void InstanceProperty()
        {
            var prop = ParseProperty("public void MyProperty");
            Assert.IsFalse(prop.HasModifier(nrefact.Modifiers.Static));
            Assert.AreEqual("MyProperty", prop.Name);
        }

        [Test]
        public void StaticProperty()
        {
            var prop = ParseProperty("public static void MyProperty");
            Assert.IsTrue(prop.HasModifier(nrefact.Modifiers.Static));
        }

        private nrefact.PropertyDeclaration ParseProperty(string signature)
        {
            return CSharpSignatureAnalyzer.AnalyzeProperty(signature);
        }

        [Test]
        public void AstNodeParsing()
        {
            var methods = new[] {@"
    public static string Method1() {
        return null;
    }", @"
    public static string Method2() {
        return null;
    }"};
            var str = @"


public static string Method1() {
        return null;
    }



    public static string Method2() {
        return null;
    }
";
            str = str.Replace("\r\n", "\n");
            //var expression = CSharpSignatureAnalyzer.Analyze("MyClass", str);
            //var start = expression.Item3.Length;
            //var parsed = expression.Item3 + expression.Item4 + expression.Item5;
            //var prevNodeLineEnd = expression.Item3.Split('\n').Count() - 1;
            //var prevNodeColEnd = 0;
            //var i = 0;
            //foreach (var node in expression.Item2)
            //{
            //    var block = CSharpSignatureAnalyzer.ParseAstNode(parsed, node, start, prevNodeLineEnd, prevNodeColEnd);
            //    start = block.Item4;
            //    prevNodeLineEnd= block.Item1.EndLocation.Line - 1;
            //    prevNodeColEnd = block.Item1.EndLocation.Column - 1;
            //    Assert.AreEqual("", block.Item2.Trim());
            //    Assert.AreEqual(methods[i++].Trim(), block.Item3.Trim());
            //}


            var parsed = CSharpSignatureAnalyzer.Analyze("MyClass", str);
            var result = parsed.ParseCSRawMethods();


            var i = 0;
            foreach (var ret in result)
            {
                if (ret.Item2 != null)
                    Assert.AreEqual(methods[i++].Trim(), ret.Item1.Trim());
            }
        }

        [Test]
        public void AstNodeParsing2()
        {
            var other = new[] { "#if TEST", "#endif" };

            var methods = new[] {@"
    public static string Method1() {
        return null;
    }", @"
    public static string Method2() {
        return null;
    }"};
            var str = @"

#if TEST
public static string Method1() {
        return null;
    }
#endif


    public static string Method2() {
        return null;
    }
";
            str = str.Replace("\r\n", "\n");
            //var expression = CSharpSignatureAnalyzer.Analyze("MyClass", str);
            //var start = expression.Item3.Length;
            //var parsed = expression.Item3 + expression.Item4 + expression.Item5;
            //var prevNodeLineEnd = expression.Item3.Split('\n').Count() - 1;
            //var prevNodeColEnd = 0;
            //var i = 0;
            //foreach (var node in expression.Item2)
            //{
            //    var block = CSharpSignatureAnalyzer.ParseAstNode(parsed, node, start, prevNodeLineEnd, prevNodeColEnd);
            //    start = block.Item4;
            //    prevNodeLineEnd = block.Item1.EndLocation.Line - 1;
            //    prevNodeColEnd = block.Item1.EndLocation.Column - 1;
            //    Assert.AreEqual(headers[i], block.Item2.Trim());
            //    Assert.AreEqual(methods[i++].Trim(), block.Item3.Trim());
            //}

            var parsed = CSharpSignatureAnalyzer.Analyze("MyClass", str);
            var result = parsed.ParseCSRawMethods();


            var i = 0;
            var j = 0;
            foreach (var ret in result)
            {
                if (ret.Item2 != null)
                    Assert.AreEqual(methods[i++].Trim(), ret.Item1.Trim());
                else
                    Assert.AreEqual(other[j++].Trim(), ret.Item1.Trim());
            }
        }

        [Test]
        public void AstNodeParsing3()
        {
            var other = new[] { "#if TEST", "#endif" };
            var methods = new[] {@"
    public static string Method1() {
        return null;
    }", @"
    public static string Method2() {
        return null;
    }"};
            var str = @"

public static string Method1() {
        return null;
    }


#if TEST
    public static string Method2() {
        return null;
    }
#endif
";
            str = str.Replace("\r\n", "\n");


            //var expression = CSharpSignatureAnalyzer.Analyze("MyClass", str);
            //var start = expression.Item3.Length;
            //var parsed = expression.Item3 + expression.Item4 + expression.Item5;
            //var prevNodeLineEnd = expression.Item3.Split('\n').Count() - 1;
            //var prevNodeColEnd = 0;
            //var i = 0;
            //foreach (var node in expression.Item2)
            //{
            //    var block = CSharpSignatureAnalyzer.ParseAstNode(parsed, node, start, prevNodeLineEnd, prevNodeColEnd);
            //    start = block.Item4;
            //    prevNodeLineEnd = block.Item1.EndLocation.Line - 1;
            //    prevNodeColEnd = block.Item1.EndLocation.Column - 1;
            //    Assert.AreEqual(headers[i], block.Item2.Trim());
            //    Assert.AreEqual(methods[i++].Trim(), block.Item3.Trim());
            //}

            //var end = parsed.Substring(start, parsed.Length - start - expression.Item5.Length);
            //Assert.AreEqual("#endif", end.Trim());


            var parsed = CSharpSignatureAnalyzer.Analyze("MyClass", str);
            var result = parsed.ParseCSRawMethods();


            var i = 0;
            var j = 0;
            foreach (var ret in result)
            {
                if (ret.Item2 != null)
                    Assert.AreEqual(methods[i++].Trim(), ret.Item1.Trim());
                else
                    Assert.AreEqual(other[j++].Trim(), ret.Item1.Trim());
            }
        }
    }

#if NREFACTORY_5_0_3
    public static class AttributedNodeExtensions
    {
        public static string GetName(this AttributedNode node)
        {
            if (node is MethodDeclaration)
                return ((MethodDeclaration)node).Name;
            if (node is ConstructorDeclaration)
                return ((ConstructorDeclaration)node).Name;
            if (node is PropertyDeclaration)
                return ((PropertyDeclaration)node).Name;
            return "";
        }

        public static AstType GetReturnType(this AttributedNode node)
        {
            if (node is MethodDeclaration)
                return ((MethodDeclaration)node).ReturnType;
            if (node is PropertyDeclaration)
                return ((PropertyDeclaration)node).ReturnType;
            return default(AstType);
        }

        public  static IEnumerable<ParameterDeclaration> GetParameters(this AttributedNode node)
        {
            if (node is MethodDeclaration)
                return ((MethodDeclaration)node).Parameters;
            if (node is ConstructorDeclaration)
                return ((ConstructorDeclaration)node).Parameters;
            return default(IEnumerable<ParameterDeclaration>);
        }

        public static string GetVisibility(this AttributedNode node)
        {
            if (node.HasModifier(Modifiers.Public))
                return "public";
            if (node.HasModifier(Modifiers.Protected))
                return "protected";
            if (node.HasModifier(Modifiers.Internal))
                return "internal";
            if (node.HasModifier(Modifiers.Private))
                return "private";
            return "public";
        }

        public static bool IsStatic(this AttributedNode node)
        {
            return node.HasModifier(Modifiers.Static);
        }

        public static bool HasAnyStructArgument(this AttributedNode node)
        {
            return node.GetParameters().Any(p => ParsingHelpers.IsStruct(p.Type.ToString()));
        }
    }

    public static class AstTypeExtensions
    {
        public static bool IsStruct(this AstType p)
        {
            return ParsingHelpers.IsStruct(p.ToString());
        }
    }
#else
    public static class EntityDeclarationExtensions
    {
        public static string GetName(this nrefact.EntityDeclaration node)
        {
            return node.Name;
        }

        public static nrefact.AstType GetReturnType(this nrefact.EntityDeclaration node)
        {
            return node.ReturnType;
        }

        public static IEnumerable<nrefact.ParameterDeclaration> GetParameters(this nrefact.EntityDeclaration node)
        {
            if (node is nrefact.MethodDeclaration)
                return ((nrefact.MethodDeclaration)node).Parameters;
            if (node is nrefact.ConstructorDeclaration)
                return ((nrefact.ConstructorDeclaration)node).Parameters;
            return new List<nrefact.ParameterDeclaration>();
        }

        public static string GetVisibility(this nrefact.EntityDeclaration node, string defaultVisibility = "public")
        {
            if (node.HasModifier(nrefact.Modifiers.Public))
                return "public";
            if (node.HasModifier(nrefact.Modifiers.Protected))
                return "protected";
            if (node.HasModifier(nrefact.Modifiers.Internal))
                return "internal";
            if (node.HasModifier(nrefact.Modifiers.Private))
                return "private";
            return defaultVisibility;
        }

        public static bool IsStatic(this nrefact.EntityDeclaration node)
        {
            return node.HasModifier(nrefact.Modifiers.Static);
        }

        public static bool HasAnyStructArgument(this nrefact.EntityDeclaration node)
        {
            return node.GetParameters().Any(p => ParsingHelpers.IsStruct(p.Type.ToString()));
        }
    }

#endif

    public static class AstTypeExtensions
    {
        public static bool IsStruct(this nrefact.AstType p)
        {
            return ParsingHelpers.IsStruct(p.ToString());
        }

        public static bool IsString(this nrefact.AstType t)
        {
            var primitiveType = t as nrefact.PrimitiveType;
            if (primitiveType == null)
                return false;

            return primitiveType.KnownTypeCode == ICSharpCode.NRefactory.TypeSystem.KnownTypeCode.String;
        }

        public static bool IsIntPtr(this nrefact.AstType p)
        {
            var typeName = p.ToString();

            switch (typeName)
            {
                case "IntPtr":
                case "System.IntPtr":
                case "UIntPtr":
                case "System.UIntPtr":
                    return true;
            }

            return false;
        }

        public static bool ShouldRewriteReturnValueAsParameterByReference(this nrefact.AstType p)
        {
            return p.IsStruct() || p.IsIntPtr();
        }
    }

    public static class AstNodeExtensions
    {
        public static string Format(this nrefact.AstNode node)
        {
            var ret = "";
            if (!(node is nrefact.EntityDeclaration))
                return ret;
            var entity = node as nrefact.EntityDeclaration;
            ret = entity.GetVisibility() + " " + entity.GetName();
            if (!(node is nrefact.TypeDeclaration) || ((nrefact.TypeDeclaration)node).ClassType != nrefact.ClassType.Enum)
                return ret;
            if (((nrefact.TypeDeclaration)node).BaseTypes.Any())
                ret += " : " + ((nrefact.TypeDeclaration)node).BaseTypes.First().ToString();
            return ret;
        }
    }

    public static class ParameterExtensions
    {
        public static bool IsRef(this nrefact.ParameterDeclaration decl)
        {
            return decl.ParameterModifier.HasFlag(nrefact.ParameterModifier.Ref);
        }

        public static bool IsOut(this nrefact.ParameterDeclaration decl)
        {
            return decl.ParameterModifier.HasFlag(nrefact.ParameterModifier.Out);
        }

        public static string GetDirection(this nrefact.ParameterDeclaration decl)
        {
            if (decl.ParameterModifier.HasFlag(nrefact.ParameterModifier.Out))
                return "out";
            return decl.ParameterModifier.HasFlag(nrefact.ParameterModifier.Ref) ? "ref" : "";
        }

        public static string GetDirectionForCall(this nrefact.ParameterDeclaration decl)
        {
            if (decl.ParameterModifier.HasFlag(nrefact.ParameterModifier.Out))
                return "out";
            if (decl.ParameterModifier.HasFlag(nrefact.ParameterModifier.Ref))
                return "ref";
            return decl.IsStruct() ? "ref" : "";
        }

        public static string GetParamSignature(this nrefact.ParameterDeclaration x)
        {
            var attributeList = x.Attributes.SelectMany(
                a => a.Descendants.OfType<nrefact.Attribute>()).Select(t => t.Type.ToString()).ToArray();

            var attrs = string.Join(", ", attributeList);
            if (attributeList.Length > 0)
                attrs = "[" + attrs + "]";
            return attrs + x.Type + " " + x.Name;
        }

        public static bool IsStruct(this nrefact.ParameterDeclaration p)
        {
            return ParsingHelpers.IsStruct(p.Type.ToString());
        }

        public static bool HasDefaultExpression(this nrefact.ParameterDeclaration p)
        {
            return !p.DefaultExpression.IsNull;
        }
    }

    abstract class CSharpSignatureAnalyzer
    {
        public static
#if NREFACTORY_5_0_3
            AttributedNode
#else
        nrefact.EntityDeclaration
#endif
        AnalyzeMethod(string className, string signature)
        {
            var compilationUnit = new nrefact.CSharpParser().Parse(WrapMethodInType(className, signature), "myfile.cs");

            var ret = compilationUnit.Descendants.OfType<nrefact.MethodDeclaration>().FirstOrDefault();
            if (ret == null)
                return compilationUnit.Descendants.OfType<nrefact.ConstructorDeclaration>().FirstOrDefault();
            return ret;
        }

        public static nrefact.PropertyDeclaration AnalyzeProperty(string signature)
        {
            var compilationUnit = new nrefact.CSharpParser().Parse(WrapPropInType(signature), "myfile.cs");

            return compilationUnit.Descendants.OfType<nrefact.PropertyDeclaration>().FirstOrDefault();
        }

        static string WrapMethodInType(string className, string signature)
        {
            className = className ?? "MyClass";
            return "class " + className + " { " + signature + "{ throw new NotImplementedException(); } }";
        }

        static string WrapPropInType(string signature)
        {
            return "class MyClass { " + signature + "{ get { throw new NotImplementedException(); } } }";
        }

        public static CSharpSignature Analyze(string className, string fragment)
        {
            var parser = new nrefact.CSharpParser();
            fragment = fragment.Trim();

            if (className == null)
                className = "MyClass";

            var eol = "\n";

            fragment = fragment.Replace("\r\n", "\n");

            CSharpSignature ret;
            string header, footer;

            header = "class " + className + " {" + eol;
            footer = eol + "}";
            ret = ParseCode(parser, eol, className, fragment, header, footer, true);
            if (ret != null)
                return ret;

            footer = eol + "}}";
            ret = ParseCode(parser, eol, className, fragment, header, footer, false);
            if (ret != null)
                return ret;

            footer = eol + "{}}";
            ret = ParseCode(parser, eol, className, fragment, header, footer, false);
            if (ret != null)
                return ret;

            header = eol;
            footer = eol + "class " + className + " {}" + eol;
            ret = ParseCode(parser, eol, className, fragment, header, footer, true);
            return ret;
        }

        static CSharpSignature ParseCode(nrefact.CSharpParser parser, string eol,
            string className, string fragment, string header, string footer, bool complete)
        {
            var type = "";
            nrefact.AstNode[] returnNodes = null;
            var ret = parser.Parse(header + fragment + footer);

            if (!parser.HasErrors)
            {
                var parent = ret.Descendants.FirstOrDefault(n => n.NodeType != nrefact.NodeType.Whitespace);
                if (parent != null)
                {
                    var list = new IndentedWriter();
                    foreach (var preproc in parent.Children.OfType<nrefact.PreProcessorDirective>())
                    {
                        if (preproc.Argument != null)
                            list.AppendLine("#define " + preproc.Argument);
                    }
                    if (list.Length > 0)
                    {
                        header = list.ToString();
                        header += "class " + className + " {" + eol;
                        header = header.Replace("\r\n", "\n");
                        ret = parser.Parse(header + fragment + footer);
                        parent = ret.Descendants.FirstOrDefault(x => x is nrefact.TypeDeclaration);
                    }

                    var hasMethods = parent.Children.OfType<nrefact.MethodDeclaration>().Any();
                    var hasProperties = parent.Children.OfType<nrefact.PropertyDeclaration>().Any();
                    var hasTypes = parent.Children.OfType<nrefact.TypeDeclaration>().Any();
                    var hasFields = parent.Children.OfType<nrefact.FieldDeclaration>().Any();
                    var hasConstructor = parent.Children.OfType<nrefact.ConstructorDeclaration>().Any();
                    var hasAttributes = parent.Children.OfType<nrefact.AttributeSection>().Any();

                    var mask =
                        (hasAttributes ? 32 : 0) |
                        (hasConstructor ? 16 : 0) |
                        (hasFields ? 8 : 0) |
                        (hasTypes ? 4 : 0) |
                        (hasProperties ? 2 : 0) |
                        (hasMethods ? 1 : 0);


                    if ((mask & 1) == 1)
                    {
                        returnNodes = parent.Children.OfType<nrefact.MethodDeclaration>().Select(x => x as nrefact.AstNode).ToArray();
                        type = "method";
                    }
                    else if ((mask & (1 << 1)) == (1 << 1))
                    {
                        returnNodes = parent.Children.OfType<nrefact.PropertyDeclaration>().Select(x => x as nrefact.AstNode).ToArray();
                        type = "property";
                    }
                    else if ((mask & (1 << 3)) == (1 << 3))
                    {
                        returnNodes = parent.Children.OfType<nrefact.FieldDeclaration>().Select(x => x as nrefact.AstNode).ToArray();
                        type = "field";
                    }
                    else if ((mask & (1 << 4)) == (1 << 4))
                    {
                        returnNodes = parent.Children.OfType<nrefact.ConstructorDeclaration>().Select(x => x as nrefact.AstNode).ToArray();
                        type = "constructor";
                    }
                    else if ((mask & (1 << 2)) == (1 << 2))
                    {
                        returnNodes = parent.Children.OfType<nrefact.TypeDeclaration>().Select(x => x as nrefact.AstNode).ToArray();
                        type = complete ? "enum" : "enumfragment";
                    }
                    else if ((mask & (1 << 5)) == (1 << 5))
                    {
                        returnNodes = parent.Children.OfType<nrefact.AttributeSection>().Select(x => x as nrefact.AstNode).ToArray();
                        type = "attribute";
                    }
                }
            }

            if (returnNodes != null)
                return new CSharpSignature(className, type, returnNodes, header, fragment, footer);
            return null;
        }

        public class CSharpSignature
        {
            public string ClassName;
            public string Type;
            public nrefact.AstNode[] Nodes;
            public List<Tuple<string, nrefact.AstNode>> Lines;

            public string WrapHeader;
            public string Content;
            public string WrapFooter;

            private bool _parsed = false;

            public CSharpSignature(string className, string type, nrefact.AstNode[] nodes, string header, string content, string footer)
            {
                ClassName = className;
                Type = type;
                Nodes = nodes;
                WrapHeader = header;
                Content = content;
                WrapFooter = footer;
                Lines = new List<Tuple<string, nrefact.AstNode>>();
            }

            public List<Tuple<string, nrefact.AstNode>> ParseCSRawMethods()
            {
                if (Type != "method")
                    return null;
                if (_parsed)
                    return Lines;

                var start = WrapHeader.Length;
                var parsed = WrapHeader + Content + WrapFooter;
                var prevNodeLineEnd = WrapHeader.Split('\n').Count() - 1;
                var prevNodeColEnd = 0;
                foreach (var node in Nodes)
                {
                    var block = ParseAstNode(parsed, node, start, prevNodeLineEnd, prevNodeColEnd);
                    start = block.Item4;
                    prevNodeLineEnd = block.Item1.EndLocation.Line - 1;
                    prevNodeColEnd = block.Item1.EndLocation.Column - 1;
                    if (block.Item2.Length > 0)
                        Lines.Add(Tuple.Create(block.Item2, (nrefact.AstNode)null));
                    Lines.Add(Tuple.Create(block.Item3, node));
                }

                if (parsed.Length - start - WrapFooter.Length > 0)
                {
                    var end = parsed.Substring(start, parsed.Length - start - WrapFooter.Length);
                    Lines.Add(Tuple.Create(end, (nrefact.AstNode)null));
                }
                return Lines;
            }

            static Tuple<nrefact.AstNode, string, string, int> ParseAstNode(string snippet, nrefact.AstNode node, int start, int prevNodeLineEnd, int prevNodeColEnd)
            {
                snippet = snippet.Replace("\r\n", "\n").TrimStart();

                var lines = snippet.Split("\n", StringSplitOptions.None);

                var linestart = node.StartLocation.Line - 1;
                var lineend = node.EndLocation.Line - 1;
                if (lineend > lines.Length - 1)
                    lineend = lines.Length - 1;
                var colstart = node.StartLocation.Column - 1;
                var colend = node.EndLocation.Column - 1;

                var pos = start;

                if (prevNodeLineEnd != linestart)
                    pos += lines[prevNodeLineEnd].Length - prevNodeColEnd;
                for (var i = prevNodeLineEnd + 1; i < linestart; i++)
                    pos += lines[i].Length + 1;
                pos += colstart;
                var header = snippet.Substring(start, pos - start);

                var sb = new IndentedWriter();

                if (linestart < lineend)
                {
                    sb.AppendLine(lines[linestart].Substring(colstart));
                    for (var i = linestart + 1; i < lineend; i++)
                        sb.AppendLine(lines[i]);
                    sb.Append(lines[lineend].Substring(0, colend));
                }
                else
                    sb.Append(lines[linestart].Substring(colstart, colend - colstart));

                var str = sb.Output;
                start = pos + str.Length + 1;
                return Tuple.Create(node, header, str, start);
            }
        }


        public static string PrepareSignatureMethod(string signature)
        {
            var str = "";
            var end = signature.IndexOf('(');
            var parts = signature.Substring(0, end).Split(' ', StringSplitOptions.RemoveEmptyEntries);
            for (var i = 0; i < parts.Length; i++)
            {
                var part = parts[i];
                if (part == "unsigned")
                    part = parts[++i];
                str += part + " ";
            }

            if (!signature.Trim().EndsWith("}"))
                signature += "{}";
            return str + signature.Substring(end);
        }
    }
}
