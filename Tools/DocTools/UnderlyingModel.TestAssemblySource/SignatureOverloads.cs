using System;
using UnityEngine.Internal;

namespace UnityEngine.Internal
{
    [Serializable]
    public class ExcludeFromDocsAttribute : Attribute
    {
    }
}

public class MethodWithDefaultValue
{
    [ExcludeFromDocs]
    public static void Foo(string text)
    {
    }

    public static void Foo(string text, [DefaultValue("true")] bool flag)
    {
    }
}
