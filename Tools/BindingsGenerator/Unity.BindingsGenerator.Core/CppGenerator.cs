using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine.Bindings;
using Unity.BindingsGenerator.Core.Nodes;

namespace Unity.BindingsGenerator.Core
{
    public class CppGenerator
    {
        private Indent _indent;

        public CppGenerator()
        {
            _indent = new Indent();
        }

        public CppGenerator(int startingIndent)
        {
            _indent = new Indent(startingIndent);
        }

        public string Generate(FileNode sourceFile)
        {
            var s = "";

            if (sourceFile.IncludeFiles != null && sourceFile.IncludeFiles.Count > 0)
            {
                foreach (var filename in sourceFile.IncludeFiles)
                    s += string.Format("{0}#include \"{1}\"\n", _indent, filename);

                s += "\n";
            }

            foreach (var n in sourceFile.Namespaces)
            {
                s += Generate(n);
            }

            return s;
        }

        private enum IndentationOptions
        {
            Indent,
            NoIndent,
        }

        private string BeginNamespace(string name, IndentationOptions indentationOptions = IndentationOptions.NoIndent)
        {
            var s = string.Format("namespace {0}\n{{\n", name);

            if (indentationOptions == IndentationOptions.Indent)
                _indent++;

            return s;
        }

        private string EndNamespace(string name, IndentationOptions indentationOptions = IndentationOptions.NoIndent)
        {
            if (indentationOptions == IndentationOptions.Indent)
                _indent--;

            return string.Format("\n}}\n");
        }

        public string Generate(NamespaceNode n)
        {
            string s = string.Empty;

            if (n.IsEmpty)
                return s;

            if (!string.IsNullOrWhiteSpace(n.Comment))
                s += string.Format("{0}// {1}\n", _indent, n.Comment);

            if (n.Name != string.Empty)
                s += BeginNamespace(n.Name);

            s += string.Join("\n", n.TypeDefs.Select(Generate)
                .Concat(n.Structs.Select(Generate))
                .Concat(n.Functions.Select(Generate)));

            if (n.Name != string.Empty)
                s += EndNamespace(n.Name);

            return s;
        }

        public string Generate(StructDefinitionNode structNode)
        {
            string s = "";
            if (structNode.ForceUniqueViaPreprocessor)
            {
                var define = UniquenessPreprocessorDefine(structNode);
                s += $"#ifndef {define}\n#define {define}\n";
            }
            string templateSpecialization = structNode.TemplateSpecialization.Count > 0 ?
                $"<{string.Join(", ", structNode.TemplateSpecialization)}>" : "";

            string template = structNode.IsTemplate
                ? $"{_indent}template<{string.Join(", ",structNode.TemplateArguments)}>\n"
                : "";

            s += $"{template}{_indent}struct {structNode.Name}{templateSpecialization}\n{_indent}{{\n";
            _indent++;

            string typedefs = structNode.TypeDefs == null ? "" : string.Concat(structNode.TypeDefs.Select(Generate));
            string members = structNode.Members == null ? "" : string.Concat(structNode.Members.Select(Generate));
            string functions = structNode.FunctionDefinitions == null ? "" : string.Concat(structNode.FunctionDefinitions.Select(Generate));

            var toBeAdded = new[]
            {
                typedefs,
                members,
                functions
            }.Where(st => !string.IsNullOrWhiteSpace(st));

            s += string.Join("\n", toBeAdded);

            _indent--;
            s += $"{_indent}}};\n";

            if (structNode.ForceUniqueViaPreprocessor)
                s += $"#endif /*{UniquenessPreprocessorDefine(structNode)}*/\n";

            return s;
        }

        private string UniquenessPreprocessorDefine(StructDefinitionNode structNode)
        {
            string define = string.Join("_", new[] { structNode.Name }.Concat(structNode.TemplateArguments).Concat(structNode.TemplateSpecialization));
            define = define.Replace("::", "_");
            define += "_DEFINED";
            return define;
        }

        public string Generate(TypeDefNode typedefNode)
        {
            return string.Format("{0}typedef {1} {2};\n", _indent, typedefNode.SourceType, typedefNode.DestType);
        }

        public string Generate(FunctionNode function)
        {
            var s = "";

            var templateSpecializationPrefixStr = (function.TemplateSpecialization?.Count > 0) ? $"{_indent}template <>\n" : "";
            var templateSpecializationSuffixStr = (function.TemplateSpecialization?.Count > 0) ? $"<{string.Join(", ", function.TemplateSpecialization)}>" : "";
            var declSpecStr = (function.DeclSpec == null)  ? "" : function.DeclSpec + "\n";
            var isFriendStr = (function.IsFriend == false) ? "" : "friend ";
            var isInlineStr = (function.IsInline == false) ? "" : "inline ";
            var isStaticStr = (function.IsStatic == false) ? "" : "static ";
            var callingConventionStr = (function.CallingConvention == null)  ? "" : function.CallingConvention + " ";

            var uniqueSentinelName = UniqueSentinelNameFor(function);
            if (function.UseDefineSentinel)
                s += string.Format("#ifndef {0}\n#define {0}\n", uniqueSentinelName);

            s += string.Format("{0}{1}{2}{3}{4}{5}{6} {7}{8}{9}({10})\n",
                templateSpecializationPrefixStr,
                _indent,
                declSpecStr,
                isFriendStr,
                isInlineStr,
                isStaticStr,
                function.ReturnType,
                callingConventionStr,
                function.Name,
                templateSpecializationSuffixStr,
                Generate(function.Parameters));

            s += string.Format("{0}{{\n", _indent);
            _indent++;

            if (function.Statements != null)
            {
                foreach (var statement in function.Statements)
                {
                    s += Generate(statement);
                }
            }

            _indent--;
            s += string.Format("{0}}}\n", _indent);

            if (function.UseDefineSentinel)
                s += $"#endif /*{uniqueSentinelName}*/\n";

            return s;
        }

        private static string UniqueSentinelNameFor(FunctionNode function)
        {
            var sentinelName = function.Name.ToUpper();
            if (function.TemplateSpecialization != null)
                sentinelName += "_" + string.Join("_", function.TemplateSpecialization.Select(s => s.ToUpper()));

            return sentinelName.Replace(":", "_");
        }

        public string Generate(IStatementNode statement)
        {
            if (statement is ExpressionStatementNode)
                return Generate(statement as ExpressionStatementNode);
            if (statement is ReturnStatementNode)
                return Generate(statement as ReturnStatementNode);
            if (statement is StringStatementNode)
                return Generate(statement as StringStatementNode);
            if (statement is FieldDeclarationStatementNode)
                return Generate(statement as FieldDeclarationStatementNode);
            if (statement is VariableDeclarationStatementNode)
                return Generate(statement as VariableDeclarationStatementNode);
            if (statement is IfStatementNode)
                return Generate(statement as IfStatementNode);
            if (statement is StubStatementNode)
                return Generate(statement as StubStatementNode);
            if (statement is BlockNode)
                return Generate(statement as BlockNode);
            if (statement is LabelNode)
                return Generate(statement as LabelNode);

            throw new NotImplementedException("Unhandled StatementNode type in CppGenerator");
        }

        public string Generate(ExpressionStatementNode statement)
        {
            return string.Format("{0}{1};\n", _indent, Generate(statement.Expression));
        }

        public string Generate(ReturnStatementNode statement)
        {
            if (statement.Expression != null)
                return string.Format("{0}return {1};\n", _indent, Generate(statement.Expression));
            else
                return string.Format("{0}return;\n", _indent);
        }

        public string Generate(StringStatementNode statement)
        {
            return string.Format("{0}{1};\n", _indent, statement.Str);
        }

        public string Generate(FieldDeclarationStatementNode statement)
        {
            var constModifier = (statement.IsConst ? "const " : "");
            var staticModifier = (statement.IsStatic ? "static " : "");
            string initializer = null;
            if (statement.Initializer != null)
                initializer = $" = {Generate(statement.Initializer)}";

            string array = null;
            if (statement.ElementCount != null)
                array = $"[{statement.ElementCount.Value}]";

            return $"{_indent}{staticModifier}{constModifier}{statement.Type} {statement.Name}{array}{initializer};\n";
        }

        public string Generate(VariableDeclarationStatementNode statement)
        {
            string res;
            if (statement.Initializer == null)
                res = string.Format("{0} {1};\n", statement.Type, statement.Name);
            else
                res = string.Format("{0} {1} = {2};\n", statement.Type, statement.Name, Generate(statement.Initializer));

            return _indent + res;
        }

        public string Generate(IfStatementNode statement)
        {
            var s = "";

            s += string.Format("{0}if ({1})\n", _indent, Generate(statement.Condition));
            s += GenerateBlock(statement.Statements);

            return s;
        }

        private string GenerateBlock(IEnumerable<IStatementNode> statements)
        {
            string s = "";
            s += string.Format("{0}{{\n", _indent);
            _indent++;

            foreach (var bodyStatement in statements)
            {
                s += Generate(bodyStatement);
            }

            _indent--;
            s += string.Format("{0}}}\n\n", _indent);
            return s;
        }

        public string Generate(StubStatementNode statement)
        {
            var s = "";

            s += string.Format("{0}#if {1}\n", _indent, Generate(statement.Condition));
            _indent++;

            foreach (var bodyStatement in statement.Statements)
            {
                s += Generate(bodyStatement);
            }

            _indent--;

            if (statement.StatementsForStub != null)
            {
                s += string.Format("{0}#else\n", _indent);
                _indent++;
                foreach (var statementNode in statement.StatementsForStub)
                {
                    s += Generate(statementNode);
                }
                _indent--;
            }
            s += string.Format("{0}#endif\n", _indent);

            return s;
        }

        public string Generate(BlockNode statement)
        {
            return GenerateBlock(statement.Statements);
        }

        public string Generate(LabelNode statement)
        {
            return string.Format("{0}:\n", statement.Name);
        }

        public string Generate(IExpressionNode expression)
        {
            if (expression is FunctionCallNode)
                return Generate(expression as FunctionCallNode);
            if (expression is FieldReferenceNode)
                return Generate(expression as FieldReferenceNode);
            if (expression is StringExpressionNode)
                return Generate(expression as StringExpressionNode);
            if (expression is AssignNode)
                return Generate(expression as AssignNode);

            throw new NotImplementedException("Unhandled ExpressionNode type in CppGenerator");
        }

        public string Generate(FunctionCallNode callNode)
        {
            return string.Format("{0}{1}({2})", Generate(callNode.Qualifier), callNode.FunctionName,
                Generate(callNode.Arguments));
        }

        public string Generate(FieldReferenceNode referenceNode)
        {
            return string.Format("{0}{1}", Generate(referenceNode.Qualifier), referenceNode.Name);
        }

        public string Generate(AssignNode statement)
        {
            return string.Format("{0} = {1}", Generate(statement.Lhs), Generate(statement.Rhs));
        }

        private object Generate(IMemberQualifier qualifier)
        {
            if (qualifier is StaticMemberQualifier)
                return Generate((StaticMemberQualifier)qualifier);
            if (qualifier is FreeQualifier)
                return Generate((FreeQualifier)qualifier);
            if (qualifier is InstanceMemberQualifier)
                return Generate((InstanceMemberQualifier)qualifier);

            throw new NotImplementedException("Unhandled MemberQualifier type in CppGenerator");
        }

        public string Generate(StaticMemberQualifier call)
        {
            return string.Format("{0}::", call.ClassName);
        }

        public string Generate(FreeQualifier call)
        {
            return string.Empty;
        }

        public string Generate(InstanceMemberQualifier call)
        {
            return call.Expression + (call.IsReference ? "." : "->");
        }

        public string Generate(StringExpressionNode node)
        {
            return node.Str;
        }

        public string Generate(IEnumerable<FunctionParameter> parameters)
        {
            if (parameters == null)
                return string.Empty;
            return string.Join(", ", parameters.Select(p => p.Type + " " + p.Name));
        }

        public string Generate(IEnumerable<FunctionArgument> arguments)
        {
            if (arguments == null)
                return string.Empty;
            return string.Join(", ", arguments.Select(t => t.Name));
        }

        public string Generate(FunctionParameter parameter)
        {
            return $"{parameter.Type} {parameter.Name}";
        }

        public string Generate(FunctionArgument argument)
        {
            return argument.Name;
        }

        public class Indent
        {
            private string _cur = "";

            public Indent()
            {
            }

            public Indent(int initialIndent)
            {
                _cur = new string('\t', initialIndent);
            }

            public static Indent operator++(Indent i)
            {
                i._cur = i._cur + "\t";
                return i;
            }

            public static Indent operator--(Indent i)
            {
                if (i._cur.Length > 0)
                    i._cur = i._cur.Substring(0, i._cur.Length - 1);

                return i;
            }

            public override string ToString()
            {
                return _cur;
            }

            public static implicit operator string(Indent i)
            {
                return i._cur;
            }
        }
    }
}
