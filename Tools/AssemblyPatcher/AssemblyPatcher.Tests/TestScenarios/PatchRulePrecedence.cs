// Rules:
//
//~ System.String A::Method1() -> System.String C::Method1()
//~ A -> B
//~ System.String A::Method3() -> System.String C::Method1()

using System.Diagnostics;

public class PatchRulePrecedence
{
    public string Run()
    {
        //:< A::Method1()/A::Method2()/A::Method3()
        //:> C::Method1()/B::Method2()/B::Method3()
        return A.Method1() + "/" +
            A.Method2() + "/" +
            A.Method3();
    }
}

class A
{
    public static string Method1()
    {
        return "A::Method1()";
    }

    public static string Method2()
    {
        return "A::Method2()";
    }

    public static string Method3()
    {
        return "A::Method3()";
    }
}

class B
{
    public static string Method2()
    {
        return "B::Method2()";
    }

    public static string Method3()
    {
        return "B::Method3()";
    }
}

class C
{
    public static string Method1()
    {
        return "C::Method1()";
    }
}
