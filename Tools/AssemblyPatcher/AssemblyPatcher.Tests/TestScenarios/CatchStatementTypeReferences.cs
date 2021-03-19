//~ MyException -> System.Exception

using System;

public class CatchStatementTypeReferences
{
    public string Run()
    {
        //:< MyException
        //:> System.Exception
        try
        {
            throw new MyException("foo");
        }
        catch (MyException mex)
        {
            return mex.GetType().FullName;
        }

        return "UnExpected";
    }
}

public class MyException : System.Exception
{
    public MyException(string msg) : base(msg)
    {
    }
}
