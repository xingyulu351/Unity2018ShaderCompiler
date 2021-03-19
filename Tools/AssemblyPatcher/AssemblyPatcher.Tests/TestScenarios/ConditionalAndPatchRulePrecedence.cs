//~|System.String B::Method2()| System.String A::Method1() -> System.String C::Method1()
//~A -> B

public class ConditionalAndPatchRulePrecedence
{
    public string Run()
    {
        // Expectation (:< unpatched , :> patched )

        //:< C::Method2()/C::Method2()
        //:> B::Method1()/C::Method1()
        return A.Method1() + "/" + A.Method2();
    }
}

class A
{
    public static string Method1()
    {
        return C.Method2();
    }

    public static string Method2()
    {
        return A.Method1();
    }
}

class B
{
    public static string Method1()
    {
        return "B::Method1()";
    }

    public static string Method2()
    {
        return A.Method1();
    }
}

class C
{
    public static string Method1()
    {
        return "C::Method1()";
    }

    public static string Method2()
    {
        return "C::Method2()";
    }
}
