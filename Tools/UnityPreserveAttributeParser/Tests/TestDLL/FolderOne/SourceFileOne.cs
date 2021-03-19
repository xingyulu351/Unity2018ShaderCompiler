using System;

namespace UnityEngineTestDLL.FolderOne
{
    public class PublicClass
    {
    }

    class TestClass
    {
        void InstanceMethodNoArgs() {}

        void InstanceMethodArgs(int arg1, float arg2, string arg3, TestClass arg4) {}

        void InstanceMethodProxyStructArg(Proxies.TestStruct arg) {}

        int InstanceMethodReturnsInt() { return 0; }

        string[] InstanceMethodReturnsArray() { return null; }

        static void StaticMethodNoArgs() {}

        static void StaticMethodStructArg(RequiredStructOne arg) {}

        static void StaticMethodOutArg(out int arg) { arg = 0; }
    }

    [RequiredByNativeCode]
    class RequiredClassOne
    {
        [RequiredByNativeCode]
        void RequiredMethod()
        {
        }

        [UsedByNativeCode]
        void UsedMethod()
        {
        }

        void UnusedMethod()
        {
        }
    }

    [UsedByNativeCode]
    class UsedClassOne
    {
        [RequiredByNativeCode]
        void RequiredMethod()
        {
        }

        [UsedByNativeCode]
        void UsedMethod()
        {
        }

        void UnusedMethod()
        {
        }
    }

    class UnusedClassOne
    {
        [RequiredByNativeCode]
        void RequiredMethod()
        {
        }

        [UsedByNativeCode]
        void UsedMethod()
        {
        }

        void UnusedMethod()
        {
        }
    }

    class UnusedClassWithNoUsedMethodsOne
    {
        void UnusedMethod()
        {
        }
    }

    [RequiredByNativeCode]
    class RequiredClassWithNoMethodsOne
    {
    }

    [UsedByNativeCode]
    class UsedClassWithNoMethodsOne
    {
    }

    class UnusedClassWithNoMethodsOne
    {
    }

    [RequiredByNativeCode]
    struct RequiredStructOne
    {
        [RequiredByNativeCode]
        void RequiredMethod()
        {
        }

        [UsedByNativeCode]
        void UsedMethod()
        {
        }

        void UnusedMethod()
        {
        }
    }

    [UsedByNativeCode]
    struct UsedStructOne
    {
        [RequiredByNativeCode]
        void RequiredMethod()
        {
        }

        [UsedByNativeCode]
        void UsedMethod()
        {
        }

        void UnusedMethod()
        {
        }
    }

    struct UnusedStructOne
    {
        [RequiredByNativeCode]
        void RequiredMethod()
        {
        }

        [UsedByNativeCode]
        void UsedMethod()
        {
        }

        void UnusedMethod()
        {
        }
    }

    struct UnusedStructWithNoUsedMethodsOne
    {
        void UnusedMethod()
        {
        }
    }

    [RequiredByNativeCode]
    struct RequiredStructWithNoMethodsOne
    {
    }

    [UsedByNativeCode]
    struct UsedStructWithNoMethodsOne
    {
    }

    struct UnusedStructWithNoMethodsOne
    {
    }

    [RequiredByNativeCode]
    interface RequiredInterfaceOne
    {
        [RequiredByNativeCode]
        void RequiredMethod();

        [UsedByNativeCode]
        void UsedMethod();

        void UnusedMethod();
    }

    [UsedByNativeCode]
    interface UsedInterfaceOne
    {
        [RequiredByNativeCode]
        void RequiredMethod();

        [UsedByNativeCode]
        void UsedMethod();

        void UnusedMethod();
    }

    interface UnusedInterfaceOne
    {
        [RequiredByNativeCode]
        void RequiredMethod();

        [UsedByNativeCode]
        void UsedMethod();

        void UnusedMethod();
    }

    [RequiredByNativeCode]
    enum RequiredEnumOne
    {
    }

    [UsedByNativeCode]
    enum UsedEnumOne
    {
    }

    enum UnusedEnumOne
    {
    }

    [RequiredByNativeCode]
    class BaseRequiredClassOne
    {
    }

    class SuperRequiredClassOne : BaseRequiredClassOne
    {
    }

    [UsedByNativeCode("customName")]
    public class WithCustomNameOne
    {
    }

    [UsedByNativeCode(Name = "customName")]
    public class WithCustomNameAsFieldOne
    {
    }

    [RequiredByNativeCode("customNameRequired")]
    public class WithCustomNameRequiredOne
    {
    }

    [RequiredByNativeCode(Name = "customNameRequired")]
    public class WithCustomNameRequiredAsFieldOne
    {
    }

    public class WithMethodWithCustomNamesOne
    {
        [RequiredByNativeCode("customNameRequired")]
        void RequiredMethod()
        {
        }

        [UsedByNativeCode("customName")]
        void UsedMethod()
        {
        }

        void UnusedMethod()
        {
        }
    }

    public class WithMethodWithCustomNamesAsFieldOne
    {
        [RequiredByNativeCode(Name = "customNameRequired")]
        void RequiredMethod()
        {
        }

        [UsedByNativeCode(Name = "customName")]
        void UsedMethod()
        {
        }

        void UnusedMethod()
        {
        }
    }

    public class ParentClassOne
    {
        [RequiredByNativeCode]
        public class RequiredNestedOne
        {
        }

        [RequiredByNativeCode("customName")]
        public class RequiredNested2One
        {
        }
    }

    [RequiredByNativeCode(true)]
    public class RequiredClassMarkedOptional1One
    {
    }

    [RequiredByNativeCode(Optional = true)]
    public class RequiredClassMarkedOptional2One
    {
    }

    [RequiredByNativeCode("customName", true)]
    public class RequiredClassMarkedOptional3One
    {
    }

    [RequiredByNativeCode("customName", Optional = true)]
    public class RequiredClassMarkedOptional4One
    {
    }

    [RequiredByNativeCode(Name = "customName", Optional = true)]
    public class RequiredClassMarkedOptional5One
    {
    }

    public class TheParentClassOne
    {
        public class RequiredNestedOne
        {
            [RequiredByNativeCode]
            void RequiredMethod1()
            {
            }

            [RequiredByNativeCode(false)]
            void RequiredMethod2()
            {
            }

            [RequiredByNativeCode(Optional = false)]
            void RequiredMethod3()
            {
            }

            [RequiredByNativeCode("customName", false)]
            void RequiredMethod4()
            {
            }

            [RequiredByNativeCode("customName", Optional = false)]
            void RequiredMethod5()
            {
            }

            [RequiredByNativeCode(Name = "customName", Optional = false)]
            void RequiredMethod6()
            {
            }

            [RequiredByNativeCode(true)]
            void UsedMethod1()
            {
            }

            [RequiredByNativeCode(Optional = true)]
            void UsedMethod2()
            {
            }

            [RequiredByNativeCode("customName", true)]
            void UsedMethod3()
            {
            }

            [RequiredByNativeCode("customName", Optional = true)]
            void UsedMethod4()
            {
            }

            [RequiredByNativeCode(Name = "customName", Optional = true)]
            void UsedMethod5()
            {
            }

            void UnusedMethod()
            {
            }
        }
    }
}
