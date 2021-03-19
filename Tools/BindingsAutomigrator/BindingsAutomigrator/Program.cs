using System;
using System.Collections.Generic;
using System.Diagnostics;
using Microsoft.CodeAnalysis.CSharp;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using Microsoft.CodeAnalysis;
using Microsoft.CodeAnalysis.CSharp.Syntax;
using Unity.CommonTools;

namespace BindingsAutomigrator
{
    class Walker : CSharpSyntaxRewriter
    {
        private List<UsingDirectiveSyntax> usings = new List<UsingDirectiveSyntax>();
        private CSharpCompilation _compilation;
        private bool _needsInteropNamespace;
        private string _generatedBindingsCsFile;

        public Walker(CSharpCompilation compilation, string generatedBindingsCsFile)
        {
            _generatedBindingsCsFile = generatedBindingsCsFile;
            _compilation = compilation;
        }

        public override SyntaxNode VisitIfDirectiveTrivia(IfDirectiveTriviaSyntax node)
        {
            return base.VisitIfDirectiveTrivia(node.WithBranchTaken(true));
        }

        public override SyntaxNode VisitAttribute(AttributeSyntax node)
        {
            var sm = _compilation.GetSemanticModel(node.SyntaxTree);
            var type = sm.GetTypeInfo(node);

            if (type.Type == null)
            {
                return node;
            }

            var attributeFQN = type.Type.ToString();
            var structLayoutType = typeof(StructLayoutAttribute);
            if (node.Name.IsKind(SyntaxKind.QualifiedName) && attributeFQN == structLayoutType.FullName)
            {
                _needsInteropNamespace = true;

                var attribute = (AttributeSyntax)base.VisitAttribute(node);
                // Use simple names for StructLayout
                return attribute.WithName(SyntaxFactory.IdentifierName(structLayoutType.Name));
            }

            return base.VisitAttribute(node);
        }

        public override SyntaxNode VisitAttributeArgument(AttributeArgumentSyntax node)
        {
            var mae = node.Expression as MemberAccessExpressionSyntax;
            if (mae == null || !mae.Expression.IsKind(SyntaxKind.SimpleMemberAccessExpression))
                return node;

            var sm = _compilation.GetSemanticModel(node.SyntaxTree);
            var type = sm.GetTypeInfo(mae.Expression);

            if (type.Type.ToString() != typeof(LayoutKind).FullName)
                return node;

            return node.WithExpression(mae.WithExpression(SyntaxFactory.IdentifierName(typeof(LayoutKind).Name)));
        }

        public override SyntaxNode VisitAttributeList(AttributeListSyntax node)
        {
            var ret = (AttributeListSyntax)base.VisitAttributeList(node);
            return ret.Attributes.Count == 0 ? null : ret;
        }

        public override SyntaxNode VisitUsingDirective(UsingDirectiveSyntax node)
        {
            if (node.Alias != null)
                return base.VisitUsingDirective(node);

            usings.Add(node);
            return base.VisitUsingDirective(node);
        }

        public override SyntaxNode VisitCompilationUnit(CompilationUnitSyntax node)
        {
            var compilationUnit = (CompilationUnitSyntax)base.VisitCompilationUnit(node);

            if (_needsInteropNamespace)
            {
                var usingName = typeof(LayoutKind).Namespace;
                if (usings.All(ud => ud.Name.ToString() != usingName))
                    compilationUnit = compilationUnit.AddUsings(
                        SyntaxFactory.UsingDirective(SyntaxFactory.ParseName(usingName).WithLeadingTrivia(SyntaxFactory.Space)).WithTrailingTrivia(SyntaxFactory.LineFeed));
            }

            return compilationUnit.AddUsings(
                SyntaxFactory.UsingDirective(SyntaxFactory.ParseName("UnityEngine.Bindings").WithLeadingTrivia(SyntaxFactory.Space)).WithTrailingTrivia(SyntaxFactory.LineFeed));
        }

        public override SyntaxNode VisitStructDeclaration(StructDeclarationSyntax node)
        {
            var o = (StructDeclarationSyntax)base.VisitStructDeclaration(node);
            var attributeList = CreateNativeAttributeIfRequired(node, "NativeStruct");
            return attributeList != null
                ? o.AddAttributeLists(attributeList)
                : o;
        }

        public override SyntaxNode VisitClassDeclaration(ClassDeclarationSyntax node)
        {
            var attributeList = CreateNativeAttributeIfRequired(node, "NativeType");
            if (attributeList != null)
            {
                return ((ClassDeclarationSyntax)base.VisitClassDeclaration(node)).AddAttributeLists(attributeList);
            }

            return base.VisitClassDeclaration(node);
        }

        private AttributeListSyntax CreateNativeAttributeIfRequired(SyntaxNode node, string attributeTypeName)
        {
            var methods = node.DescendantNodes().OfType<MethodDeclarationSyntax>();

            var injectNativeAttribute = methods.Any(m => m.Modifiers.Any(mod => mod.IsKind(SyntaxKind.ExternKeyword)));
            if (!injectNativeAttribute)
            {
                var props = node.DescendantNodes().OfType<PropertyDeclarationSyntax>();
                injectNativeAttribute = props.Any(prop => prop.Modifiers.Any(mod => mod.IsKind(SyntaxKind.ExternKeyword)));
            }

            if (!injectNativeAttribute)
                return null;

            var attribute = SyntaxFactory.Attribute(SyntaxFactory.ParseName(attributeTypeName));

            var argList = SyntaxFactory.ParseAttributeArgumentList("(Header = \"" + _generatedBindingsCsFile + "\")");

            attribute = attribute.WithArgumentList(argList);

            return SyntaxFactory.AttributeList(SyntaxFactory.SeparatedList(new[] {attribute})).WithTrailingTrivia(SyntaxFactory.LineFeed);
        }
    }

    public static class Extensions
    {
        public static string ConstRef(this string s)
        {
            return string.Format("const {0}&", s);
        }

        public static string Capitalize(this string s)
        {
            return Char.ToUpper(s[0]) + s.Substring(1);
        }

        public static TypeDeclarationSyntax DeclaringType(this SyntaxNode node)
        {
            return node.Ancestors().OfType<TypeDeclarationSyntax>().First();
        }
    }

    class TypeUtil
    {
        public enum Usage
        {
            ReturnType,
            ParameterType,
        }

        public static string MapType(TypeSyntax type, Usage usage)
        {
            if (type.IsKind(SyntaxKind.ArrayType))
            {
                var t = string.Format("std::vector<{0}>", MapType(((ArrayTypeSyntax)type).ElementType, Usage.ReturnType));

                switch (usage)
                {
                    case Usage.ReturnType: return t;
                    case Usage.ParameterType: return t.ConstRef();
                }
            }

            switch (type.ToString())
            {
                case "string":
                    return "core::string";
            }

            return type.ToString();
        }
    }

    class CppStubGeneratorWalker : CSharpSyntaxWalker
    {
        private StringBuilder _cpp = new StringBuilder();
        private string[] _generatedCppContent;

        public CppStubGeneratorWalker(string[] generatedCppContent)
        {
            _generatedCppContent = generatedCppContent;

            var directiveStartLine = FindIndexOfLineContaining("#if !defined(INTERNAL_CALL_STRIPPING)");
            if (directiveStartLine == -1)
            {
                if (generatedCppContent.Length < 4) // Skip dummy cpp files.
                    return;

                throw new Exception("Couldn't find registration code start.");
            }
            _generatedCppContent = generatedCppContent.Where(s => !s.Trim().StartsWith("SCRIPTINGAPI_") && !s.Trim().StartsWith("UNUSED")).TakeWhile(s => !s.StartsWith("#if !defined(INTERNAL_CALL_STRIPPING)")).ToArray();
            _generatedCppContent[_generatedCppContent.Length - 1] = "SCRIPT_BINDINGS_EXPORT_DECL";
        }

        private int FindIndexOfLineContaining(string generatedMethodName, int startLine = 0)
        {
            for (int i = startLine; i < _generatedCppContent.Length; i++)
            {
                if (_generatedCppContent[i].Contains(generatedMethodName))
                    return i;
            }

            return -1;
        }

        private string ExtractGeneratedMethodBody(string generatedMethodName)
        {
            var startIndex = FindIndexOfLineContaining(generatedMethodName);
            if (startIndex == -1)
                return null;

            var endIndex = FindIndexOfLineContaining("SCRIPT_BINDINGS_EXPORT_DECL", startIndex);
            if (endIndex == -1)
                throw new Exception("Couldn't find end marker for method.");

            return _generatedCppContent.Skip(startIndex).Take(endIndex - startIndex - 1).Aggregate("", (acc, current) => acc + "\t\t// " + current + "\n");
        }

        public string Cpp
        {
            get { return _cpp.ToString(); }
        }

        public override void VisitClassDeclaration(ClassDeclarationSyntax node)
        {
            var oldLength = _cpp.Length;
            base.VisitClassDeclaration(node);

            if (oldLength != _cpp.Length)
            {
                _cpp.Insert(oldLength, string.Format("class {0}\n{{\npublic:\n", node.Identifier));
                _cpp.AppendLine("};\n");
            }
        }

        public override void VisitStructDeclaration(StructDeclarationSyntax node)
        {
            var oldLength = _cpp.Length;
            base.VisitStructDeclaration(node);

            if (oldLength != _cpp.Length)
            {
                _cpp.Insert(oldLength, string.Format("struct {0}\n{{\n", node.Identifier));
                _cpp.AppendLine("};\n");
            }
        }

        public override void VisitMethodDeclaration(MethodDeclarationSyntax node)
        {
            var shouldGenerate = node.Modifiers.Any(mod => mod.IsKind(SyntaxKind.ExternKeyword));
            if (!shouldGenerate)
                return;

            _cpp.AppendFormat("\t{0}{1} {2}(", node.Modifiers.Any(mod => mod.IsKind(SyntaxKind.StaticKeyword)) ? "static " : "", TypeUtil.MapType(node.ReturnType, TypeUtil.Usage.ReturnType), node.Identifier);

            var pars =
                node.ParameterList.Parameters.Select(
                    p => string.Format("{0} {1}", TypeUtil.MapType(p.Type, TypeUtil.Usage.ParameterType), p.Identifier));
            _cpp.Append(string.Join(", ", pars));

            var potentialFunctionName1 = string.Format("{0}_CUSTOM_{1}", node.DeclaringType().Identifier, node.Identifier);
            var potentialFunctionName2 = string.Format("{0}_CUSTOM_INTERNAL_CALL_{1}", node.DeclaringType().Identifier, node.Identifier);

            var body = ExtractGeneratedMethodBody(potentialFunctionName1) ?? ExtractGeneratedMethodBody(potentialFunctionName2);

            _cpp.Append(")\n\t{\n" + body + "\t}\n\n");
        }

        public override void VisitPropertyDeclaration(PropertyDeclarationSyntax node)
        {
            var shouldGenerate = node.Modifiers.Any(mod => mod.IsKind(SyntaxKind.ExternKeyword));
            if (!shouldGenerate)
                return;

            var propertyName = node.Identifier.ToString().Capitalize();

            var staticString = node.Modifiers.Any(mod => mod.IsKind(SyntaxKind.StaticKeyword)) ? "static " : "";

            if (node.AccessorList.Accessors.Any(acc => acc.IsKind(SyntaxKind.GetAccessorDeclaration)))
            {
                var generatedFunctionName = string.Format("{0}_Get_Custom_Prop{1}", node.DeclaringType().Identifier, propertyName);
                var body = ExtractGeneratedMethodBody(generatedFunctionName);

                _cpp.AppendFormat("\t{0}{1} Get{2}(", staticString, TypeUtil.MapType(node.Type, TypeUtil.Usage.ReturnType), propertyName);
                _cpp.Append(")\n\t{\n" + body + "\t}\n\n");
            }

            if (node.AccessorList.Accessors.Any(acc => acc.IsKind(SyntaxKind.SetAccessorDeclaration)))
            {
                var generatedFunctionName = string.Format("{0}_Set_Custom_Prop{1}", node.DeclaringType().Identifier, propertyName);
                var body = ExtractGeneratedMethodBody(generatedFunctionName);

                _cpp.AppendFormat("\t{0}void Set{1}({2} value", staticString, propertyName, TypeUtil.MapType(node.Type, TypeUtil.Usage.ParameterType));
                _cpp.Append(")\n\t{\n" + body + "\t}\n\n");
            }
        }
    }

    class Program
    {
        // NOTE(rb): Running the tools for more bindings
        // for /F %f in ('dir runtime\*.bindings /b /s') do Tools\BindingsAutomigrator\BindingsAutomigrator\bin\Debug\BindingsAutomigrator.exe %f

        static void Main(string[] args)
        {
            var testBindingsPath = @"Editor\Mono\GUID.bindings";
            var unityEnginePath = Workspace.UnityEngineAssemblyPath;

            var originalBindingsFilePath = args.Length >= 1 ? args[0] : testBindingsPath;

            if (Path.IsPathRooted(originalBindingsFilePath))
                throw new ArgumentException("Paths must be relative to unity root folder.");

            originalBindingsFilePath = originalBindingsFilePath.Replace("\\", "/");

            var isEditorSource = originalBindingsFilePath.StartsWith("Editor/") || originalBindingsFilePath.StartsWith("editor/");
            var inputFilePath = isEditorSource
                ? string.Format("artifacts/generated/common/editor/{0}Bindings.gen.cs", Path.GetFileNameWithoutExtension(originalBindingsFilePath))
                : string.Format("artifacts/generated/common/runtime/{0}Bindings.gen.cs", Path.GetFileNameWithoutExtension(originalBindingsFilePath));

            var isModule = false;
            if (!File.Exists(inputFilePath))
            {
                inputFilePath = FindBindingFileInFolder(Path.GetFileName(inputFilePath), @"artifacts/generated/common/modules/");
                if (inputFilePath != null)
                    isModule = true;
                else
                    throw new Exception("Could not find generated source from " + inputFilePath);
            }

            var definedSymbols = args.Length == 2 ? args[1].Split(',') : null;
            if (definedSymbols == null)
            {
                var collectedDefines = CollectPreProcessorSymbols(inputFilePath);
                if (collectedDefines.Any())
                {
                    Console.WriteLine("'{0}' contains #ifs/#elifs (we may skip code in such blocks). Please rerun specifying one or more preprocessor symbols from the list:\n{1}", originalBindingsFilePath, string.Join(",", collectedDefines));
                    return;
                }
            }

            var generatedBindingsCsContent = RemoveAttributes(inputFilePath);
            var ast = CSharpSyntaxTree.ParseText(generatedBindingsCsContent, new CSharpParseOptions(preprocessorSymbols: definedSymbols));
            var compilation = CSharpCompilation.Create("dummy", new[] { ast }, new[] { MetadataReference.CreateFromFile(typeof(object).Assembly.Location), MetadataReference.CreateFromFile(unityEnginePath) });

            var automigratedBindingsFilePath = originalBindingsFilePath + ".cs";
            var cppStubPath = originalBindingsFilePath + ".h";

            var walker = new Walker(compilation, cppStubPath);
            var transformedAst = walker.Visit(ast.GetRoot());

            File.WriteAllText(automigratedBindingsFilePath, transformedAst.ToString());

            RunFormatter(automigratedBindingsFilePath);

            GenerateCppStub(originalBindingsFilePath, inputFilePath, ast, cppStubPath);

            if (!isModule)
            {
                UpdateMonoICallRegistration(Path.GetFileNameWithoutExtension(originalBindingsFilePath));

                // NOTE(rb): Uncommented removal from Jam file because it was not feasible for modules, so better to be consistent?
                //RemoveFromJamFiles(originalBindingsFilePath, Path.Combine("Projects/Jam/", isEditorSource ? "EditorFiles.jam" : "RuntimeFiles.jam"));
            }
            else
            {
                Console.WriteLine("[Warning] Bindings file is part of a module. You must update ICall registrations manually.");
                //Console.WriteLine("[Warning] Bindings file is part of a module. You must update module jam manually.");
            }
        }

        private static string RemoveAttributes(string inputFilePath)
        {
            var searchString1 = "[UnityEngine.Scripting.GeneratedByOldBindingsGeneratorAttribute]";
            var searchString2 = "[System.Runtime.CompilerServices.MethodImplAttribute((System.Runtime.CompilerServices.MethodImplOptions)0x1000)]";

            var lines = File.ReadAllLines(inputFilePath);
            return string.Join("\n", lines.Where(l => !l.Contains(searchString1) && !l.Contains(searchString2)));
        }

        private static void GenerateCppStub(string originalBindingsPath, string inputFilePath, SyntaxTree ast, string cppStubPath)
        {
            var cppGeneratedFilePath = Path.ChangeExtension(inputFilePath, ".cpp");
            var generatedCppLines = File.ReadAllLines(cppGeneratedFilePath);

            var bindingsContent = string.Join("\n", File.ReadAllLines(originalBindingsPath)).TrimStart();

            var cppGenerator = new CppStubGeneratorWalker(generatedCppLines);
            cppGenerator.Visit(ast.GetRoot());

            var cppStub = cppGenerator.Cpp;
            if (bindingsContent.TrimStart().StartsWith("C++RAW"))
            {
                var csrawIndex = bindingsContent.IndexOf("CSRAW", StringComparison.InvariantCulture);
                if (csrawIndex != -1)
                {
                    var cppPreamble = bindingsContent.Substring(6, csrawIndex - 6).Trim();
                    var transformedCppPreamble = string.Join("\n", cppPreamble.Split('\n').Select(l => "// " + l.Trim())) + "\n\n";

                    cppStub = transformedCppPreamble + cppStub;
                }
            }

            File.WriteAllText(cppStubPath, cppStub);
        }

        private static void RunFormatter(string automigratedBindingsFilePath)
        {
            ProcessStartInfo startInfo = new ProcessStartInfo("hg", "format " + automigratedBindingsFilePath);
            startInfo.WindowStyle = ProcessWindowStyle.Hidden;
            using (var process = Process.Start(startInfo))
            {
                process.WaitForExit(10000);
                if (process.ExitCode != 0)
                    Console.WriteLine("[Warning] 'hg format' failed. Did @mihai include you in his beta program?");
            }

            RunCustomFormatter(automigratedBindingsFilePath);
        }

        private static void RunCustomFormatter(string automigratedBindingsFilePath)
        {
            var lines = File.ReadAllLines(automigratedBindingsFilePath).ToList();

            int passes = 0;
            bool linesChanged = true;

            while (linesChanged && passes < 10)
            {
                linesChanged = false;
                passes++;

                // Two empty lines -> one empty line
                for (int i = 0; i < lines.Count - 1; i++)
                {
                    if (lines[i].Trim() == "" && lines[i + 1].Trim() == "")
                    {
                        linesChanged = true;
                        lines.RemoveAt(i);
                    }
                }

                // Remove empty line below [NativeType]
                for (int i = 0; i < lines.Count - 1; i++)
                {
                    if (lines[i].Trim().StartsWith("[NativeType") && lines[i + 1].Trim() == "")
                    {
                        linesChanged = true;
                        lines.RemoveAt(i + 1);
                    }
                }

                // Add empty line above [NativeType]
                for (int i = 0; i < lines.Count - 1; i++)
                {
                    if (lines[i].Trim().StartsWith("[NativeType") && lines[i - 1].Trim() != "" && lines[i - 1].Trim() != "{")
                    {
                        linesChanged = true;
                        lines.Insert(i, "");
                    }
                }

                // Remove empty line below #if
                for (int i = 0; i < lines.Count - 1; i++)
                {
                    if (lines[i].Trim().StartsWith("#if") && lines[i + 1].Trim() == "")
                    {
                        linesChanged = true;
                        lines.RemoveAt(i + 1);
                    }
                }

                // Add empty line above #if
                for (int i = 0; i < lines.Count - 1; i++)
                {
                    if (lines[i].Trim().StartsWith("#if") && lines[i - 1].Trim() != "" && lines[i - 1].Trim() != "{")
                    {
                        linesChanged = true;
                        lines.Insert(i, "");
                    }
                }

                // Remove empty line above #endif
                for (int i = 1; i < lines.Count; i++)
                {
                    if (lines[i].Trim().StartsWith("#endif") && lines[i - 1].Trim() == "")
                    {
                        linesChanged = true;
                        lines.RemoveAt(i - 1);
                    }
                }

                // Add empty line below #endif
                for (int i = 0; i < lines.Count - 1; i++)
                {
                    if (lines[i].Trim().StartsWith("#endif") && lines[i + 1].Trim() != "" && lines[i + 1].Trim() != "{")
                    {
                        linesChanged = true;
                        lines.Insert(i + 1, "");
                    }
                }

                // Remove empty line below {
                for (int i = 0; i < lines.Count - 1; i++)
                {
                    if (lines[i].Trim().StartsWith("{") && lines[i + 1].Trim() == "")
                    {
                        linesChanged = true;
                        lines.RemoveAt(i + 1);
                    }
                }

                // Remove empty line above }
                for (int i = 1; i < lines.Count; i++)
                {
                    if (lines[i].Trim().StartsWith("}") && lines[i - 1].Trim() == "")
                    {
                        linesChanged = true;
                        lines.RemoveAt(i - 1);
                    }
                }
            }

            File.WriteAllLines(automigratedBindingsFilePath, lines);
        }

        // NOTE(rb): Unused and not functioning, may pick back up later
        //private static void RemoveFromJamFiles(string bindingsFilePath, string jamFilePath)
        //{
        //    var fileContent = File.ReadAllText(jamFilePath);
        //    var normalizedBindingsFilePath = bindingsFilePath.ToLower());
        //    var index = fileContent.IndexOf(normalizedBindingsFilePath, StringComparison.InvariantCultureIgnoreCase);

        //    if (index == -1)
        //    {
        //        Console.WriteLine("[Warning] Could not remove {0} from {1}! You must remove from Jam file manually.", bindingsFilePath, jamFilePath);
        //        return;
        //    }

        //    var lineStartIndex = fileContent.LastIndexOf("\n", index) + 1;
        //       var lineEndIndex = fileContent.IndexOf("\n", index);
        //    var newFileContent = fileContent.Remove(lineStartIndex, lineEndIndex - lineStartIndex);
        //       File.WriteAllText(bindingsFilePath, newFileContent);
        //   }

        private static void UpdateMonoICallRegistration(string bindingsName)
        {
            var registrationFilePath = "Runtime/Export/MonoICallRegistration.cpp";
            var fileContents = File.ReadAllText(registrationFilePath);
            var oldRegistrationFunctionName = "Export" + bindingsName + "Bindings";
            var newContents = fileContents.Replace(oldRegistrationFunctionName + "(", oldRegistrationFunctionName + "New(");
            newContents = newContents.Replace(oldRegistrationFunctionName + ",", oldRegistrationFunctionName + "New,");

            File.WriteAllText(registrationFilePath, newContents);
        }

        private class SymbolsCollector : CSharpSyntaxWalker
        {
            public ISet<string> Symbols { get; private set; }

            public override void VisitIdentifierName(IdentifierNameSyntax node)
            {
                base.VisitIdentifierName(node);
                Symbols.Add(node.ToString());
            }

            public SymbolsCollector()
            {
                Symbols = new HashSet<string>();
            }
        }

        private static IEnumerable<string> CollectPreProcessorSymbols(string filePath)
        {
            var ast = CSharpSyntaxTree.ParseText(File.ReadAllText(filePath));

            var collector = new SymbolsCollector();

            var trivias = ast.GetRoot().DescendantTrivia();
            foreach (var syntaxTrivia in trivias)
            {
                if (syntaxTrivia.Kind() == SyntaxKind.IfDirectiveTrivia)
                {
                    var ifDirective = (IfDirectiveTriviaSyntax)syntaxTrivia.GetStructure();
                    collector.Visit(ifDirective);
                }
            }

            return collector.Symbols;
        }

        private static string FindBindingFileInFolder(string fileName, string baseFolder)
        {
            var files = Directory.GetFiles(baseFolder, fileName, SearchOption.AllDirectories);
            return files.SingleOrDefault();
        }
    }
}
