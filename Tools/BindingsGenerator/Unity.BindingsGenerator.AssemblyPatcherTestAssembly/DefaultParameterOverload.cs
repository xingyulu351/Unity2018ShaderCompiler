using System;
using System.Runtime.CompilerServices;
using UnityEngine.Bindings;

namespace Unity.BindingsGenerator.AssemblyPatcherTestAssembly
{
    class DefaultParameterOverloadSimple
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void OneParameterWithDefault(int a = 19912);
    }

    class DefaultParameterOverloadSimpleExpected
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void OneParameterWithDefault(int a);

        public void OneParameterWithDefault()
        {
            OneParameterWithDefault(19912);
        }
    }

    class DefaultParameterOverloadTriple
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void OneParameterWithDefault(int a = 1, int b = 2, int c = 3);
    }

    class DefaultParameterOverloadTripleExpected
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void OneParameterWithDefault(int a, int b, int c);

        public void OneParameterWithDefault(int a, int b)
        {
            OneParameterWithDefault(a, b, 3);
        }

        public void OneParameterWithDefault(int a)
        {
            OneParameterWithDefault(a, 2, 3);
        }

        public void OneParameterWithDefault()
        {
            OneParameterWithDefault(1, 2, 3);
        }
    }

    class DefaultParameterOverloadMixed
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void OneParameterWithDefault(int a, int b, int c = 3, int d = 4);
    }

    class DefaultParameterOverloadMixedExpected
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void OneParameterWithDefault(int a, int b, int c, int d);

        public void OneParameterWithDefault(int a, int b, int c)
        {
            OneParameterWithDefault(a, b, c, 4);
        }

        public void OneParameterWithDefault(int a, int b)
        {
            OneParameterWithDefault(a, b, 3, 4);
        }
    }

    class SomeAttributeAttribute
    {
    }

    class DefaultParameterOverloadWithAttribute
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        [SomeAttribute]
        public extern void MethodWithDefaults([SomeAttribute] int a = 19912, [SomeAttribute] int b = 22);
    }

    class DefaultParameterOverloadWithAttributeExpected
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        [SomeAttribute]
        public extern void MethodWithDefaults([SomeAttribute] int a, [SomeAttribute] int b);

        [SomeAttribute]
        public void MethodWithDefaults([SomeAttribute] int a)
        {
            MethodWithDefaults(a, 22);
        }

        [SomeAttribute]

        public void MethodWithDefaults()
        {
            MethodWithDefaults(19912, 22);
        }
    }

    class DefaultParameterOverloadByte
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void OneParameterWithDefault(byte a = 19);
    }

    class DefaultParameterOverloadByteExpected
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void OneParameterWithDefault(byte a);

        public void OneParameterWithDefault()
        {
            OneParameterWithDefault(19);
        }
    }

    class DefaultParameterOverloadSByte
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void OneParameterWithDefault(sbyte a = 19);
    }

    class DefaultParameterOverloadSByteExpected
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void OneParameterWithDefault(sbyte a);

        public void OneParameterWithDefault()
        {
            OneParameterWithDefault(19);
        }
    }

    class DefaultParameterOverloadChar
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void OneParameterWithDefault(char a = 'q');
    }

    class DefaultParameterOverloadCharExpected
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void OneParameterWithDefault(char a);

        public void OneParameterWithDefault()
        {
            OneParameterWithDefault('q');
        }
    }

    class DefaultParameterOverloadBoolean
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void OneParameterWithDefault(bool a = true);
    }

    class DefaultParameterOverloadBooleanExpected
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void OneParameterWithDefault(bool a);

        public void OneParameterWithDefault()
        {
            OneParameterWithDefault(true);
        }
    }

    class DefaultParameterOverloadInt32
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void OneParameterWithDefault(int a = 27);
    }

    class DefaultParameterOverloadInt32Expected
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void OneParameterWithDefault(int a);

        public void OneParameterWithDefault()
        {
            OneParameterWithDefault(27);
        }
    }

    class DefaultParameterOverloadUInt32
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void OneParameterWithDefault(uint a = 27);
    }

    class DefaultParameterOverloadUInt32Expected
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void OneParameterWithDefault(uint a);

        public void OneParameterWithDefault()
        {
            OneParameterWithDefault(27);
        }
    }

    class DefaultParameterOverloadInt64
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void OneParameterWithDefault(long a = 27);
    }

    class DefaultParameterOverloadInt64Expected
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void OneParameterWithDefault(long a);

        public void OneParameterWithDefault()
        {
            OneParameterWithDefault(27);
        }
    }

    class DefaultParameterOverloadUInt64
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void OneParameterWithDefault(ulong a = 27);
    }

    class DefaultParameterOverloadUInt64Expected
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void OneParameterWithDefault(ulong a);

        public void OneParameterWithDefault()
        {
            OneParameterWithDefault(27);
        }
    }

    class DefaultParameterOverloadSingle
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void OneParameterWithDefault(float a = 27);
    }

    class DefaultParameterOverloadSingleExpected
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void OneParameterWithDefault(float a);

        public void OneParameterWithDefault()
        {
            OneParameterWithDefault(27);
        }
    }

    class DefaultParameterOverloadDouble
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void OneParameterWithDefault(double a = 27);
    }

    class DefaultParameterOverloadDoubleExpected
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void OneParameterWithDefault(double a);

        public void OneParameterWithDefault()
        {
            OneParameterWithDefault(27);
        }
    }

    class DefaultParameterOverloadString
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void OneParameterWithDefault(string a = "hello");
    }

    class DefaultParameterOverloadStringExpected
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void OneParameterWithDefault(string a);

        public void OneParameterWithDefault()
        {
            OneParameterWithDefault("hello");
        }
    }

    class DefaultParameterOverloadClass
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void OneParameterWithDefault(object a = null);
    }

    class DefaultParameterOverloadClassExpected
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void OneParameterWithDefault(object a);

        public void OneParameterWithDefault()
        {
            OneParameterWithDefault(null);
        }
    }

    class DefaultParameterOverloadEnum
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void OneParameterWithDefault(ConstantLoading.SomeEnum a = ConstantLoading.SomeEnum.A);
    }

    class DefaultParameterOverloadEnumExpected
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void OneParameterWithDefault(ConstantLoading.SomeEnum a);

        public void OneParameterWithDefault()
        {
            OneParameterWithDefault(ConstantLoading.SomeEnum.A);
        }
    }
}
