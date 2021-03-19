using System;
namespace TestDLL
{
    public class TestType
    {
    }

    public class GenericType1<T>
    {
    }

    public class GenericType3<T1, T2, T3>
    {
    }

    class PrivateType
    {
    }

    [Obsolete("This is a warning", false)]
    public class ObsoleteWarningType
    {
    }

    [Obsolete("This is an error", true)]
    public class ObsoleteErrorType
    {
    }
}
