using System;
using System.Collections.Generic;
using System.Linq;
using System.Text.RegularExpressions;

namespace UnityBindingsParser
{
    public class SignatureMatch
    {
        public static SignatureMatch FromAutoProp(string content)
        {
            return From(AutoPropRegex, content);
        }

        public static SignatureMatch FromAuto(string content)
        {
            return From(AutoRegex, content);
        }

        public static string NameFrom(string content)
        {
            return FromTypeHeader(content).Name;
        }

        public static SignatureMatch FromTypeHeader(string content)
        {
            return From(OptionalVisibilityOptionalModifiersNameAndOptionalBaseTypesRegex, content);
        }

        private static SignatureMatch From(Regex regex, string content)
        {
            return new SignatureMatch(MandatoryMatchOf(regex, content));
        }

        public static bool IsVisibilityIncludedIn(string signature)
        {
            return VisibilityRegex.IsMatch(signature);
        }

        private const string VisibilityPattern = @"((?<visibility>private|internal|protected|public)\s+)";

        private const string OptionalModifiersPattern = @"((?<typemodifiers>abstract|static|virtual)\s+)?";

        private const string OptionalVisiblityPattern = VisibilityPattern + "?";

        private const string StaticnessPattern = @"(?<staticness>static\s+)?";

        private const string NamePattern = @"(?<name>\w+)";

        private const string OptionalVisiblityOptionalModifiersNameAndOptionalBaseTypesPattern = OptionalVisiblityPattern + OptionalModifiersPattern + NamePattern + @"(\s*:\s*(?<basetypes>.+))?";

        private const string TypeFollowedByNamePattern = @"(?<type>\w+)\s+" + NamePattern;

        private const string DeclarationPattern = OptionalVisiblityPattern + StaticnessPattern + TypeFollowedByNamePattern;

        private static readonly Regex VisibilityRegex = new Regex(VisibilityPattern);

        private static readonly Regex AutoPropRegex = new Regex(DeclarationPattern + @"\s+(?<getter>\w+)(\s+(?<setter>\w+))?");

        private static readonly Regex AutoRegex = new Regex(DeclarationPattern + @"\s*(\((?<parameters>.*)\))\s*;");

        private static readonly Regex OptionalVisibilityOptionalModifiersNameAndOptionalBaseTypesRegex = new Regex(OptionalVisiblityOptionalModifiersNameAndOptionalBaseTypesPattern);

        private readonly Match _match;

        private SignatureMatch(Match match)
        {
            _match = match;
        }

        private static Match MandatoryMatchOf(Regex pattern, string content)
        {
            var match = pattern.Match(content);
            if (!match.Success)
                throw new InvalidOperationException(string.Format("'{0}' doesn't match '{1}'!", content, pattern));
            return match;
        }

        public string Visibility
        {
            get { return ValueOfGroup("visibility"); }
        }

        public string Staticness
        {
            get { return ValueOfGroup("staticness"); }
        }

        public string Type
        {
            get { return ValueOfGroup("type"); }
        }

        public string Name
        {
            get { return ValueOfGroup("name"); }
        }

        public bool HasSetter
        {
            get { return !ValueOfGroup("setter").IsEmpty(); }
        }

        private string ValueOfGroup(string groupName)
        {
            return Groups[groupName].Value.Trim();
        }

        private GroupCollection Groups
        {
            get { return _match.Groups; }
        }

        public bool IsStatic
        {
            get { return !Staticness.IsEmpty(); }
        }

        public bool HasNonVoidType
        {
            get { return Type != "void"; }
        }

        public IEnumerable<string> ParameterNames
        {
            get
            {
                if (Parameters.IsEmpty())
                    return new string[0];
                return Parameters.Split(',').Select(p => p.Trim().Split(' ')[1]);
            }
        }

        public string Parameters
        {
            get { return ValueOfGroup("parameters"); }
        }

        public string BaseTypes
        {
            get { return ValueOfGroup("basetypes"); }
        }

        public string Modifiers
        {
            get { return ValueOfGroup("typemodifiers"); }
        }

        public string Getter
        {
            get { return ValueOfGroup("getter"); }
        }

        public string Setter
        {
            get { return ValueOfGroup("setter"); }
        }

        public bool HasVisibility
        {
            get { return !Visibility.IsEmpty(); }
        }
    }
}
