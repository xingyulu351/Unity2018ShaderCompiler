//~ System.Boolean System.Type::op_Equality(System.Type,System.Type) -> System.Boolean System.Object::ReferenceEquals(System.Object,System.Object)
//~ System.Boolean Class::IsEqual(Class,Class) -> System.Boolean Class::NotEqual(Class,Class)

public class TargetReplacementMethodInSameAssemblyAsSource
{
    //:<  True|True
    //:>  True|False
    public string Run()
    {
        return new Class().Run();
    }
}

public class Class
{
    public string Run()
    {
        return (typeof(Class) == typeof(Class)) + "|" + Class.IsEqual(this, this);
    }

    public static bool IsEqual(Class lhs, Class rhs)
    {
        return lhs == rhs;
    }

    public static bool NotEqual(Class lhs, Class rhs)
    {
        return !object.ReferenceEquals(lhs, rhs);
    }
}
