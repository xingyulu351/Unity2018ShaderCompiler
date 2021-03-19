//~ A ->
public class MapAllTypeMemberAccessToThrowingMethod
{
    //:< A::MethodToBeRemoved()
    //:> THROW, SO THIS SHOULD NOT MATTER
    public string Run()
    {
        A a = new A();
        return a.MethodToBeRemoved();
    }
}

public class A
{
    public string MethodToBeRemoved()
    {
        return "A::MethodToBeRemoved()";
    }
}
