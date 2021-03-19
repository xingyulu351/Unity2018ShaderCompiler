namespace UnderlyingModel
{
    //consider joining TypeKind and AssemblyType
    public enum TypeKind
    {
        Class,
        Struct,
        Enum
    }

    public enum AssemblyType
    {
        Class,
        Struct,
        Property,
        Field,
        Method,
        Constructor,
        Enum,
        EnumValue,
        Primitive
    }
}
