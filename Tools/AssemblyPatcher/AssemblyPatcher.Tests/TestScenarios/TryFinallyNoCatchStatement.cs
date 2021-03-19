// No patch rules. Just ensure we don't throw while going over the methods
using System;

public class TryFinallyNoCatchStatement
{
    public string Run()
    {
        //:< within-try | within-finally
        //:> within-try | within-finally

        var ret = "";
        try
        {
            ret = ret + "within-try";
        }
        finally
        {
            ret = ret + " | within-finally";
        }

        return ret;
    }
}
