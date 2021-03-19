using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace Unity.BindingsGenerator.TestAssembly
{
    public class ClassWithExterns
    {
        #pragma warning disable 626

        public static void NormalFunction()
        {
        }

        public static bool NormalProperty { get; set; }

        public static extern void ExternNoAttributes();

        [DllImport("user32.dll")]
        public static extern void ExternWithDllImport();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void ExternWithInternalCall();
    }

    public class ParentOfNested
    {
        private class Nested
        {
            public static extern void ExternMethod();
        }
    }

    public class ClassInBindingsWithoutExterns
    {
        public static void NormalFunction()
        {
        }
    }
}
