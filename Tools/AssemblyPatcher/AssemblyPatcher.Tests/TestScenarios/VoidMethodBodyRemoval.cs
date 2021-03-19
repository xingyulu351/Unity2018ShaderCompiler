//~ System.Void VoidMethodBodyRemoval::Method() -> {}

public class VoidMethodBodyRemoval
{
    //:<  Hello World.
    //:>  Hello
    public string Run()
    {
        Method();
        return s;
    }

    private void Method()
    {
        s = s + " " + "World.";
    }

    private string s = "Hello";
}
