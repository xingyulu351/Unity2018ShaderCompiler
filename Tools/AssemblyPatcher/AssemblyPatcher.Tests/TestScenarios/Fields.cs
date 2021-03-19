//~ A -> B
//~ C -> [AssemblyPatcher.Tests.ExternalAssembly] AssemblyPatcher.Tests.ExternalAssembly.TestClass

public class Fields
{
    //:<  A|static in A|A::Method()|42|C
    //:>  B|static in B|B::Method()|42|TestClass
    public string Run()
    {
        fldA = new A();
        return fldA.GetType().Name + "|" + A.Value + "|" + fldA.Method() + "|" + fldC.Value + "|" + fldC.GetType().Name;
    }

    public A fldA;
    public C fldC = new C(42);
}

public class A
{
    public static string Value = "static in A";

    public string Method()
    {
        return "A::Method()";
    }
}

public class B
{
    public static string Value = "static in B";

    public string Method()
    {
        return "B::Method()";
    }
}

public class C
{
    public C(int n)
    {
        this.n = n;
    }

    public int Value
    {
        get { return n; }
    }

    private readonly int n;
}
