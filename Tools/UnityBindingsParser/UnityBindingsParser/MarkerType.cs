namespace UnityBindingsParser
{
    public enum MarkerType
    {
        Unknown,
        SimpleComment,
        DocMarker,
        StartPoundIf,
        EndPoundIf,
        StructNoLayout,
        Docfile,
        ConvertExampleMultiline,
        BeginExMultiline,
        EnddocMultiline,
        EnumDecl,
        EnumMemberWithAssignment,
        EnumCancel,
        EnumMemberNoAssignment,
        CsRaw,
        CppRaw,
        Conditional,
        Attribute,
        Class,
        Struct
    }
}
