using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using ICSharpCode.NRefactory.CSharp;
using ICSharpCode.NRefactory.Editor;
using ICSharpCode.NRefactory.TypeSystem;
using Mono.Cecil;

namespace Unity.BindingsGenerator.Core
{
    public sealed class TypeFilter
    {
        private static readonly Regex CrLineEndingRegex = new Regex("\r(?!\n)", RegexOptions.Compiled);

        private readonly string[] _defines;
        private readonly string _inputFile;
        private readonly List<string> _declaredTypes = new List<string>();
        private readonly List<string> _declaredMethods = new List<string>();
        private readonly List<string> _declaredProperties = new List<string>();

        public TypeFilter(string inputFile, string[] defines)
        {
            _defines = defines;
            _inputFile = Path.GetFullPath(inputFile);

            CollectDeclarationsInBindingsFile();
        }

        public bool IsDeclaredInTheRightInputFile(MethodDefinition methodDefinition)
        {
            if (methodDefinition.IsGetter || methodDefinition.IsSetter)
                return IsDeclaredInTheRightInputFile(methodDefinition.GetMatchingPropertyDefinition());

            return _declaredMethods.Contains(NormalizeName(methodDefinition.DeclaringType.FullName) + "." + methodDefinition.Name);
        }

        public bool IsDeclaredInTheRightInputFile(PropertyDefinition propertyDefinition)
        {
            return _declaredProperties.Contains(NormalizeName(propertyDefinition.DeclaringType.FullName) + "." + propertyDefinition.Name);
        }

        private void CollectDeclarationsInBindingsFile()
        {
            var syntaxTree = SyntaxTreeFor(_defines);
            var typeSystem = syntaxTree.ToTypeSystem();
            foreach (var typeDefinition in typeSystem.GetAllTypeDefinitions())
                CollectDeclarationsForType(typeDefinition);
        }

        private void CollectDeclarationsForType(IUnresolvedTypeDefinition typeDefinition)
        {
            _declaredTypes.Add(typeDefinition.FullName);

            foreach (var method in typeDefinition.Methods)
                _declaredMethods.Add(method.FullName);

            foreach (var property in typeDefinition.Properties)
                _declaredProperties.Add(property.FullName);

            foreach (var nestedTypeDefinition in typeDefinition.NestedTypes)
                CollectDeclarationsForType(nestedTypeDefinition);
        }

        private static string ContentsOf(string path)
        {
            using (var reader = new StreamReader(path, Encoding.Default))
            {
                return FixCrOnlyNewLines(reader.ReadToEnd());
            }
        }

        private static string FixCrOnlyNewLines(string contents)
        {
            return CrLineEndingRegex.Replace(contents, "\n");
        }

        private SyntaxTree SyntaxTreeFor(IEnumerable<string> defines)
        {
            var doc = new ReadOnlyDocument(ContentsOf(_inputFile));
            var cSharpParser = new CSharpParser();

            foreach (var define in defines)
                cSharpParser.CompilerSettings.ConditionalSymbols.Add(define);

            var syntaxTree = cSharpParser.Parse(doc, _inputFile);

            if (syntaxTree.Errors.Count > 0)
                throw new ArgumentException("Input file has parsing errors. Cannot proceed with the code generation.");

            return syntaxTree;
        }

        //Replaces '/' with '.' to make NRefactory and Cecil FullName strings consistent
        private string NormalizeName(string typeFullName)
        {
            return typeFullName.Replace('/', '.');
        }
    }
}
