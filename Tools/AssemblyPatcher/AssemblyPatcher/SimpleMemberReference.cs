using System.Text.RegularExpressions;

namespace AssemblyPatcher
{
    class SimpleMemberReference
    {
        protected SimpleMemberReference(string declaringTypeFQN, string memberType, string meberName)
        {
            ParseTypeFQN(declaringTypeFQN);
            this.meberName = meberName;
            MemberType = memberType;
        }

        /// <summary>
        /// Realy simple MemberReference parser.
        ///
        /// Suports only simple member references in the form:
        ///
        /// System.Boolean UnityScript.PlatformAdapter.TypeExtensions::EnumToBoolean(Enum)
        /// </summary>
        public static SimpleMemberReference Parse(string memberReferenceFullName)
        {
            var match = Regex.Match(memberReferenceFullName, @"(?<AssemblyName>\[[^\]]+\])?\s*(?<ReturnTypeFQN>.*)\s(?<TypeFQN>.*)::(?<MemberName>[^\(\<]*)[^\(]*(?<Parameters>\(.*\))*");

            if (!match.Success)
            {
                throw new PatchingException("Invalid member fully qualified name: " + memberReferenceFullName);
            }

            var isMethodRef = match.Groups["Parameters"].Success;

            if (isMethodRef && string.IsNullOrWhiteSpace(match.Groups["ReturnTypeFQN"].Value))
            {
                throw new PatchingException("Missing return type in member fully qualified name: " + memberReferenceFullName);
            }

            var ret = new SimpleMemberReference(
                match.Groups["TypeFQN"].Value,
                match.Groups["ReturnTypeFQN"].Value,
                match.Groups["MemberName"].Value);

            if (match.Groups["AssemblyName"].Success)
                ret.AssemblyName = match.Groups["AssemblyName"].Value;

            return ret;
        }

        public string FullName
        {
            get
            {
                return (!string.IsNullOrWhiteSpace(typeNamespace) ? typeNamespace + "." : "") + typeName;
            }
        }

        public string AssemblyName
        {
            get;
            private set;
        }

        public string MemberType { get; private set; }

        public string Name
        {
            get { return meberName; }
        }

        private void ParseTypeFQN(string typeFQN)
        {
            var namespaceLimit = typeFQN.LastIndexOf('.');

            typeNamespace = typeFQN.Substring(0, (namespaceLimit > 0 ? namespaceLimit : 0));
            typeName = typeFQN.Substring(namespaceLimit + 1);
        }

        private string meberName;
        private string typeNamespace;
        private string typeName;
    }
}
