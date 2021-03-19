using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text.RegularExpressions;
using NDesk.Options;

namespace Unity.Options
{
    public class ProgramOptionsAttribute : Attribute
    {
        public string Group { get; set; }
        public string CollectionSeparator { get; set; }
    }

    public sealed class OptionsParser
    {
        private static readonly Regex NameBuilder = new Regex(@"([A-Z][a-z_0-9]*)");

        public static string[] Prepare(string[] commandLine, Type[] types)
        {
            var parser = new OptionsParser();
            foreach (var type in types)
                parser.AddType(type);
            var expandedCommandLine = ExpandCommandLineResponseFiles(commandLine);
            return parser.Parse(expandedCommandLine);
        }

        public static List<string> ExpandCommandLineResponseFiles(string[] commandLine)
        {
            var res = new List<string>();
            foreach (var item in commandLine)
            {
                if (item.Trim().StartsWith("@"))
                {
                    res.AddRange(SplitCommandLine(File.ReadAllText(item.Trim().Substring(1))));
                }
                else
                {
                    res.Add(item);
                }
            }
            return res;
        }

        public static IEnumerable<string> SplitCommandLine(string commandLine)
        {
            var inQuotes = false;

            return Split(commandLine, c =>
            {
                if (c == '\"')
                    inQuotes = !inQuotes;

                return !inQuotes && c == ' ';
            })
                    .Select(arg => TrimMatchingQuotes(arg.Trim(), '\"'))
                    .Where(arg => !string.IsNullOrEmpty(arg));
        }

        public static IEnumerable<string> Split(string str, Func<char, bool> controller)
        {
            var nextPiece = 0;

            for (var c = 0; c < str.Length; c++)
            {
                if (controller(str[c]))
                {
                    yield return str.Substring(nextPiece, c - nextPiece);
                    nextPiece = c + 1;
                }
            }

            yield return str.Substring(nextPiece);
        }

        public static string TrimMatchingQuotes(string input, char quote)
        {
            if ((input.Length >= 2) &&
                (input[0] == quote) && (input[input.Length - 1] == quote))
                return input.Substring(1, input.Length - 2);

            return input;
        }

        public static string[] Prepare(string[] commandLine, Assembly assembly)
        {
            var types = new List<Type>();
            var queue = new Stack<Assembly>();
            var processed = new HashSet<AssemblyName>(new AssemblyNameComparer());

            queue.Push(assembly);

            while (queue.Count > 0)
            {
                var current = queue.Pop();
                if (!processed.Add(current.GetName()))
                    continue;

                types.AddRange(current.GetTypes().Where(HasProgramOptionsAttribute));

                foreach (var referencedAssembly in current.GetReferencedAssemblies())
                {
                    if (referencedAssembly.Name == "mscorlib" || referencedAssembly.Name.StartsWith("System") || referencedAssembly.Name.StartsWith("Mono.Cecil") || referencedAssembly.Name.StartsWith("ICSharpCode"))
                        continue;

                    if (!processed.Contains(referencedAssembly))
                        queue.Push(Assembly.Load(referencedAssembly));
                }
            }

            return Prepare(commandLine, types.ToArray());
        }

        private readonly List<Type> _types = new List<Type>();

        internal OptionsParser()
        {
        }

        internal void AddType(Type type)
        {
            _types.Add(type);
        }

        internal static bool HasProgramOptionsAttribute(Type type)
        {
            return type.GetCustomAttributes(typeof(ProgramOptionsAttribute), false).Length > 0;
        }

        internal string[] Parse(IEnumerable<string> commandLine)
        {
            var optionSet = PrepareOptionSet();
            return optionSet.Parse(commandLine).ToArray();
        }

        private OptionSet PrepareOptionSet()
        {
            var optionSet = new OptionSet();

            foreach (var type in _types)
                ExtendOptionSet(optionSet, type);

            return optionSet;
        }

        private void ExtendOptionSet(OptionSet optionSet, Type type)
        {
            var fields = type.GetFields(BindingFlags.Static | BindingFlags.Public);

            foreach (var field in fields)
            {
                var options = (ProgramOptionsAttribute)type.GetCustomAttributes(typeof(ProgramOptionsAttribute), false)[0];
                foreach (var name in OptionNamesFor(options, field))
                {
                    optionSet.Add(
                        name,
                        DescriptionFor(field),
                        ActionFor(options, field));
                }
            }
        }

        private IEnumerable<string> OptionNamesFor(ProgramOptionsAttribute options, FieldInfo field)
        {
            var name = NormalizeName(field.Name);

            if (field.FieldType != typeof(bool))
                name += "=";

            if (options.Group == null)
            {
                yield return name;
                yield return NormalizeName(field.DeclaringType.Name) + "." + name;
            }
            else
                yield return options.Group + "." + name;
        }

        private static string NormalizeName(string name)
        {
            return NameBuilder.Matches(name)
                .Cast<Match>()
                .Select(m => m.Value.ToLower())
                .Aggregate((buff, s) => buff + "-" + s);
        }

        private static string DescriptionFor(FieldInfo field)
        {
            return "";
        }

        private static Action<string> ActionFor(ProgramOptionsAttribute options, FieldInfo field)
        {
            if (field.FieldType.IsArray)
                return v => SetArrayType(field, v, options);

            if (field.FieldType.IsGenericType)
            {
                var genericType = field.FieldType.GetGenericTypeDefinition();
                if (genericType.IsAssignableFrom(typeof(List<>)))
                    return v => SetListType(field, v, options);
            }

            if (field.FieldType == typeof(bool))
                return v => SetBoolType(field, v);

            return v => SetBasicType(field, v);
        }

        private static void SetListType(FieldInfo field, string value, ProgramOptionsAttribute options)
        {
            var listType = field.FieldType;
            var list = (IList)field.GetValue(null) ?? (IList)Activator.CreateInstance(listType);

            foreach (var v in SplitCollectionValues(options, value))
                list.Add(ParseValue(listType.GetGenericArguments()[0], v));

            field.SetValue(null, list);
        }

        private static void SetArrayType(FieldInfo field, string value, ProgramOptionsAttribute options)
        {
            var index = 0;
            var values = SplitCollectionValues(options, value);
            var arrayType = field.FieldType;
            var array = (Array)field.GetValue(null);

            if (array != null)
            {
                var oldArray = array;
                array = (Array)Activator.CreateInstance(arrayType, new object[] {oldArray.Length + values.Length});
                Array.Copy(oldArray, array, oldArray.Length);
                index = oldArray.Length;
            }
            else
                array = (Array)Activator.CreateInstance(arrayType, new object[] {values.Length});

            foreach (var v in values)
                array.SetValue(ParseValue(arrayType.GetElementType(), v), index++);

            field.SetValue(null, array);
        }

        private static void SetBoolType(FieldInfo field, string v)
        {
            field.SetValue(null, true);
        }

        private static void SetBasicType(FieldInfo field, string v)
        {
            field.SetValue(null, ParseValue(field.FieldType, v));
        }

        private static string[] SplitCollectionValues(ProgramOptionsAttribute options, string value)
        {
            return value.Split(new[] { options.CollectionSeparator ?? "," }, StringSplitOptions.None);
        }

        private static object ParseValue(Type type, string value)
        {
            if (type.IsEnum)
                return Enum.GetValues(type).Cast<object>().First(v => Enum.GetName(type, v) == value);

            var converted = Convert.ChangeType(value, type);

            if (converted == null)
                throw new NotSupportedException("Unsupported type " + type.FullName);

            return converted;
        }
    }

    class AssemblyNameComparer : IEqualityComparer<AssemblyName>
    {
        public bool Equals(AssemblyName x, AssemblyName y)
        {
            return x.FullName == y.FullName;
        }

        public int GetHashCode(AssemblyName obj)
        {
            return obj.FullName.GetHashCode();
        }
    }
}
