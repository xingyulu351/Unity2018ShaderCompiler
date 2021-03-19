//~ System.String B::CallMethodOnA(A) ->
public class MapTypeMemberAccessToThrowingMethod
{
    //:< A::MethodToBeRemoved()
    //:> THROW, SO THIS SHOULD NOT MATTER
    public string Run()
    {
        A a = new A();
        return SomeMethodExpectingTheTypeAsAParameter(a);
    }

    private string SomeMethodExpectingTheTypeAsAParameter(A a)
    {
        return B.CallMethodOnA(a);
    }
}

public class A
{
    public string MethodToBeRemoved()
    {
        return "A::MethodToBeRemoved()";
    }
}

public class B
{
    public static string CallMethodOnA(A a)
    {
        return a.MethodToBeRemoved();
    }
}
