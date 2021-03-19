using System.Runtime.CompilerServices;
using UnityEngine.Bindings;

namespace Unity.BindingsGenerator.AssemblyPatcherTestAssembly
{
    struct SomeStruct
    {
    }

    struct SomeNonPodStruct
    {
        #pragma warning disable 169
        string i;
    }

    class ClassWithInstanceStructUser
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void InstanceStructUser(SomeStruct a);
    }

    class ClassWithInstanceStructUserExpected
    {
        public void InstanceStructUser(SomeStruct a)
        {
            InstanceStructUser_Injected(ref a);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void InstanceStructUser_Injected(ref SomeStruct a);
    }

    class ClassWithStaticStructUser
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void StaticStructUser(SomeStruct a);
    }

    class ClassWithStaticStructUserExpected
    {
        public static void StaticStructUser(SomeStruct a)
        {
            StaticStructUser_Injected(ref a);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void StaticStructUser_Injected(ref SomeStruct a);
    }

    internal class ClassWithStaticMultipleStructUser
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void StaticMultipleStructUser(SomeStruct a, SomeStruct b, ref SomeStruct c, int d, ref int e);
    }

    class ClassWithStaticMultipleStructUserExpected
    {
        public static void StaticMultipleStructUser(SomeStruct a, SomeStruct b, ref SomeStruct c, int d, ref int e)
        {
            StaticMultipleStructUser_Injected(ref a, ref b, ref c, d, ref e);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void StaticMultipleStructUser_Injected(ref SomeStruct a, ref SomeStruct b, ref SomeStruct c, int d, ref int e);
    }

    internal class ClassWithNoByValueStructUser
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void StaticStructUserOnlyRefs(ref SomeStruct a, ref SomeStruct b, int c);
    }

    class ClassWithNoByValueStructUserExpected
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void StaticStructUserOnlyRefs(ref SomeStruct a, ref SomeStruct b, int c);
    }

    internal class ClassWithInstanceReturnStructUser
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern SomeStruct InstanceReturnStructUser();
    }

    class ClassWithInstanceReturnStructUserExpected
    {
        public SomeStruct InstanceReturnStructUser()
        {
            SomeStruct a;
            InstanceReturnStructUser_Injected(out a);
            return a;
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void InstanceReturnStructUser_Injected(out SomeStruct ret);
    }

    internal class ClassWithStaticReturnStructUser
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern SomeStruct StaticReturnStructUser();
    }

    class ClassWithStaticReturnStructUserExpected
    {
        public static SomeStruct StaticReturnStructUser()
        {
            SomeStruct a;
            StaticReturnStructUser_Injected(out a);
            return a;
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void StaticReturnStructUser_Injected(out SomeStruct ret);
    }

    internal class ClassWithInstancePropertyStructUser
    {
        public extern SomeStruct InstancePropertyStructUser
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            get;

            [MethodImpl(MethodImplOptions.InternalCall)]
            set;
        }
    }

    class ClassWithInstancePropertyStructUserExpected
    {
        public SomeStruct InstancePropertyStructUser
        {
            get
            {
                SomeStruct a;
                get_InstancePropertyStructUser_Injected(out a);
                return a;
            }

            set
            {
                set_InstancePropertyStructUser_Injected(ref value);
            }
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void get_InstancePropertyStructUser_Injected(out SomeStruct ret);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void set_InstancePropertyStructUser_Injected(ref SomeStruct value);
    }

    class ClassWithMixedStructUser
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern SomeStruct MixedStructUser(int a, out SomeStruct doNotTouchMe, SomeStruct touchMe);
    }

    class ClassWithMixedStructUserExpected
    {
        public SomeStruct MixedStructUser(int a, out SomeStruct doNotTouchMe, SomeStruct touchMe)
        {
            SomeStruct ret;
            MixedStructUser_Injected(a, out doNotTouchMe, ref touchMe, out ret);
            return ret;
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void MixedStructUser_Injected(int a, out SomeStruct doNotTouchMe, ref SomeStruct touchMe, out SomeStruct ret);
    }

    class ClassWithInstanceStructUserAndAttributes
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        [SomeAttribute]
        public extern void InstanceStructUser([SomeAttribute] SomeStruct a);
    }

    class ClassWithInstanceStructUserAndAttributesExpected
    {
        [SomeAttribute]
        public void InstanceStructUser([SomeAttribute] SomeStruct a)
        {
            InstanceStructUser_Injected(ref a);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void InstanceStructUser_Injected([SomeAttribute] ref SomeStruct a);
    }

    class ClassWithInstanceStructUserAndParameterWithDefaultValue
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void InstanceStructUser(SomeStruct a, object b = null);
    }

    class ClassWithInstanceStructUserAndParameterWithDefaultValueExpected
    {
        public void InstanceStructUser(SomeStruct a, object b = null)
        {
            InstanceStructUser_Injected(ref a, b);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void InstanceStructUser_Injected(ref SomeStruct a, object b = null);
    }

    class ClassWithStructProperty
    {
        public extern SomeStruct podStructProperty
        {
            [MethodImpl(MethodImplOptions.InternalCall)] get;
            [MethodImpl(MethodImplOptions.InternalCall)] set;
        }
    }

    class ClassWithStructPropertyExpected
    {
        public SomeStruct podStructProperty
        {
            get
            {
                SomeStruct value;
                get_podStructProperty_Injected(out value);
                return value;
            }
            set
            {
                set_podStructProperty_Injected(ref value);
            }
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void get_podStructProperty_Injected(out SomeStruct ret);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void set_podStructProperty_Injected(ref SomeStruct value);
    }

    class ClassWithAttributedMethod
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        [SomeAttribute]
        public extern SomeNonPodStruct AttributedInstanceStructReturner();
    }

    class ClassWithAttributedMethodExpected
    {
        [SomeAttribute]
        public SomeNonPodStruct AttributedInstanceStructReturner()
        {
            SomeNonPodStruct ret;
            AttributedInstanceStructReturner_Injected(out ret);
            return ret;
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void AttributedInstanceStructReturner_Injected(out SomeNonPodStruct ret);
    }
}
