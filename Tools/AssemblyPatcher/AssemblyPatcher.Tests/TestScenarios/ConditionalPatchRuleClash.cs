/**
 * Ensures that
 *  - R1/R2 will be discarded when processing A.Method1() call insinde Run() (they are applicable only inside A.Method3()/A.Method2() respectively)
 *    and R3 will be picked up instead.
 *
 *  - R3 will be picked up in Run() method
 *
 */
//~ |System.String A::Method3()| System.String A::Method1() -> System.String C::Method1() # R1
//~ |System.String A::Method2()| System.String A::Method1() -> System.String B::Method2() # R2
//~                              System.String A::Method1() -> System.String B::Method1() # R3

public class ConditionalPatchRuleClash
{
    public string Run()
    {
        // Expectation (:< unpatched , :> patched )

        //:< C::Method2()/C::Method2()/C::Method2()
        //:> B::Method1()/B::Method2()/C::Method1()
        //   R3          /R2          /R1
        return A.Method1() + "/" + A.Method2() + "/" + A.Method3();
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

    public static string Method3()
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
        return "B::Method2()";
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
