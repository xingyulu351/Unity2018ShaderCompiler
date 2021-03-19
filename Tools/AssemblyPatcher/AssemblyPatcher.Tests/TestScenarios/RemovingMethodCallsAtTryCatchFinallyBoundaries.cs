//~ System.Void A::Bar() ->
using System;
using System.Collections;

public class RemovingMethodCallsAtTryCatchFinallyBoundaries
{
    public string Run()
    {
        //:< try finally 1
        //:>
        try
        {
            ret = "try";
        }
        finally
        {
            ret += " finally ";
        }

        A.Bar(); // Removing this method was causing one 'push' instruction to be injected in the finally block.

        return ret + A.Value;
    }

    private string ret = String.Empty;
}

public class A
{
    public static void Bar()
    {
        ret++;
    }

    public static IEnumerable Items { get { return new object[] {}; } }

    public static string Value { get { return "" + ret; } }

    private static int ret = 0;
}
