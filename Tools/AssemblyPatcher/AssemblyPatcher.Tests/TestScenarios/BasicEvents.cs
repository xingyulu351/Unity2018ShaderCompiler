//~ A -> B
//~ System.Void B::Method1() ->
//~ System.MissingMethodException -> System.MissingMemberException

// Basically this test is just meant apply patches and trigger the respective events.
public class BasicEvents
{
    //:<  static in A|A::Method()|A
    //:>  static in B|B::Method()|B
    public string Run()
    {
        try
        {
            if (avoidOptimization)
            {
                B.Method1();
            }

            A a = fldA; // local variable / field reference
            return A.Value + "|" // field reference
                + a.Method() + "|"    // method reference
                + ReturnA(a)    // Method return type / parameter type
            ;
        }
        catch (System.MissingMethodException me)
        {
        }

        return string.Empty;
    }

    public A ReturnA(A a)
    {
        return a;
    }

    public A fldA = new A();
    public bool avoidOptimization = false;
}

public class A
{
    public static string Value = "static in A";

    public int field = 42;

    public string Method() { return "A::Method()"; }

    public override string ToString() { return "A"; }
}

public class DerivedFromA : A
{
}

public class B
{
    public int field = 0x414356;

    public static string Value = "static in B";

    public string Method() { return "B::Method()"; }

    public static void Method1() {}

    public override string ToString() { return "B"; }
}
