//~ System.String NonVoidMethodBodyRemovalThrows::Method() -> {}
public class NonVoidMethodBodyRemovalThrows
{
    //:<  Hello World.
    //:>
    public string Run()
    {
        string x = Method();
        return x;
    }

    private string Method()
    {
        return "Hello World.";
    }
}
