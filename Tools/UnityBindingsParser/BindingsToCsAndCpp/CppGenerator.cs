using System.Collections.Generic;
using System.Linq;
using System.Text.RegularExpressions;

namespace BindingsToCsAndCpp
{
    class CppGenerator : GeneratorBase
    {
        readonly List<CppUnit> _units = new List<CppUnit>();

        readonly string _targetPlatform;
        readonly string _filename;

        readonly Dictionary<string, string> _typeMap;
        readonly Dictionary<string, string> _retTypeMap;

        readonly IndentedWriter _accumulatedContent = new IndentedWriter();

        public CppGenerator(string filename, Dictionary<string, string> typeMap, Dictionary<string, string> retTypeMap, string target)
        {
            _filename = filename;
            _typeMap = typeMap;
            _retTypeMap = retTypeMap;
            _targetPlatform = target;
        }

        bool _isOnCsRaw;
        public override void OnCsRaw(string[] content)
        {
            var snippet = ParsingHelpers.RemoveComments(content);
            if (string.IsNullOrWhiteSpace(snippet))
                return;

            _isOnCsRaw = _onCsRaw;
            if (!_onCsRaw)
                SaveConditions();

            base.OnCsRaw(content);

            snippet = snippet.Replace("\r\n", "\n");
            var blocks = ParsingHelpers.ParseRawCS(snippet);
            foreach (var block in blocks)
                ParseCsRaw(block.Item1, block.Item2, block.Item3);
        }

        void ParseCsRaw(string snippet, bool parsed, bool ignore)
        {
            if (!parsed)
            {
                if (ignore)
                    return;
                var match = Regex.Match(snippet.Trim(), @"^namespace\s+([a-zA-Z0-9_\.\s]*)");
                if (match.Success)
                {
                    StartNamespace(match.Groups[1].Value.Trim());
                    return;
                }
            }

            var expression = CSharpSignatureAnalyzer.Analyze(CurrentBlockName, snippet);
            if (expression == null)
            {
                if (_accumulatedContent.Length > 0)
                {
                    var str = _accumulatedContent + "\n" + snippet;
                    expression = CSharpSignatureAnalyzer.Analyze(CurrentBlockName, str);
                    if (expression == null || expression.Type != "enum")
                        return;
                    EndConditions();
                    SaveConditions();
                    _accumulatedContent.Clear();
                }
                else
                {
                    if (snippet.Trim() == "}")
                        EndNamespace();
                }
            }
            else
            {
                var ended = false;
                switch (expression.Type)
                {
                    case "method":
                    {
                        var result = expression.ParseCSRawMethods();
                        foreach (var line in result)
                        {
                            if (line.Item2 == null || ended)
                                continue;
                            EndConditions();
                            ended = true;
                            SaveConditions();
                        }
                        break;
                    }
                    case "enumfragment":
                    {
                        Accumulate(snippet);
                        break;
                    }
                    case "enum":
                    case "constructor":
                    case "property":
                    {
                        EndConditions();
                        SaveConditions();
                        break;
                    }
                    case "attribute":
                    {
                        if (!_isOnCsRaw)
                            PushBackConditions();
                        break;
                    }
                }
            }
        }

        private void Accumulate(string snippet)
        {
            _accumulatedContent.AppendLine(snippet);
        }

        public override void OnCppRaw(string[] content)
        {
            Stats.CppContentFound = true;
            base.OnCppRaw(content);

            var snippet = ParsingHelpers.RemoveComments(content).Replace("\r\n", "\n");

            // whitespace hacks for comparison's sake
            if (snippet.StartsWith("#") ||
                (!snippet.EndsWith("\n") && snippet.Contains("\n")))
                snippet += "\n";

            _units.Add(new CppText(_typeMap, _retTypeMap, snippet) { Parent = CurrentClass });
        }

        public override void StartClass(string content)
        {
            base.StartClass(content);
            SaveConditions(CurrentClassConditions);

            var name = ParsingHelpers.GetClassFrom(content);
            _classes.Push(
                new Unit
                {
                    Name = name,
                    Namespace = Namespace,
                    IsThreadSafe = _isClassThreadSafe,
                    Conditions = GetConditions(),
                    Parent = CurrentClass,
                    TargetPlatform = _targetPlatform
                });
        }

        public override void StartNonSealedClass(string content)
        {
            base.StartClass(content);
            SaveConditions(CurrentClassConditions);

            var name = ParsingHelpers.GetClassFrom(content);
            _classes.Push(
                new Unit
                {
                    Name = name,
                    Namespace = Namespace,
                    IsThreadSafe = _isClassThreadSafe,
                    Conditions = GetConditions(),
                    Parent = CurrentClass,
                    TargetPlatform = _targetPlatform
                });
        }

        public override void StartStaticClass(string content)
        {
            base.StartStaticClass(content);
            SaveConditions(CurrentClassConditions);

            var name = ParsingHelpers.GetClassFrom(content);
            _classes.Push(
                new Unit()
                {
                    Name = name,
                    Namespace = Namespace,
                    IsThreadSafe = _isClassThreadSafe,
                    Conditions = GetConditions(),
                    Parent = CurrentClass,
                    TargetPlatform = _targetPlatform
                });
        }

        public override void EndClass()
        {
            base.EndClass();
            EndConditions();

            _classes.Pop();
        }

        public override void EndNonSealedClass()
        {
            base.EndNonSealedClass();
            EndConditions();

            _classes.Pop();
        }

        public override void EndStaticClass()
        {
            base.EndStaticClass();
            EndConditions();

            _classes.Pop();
        }

        public override void StartStruct(string content)
        {
            base.StartStruct(content);
            SaveConditions();

            _structName = ParsingHelpers.GetLastWord(content);
            _classes.Push(
                new Unit
                {
                    Name = _structName,
                    Namespace = Namespace,
                    IsThreadSafe = _isClassThreadSafe,
                    Conditions = GetConditions(),
                    Parent = CurrentClass,
                    TargetPlatform = _targetPlatform
                });
        }

        public override void StartStructNoLayout(string content)
        {
            base.StartStructNoLayout(content);
            SaveConditions();

            _structName = ParsingHelpers.GetLastWord(content);
            _classes.Push(
                new Unit
                {
                    Name = _structName,
                    Namespace = Namespace,
                    IsThreadSafe = _isClassThreadSafe,
                    Conditions = GetConditions(),
                    Parent = CurrentClass,
                    TargetPlatform = _targetPlatform
                });
        }

        public override void EndStruct()
        {
            base.EndStruct();
            EndConditions();

            _classes.Pop();
            _structName = null;
        }

        public override void EndStructNoLayout()
        {
            base.EndStructNoLayout();
            EndConditions();

            _classes.Pop();
            _structName = null;
        }

        public override void OnAutoProp(string content)
        {
            Stats.CppContentFound = true;
            base.OnAutoProp(content);
            SaveConditions(CurrentClassConditions);

            content = ParsingHelpers.RemoveCommentsFromLine(content);

            var unit = new CSAutoProperty(CurrentBlockName, content)
            {
                Namespace = Namespace,
                ParentIsStruct = InsideStruct,
                IsSync = _isSync,
                IsThreadSafe = _isThreadSafe || CurrentClass.IsThreadSafe,
                Conditions = GetConditions(),
                Parent = CurrentClass,
                TargetPlatform = _targetPlatform
            };

            _units.AddRange(CppProperty.ParseProperty(_typeMap, _retTypeMap, unit));
            ClearFlags();
        }

        public override void OnAutoPtrProp(string content)
        {
            Stats.CppContentFound = true;
            base.OnAutoProp(content);
            SaveConditions(CurrentClassConditions);

            var unit = new CSAutoProperty(CurrentBlockName, content)
            {
                Namespace = Namespace,
                ParentIsStruct = InsideStruct,
                IsSync = _isSync,
                IsThreadSafe = _isThreadSafe || CurrentClass.IsThreadSafe,
                Conditions = GetConditions(),
                Parent = CurrentClass,
                IsPointer = true,
                TargetPlatform = _targetPlatform
            };
            _units.AddRange(CppProperty.ParseProperty(_typeMap, _retTypeMap, unit));
            ClearFlags();
        }

        public override void OnAuto(string content)
        {
            Stats.CppContentFound = true;
            base.OnAuto(content);
            SaveConditions(CurrentClassConditions);

            var unit = new CSAutoMethod(CurrentBlockName, content)
            {
                Namespace = Namespace,
                ParentIsStruct = InsideStruct,
                IsSync = _isSync,
                IsThreadSafe = _isThreadSafe || CurrentClass.IsThreadSafe,
                Conditions = GetConditions(),
                Parent = CurrentClass,
                TargetPlatform = _targetPlatform
            };

            _units.Add(new CppMethod(_typeMap, _retTypeMap, unit));
            ClearFlags();
        }

        public override void OnCustom(string content)
        {
            content = content.Clean("\r\n", "\n");
            content = ParsingHelpers.RemoveComments(content.Split('\n'));

            Stats.CppContentFound = true;
            base.OnCustom(content);
            SaveConditions(CurrentClassConditions);

            var decl = ParsingHelpers.SplitSignatureAndBodyFromCustom(content);
            var signature = decl.Item1 + "{}";

            var expression = CSharpSignatureAnalyzer.Analyze(CurrentBlockName, signature);

            if (expression != null && expression.Type == "constructor")
            {
                var unit = new CSConstructor(CurrentBlockName, content)
                {
                    Namespace = Namespace,
                    ParentIsStruct = InsideStruct,
                    IsSync = _isSync,
                    IsThreadSafe = _isThreadSafe || CurrentClass.IsThreadSafe,
                    Conditions = GetConditions(),
                    Parent = CurrentClass,
                    TargetPlatform = _targetPlatform
                };
                _units.Add(new CppMethod(_typeMap, _retTypeMap, unit));
            }
            else
            {
                var unit = new CSMethod(CurrentBlockName, content)
                {
                    Namespace = Namespace,
                    ParentIsStruct = InsideStruct,
                    IsSync = _isSync,
                    IsThreadSafe = _isThreadSafe || CurrentClass.IsThreadSafe,
                    Conditions = GetConditions(),
                    Parent = CurrentClass,
                    TargetPlatform = _targetPlatform
                };

                _units.Add(new CppMethod(_typeMap, _retTypeMap, unit));
            }
            ClearFlags();
        }

        public override void OnCustomProp(string content)
        {
            content = content.Clean("\r\n", "\n");
            content = ParsingHelpers.RemoveComments(content.Split('\n'));

            Stats.CppContentFound = true;
            base.OnCustomProp(content);
            SaveConditions(CurrentClassConditions);

            var unit = new CSProperty(CurrentBlockName, content)
            {
                Namespace = Namespace,
                ParentIsStruct = InsideStruct,
                IsSync = _isSync,
                IsThreadSafe = _isThreadSafe || CurrentClass.IsThreadSafe,
                Conditions = GetConditions(),
                Parent = CurrentClass,
                TargetPlatform = _targetPlatform
            };
            _units.AddRange(CppProperty.ParseProperty(_typeMap, _retTypeMap, unit));
            ClearFlags();
            EndConditions();
        }

        public override void StartEnum(string content)
        {
            base.StartEnum(content);
            EndConditions();
        }

        public override void OnDocumentation(string content)
        {
            base.OnDocumentation(content);

            // TODO: take this out once we're not comparing output with the perl parser,
            // this is purely whitespace matching
            if (!_onCppRaw)
                return;
            var unit = _units.LastOrDefault();
            if (unit != null && unit is CppText && !unit.OriginalContent.EndsWith("\n"))
                unit.OriginalContent += "\n";
        }

        public override string GetOutput()
        {
            if (!Stats.CppContentFound)
                return "//just a dummyfile\n";

            var exports = CppExports.Prepare(_filename, _units, _targetPlatform);
            exports.Generate();
            _stringBuilder.Append(exports.Output);

            return _stringBuilder.Output;
        }
    }
}
