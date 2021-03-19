namespace AssemblyPatcher.Tests.ExternalAssembly
{
    public class TestClass
    {
        public TestClass(int n)
        {
            this.n = n;
        }

        public int Value
        {
            get { return n; }
        }

        private readonly int n;
    }
}
