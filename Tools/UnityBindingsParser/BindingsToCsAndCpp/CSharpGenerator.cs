using System.Linq;
using System.Text.RegularExpressions;
using ICSharpCode.NRefactory.CSharp;

namespace BindingsToCsAndCpp
{
    internal class CSharpGenerator : GeneratorBase
    {
        private string _obsoleteKind;

        public CSharpGenerator()
        {
            AppendLine("using scm=System.ComponentModel;");
            AppendLine("using uei=UnityEngine.Internal;");
            AppendLine("using RequiredByNativeCodeAttribute=UnityEngine.Scripting.RequiredByNativeCodeAttribute;");
            AppendLine("using UsedByNativeCodeAttribute=UnityEngine.Scripting.UsedByNativeCodeAttribute;");
        }

        private void WriteClassStart(string content, bool isSealed, bool isStatic = false)
        {
            var sig = content.TrimStart();
            var visibility = ParseVisibility(ref sig, "public");
            var mod = ParseAndRemoveKeyword(ref sig, new[] {"abstract"}, "");
            if (mod != "")
                mod += " ";
            var parts = sig.Split(':');

            Append("{0} {1}{2}{3}partial class {4}", visibility, isStatic ? "static " : "", mod, isSealed ? "sealed " : "", parts[0].Trim());
            if (parts.Length > 1)
                Append(" : {0}", parts[1].TrimStart());
            AppendLine();
            AppendStartBlock();
        }

        private static string ParseVisibility(ref string sig, string visibility)
        {
            return ParseAndRemoveKeyword(ref sig, ValidationHelpers.ValidVisibility, visibility);
        }

        private static readonly string[] unsafeKeyword = { "unsafe" };

        private static string ParseUnsafeSpecifier(ref string sig)
        {
            return ParseAndRemoveKeyword(ref sig, unsafeKeyword, null);
        }

        private static string ParseAndRemoveKeyword(ref string sig, string[] keywords, string defaultValue)
        {
            var end = sig.IndexOf('(');
            var str = sig;
            if (end > 0)
                str = sig.Substring(end - 1);

            var ret = defaultValue;
            foreach (var keyword in keywords)
            {
                var key = keyword + " ";
                var m = Regex.Match(str, key);
                if (!m.Success)
                    continue;
                str = str.Substring(0, m.Index) + str.Substring(m.Index + key.Length);
                ret = keyword;
                break;
            }

            sig = str;
            return ret;
        }

        private void OutputObsoleteAndClearFlag()
        {
            if (_obsoleteKind != "planned")
            {
                if (_isObsolete)
                    AppendLine(ParsingHelpers.GenerateObsolete(_obsoleteKind, _obsoleteText));
            }

            ClearObsolete();
        }

        private void ClearObsolete()
        {
            _isObsolete = false;
            _obsoleteText = null;
            _obsoleteKind = null;
        }

        protected override void OnEndCsRaw()
        {
            base.OnEndCsRaw();
            if (!InsideTag("ENUM"))
                AppendEndConditions();
        }

        bool _isOnCsRaw;

        public override void OnCsRaw(string[] content)
        {
            var snippet = ParsingHelpers.RemoveComments(content);
            if (string.IsNullOrWhiteSpace(snippet))
                return;

            _isOnCsRaw = _onCsRaw;

            base.OnCsRaw(content);

            if (!InsideTag("ENUM"))
                SaveConditions();

            snippet = snippet.Replace("\r\n", "\n");
            var blocks = ParsingHelpers.ParseRawCS(snippet);
            foreach (var block in blocks)
                ParseCsRaw(block.Item1, block.Item2, block.Item3);
            if (snippet.EndsWith("\n"))
                AppendLine();
        }

        void ParseCsRaw(string snippet, bool parsed, bool ignore)
        {
            if (!parsed)
            {
                if (ignore)
                {
                    AppendLine(snippet);
                    return;
                }
                var match = Regex.Match(snippet.Trim(), @"^namespace\s+([a-zA-Z0-9_\.\s]*)");
                if (match.Success)
                {
                    StartNamespace(match.Groups[1].Value.Trim());
                    AppendLine(snippet);
                    return;
                }
            }

            var expression = CSharpSignatureAnalyzer.Analyze(CurrentBlockName, snippet);
            if (expression == null)
            {
                if (InsideTag("ENUM"))
                {
                    foreach (var line in snippet.Split('\n'))
                    {
                        if (string.IsNullOrWhiteSpace(line))
                            continue;
                        if (line.Trim() == "{")
                            continue;
                        if (line.Trim() == "}")
                        {
                            EndEnum();
                            break;
                        }
                        OnEnumMember(line);
                    }
                }
                else
                {
                    if (snippet.Trim() == "}")
                        EndNamespace();
                    AppendLine(snippet);
                }
            }
            else
            {
                var ended = false;

                switch (expression.Type)
                {
                    case "method":
                    {
                        // it might be a valid method signature, but if parsed == false
                        // then the snippet isn't complete, so we shouldn't be parsing it
                        if (!parsed)
                        {
                            AppendLine(snippet);
                            return;
                        }
                        var result = expression.ParseCSRawMethods();
                        foreach (var line in result)
                        {
                            if (line.Item2 != null)
                            {
                                var method = new CSRawMethod(CurrentBlockName, line.Item1, line.Item2 as MethodDeclaration)
                                {
                                    ParentIsStruct = InsideStruct,
                                    IsSync = _isSync,
                                    IsObsolete = _isObsolete,
                                    ObsoleteKind = _obsoleteKind,
                                    ObsoleteText = _obsoleteText,
                                };
                                ClearObsolete();
                                AppendLine(method.Output);
                                if (!ended)
                                {
                                    AppendEndConditions();
                                    ended = true;
                                    SaveConditions();
                                }
                            }
                            else
                                AppendLine(line.Item1);
                        }
                        break;
                    }
                    case "enum":
                        AppendLine(snippet);
                        AppendEndConditions();
                        SaveConditions();
                        break;
                    case "enumfragment":
                        PushBackConditions();
                        StartEnum(expression.Nodes[0].Format());
                        break;
                    case "constructor":
                    case "property":
                        OutputObsoleteAndClearFlag();
                        AppendLine(snippet);
                        AppendEndConditions();
                        SaveConditions();
                        break;
                    case "attribute":
                        OutputObsoleteAndClearFlag();
                        AppendLine(snippet);
                        if (!_isOnCsRaw)
                            PushBackConditions();
                        break;
                    case "field":
                        OutputObsoleteAndClearFlag();
                        AppendLine(snippet);
                        break;
                    default:
                        AppendLine(snippet);
                        break;
                }
            }
        }

        public override void OnConditional(string content)
        {
            if (_onCsRaw)
                EndCsRaw();
            AppendLineNoIndent("#if {0}", content);
            base.OnConditional(content);
        }

        public override void OnAttribute(string content)
        {
            base.OnAttribute(content);
            AppendLine(content);
        }

        public override void OnObsolete(string kind, string content)
        {
            base.OnObsolete(kind, content);
            _isObsolete = true;
            _obsoleteKind = kind;
            _obsoleteText = content;
        }

        public override void StartEnum(string content)
        {
            base.StartEnum(content);

            var signature = ParsingHelpers.ParseEnum(content);

            ValidationHelpers.ValidateVisibility(signature.Item2);
            ParsingHelpers.Enums.Add(signature.Item1);

            OutputObsoleteAndClearFlag();

            Append("{0} enum {1}", signature.Item2, signature.Item1);
            if (signature.Item3 != null)
                Append(" : {0}", signature.Item3);
            AppendLine();
            AppendStartBlock();
        }

        public override void OnEnumMember(string content)
        {
            base.OnEnumMember(content);
            content = ParsingHelpers.RemoveCommentsFromLine(content);

            OutputObsoleteAndClearFlag();

            AppendLine(content.Trim());
        }

        public override void EndEnum()
        {
            base.EndEnum();
            AppendEndBlock();
            AppendEndConditions();
        }

        public override void OnPlainContent(string content)
        {
            content = content.Clean("\r\n", "\n");
            base.OnPlainContent(content);
            content = ParsingHelpers.RemoveComments(content.Split('\n'));
            AppendLine(content);
        }

        public override void StartClass(string content)
        {
            var name = ParsingHelpers.GetClassFrom(content);

            base.StartClass(content);
            SaveConditions();

            OutputObsoleteAndClearFlag();

            WriteClassStart(content, true);
            _classes.Push(
                new Unit
                {
                    Name = name,
                    Namespace = Namespace,
                    Conditions = GetConditions(),
                    IsThreadSafe = _isClassThreadSafe
                });
        }

        public override void EndClass()
        {
            base.EndClass();
            AppendEndBlock();

            AppendEndConditions();

            _classes.Pop();
        }

        public override void StartNonSealedClass(string content)
        {
            var name = ParsingHelpers.GetClassFrom(content);

            base.StartNonSealedClass(content);
            SaveConditions();

            OutputObsoleteAndClearFlag();

            WriteClassStart(content, false);
            _classes.Push(
                new Unit
                {
                    Name = name,
                    Namespace = Namespace,
                    Conditions = GetConditions(),
                    IsThreadSafe = _isClassThreadSafe
                });
        }

        public override void EndNonSealedClass()
        {
            base.EndNonSealedClass();
            AppendEndBlock();
            AppendEndConditions();
            _classes.Pop();
        }

        public override void StartStaticClass(string content)
        {
            var name = ParsingHelpers.GetClassFrom(content);

            base.StartStaticClass(content);
            SaveConditions();

            OutputObsoleteAndClearFlag();

            WriteClassStart(content, false, true);
            _classes.Push(
                new Unit()
                {
                    Name = name,
                    Namespace = Namespace,
                    Conditions = GetConditions(),
                    IsThreadSafe = _isClassThreadSafe
                });
        }

        public override void EndStaticClass()
        {
            base.EndStaticClass();
            AppendEndBlock();
            AppendEndConditions();
            _classes.Pop();
        }

        public override void OnAutoProp(string content)
        {
            base.OnAutoProp(content);
            SaveConditions();

            content = ParsingHelpers.RemoveCommentsFromLine(content);

            var unit = new CSAutoProperty(CurrentBlockName, content)
            {
                IsObsolete = _isObsolete,
                ObsoleteKind = _obsoleteKind,
                ObsoleteText = _obsoleteText,
                ParentIsStruct = InsideStruct,
                IsSync = _isSync,
                IsThreadSafe = _isThreadSafe
            };
            ClearObsolete();

            AppendNoIndent(unit.GenerateOutput(_stringBuilder.CurrentIndentationLevel));
            AppendEndConditions();
        }

        public override void OnAutoPtrProp(string content)
        {
            base.OnAutoProp(content);
            SaveConditions();
            var unit = new CSAutoProperty(CurrentBlockName, content)
            {
                IsObsolete = _isObsolete,
                ObsoleteKind = _obsoleteKind,
                ObsoleteText = _obsoleteText,
                ParentIsStruct = InsideStruct,
                IsSync = _isSync,
                IsPointer = true,
                IsThreadSafe = _isThreadSafe
            };
            ClearObsolete();

            AppendNoIndent(unit.GenerateOutput(_stringBuilder.CurrentIndentationLevel));
            AppendEndConditions();

            ClearFlags();
        }

        public override void OnAuto(string content)
        {
            base.OnAuto(content);
            SaveConditions();
            var unit = new CSAutoMethod(CurrentBlockName, content)
            {
                IsObsolete = _isObsolete,
                ObsoleteKind = _obsoleteKind,
                ObsoleteText = _obsoleteText,
                ParentIsStruct = InsideStruct,
                IsSync = _isSync,
                IsThreadSafe = _isThreadSafe
            };
            ClearObsolete();

            AppendNoIndent(unit.GenerateOutput(_stringBuilder.CurrentIndentationLevel));
            AppendEndConditions();

            ClearFlags();
        }

        public override void OnCustom(string content)
        {
            content = content.Clean("\r\n", "\n");
            content = ParsingHelpers.RemoveComments(content.Split('\n'));

            base.OnCustom(content);
            SaveConditions();

            var decl = ParsingHelpers.SplitSignatureAndBodyFromCustom(content);
            var signature = CSharpSignatureAnalyzer.PrepareSignatureMethod(decl.Item1);

            var expression = CSharpSignatureAnalyzer.Analyze(CurrentBlockName, signature);
            Unit unit = null;

            if (expression != null && expression.Type == "constructor")
                unit = new CSConstructor(CurrentBlockName, content)
                {
                    IsObsolete = _isObsolete,
                    ObsoleteKind = _obsoleteKind,
                    ObsoleteText = _obsoleteText,
                    ParentIsStruct = InsideStruct,
                    IsSync = _isSync,
                    IsThreadSafe = _isThreadSafe
                };
            else
                unit = new CSMethod(CurrentBlockName, content)
                {
                    IsObsolete = _isObsolete,
                    ObsoleteKind = _obsoleteKind,
                    ObsoleteText = _obsoleteText,
                    ParentIsStruct = InsideStruct,
                    IsSync = _isSync,
                    IsThreadSafe = _isThreadSafe
                };
            ClearObsolete();

            AppendNoIndent(unit.GenerateOutput(_stringBuilder.CurrentIndentationLevel));
            AppendEndConditions();

            ClearFlags();
        }

        public override void OnCustomProp(string content)
        {
            base.OnCustomProp(content);
            SaveConditions();

            var unit = new CSProperty(CurrentBlockName, content)
            {
                IsObsolete = _isObsolete,
                ObsoleteKind = _obsoleteKind,
                ObsoleteText = _obsoleteText,
                ParentIsStruct = InsideStruct,
                IsSync = _isSync,
                IsThreadSafe = _isThreadSafe
            };
            ClearObsolete();

            AppendNoIndent(unit.GenerateOutput(_stringBuilder.CurrentIndentationLevel));
            AppendEndConditions();

            ClearFlags();
        }

        public override void StartStruct(string content)
        {
            base.StartStruct(content);
            SaveConditions();

            var unsafeSpec = ParseUnsafeSpecifier(ref content);
            if (unsafeSpec != null)
                unsafeSpec += " ";
            var visibility = ParseVisibility(ref content, "public");

            OutputObsoleteAndClearFlag();

            AppendLine("[System.Runtime.InteropServices.StructLayout (System.Runtime.InteropServices.LayoutKind.Sequential)]");
            AppendLine("{0} {1}partial struct {2}", visibility, unsafeSpec, content);

            AppendStartBlock();

            _structName = ParsingHelpers.GetLastWord(content);
        }

        public override void StartStructNoLayout(string content)
        {
            base.StartStructNoLayout(content);
            SaveConditions();

            OutputObsoleteAndClearFlag();

            var unsafeSpec = ParseUnsafeSpecifier(ref content);
            if (unsafeSpec != null)
                unsafeSpec += " ";
            var visibility = ParseVisibility(ref content, "public");

            AppendLine("{0} {1}partial struct {2}", visibility, unsafeSpec, content);
            AppendStartBlock();
            _structName = ParsingHelpers.GetLastWord(content);
        }

        public override void EndStruct()
        {
            base.EndStruct();
            FinishStruct();
        }

        public override void EndStructNoLayout()
        {
            base.EndStructNoLayout();
            FinishStruct();
        }

        void FinishStruct()
        {
            AppendEndBlock();
            AppendEndConditions();
            _structName = null;
        }

        public override string GetOutput()
        {
            return _stringBuilder.Output;
        }
    }
}
