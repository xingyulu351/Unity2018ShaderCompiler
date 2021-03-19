namespace Unity.BindingsGenerator.TestAssembly
{
    #pragma warning disable 626
    public class ClassWithoutExterns
    {
        public static void NormalFunction()
        {
        }

        public static bool NormalProperty { get; set; }
    }

    public class ClassWithExternsInCsFile
    {
        public static extern void ExternNoAttributes();
    }
}
