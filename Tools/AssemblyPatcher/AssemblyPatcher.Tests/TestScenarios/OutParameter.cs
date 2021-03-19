//~ System.Void OutParameter::Method(System.String&) ->

public class OutParameter
{
    //:<  Test
    //:>  Hello
    public string Run()
    {
        string parameter;
        Method(out parameter);
        return parameter;
    }

    private void Method(out string a)
    {
        a = "Test";
    }
}
