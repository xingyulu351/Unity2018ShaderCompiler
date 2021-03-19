using System;
using System.Collections;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Text.RegularExpressions;
using Bee.Tools;
using NiceIO;
using Unity.TinyProfiling;
using Unity.BuildTools;

namespace JamSharp.Runtime
{
    public sealed class JamList : IEnumerable<string>
    {
        static readonly char[] PathSeparators = {'/', '\\'};

        [DebuggerStepThrough]
        public JamList()
        {
            Elements = Array.Empty<string>();
            SetupStats();
        }

        [DebuggerStepThrough]
        public JamList(string value)
        {
            Elements = new[] { value };
            SetupStats();
        }

        [DebuggerStepThrough]
        public JamList(params string[] values)
        {
            if (values == null)
                Elements = Array.Empty<string>();
            else
                Elements = values;
            SetupStats();
        }

        [DebuggerStepThrough]
        public JamList(IEnumerable<string> values)
        {
            Elements = values.ToArray();
            SetupStats();
        }

        [DebuggerStepThrough]
        public JamList(params JamList[] values)
        {
            Elements = ElementsOf(values);
            SetupStats();
        }

        [DebuggerStepThrough]
        public JamList(IEnumerable<NPath> paths)
        {
            Elements = paths.Select(p => p.ToString(SlashMode.Forward)).ToArray();
            SetupStats();
        }

        [DebuggerStepThrough]
        public JamList(IEnumerable<JamList> values)
        {
            Elements = ElementsOf(values.ToArray());
            SetupStats();
        }

        [DebuggerStepThrough]
        JamList(JamTarget[] targets)
        {
            Elements = targets.Select(t => t.JamTargetName).ToArray();
            SetupStats();
        }

        string[] _arrayBacking;

        //the _multiArrayBacking storage option is here to speed up a very rare, but important case where there is code
        //that appends a single line to an existing value like 50k times. (this happens on project generation).  In this
        //case it's too expensive to keep flattening into a new array, so we store appends in an array, and only flatten
        //to a single array when our elements are requested.
        List<string[]> _multiArrayBacking;

        public int Count => Elements.Length;

        public string[] Elements
        {
            [DebuggerStepThrough]
            get
            {
                if (_multiArrayBacking != null)
                {
                    _arrayBacking = _multiArrayBacking.SelectMany(s => s).ToArray();
                    _multiArrayBacking = null;
                }
                return _arrayBacking;
            }
            [DebuggerStepThrough]
            set
            {
                _arrayBacking = value;
                _multiArrayBacking = null;
            }
        }

        public string this[int index] => Elements[index];

        public IEnumerable<JamList> ElementsAsJamLists => Elements.Select(e => new JamList(e));

        public bool IsEmpty => Elements.Length == 0;

        public void Append(JamList values)
        {
            Append(values.Elements);
        }

        public void Append(params JamList[] values)
        {
            Append(ElementsOf(values));
        }

        public void Append(IEnumerable<string> values)
        {
            Append(values.ToArray());
        }

        public void Append(string[] addition)
        {
            AddHistory(ListAction.Add, addition.Length);

            if (addition.Length == 0)
                return;

            if (_multiArrayBacking != null)
            {
                _multiArrayBacking.Add(addition);
            }
            else
            {
                _multiArrayBacking = new List<string[]>() {_arrayBacking, addition};
                _arrayBacking = null;
            }
        }

        public void Subtract(params JamList[] values)
        {
            var elementsOf = new HashSet<string>(ElementsOf(values));

            AddHistory(ListAction.Subtract, elementsOf.Count);
            Elements = Elements.Where(e => !elementsOf.Contains(e)).ToArray();
        }

        public void AssignIfEmpty(params JamList[] values)
        {
            if (Elements.Any())
                return;
            Assign(values);
        }

        public void AssignIfEmpty(params string[] values)
        {
            if (Elements.Any())
                return;
            Assign(values);
        }

        public void AssignIfEmpty(JamList value)
        {
            if (Elements.Any())
                return;
            Assign(value);
        }

        public void Assign()
        {
            Elements = Array.Empty<string>();
            AddHistory(ListAction.Assign, Elements.Length);
        }

        public void Assign(string value)
        {
            Elements = new[] { value };
        }

        public void Assign(params string[] values)
        {
            Elements = values;
            AddHistory(ListAction.Assign, Elements.Length);
        }

        public void Assign(IEnumerable<string> values)
        {
            Elements = values.ToArray();
            AddHistory(ListAction.Assign, Elements.Length);
        }

        public void Assign(JamList values)
        {
            Elements = values.Elements;
            AddHistory(ListAction.Assign, Elements.Length);
        }

        public void Assign(params JamList[] values)
        {
            Elements = ElementsOf(values);
            AddHistory(ListAction.Assign, Elements.Length);
        }

        [DebuggerStepThrough]
        public static implicit operator JamList(string input)
        {
            return new JamList(input);
        }

        public static implicit operator JamList(NPath path)
        {
            return new JamList(path.ToString());
        }

        public static implicit operator JamList(NPath[] paths)
        {
            return new JamList(paths.Select(p => p.ToString()).ToArray());
        }

        public static implicit operator JamList(JamTarget jamTarget)
        {
            return new JamList(jamTarget.JamTargetName);
        }

        public static implicit operator JamList(JamTarget[] target)
        {
            return new JamList(target);
        }

        public JamList ToUpper()
        {
            return MapElementsToNewJamList(e => e.ToUpperInvariant(), "U");
        }

        public JamList ToLower()
        {
            return MapElementsToNewJamList(e => e.ToLowerInvariant(), "L");
        }

        public JamList JoinWithValue(JamList seperator)
        {
            return JoinWithString(seperator.Elements.Single());
        }

        public JamList Join()
        {
            return JoinWithString(string.Empty);
        }

        JamList JoinWithString(string seperator)
        {
            return Elements.Length == 0 ? new JamList() : new JamList(string.Join(seperator, Elements));
        }

        public bool AsBool()
        {
            return Elements.Length > 0 && Elements.Any(e => e.Length > 0);
        }

        public JamList BackSlashify() => MapElementsToNewJamList(s => s.Replace('/', '\\'), "\\");

        public JamList ForwardSlashify() => MapElementsToNewJamList(s => s.Replace('\\', '/'), "/");

        public JamList Escape()
        {
            return MapElementsToNewJamList(Escape, "C");
        }

        static string Escape(string value)
        {
            if (Environment.OSVersion.Platform.ToString().Contains("Win"))
                return (value.Contains(" ") || value.Contains("/")) ? $"\"{value}\"" : value;

            var needsEscape = new[] {'"', ' ', '(', ')', '\\', '$'};
            var sb = new StringBuilder();
            for (int i = 0; i != value.Length; i++)
            {
                var c = value[i];
                if (needsEscape.Contains(c))
                    sb.Append('\\');
                sb.Append(c);
            }
            return sb.ToString();
        }

        public JamList Dereference()
        {
            return GlobalVariables.Singleton.DereferenceElements(Elements);
        }

        public JamList Include(JamList patterns)
        {
            var list = new List<string>(128);

            for (int i = 0; i < patterns.Count; i++)
            {
                foreach (var value in Elements)
                {
                    bool add = true;
                    for (int p = i; p < patterns.Count; p++)
                    {
                        if (!Regex.IsMatch(value, ConvertJamRegexToDotNet(patterns[p])))
                        {
                            add = false;
                            break;
                        }
                    }
                    if (add)
                        list.Add(value);
                }
            }

            return new JamList(list);
        }

        public static string ConvertJamRegexToDotNet(string pattern)
        {
            if (pattern.Contains(@"\<") || pattern.Contains(@"\>"))
                throw new Exception(@"'\<' and '\>' are not supported in jam#'s regex.");

            var regex = new StringBuilder(pattern.Length);
            bool escaping = false;

            for (int i = 0; i < pattern.Length; i++)
            {
                char ch = pattern[i];
                if (escaping)
                {
                    if (new char[] { '\\', '.', '+', '?', '*', '^', '$', ':', '(', ')', '|', '[', ']', '<', '>' }.Contains(ch))
                        regex.Append('\\');
                    regex.Append(ch);
                    escaping = false;
                }
                else
                {
                    if (ch == '\\')
                    {
                        // Jam's regex doesn't support trailing '\', so use regex that matches nothing.
                        if (i + 1 == pattern.Length)
                        {
                            BuiltinRules.Echo(@"re error trailing \");
                            BuiltinRules.Echo(@"re error NULL parameter");
                            return "$^";
                        }
                        escaping = true;
                    }
                    else
                        regex.Append(ch);
                }
            }

            return regex.ToString();
        }

        private bool MatchesPatterns(string input, string pattern)
        {
            return Regex.IsMatch(input, pattern);
        }

        public JamList Exclude(JamList patterns)
        {
            JamList csharp;
            using (TinyProfiler.Section("Exclude", patterns.ToString()))
            {
                try
                {
                    csharp = new JamList(Elements.Where(e => !MatchesPatterns(e, patterns.Elements.Single())));
                }
                catch (Exception e)
                {
                    csharp = new JamList(e.ToString());
                }
            }

            return csharp;
        }

        public JamList Parent()
        {
            return MapElementsToNewJamList(GetParent, "P");
        }

        static string GetParent(string element)
        {
            string grist;
            string withoutGrist;
            SplitGristed(element, out grist, out withoutGrist);

            var lastSlash = withoutGrist.LastIndexOf('/');
            // Jam ignores \ on unix systems when parsing path, but later normalizes \'s to / when reporting result
            if (Path.DirectorySeparatorChar == '\\')
            {
                var windowsSlash = withoutGrist.LastIndexOf('\\');
                if (windowsSlash > lastSlash)
                    lastSlash = windowsSlash;
            }

            var dir = lastSlash >= 0 ? withoutGrist.Substring(0, lastSlash) : string.Empty;
            dir = NormalizePath(dir);

            // Jam reports C:/dir as C:
            if (dir.Length == 3 && char.IsLetter(dir[0]) && dir[1] == ':' && (dir[2] == '/' || dir[2] == '\\'))
                dir = dir.Substring(0, 2);

            return grist + dir;
        }

        public JamList GetDirectory()
        {
            return MapElementsToNewJamList(GetDirectory, "D");
        }

        static string GetDirectory(string element)
        {
            var nonGrist = StripGrist(element);
            var lastSlash = nonGrist.LastIndexOfAny(new[] {'/', '\\'});
            var dir = lastSlash >= 0 ? nonGrist.Substring(0, lastSlash) : string.Empty;
            return NormalizePath(dir);
        }

        public JamList SetDirectory(JamList value)
        {
            return MapElementsToNewJamList(SetDirectory, "D", value);
        }

        string SetDirectory(string element, string newDirectory)
        {
            string grist;
            string nonGristElement;
            SplitGristed(element, out grist, out nonGristElement);
            var baseAndSuffix = BaseAndSuffix(nonGristElement);
            var path = (newDirectory.Length > 0 ? newDirectory + "/" : string.Empty) + baseAndSuffix;
            return grist + NormalizePath(path);
        }

        public static string StripGrist(string value)
        {
            int gristEnd = value.IndexOf('>');
            if (gristEnd > 0)
                return value.Substring(gristEnd + 1);
            else
                return value;
        }

        public static void SplitGristed(string value, out string grist, out string withoutGrist)
        {
            withoutGrist = StripGrist(value);
            if (withoutGrist.Length != value.Length)
                grist = value.Substring(0, value.Length - withoutGrist.Length);
            else
                grist = "";
        }

        public JamList Rooted(JamList value)
        {
            return MapElementsToNewJamList(Rooted, "R", value);
        }

        static string Rooted(string element, string root)
        {
            if (root == ".")
                return element;

            string grist;
            SplitGristed(element, out grist, out element);

            string path;
            if ((element.Length > 0) && (element[0] == '/'))
                path = element;
            else
                path = root.Length == 0 ? element : System.IO.Path.Combine(root, element);

            return grist + NormalizePath(path);
        }

        public JamList BaseAndSuffix()
        {
            return MapElementsToNewJamList(BaseAndSuffix, "BS");
        }

        static string BaseAndSuffix(string element)
        {
            var nonGrist = StripGrist(element);
            var lastSlash = nonGrist.LastIndexOfAny(new[] {'/', '\\'});
            return nonGrist.Substring(lastSlash < 0 ? 0 : lastSlash + 1);
        }

        public JamList SetBasePath(JamList values)
        {
            return MapElementsToNewJamList(SetBaseFileName, "B", values);
        }

        public string SetBaseFileName(string path, string value)
        {
            string grist;
            string withoutGrist;
            SplitGristed(path, out grist, out withoutGrist);

            string directory;
            string fileName;
            SplitFilePath(withoutGrist, out directory, out fileName);

            var fileExtension = Path.GetExtension(fileName);

            return grist + NormalizePath(directory) + "/" + value + fileExtension;
        }

        void SplitFilePath(string path, out string directory, out string fileName)
        {
            var index = path.LastIndexOfAny(new char[] {'\\', '/'});
            if (index >= 0)
            {
                directory = path.Substring(0, index);
                fileName = path.Substring(index + 1, path.Length - index - 1);
            }
            else
            {
                directory = path;
                fileName = string.Empty;
            }
        }

        public JamList GetFileNameBase()
        {
            return MapElementsToNewJamList(GetFileNameBase, "B");
        }

        public string GetFileNameBase(string element)
        {
            var stripDir = BaseAndSuffix(element);
            var firstDot = stripDir.LastIndexOf(".");
            return firstDot == -1 ? stripDir : stripDir.Substring(0, firstDot);
        }

        // Some file globbing queries are repeated with the same parameters. Use a cache
        // to return previous results for repeated identical requests.
        private static Dictionary<string, JamList> _globCache = new Dictionary<string, JamList>();

        public JamList JamGlob()
        {
            var key = ToString();
            JamList cachedResult;
            if (_globCache.TryGetValue(key, out cachedResult))
                return cachedResult.Clone();

            var csharp = new JamList(Elements.SelectMany(Glob.Search));
            _globCache.Add(key, csharp.Clone());
            return csharp;
        }

        public JamList JamGlob(JamList value)
        {
            var key = ToString() + "!" + value;
            JamList cachedResult;
            if (_globCache.TryGetValue(key, out cachedResult))
                return cachedResult.Clone();

            var glob = Elements.SelectMany(Glob.Search);
            var list = new List<string>();
            foreach (var strip in value.Elements)
                foreach (var path in glob)
                    list.Add(path.StartsWith(strip) ? path.Substring(strip.Length) : path);

            var csharp = new JamList(list);
            _globCache.Add(key, csharp.Clone());
            return csharp;
        }

        public JamList GetBoundPath() => MapElementsToNewJamList(GetBoundPath, "T");

        static readonly Dictionary<string, string> _target_to_boundpath = new Dictionary<string, string>();

        static string GetBoundPath(string target)
        {
            if (_target_to_boundpath.TryGetValue(target, out var result))
                return result;

            var bound = GetBoundPathInternal(target);
            _target_to_boundpath.Add(target, bound);
            return bound;
        }

        static string GetBoundPathInternal(string target)
        {
            // take care of EXPAND_TEXT setting, if any (only used in gcc-like toolchains for linked libraries in V1)
            var targetVars = GlobalVariables.Singleton.Storage.AllOnTargetVariablesOn(target);
            bool hasExpandText = targetVars.TryGetValue("EXPAND_TEXT", out JamList expandText);
            if (hasExpandText && expandText.Count > 0)
                return expandText[0];

            NPath bound;

            var binding = GlobalVariables.Singleton.Storage.VariableOnTarget("BINDING", target);
            if ((binding != null) && (binding.Count > 0))
                bound = new NPath(StripGrist(binding[0]));
            else
                bound = new NPath(StripGrist(target));

            var locate = GlobalVariables.Singleton.Storage.VariableOnTarget("LOCATE", target);
            var search = GlobalVariables.Singleton.Storage.VariableOnTarget("SEARCH", target);
            if ((locate != null) && (locate.Count > 0))
            {
                var path = locate[0];
                if (path == ".")
                    path = "";
                if (bound.IsRelative)
                    bound = new NPath(path).Combine(bound);
            }
            else if (search != null)
            {
                if (bound.IsRelative)
                {
                    foreach (var path in search)
                    {
                        var check = new NPath(path).Combine(bound.ToString());
                        if (check.Exists())
                            return check.ToString(SlashMode.Forward);
                    }
                }

                var searchExtensions = GlobalVariables.Singleton.Storage.VariableOnTarget("SEARCH_EXTENSIONS", target);
                foreach (var extension in searchExtensions)
                {
                    foreach (var path in search)
                    {
                        var check = bound.IsRelative
                            ? new NPath(path).Combine(bound.ToString() + extension)
                            : new NPath(bound.ToString() + extension);

                        if (check.Exists())
                            return check.ToString(SlashMode.Forward);
                    }
                }
            }

            return bound.ToString(SlashMode.Forward);
        }

        public JamList IfEmptyUse()
        {
            return MapElementsToNewJamList(e => e, "E");
        }

        public JamList SetGrist(JamList grists)
        {
            using (TinyProfiler.Section("SetGrist", grists.ToString()))
                return MapElementsToNewJamList(SetGrist, "G", grists);
        }

        static string SetGrist(string element, string grist)
        {
            string result = NormalizePath(StripGrist(element));
            grist = grist.TrimStart('<').TrimEnd('>');
            if (grist.Length != 0)
                result = '<' + grist + '>' + result;

            return result;
        }

        public static string NormalizePath(string path)
        {
            var npath = new NPath(path);
            var normalizePath = npath.ToString(SlashMode.Forward);

            if (normalizePath.Length == 0 || normalizePath == ".")
                return "";

            return normalizePath;
        }

        public JamList Grist()
        {
            return MapElementsToNewJamList(Grist, "G");
        }

        JamList MapElementsToNewJamList(Func<string, string> mapFunc, string jamModifier)
        {
            return MapElementsToNewJamList((value, arg) => mapFunc(value), jamModifier, null);
        }

        JamList MapElementsToNewJamList(Func<string, string, string> mapFunc, string jamModifier, JamList jamModifierArgument)
        {
            var result = new List<string>();
            var arguments = jamModifierArgument == null ? new string[] { null } : jamModifierArgument.Elements;

            foreach (var argument in arguments)
            {
                for (int i = 0; i != Elements.Length; i++)
                    result.Add(mapFunc(Elements[i], argument));
            }

            return new JamList(result);
        }

        private static string Grist(string element)
        {
            var start = element.IndexOf('<');
            var end = element.IndexOf('>');
            return start >= 0 ? element.Substring(start, end + 1) : string.Empty;
        }

        public JamList IfEmptyUse(JamList value)
        {
            return (Elements.Length > 0 ? this : value).Clone();
        }

        public JamList WithSuffix(JamList suffixes)
        {
            return MapElementsToNewJamList(SetSuffix, "S", suffixes);
        }

        string SetSuffix(string element, string suffix)
        {
            string grist;
            string elementWithoutGrist;
            SplitGristed(element, out grist, out elementWithoutGrist);
            var oldsuffix = GetSuffix(elementWithoutGrist);
            return grist + NormalizePath(elementWithoutGrist.Substring(0, elementWithoutGrist.Length - oldsuffix.Length) + suffix);
        }

        public JamList GetSuffix()
        {
            return MapElementsToNewJamList(GetSuffix, "S");
        }

        string GetSuffix(string element)
        {
            string grist;
            string nonGrist;
            SplitGristed(element, out grist, out nonGrist);

            if (nonGrist.Length == 0)
                return string.Empty;

            var fileNameStart = nonGrist.LastIndexOfAny(PathSeparators);
            int count = fileNameStart < 0 ? nonGrist.Length : nonGrist.Length - fileNameStart;

            var lastDot = nonGrist.LastIndexOf('.', nonGrist.Length - 1, count);
            return lastDot < 0 ? string.Empty : nonGrist.Substring(lastDot);
        }

        public JamList IndexedBy(int indexStartAtOne)
        {
            // match behavior of DoIndex
            if (indexStartAtOne < 1)
                indexStartAtOne = 1;
            int index = indexStartAtOne - 1;
            if (index >= Elements.Length)
                return new JamList();
            return new JamList(Elements[index]);
        }

        public JamList IndexedBy(JamList indices)
        {
            var result = new List<string>();
            foreach (var index in indices.Elements)
            {
                var indexJamList = DoIndex(Elements, index);
                if (indexJamList != null)
                    result.AddRange(indexJamList);
            }

            return new JamList(result);
        }

        static string[] DoIndex(string[] elements, string indexString)
        {
            int lowerBound = 1;
            var higherBound = elements.Length;
            var dashIndex = indexString.IndexOf("-");
            var nsException = new NotSupportedException("Cannot index by non-integer: " + indexString);
            if (dashIndex == -1)
            {
                if (!int.TryParse(indexString, out higherBound))
                    throw nsException;
                lowerBound = higherBound;
            }
            else if (dashIndex == 0)
            {
                if (!int.TryParse(indexString.Substring(1), out higherBound))
                    throw nsException;
            }
            else if (dashIndex == indexString.Length - 1)
            {
                if (!int.TryParse(indexString.Substring(0, indexString.Length - 1), out lowerBound))
                    throw nsException;
            }
            else
            {
                var split = indexString.Split('-');
                if (!int.TryParse(split[0], out lowerBound))
                    throw nsException;
                if (!int.TryParse(split[1], out higherBound))
                    throw nsException;
            }

            if (lowerBound > elements.Length)
                return null;

            if (higherBound < lowerBound)
                higherBound = lowerBound;
            if (lowerBound < 1)
                lowerBound = 1;
            if (higherBound < 1)
                higherBound = 1;
            if (higherBound > elements.Length)
                higherBound = elements.Length;

            //jam list indexing starts counting at 1.
            return elements.Skip(lowerBound - 1).Take(higherBound - lowerBound + 1).ToArray();
        }

        public JamList Sorted()
        {
            return new JamList(Elements.OrderBy(s => s));
        }

        public bool JamEquals(JamList other)
        {
            var otherElements = other.Elements;

            if (otherElements.Length > 0 && otherElements[0].Length == 0 && Elements.Length == 0)
                return true;

            var otherElementsLength = otherElements.Length;
            var myElements = Elements;

            if (otherElementsLength != myElements.Length)
                return false;

            for (var i = 0; i != otherElementsLength; i++)
            {
                if (myElements[i] != otherElements[i])
                    return false;
            }

            return true;
        }

        public bool NotJamEquals(JamList value)
        {
            return !JamEquals(value);
        }

        public static bool operator==(JamList jamList, string value)
        {
            if (value == null)
                return ((object)jamList) == null;

            if (value.Length == 0 && jamList.Elements.Length == 0)
                return true;

            if (jamList.Elements.Length != 1)
                return false;
            return jamList.Elements[0] == value;
        }

        public static bool operator!=(JamList a, string value)
        {
            return !(a == value);
        }

        public static implicit operator bool(JamList x)
        {
            return x.AsBool();
        }

        public JamTarget[] AsJamTargets()
        {
            return Elements.Select(e => new JamTarget(e)).ToArray();
        }

        public NPath[] ToNPaths()
        {
            return Elements.Select(e => new NPath(e)).ToArray();
        }

        public IEnumerator<string> GetEnumerator()
        {
            return ((IEnumerable<string>)Elements).GetEnumerator();
        }

        IEnumerator IEnumerable.GetEnumerator()
        {
            return GetEnumerator();
        }

        public override bool Equals(object obj)
        {
            return base.Equals(obj);
        }

        public override int GetHashCode()
        {
            return base.GetHashCode();
        }

        public bool IsIn(params JamList[] values) => Elements.All(e => values.SelectMany(l => l.Elements).Contains(e));

        public bool GreaterThan(JamList right)
        {
            return CompareWith(right) > 0;
        }

        public bool GreaterThanOrEqual(JamList right)
        {
            return CompareWith(right) >= 0;
        }

        public bool LessThan(JamList right)
        {
            return CompareWith(right) < 0;
        }

        public bool LessThanOrEqual(JamList right)
        {
            return CompareWith(right) <= 0;
        }

        int CompareWith(JamList right)
        {
            return StringComparer.InvariantCulture.Compare(Elements.Single(), right.Elements.Single());
        }

        public bool And(JamList value)
        {
            return AsBool() && value.AsBool();
        }

        public bool Or(JamList value)
        {
            return AsBool() || value.AsBool();
        }

        public override string ToString()
        {
            return string.Join(" ", Elements);
        }

        JamList InvokeInternalModifier(string modifierLetter, JamList argument = null)
        {
            JamCore.SetVar("internal_temp1", Operator.VAR_SET, Elements);
            if (argument == null)
                return new JamList(JamCore.Expand($"$(internal_temp1:{modifierLetter})"));

            if (!argument.Elements.Any())
                return new JamList(JamCore.Expand($"$(internal_temp1:{modifierLetter}=$(nonexisting_))"));

            JamCore.SetVar("internal_temp2", Operator.VAR_SET, argument.Elements);
            return new JamList(JamCore.Expand($"$(internal_temp1:{modifierLetter}=$(internal_temp2))"));
        }

        static string[] ElementsOf(JamList[] values)
        {
            return values.Length == 1
                ? values[0].Elements
                : values.SelectMany(v => v.Elements).ToArray();
        }

        static string[] MergeArrays(string[] one, string[] two)
        {
            if (one.Length == 0)
                return two;
            if (two.Length == 0)
                return one;

            var result = new string[one.Length + two.Length];
            Array.Copy(one, result, one.Length);
            Array.ConstrainedCopy(two, 0, result, one.Length, two.Length);
            return result;
        }

        public enum ListAction
        {
            Add,
            Assign,
            Subtract,
            Create
        }

#if Histograms
        static readonly List<JamList> s_AllJamLists = new List<JamList>();

        public List<KeyValuePair<ListAction, int>> _history;

        [Conditional("Histograms")]
        void SetupStats()
        {
            s_AllJamLists.Add(this);
            if (_history == null)
                _history = new List<KeyValuePair<ListAction, int>>();
            _history.Add(new KeyValuePair<ListAction, int>(ListAction.Create, _elements.Length));
        }

        [Conditional("Histograms")]
        void AddHistory(ListAction action, int additionLength)
        {
            _history.Add(new KeyValuePair<ListAction, int>(action, additionLength));
        }

        public static void PrintStats()
        {
            var s = s_AllJamLists.OrderByDescending(l => l._history.Count);
            foreach (var l in s.Take(50))
            {
                var cs = (CSharpVariableStorage)GlobalVariables.Singleton.Storage;

                Console.WriteLine("Var: " + cs.VariableNameOf(l));
                foreach (var h in l._history.Take(10))
                {
                    Console.Write(h.Key);
                    Console.Write(" ");
                    Console.Write(h.Value);
                    Console.Write(" ");
                }
                Console.WriteLine();
                Console.WriteLine();
            }


            Console.WriteLine("Historram of assign amounts:");
            foreach (
                var group in
                s_AllJamLists.SelectMany(l => l._history)
                    .Where(kvp => kvp.Key == ListAction.Assign)
                    .GroupBy(kvp => kvp.Value).OrderByDescending(g => g.Count()).Take(30))
            {
                Console.WriteLine(group.Key + " count:" + group.Count());
            }

            Console.WriteLine("");
            Console.WriteLine("Historram of add amounts:");
            foreach (
                var group in
                s_AllJamLists.SelectMany(l => l._history)
                    .Where(kvp => kvp.Key == ListAction.Add)
                    .GroupBy(kvp => kvp.Value).OrderByDescending(g => g.Count()).Take(30))
            {
                Console.WriteLine(group.Key + " count:" + group.Count());
            }
        }

#else
        [Conditional("Histograms")]
        void SetupStats() {}

        [Conditional("Histograms")]
        void AddHistory(ListAction action, int additionLength) {}
#endif

        public JamList FailIfNotSorted([CallerFilePath] string sourceFilePath = "", [CallerLineNumber] int sourceLineNumber = 0)
        {
            for (int i = 0; i != Elements.Length - 1; i++)
            {
                if (String.Compare(Elements[i], Elements[i + 1], StringComparison.InvariantCultureIgnoreCase) == 1)
                {
                    Errors.PrintErrorWithLocation($"This list needs to be sorted to reduce amount of merge conflicts. Element {Elements[i+1].InQuotes()} is not in its proper location.\nThe correct sorting order for this list is:\n{Elements.OrderBy(a=>a).InQuotes().SeparateWith(",\n")}", sourceFilePath, sourceLineNumber);
                    throw new ArgumentException("Unsorted source file list");
                }
            }
            return this;
        }

        public JamList FailIfHasDuplicates([CallerFilePath] string sourceFilePath = "", [CallerLineNumber] int sourceLineNumber = 0)
        {
            var sorted = Sorted();

            for (int i = 0; i != sorted.Elements.Length - 1; i++)
            {
                if (sorted.Elements[i] == sorted.Elements[i + 1])
                {
                    Errors.PrintErrorWithLocation($"This list can not contain duplicate entries. Element {sorted.Elements[i].InQuotes()} found multiple times.", sourceFilePath, sourceLineNumber);
                    throw new ArgumentException("List contains duplicates, not allowed here");
                }
            }
            return this;
        }
    }

    static class Helper
    {
        public static string ToPrintableString(this JamList[] lists)
        {
            var builder = new StringBuilder(128);
            foreach (var list in lists)
            {
                if (list == null)
                {
                    builder.Append("<null>");
                }
                else
                {
                    builder.Append('[');
                    builder.Append(list.ToString());
                    builder.Append(']');
                }
            }
            return builder.ToString();
        }

        //stolen from eric lippert
        internal static IEnumerable<IEnumerable<string>> CartesianProduct(this IEnumerable<IEnumerable<string>> sequences)
        {
            IEnumerable<IEnumerable<string>> emptyProduct = new[] { Enumerable.Empty<string>() };
            return sequences.Aggregate(emptyProduct, (accumulator, sequence) => from accseq in accumulator from item in sequence select accseq.Concat(new[] { item }));
        }
    }
}
