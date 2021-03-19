using System;
using System.Collections.Generic;
using System.IO;
using System.Text.RegularExpressions;
using Mono.Cecil;

namespace UnderlyingModel
{
    public partial class MemberItem
    {
        private MemberItemNames m_CachedMemberItemNames;
        private bool m_NamesCalculated = false;
        private string m_CachedDisplayName = null;

        public struct MemberItemNames
        {
            public string NamespaceName;
            public string ClassName;
            public string MemberName;

            public string ClassAndMember
            {
                get { return MemberName.IsEmpty() ? ClassName : ClassName.IsEmpty() ? MemberName : String.Format("{0}.{1}", ClassName, MemberName); }
            }

            public string NamespaceAndClass
            {
                get { return NamespaceName.IsEmpty() ? ClassName : ClassName.IsEmpty() ? NamespaceName : String.Format("{0}.{1}", NamespaceName, ClassName); }
            }

            public string FullName
            {
                get { return MemberName.IsEmpty() ? NamespaceAndClass : String.Format("{0}.{1}", NamespaceAndClass, MemberName); }
            }
        }

        private readonly List<string> m_DllPaths = new List<string>();

        internal void AddDllPath(string value)
        {
            if (value != null && !m_DllPaths.Contains(value))
                m_DllPaths.Add(value);
        }

        public List<string> GetFullDllPaths()
        {
            return m_DllPaths;
        }

        public AsmEntry FirstSignatureAsm
        {
            get
            {
                return Signatures.Count == 0 ? null : Signatures[0].Asm;
            }
        }

        public TypeDefinition FirstCecilType
        {
            get
            {
                if (FirstSignatureAsm == null)
                    return null;

                //return the member type itself if it's a TypeDefinition, otherwise, return the parent type
                return FirstSignatureAsm.MemberCecilType as TypeDefinition ?? FirstSignatureAsm.ParentCecilType;
            }
        }

        public bool IsStatic
        {
            get { return FirstSignatureAsm != null && FirstSignatureAsm.IsStatic; }
        }

        public bool IsDocOnly
        {
            get { return DocModel != null && DocModel.IsDocOnly(); }
        }

        public bool IsUndoc
        {
            get { return DocModel != null && DocModel.IsUndoc(); }
        }

        public ObsoleteInfoType ObsoleteInfo
        {
            get { return FirstSignatureAsm == null ? new ObsoleteInfoType() : FirstSignatureAsm.ObsoleteInfo; }
        }

        public bool IsObsolete
        {
            get { return Signatures.Find(m => (m.InAsm && (m.Asm.ObsoleteInfo == null || m.Asm.ObsoleteInfo.IsObsolete == false)) || !m.InAsm) == null; }
        }

        public bool IsProtected
        {
            get
            {
                if (FirstSignatureAsm == null)
                    return false;
                return FirstSignatureAsm.IsProtected || FirstSignatureAsm.IsProtectedUnsealed;
            }
        }

        public bool IsUnityEditor
        {
            get { return ActualNamespace.StartsWith("UnityEditor"); }
        }

        //workaround for case 622141
        public bool IsEditorSpritesDataUtility
        {
            get { return ItemName == "DataUtility" && ActualNamespace == "UnityEditor.Sprites"; }
        }

        public string DisplayName
        {
            get
            {
                if (m_CachedDisplayName != null)
                    return m_CachedDisplayName;

                var names = GetNames();
                MakeNamesGeneric(ref names);
                if (DocModel != null && DocModel.IsDocOnly())
                    return DocModel.AssemblyKind == AssemblyType.Class
                        ? names.ClassName
                        : names.ClassAndMember;

                var sig = SignatureDisplayName();

                var needsClassName = AssemblyTypeUtils.CanHaveChildren(ItemType) || ItemType == AssemblyType.Assembly || ItemType == AssemblyType.Interface || ItemType == AssemblyType.Delegate;
                m_CachedDisplayName = needsClassName ? sig : String.Format("{0}.{1}", names.ClassName, sig);
                return m_CachedDisplayName;
            }
        }

        public void MakeNamesGeneric(ref MemberItemNames names)
        {
            names.ClassName = MakeGenericNameDisplayable(names.ClassName); //generic classes
            names.MemberName = MakeGenericNameDisplayable(names.MemberName); //generic members (functions, delegates)
        }

        public string MakeGenericNameDisplayable(string className)
        {
            if (className.IsEmpty())
                return className;
            if (FirstCecilType != null)
            {
                var genericParams = FirstCecilType.GenericParameters;
                if (genericParams.Count > 0)
                    className = GenericClassName(className, genericParams.Count);
            }
            return className;
        }

        public static string GenericClassName(string className, int genericParamsCount)
        {
            if (className.EndsWith(">"))
                return className; //already generic
            int index = className.LastIndexOf('_');
            if (index > 0)
                className = className.Substring(0, index);
            className += "<";
            for (int i = 0; i < genericParamsCount; i++)
            {
                className += String.Format("T{0}", i);
                if (i < genericParamsCount - 1)
                    className += ",";
            }
            className += ">";
            return className;
        }

        /// <summary>
        /// Display name for this signature
        /// </summary>
        /// <param name="entryName">expecting a name without the class</param>
        /// <param name="entryType">Constructor, Operator, Method, or whatever</param>
        /// <param name="curClass">the name of the class</param>
        /// <returns></returns>
        public static string SignatureDisplayName(string entryName, AssemblyType entryType, string curClass)
        {
            bool isGenericName = Regex.IsMatch(entryName, "[^0-9]_[0-9]$");
            int numParams = 0;
            if (isGenericName)
                numParams = int.Parse(entryName[entryName.Length - 1].ToString());
            switch (entryType)
            {
                case AssemblyType.Constructor:
                    return curClass;
                case AssemblyType.Operator:
                    return OperatorFromEntryName(entryName);
                case AssemblyType.Delegate:
                    if (isGenericName)
                    {
                        entryName = GenericClassName(entryName, numParams);
                        return entryName;
                    }

                    return entryName;
                default:
                    if (entryName.StartsWith("this"))
                        return entryName.Replace('(', '[').Replace(')', ']');

                    curClass = GenericClassName(curClass, numParams);
                    return isGenericName ? curClass : entryName;
            }
        }

        public string SignatureDisplayName()
        {
            var asm = FirstSignatureAsm;
            var names = GetNames();
            string displayName = asm != null
                ? asm.SignatureDeclarationName(names.ClassName)
                : (IsDocOnly || RealOrSuggestedItemType == AssemblyType.Unknown || RealOrSuggestedItemType == AssemblyType.Property)
                && RealOrSuggestedItemType != AssemblyType.Constructor
                ? names.MemberName
                : names.ClassName;
            return displayName;
        }

        //This converts from the mem file naming convention to the naming convention in the live docs
        public string HtmlName
        {
            get
            {
                var names = GetNames();
                var beforeNameSpace = HtmlNameBeforeNamespace(names);

                var regexSearch = new string(Path.GetInvalidFileNameChars()) + new string(Path.GetInvalidPathChars());
                var r = new Regex(string.Format("[{0}]", Regex.Escape(regexSearch)));
                beforeNameSpace = r.Replace(beforeNameSpace, "");

                if (names.NamespaceName.IsEmpty())
                    return beforeNameSpace;
                return String.Format("{0}.{1}", names.NamespaceName, beforeNameSpace);
            }
        }

        private string HtmlNameBeforeNamespace(MemberItemNames names)
        {
            if (!AssemblyTypeUtils.IsOperatorOrImpl(RealOrSuggestedItemType))
            {
                if (names.MemberName == "this")
                    return String.Format("{0}.Index_operator", names.ClassName);
                if (!names.MemberName.IsEmpty() && names.MemberName != "iOS" && names.MemberName != "tvOS" && Char.IsLower(names.MemberName[0])) //property
                    return String.Format("{0}-{1}", names.ClassName, names.MemberName);
                return names.ClassAndMember;
            }
            switch (RealOrSuggestedItemType)
            {
                case AssemblyType.Constructor:
                    return String.Format("{0}.{1}", names.ClassName, names.ClassName);
                case AssemblyType.Operator:
                    return HtmlNameForOp(names);
                case AssemblyType.ImplOperator:
                    return String.Format("{0}-operator_{1}", names.ClassName, FirstSignatureAsm.ParamList[0].Type);
                default:
                    return names.ClassName;
            }
        }

        private static string HtmlNameForOp(MemberItemNames names)
        {
            var strippedOpName = names.MemberName.Replace("op_", "");
            string htmlOpName;
            switch (strippedOpName)
            {
                case "Plus":
                    htmlOpName = "add";
                    break;
                case "Minus":
                    htmlOpName = "subtract";
                    break;
                case "Divide":
                    htmlOpName = "divide";
                    break;
                case "Multiply":
                    htmlOpName = "multiply";
                    break;
                case "Equal":
                    htmlOpName = "eq";
                    break;
                case "NotEqual":
                    htmlOpName = "ne";
                    break;
                case "BitwiseOr":
                    htmlOpName = "bitwiseor";
                    break;
                case "GreaterThan":
                    htmlOpName = "gt";
                    break;
                case "LessThan":
                    htmlOpName = "lt";
                    break;
                default:
                    htmlOpName = "??";
                    break;
            }
            return String.Format("{0}-operator_{1}", names.ClassName, htmlOpName);
        }

        public MemberItemNames GetNames()
        {
            if (m_NamesCalculated)
                return m_CachedMemberItemNames;

            string itemName = ItemName;
            string nameSpace = ActualNamespace;
            nameSpace = CecilHelpers.SimplifyNamespace(nameSpace);

            if (!nameSpace.IsEmpty())
            {
                if (itemName.StartsWith(nameSpace))
                    itemName = itemName.Substring(nameSpace.Length + 1);
            }
            var splitByDot = itemName.Split('.');
            var numParts = splitByDot.Length;
            string className = "";
            string memberName = "";
            if (numParts == 1)
            {
                if (ItemType == AssemblyType.Delegate)
                    memberName = CecilHelpers.Backticks2Underscores(itemName);
                else
                    className = CecilHelpers.Backticks2Underscores(itemName);
            }
            else
            {
                for (var i = 0; i < numParts - 1; i++)
                {
                    className += CecilHelpers.Backticks2Underscores(splitByDot[i]);
                    if (i < numParts - 2)
                        className += ".";
                }
                memberName = splitByDot[numParts - 1];
                if (memberName.StartsWith("_"))
                    memberName = memberName.Substring(1);
            }

            m_NamesCalculated = true;
            m_CachedMemberItemNames = new MemberItemNames
            {
                NamespaceName = nameSpace,
                ClassName = className,
                MemberName = memberName
            };
            return m_CachedMemberItemNames;
        }

        public MemberItemNames GetNamesForMonoDoc()
        {
            string itemName = ItemName;
            string nameSpace = ActualNamespace;

            if (!nameSpace.IsEmpty())
            {
                var simplifiedNsPrefix = CecilHelpers.SimplifyNamespace(nameSpace) + '.';
                if (itemName.StartsWith(simplifiedNsPrefix))
                    itemName = itemName.Substring(simplifiedNsPrefix.Length);
            }
            var splitByDot = itemName.Split('.');
            var numParts = splitByDot.Length;
            string className = "";
            string memberName = "";
            if (numParts == 1)
            {
                if (ItemType == AssemblyType.Delegate)
                    memberName = CecilHelpers.Backticks2Underscores(itemName);
                else
                    className = CecilHelpers.Backticks2Underscores(itemName);
            }
            else
            {
                for (var i = 0; i < numParts - 1; i++)
                {
                    className += CecilHelpers.Backticks2Underscores(splitByDot[i]);
                    if (i < numParts - 2)
                        className += ".";
                }
                memberName = splitByDot[numParts - 1];
                if (memberName.StartsWith("_"))
                    memberName = memberName.Substring(1);
            }

            return new MemberItemNames
            {
                NamespaceName = nameSpace,
                ClassName = className,
                MemberName = memberName
            };
        }

        //to distinguish from GetNames().NamespaceName, which is simplified, and excludes the UnityEngine / UnityEditor prefixes
        public string ActualNamespace
        {
            get
            {
                return CecilHelpers.NamespaceFromDescendants(FirstCecilType);
            }
        }

        //note that this is similar to GetNames, except we don't have the MemberItem available here
        //inferring names from the filename alone
        public static MemberItemNames GetNamesFromMemberID(string itemName, string nameSpace = "")
        {
            nameSpace = CecilHelpers.SimplifyNamespace(nameSpace);

            if (!nameSpace.IsEmpty())
            {
                if (itemName.StartsWith(nameSpace))
                    itemName = itemName.Substring(nameSpace.Length + 1);
            }
            var names = new MemberItemNames { NamespaceName = nameSpace };
            var splitByDot = itemName.Split('.');
            var numParts = splitByDot.Length;
            if (numParts == 1)
            {
                names.ClassName = CecilHelpers.Backticks2Underscores(itemName);
                names.MemberName = "";
            }
            else
            {
                //names.ClassName = string.Join(".", splitByDot.);
                for (var i = 0; i < numParts - 1; i++)
                {
                    names.ClassName += CecilHelpers.Backticks2Underscores(splitByDot[i]);
                    if (i < numParts - 2)
                        names.ClassName += ".";
                }
                names.MemberName = splitByDot[numParts - 1];
                if (names.MemberName.StartsWith("_"))
                    names.MemberName = names.MemberName.Substring(1);
            }
            return names;
        }

        private static string OperatorFromEntryName(string entryName)
        {
            switch (entryName)
            {
                case "Plus":
                    return "operator +";
                case "Minus":
                    return "operator -";
                case "Multiply":
                    return "operator *";
                case "Divide":
                    return "operator /";
                case "NotEqual":
                    return "operator !=";
                case "Equal":
                    return "operator ==";
                case "BitwiseOr":
                    return "operator |=";
                case "GreaterThan":
                    return "operator >";
                case "LessThan":
                    return "operator <";
                default:
                    return "Unknown operator";
            }
        }
    }
}
