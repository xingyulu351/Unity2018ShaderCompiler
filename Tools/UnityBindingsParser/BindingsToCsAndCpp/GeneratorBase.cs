using System;
using System.Collections.Generic;
using System.Linq;
using UnityBindingsParser;

namespace BindingsToCsAndCpp
{
    internal class Statistics
    {
        public Stack<string> OpenTags = new Stack<string>();

        int _auto;
        int _class;
        int _cppRaw;
        int _csRaw;
        int _custom;
        int _customProp;
        int _conditional;
        int _document;
        int _enum;
        int _obsolete;
        int _typemap;
        int _struct;
        int _other;

        public int Auto
        {
            get { return _auto; }
            set
            {
                _auto = value;
            }
        }

        public int Class
        {
            get { return _class; }
            set
            {
                _class = value;
                OpenTags.Push("CLASS");
            }
        }

        public int CsRaw
        {
            get { return _csRaw; }
            set
            {
                _csRaw = value;
            }
        }

        public int Custom
        {
            get { return _custom; }
            set
            {
                _custom = value;
            }
        }

        public int CppRaw
        {
            get { return _cppRaw; }
            set
            {
                _cppRaw = value;
            }
        }

        public int CustomProp
        {
            get { return _customProp; }
            set
            {
                _customProp = value;
            }
        }

        public int Conditional
        {
            get { return _conditional; }
            set
            {
                _conditional = value;
            }
        }

        public int Document
        {
            get { return _document; }
            set
            {
                _document = value;
            }
        }

        public int Enum
        {
            get { return _enum; }
            set
            {
                _enum = value;
                OpenTags.Push("ENUM");
            }
        }

        public int Obsolete
        {
            get { return _obsolete; }
            set
            {
                _obsolete = value;
            }
        }

        public int Struct
        {
            get { return _struct; }
            set
            {
                _struct = value;
                OpenTags.Push("STRUCT");
            }
        }

        public int Other
        {
            get { return _other; }
            set
            {
                _other = value;
            }
        }

        public int Typemap
        {
            get { return _typemap; }
            set
            {
                _typemap = value;
            }
        }


        public int TotalFound
        {
            get
            {
                return Auto + _class + _conditional + _cppRaw + _csRaw + _custom +
                    _customProp + _document + _enum + _obsolete + _struct + _typemap + _other;
            }
        }

        public bool OnlyRawContentFound { get { return TotalFound - _csRaw - _cppRaw == 0; } }

        public bool OnlyConditionalsFound { get { return TotalFound - _cppRaw - _csRaw - _conditional == 0; } }


        public bool CppContentFound = false;
    }

    internal class GeneratorBase : IBindingsConsumer
    {
        protected readonly IndentedWriter _stringBuilder = new IndentedWriter();

        public Statistics Stats = new Statistics();
        List<string> _conditions = new List<string>();
        protected List<string> CurrentOpenConditions { get { return _conditions; } }

        readonly Stack<string[]> _savedConditions = new Stack<string[]>();
        protected bool _isObsolete = false;
        protected string _obsoleteText;
        protected bool _onCsRaw;
        protected bool _onCppRaw;
        protected Stack<Unit> _classes = new Stack<Unit>();
        protected string _structName;

        // to get over a bug in cspreprocess, remember if we've seen a
        // struct recently, to tweak the property output in case of a sync_jobs tag
        protected bool _isSync;
        protected bool _isThreadSafe;
        protected bool _isClassThreadSafe;
        readonly Stack<string> _namespaces = new Stack<string>();
        protected string Namespace
        {
            get
            {
                if (_namespaces.Count == 0)
                    return "UnityEngine;";
                return string.Join(".", _namespaces.ToArray().Reverse());
            }
        }

        protected void AppendNoIndent(string s, params object[] args)
        {
            _stringBuilder.AppendNoIndent(s, args);
        }

        protected void Append(string s, params object[] args)
        {
            _stringBuilder.Append(s, args);
        }

        protected void AppendLine()
        {
            _stringBuilder.AppendLine();
        }

        protected void AppendLine(string s, params object[] args)
        {
            _stringBuilder.AppendLine(s, args);
        }

        protected void AppendLineNoIndent()
        {
            _stringBuilder.AppendLineNoIndent();
        }

        protected void AppendLineNoIndent(string s, params object[] args)
        {
            _stringBuilder.AppendLineNoIndent(s, args);
        }

        protected void AppendIndent()
        {
            _stringBuilder.AppendIndent();
        }

        protected void AppendStartBlock(string padding = "")
        {
            _stringBuilder.AppendStartBlock(padding);
        }

        protected void AppendEndBlock(string padding = "")
        {
            _stringBuilder.AppendEndBlock(padding);
        }

        protected void StartBlock()
        {
            _stringBuilder.StartBlock();
        }

        protected void EndBlock()
        {
            _stringBuilder.EndBlock();
        }

        /// <summary>
        /// Get all the CONDITIONAL tags that are currently open and
        /// save them for the current block, meaning that this block is
        /// going to be encased in an #if/#endif
        /// </summary>
        /// <param name="inherited"></param>
        protected void SaveConditions(List<string> inherited = null)
        {
            //if (CurrentClass != null && CurrentClass.Name.Contains("AudioSettings"))
            //    System.Diagnostics.Debugger.Break();

            var conds = new List<string>();
            if (inherited != null)
                conds.AddRange(inherited);
            conds.AddRange(_conditions);
            PushSaveConditions(conds);
            _conditions.Clear();
        }

        private void PushSaveConditions(List<string> conds)
        {
            _savedConditions.Push(conds.ToArray());
        }

        private List<string> PopSaveConditions()
        {
            return _savedConditions.Pop().ToList();
        }

        /// <summary>
        /// There are cases where a certain block will be processed in multiple parts
        /// (i.e., a CSRAW block with an [attribute] and a method, or with a bunch of enums
        /// or documentation between the code.
        /// In those cases, the #if should only be closed after the only CSRAW block is processed
        /// and not before. This call will pop any conditions that were saved by a previous SaveConditions
        /// call so that they won't be closed on the next EndConditions or AppendEndConditions call.
        /// It makes sense, I promise :P
        /// </summary>
        protected void PushBackConditions()
        {
            if (_savedConditions.Count > 0)
                _conditions = PopSaveConditions();
        }

        protected List<string> GetConditions()
        {
            return _savedConditions.Peek().ToList();
        }

        protected List<string> EndConditions()
        {
            return PopSaveConditions();
        }

        protected string CurrentConditionBlock()
        {
            return _savedConditions.Count > 0 ? _savedConditions.Peek().LastOrDefault() ?? "" : "";
        }

        protected virtual void AppendStartConditions(List<string> conditions, bool prependNewLine = false)
        {
            if (conditions.Count == 0)
                return;
            var str = string.Format("#if {0}",
                conditions.Count > 1
                ? string.Join(" && ", conditions.Select(x => "(" + x + ")"))
                : string.Join("", conditions));
            AppendLineNoIndent(str);
        }

        protected virtual void AppendEndConditions(bool prependNewLine = false)
        {
            if (_savedConditions.Count == 0)
                return;
            var conds = PopSaveConditions();
            AppendEndConditions(conds, prependNewLine);
        }

        protected virtual void AppendEndConditions(List<string> conditions, bool prependNewLine = false)
        {
            if (conditions.Count == 0)
                return;
            if (prependNewLine)
                AppendLineNoIndent();
            foreach (var c in conditions)
                AppendLineNoIndent("#endif");
        }

        protected virtual void AppendEndCondition(bool prependNewLine = false)
        {
            if (prependNewLine)
                AppendLine();
            AppendLineNoIndent("#endif");
        }

        protected virtual void StartNamespace(string nspace)
        {
            _namespaces.Push(nspace);
        }

        protected virtual void EndNamespace()
        {
            if (_namespaces.Count > 0)
                _namespaces.Pop();
            else
                Console.WriteLine("Warning: found end of namespace, but no start");
        }

        #region consumer implementation

        protected virtual void OnEndCsRaw()
        {
        }

        protected virtual void OnEndCppRaw()
        {
        }

        protected void EndRaw()
        {
            EndCsRaw();
            EndCppRaw();
        }

        protected void EndCsRaw()
        {
            if (_onCsRaw)
            {
                _onCsRaw = false;
                OnEndCsRaw();
            }
        }

        protected void EndCppRaw()
        {
            if (_onCppRaw)
            {
                _onCppRaw = false;
                OnEndCppRaw();
            }
        }

        public virtual void OnCppRaw(string content)
        {
        }

        public virtual void OnCppRaw(string[] content)
        {
            EndCsRaw();
            Stats.CppRaw++;
            _onCppRaw = true;
        }

        public virtual void OnCsRaw(string content)
        {
        }

        public virtual void OnCsRaw(string[] content)
        {
            EndCsRaw();
            EndCppRaw();
            Stats.CsRaw++;
            _onCsRaw = true;
        }

        public virtual void OnDocumentation(string content)
        {
            Stats.Document++;
        }

        public virtual void OnPlainContent(string content)
        {
            Stats.Other++;
        }

        public virtual void OnAttribute(string content)
        {
        }

        public virtual void OnConvertExample()
        {
            Stats.Other++;
        }

        public virtual void OnExample(string content)
        {
            Stats.Other++;
        }

        public virtual void OnObsolete(string kind, string content)
        {
            Stats.Obsolete++;
            EndCsRaw();
        }

        public virtual void OnConditional(string content)
        {
            //if (CurrentClass != null && CurrentClass.Name.Contains("AudioSettings"))
            //    System.Diagnostics.Debugger.Break();
            Stats.Conditional++;
            _conditions.Add(content);
        }

        public virtual void StartEnum(string content)
        {
            Stats.Enum++;
            EndRaw();
            SaveConditions();
        }

        public virtual void OnEnumMember(string content)
        {
        }

        public virtual void EndEnum()
        {
            Stats.OpenTags.Pop();
        }

        public virtual void StartClass(string codeContent)
        {
            Stats.Class++;
            EndRaw();
            _isThreadSafe = false;
        }

        public virtual void StartNonSealedClass(string content)
        {
            Stats.Class++;
            EndRaw();
            _isThreadSafe = false;
        }

        public virtual void StartStaticClass(string content)
        {
            Stats.Class++;
            EndRaw();
            _isThreadSafe = false;
        }

        public virtual void EndClass()
        {
            EndRaw();
            Stats.OpenTags.Pop();
        }

        public virtual void EndNonSealedClass()
        {
            EndRaw();
            Stats.OpenTags.Pop();
        }

        public virtual void EndStaticClass()
        {
            EndRaw();
            Stats.OpenTags.Pop();
        }

        public virtual void OnThreadSafe()
        {
            EndRaw();
            Stats.Other++;
            _isThreadSafe = true;
            _isClassThreadSafe = true;
        }

        public virtual void OnCustom(string content)
        {
            EndRaw();
            Stats.Custom++;
            _isClassThreadSafe = false;
        }

        public virtual void OnCustomProp(string content)
        {
            EndRaw();
            Stats.CustomProp++;
            _isClassThreadSafe = false;
        }

        public virtual void OnAuto(string content)
        {
            EndRaw();
            Stats.Auto++;
            _isClassThreadSafe = false;
        }

        public virtual void OnAutoProp(string codeContent)
        {
            Stats.Other++;
            EndRaw();
            _isClassThreadSafe = false;
        }

        public virtual void OnAutoPtrProp(string content)
        {
            EndRaw();
            Stats.Other++;
            _isClassThreadSafe = false;
        }

        public virtual void OnCsNone(string content)
        {
            Stats.Other++;
        }

        public virtual void StartStruct(string content)
        {
            Stats.Struct++;
            EndRaw();
        }

        public virtual void EndStruct()
        {
            EndRaw();
            Stats.OpenTags.Pop();
        }

        public virtual void StartStructNoLayout(string content)
        {
            Stats.Struct++;
            EndRaw();
        }

        public virtual void EndStructNoLayout()
        {
            EndRaw();
            Stats.OpenTags.Pop();
        }

        public virtual void OnSyncJobs()
        {
            _isSync = true;
            EndRaw();
        }

        #endregion

        protected bool InsideStruct
        {
            get
            {
                var tags = Stats.OpenTags.ToArray().Reverse();
                var parent = tags.SkipWhile(x => x != "STRUCT" && x != "CLASS").FirstOrDefault();
                return parent == "STRUCT";
            }
        }

        protected string CurrentBlockName
        {
            get
            {
                if (_structName != null)
                    return _structName;
                if (_classes.Count > 0)
                    return _classes.Peek().Name;
                return null;
            }
        }

        protected Unit CurrentClass
        {
            get { return _classes.Count > 0 ? _classes.Peek() : null; }
        }

        protected List<string> CurrentClassConditions
        {
            get { return _classes.Count > 0 ? _classes.Peek().Conditions : null; }
        }

        protected virtual void ClearFlags()
        {
            _isSync = false;
            _isThreadSafe = false;
        }

        protected bool InsideTag(string tag)
        {
            return Stats.OpenTags.Count > 0 && Stats.OpenTags.Peek() == tag;
        }

        public virtual string GetOutput()
        {
            throw new NotImplementedException();
        }
    }
}
