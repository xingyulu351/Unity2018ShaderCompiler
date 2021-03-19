class Program
{
    public static int Main()
    {
        var root = "../../";
        var path = "Runtime/Export/Assertions/Assert/";

        var generator = new AssertGenerator();
        generator.Generate(System.IO.Path.Combine(root, path));

        return 0;
    }
}
