using System;

namespace UnderlyingModel
{
    //for identifying things coming from the docs
    public enum TypeKind
    {
        Class,
        Struct,
        Enum,
        AutoProp,
        PureSignature
    }

    //for identifying things coming from the assembly

    [Flags]
    public enum ItemExistenceCode
    {
        AsmItemExists = 0,
        AsmItemHasAsm = 1,
        AsmItemHasNoDoc = 2,
        DocItemExists = 4,
        DocItemHasDoc = 8,
        DocItemHasNoAsm = 16
    }

    [Flags]
    public enum DataItemPresence
    {
        Neither,
        Assembly,
        Doc,
        Combined
    }

    public static class TxtToken
    {
        public const string BeginEx = "BEGIN EX";
        public const string EndEx = "END EX";
        public const string ConvertExample = "CONVERTEXAMPLE";
        public const string UnityScriptDelimiter = "---UnityScript---";
        public const string CSharpDelimiter = "---C#---";
        public const string NoCheck = "NOCHECK";
        public const string Param = "@param";
        public const string Return = "@return";
        public const string DocComment = "///";
        public const string CsNone = "CSNONE";
        public const string UndocNoSlashes = "*undocumented*";
        public const string ListOnlyNoSlashes = "*listonly*";
    }

    public static class MemToken
    {
        public const string SignatureOpen = "<signature>";
        public const string SignatureClose = "</signature>";
        public const string TxtTagOpen = "<txttag>";
        public const string TxtTagClose = "</txttag>";
        public const string SuggestedOpen = "<suggestedtype>";
        public const string SuggestedClose = "</suggestedtype>";

        public const string Undoc = "UNDOC";
    }

    public static class DirectoryToken
    {
        public const string ApiDocs = "ApiDocs";
        public const string Documentation = "Documentation";
        public const string PlatformDependent = "PlatformDependent";
    }
}
