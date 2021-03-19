using System;

[Obsolete("I am obsolete", false)]
public class ClassAObsolete
{
    [Obsolete("I am also obsolete", true)]
    public void MethodA() {}

    //not obsolete, but its parent is
    public void MethodB() {}

    public enum EnumA
    {
        ValueA,
        ValueB
    }
}

public class ClassANotObsolete
{
    [Obsolete("I am also obsolete", true)]
    public void MethodA() {}

    //not obsolete, but and neither is its parent
    public void MethodB() {}
}

[Obsolete("I am obsolete", false)]
public enum EnumAObsolete
{
    [Obsolete("I am also obsolete", true)]
    ValueA,

    //not obsolete, but its parent is
    ValueB
}

public enum EnumANotObsolete
{
    [Obsolete("I am obsolete", true)]
    ValueA,

    //not obsolete
    ValueB
}
