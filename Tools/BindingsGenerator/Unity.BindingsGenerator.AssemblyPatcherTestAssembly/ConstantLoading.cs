using System;
using System.Security.AccessControl;

namespace Unity.BindingsGenerator.AssemblyPatcherTestAssembly
{
    public class ConstantLoading
    {
        public void ConstLoadBool()
        {
            GenerateConstLoadInstructionsBool();
        }

        private static void GenerateConstLoadInstructionsBool(bool a = true, bool b = false)
        {
        }

        public void ConstLoadByte()
        {
            GenerateConstLoadInstructionsByte();
        }

        private static void GenerateConstLoadInstructionsByte(byte a = 0, byte b = 1, byte c = 2, byte d = 3, byte e = 4, byte f = 5, byte g = 6, byte h = 7, byte i = 8, byte j = 127, byte k = 128, byte l = 0xFF)
        {
        }

        public void ConstLoadSByte()
        {
            GenerateConstLoadInstructionsSByte();
        }

        private static void GenerateConstLoadInstructionsSByte(sbyte a = -128, sbyte b = -127, sbyte c = -1, sbyte d = 0, sbyte e = 1, sbyte f = 2, sbyte g = 3, sbyte h = 4, sbyte i = 5, sbyte j = 6, sbyte k = 7, sbyte l = 8, sbyte m = 127)
        {
        }

        public void ConstLoadChar()
        {
            GenerateConstLoadInstructionsChar();
        }

        // Converted to Int32 internally, not sure what to put in here for better tests
        private static void GenerateConstLoadInstructionsChar(char a = 'a', char b = '\u14D6')
        {
        }

        public void ConstLoadInt16()
        {
            GenerateConstLoadInstructionsInt16();
        }

        private static void GenerateConstLoadInstructionsInt16(short a = short.MinValue, short b = short.MinValue + 1, short c = -129, short d = -128, short e = -127, short f = -1, short g = 0, short h = 1, short i = 2, short j = 3, short k = 4, short l = 5, short m = 6, short n = 7, short o = 8, short p = 127, short q = 128, short r = 129, short u = short.MaxValue - 1, short s = short.MaxValue)
        {
        }

        public void ConstLoadUInt16()
        {
            GenerateConstLoadInstructionsUInt16();
        }

        private static void GenerateConstLoadInstructionsUInt16(ushort a = 0, ushort b = 1, ushort c = 2, ushort d = 3, ushort e = 4, ushort f = 5, ushort g = 6, ushort h = 7, ushort i = 8, ushort j = 127, ushort k = 128, ushort l = 129, ushort m = (ushort)Int16.MaxValue - 1, ushort n = (ushort)Int16.MaxValue, ushort o = ushort.MaxValue - 1, ushort p = ushort.MaxValue)
        {
        }

        public void ConstLoadInt32()
        {
            GenerateConstLoadInstructionsInt32();
        }

        private static void GenerateConstLoadInstructionsInt32(int a = int.MinValue, int b = int.MinValue + 1, int c = -129, int d = -128, int e = -127, int f = -1, int g = 0, int h = 1, int i = 2, int j = 3, int k = 4, int l = 5, int m = 6, int n = 7, int o = 8, int p = 127, int q = 128, int u = 129, int v = int.MaxValue - 1, int w = int.MaxValue)
        {
        }

        public void ConstLoadUInt32()
        {
            GenerateConstLoadInstructionsUInt32();
        }

        private static void GenerateConstLoadInstructionsUInt32(uint a = 0, uint b = 1, uint c = 2, uint d = 3, uint e = 4, uint f = 5, uint g = 6, uint h = 7, uint i = 8, uint j = 127, uint k = 128, uint l = 129, uint m = int.MaxValue - 1, uint n = int.MaxValue, uint o = (uint)int.MaxValue + 1, uint p = uint.MaxValue - 1, uint q = uint.MaxValue)
        {
        }

        public void ConstLoadInt64()
        {
            GenerateConstLoadInstructionsInt64();
        }

        private static void GenerateConstLoadInstructionsInt64(long a = long.MinValue, long b = long.MinValue + 1, long c = ((long)int.MinValue) - 1, long d = int.MinValue, long e = int.MinValue + 1, long f = -129, long g = -128, long h = -127, long i = -1, long j = 0, long k = 1, long l = 2, long m = 3, long n = 4, long o = 5, long p = 6, long q = 7, long u = 8, long v = 127, long w = 128, long x = 129, long y = int.MaxValue - 1, long z = int.MaxValue, long aa = ((long)int.MaxValue) + 1, long ab = ((long)int.MaxValue) + 2, long ac = uint.MaxValue - 1, long ad = uint.MaxValue, long ae = ((long)uint.MaxValue) + 1, long af = long.MaxValue - 1, long ag = long.MaxValue)
        {
        }

        public void ConstLoadUInt64()
        {
            GenerateConstLoadInstructionsUInt64();
        }

        private static void GenerateConstLoadInstructionsUInt64(ulong a = 0, ulong b = 1, ulong c = 2, ulong d = 3, ulong e = 4, ulong f = 5, ulong g = 6, ulong h = 7, ulong i = 8, ulong j = 127, ulong k = 128, ulong l = 129, ulong m = int.MaxValue - 1, ulong n = int.MaxValue, ulong o = ((long)int.MaxValue) + 1, ulong p = uint.MaxValue - 1, ulong q = uint.MaxValue, ulong u = ((long)uint.MaxValue) + 1, ulong v = long.MaxValue - 1, ulong w = long.MaxValue, ulong x = ((ulong)long.MaxValue) + 1, ulong y = ulong.MaxValue - 1, ulong z = ulong.MaxValue)
        {
        }

        public void ConstLoadSingle()
        {
            GenerateConstLoadInstructionsSingle();
        }

        private static void GenerateConstLoadInstructionsSingle(float a = 0, float b = 1, float c = -1, float d = float.MinValue, float e = float.MaxValue, float f = float.NaN, float g = float.NegativeInfinity, float i = float.PositiveInfinity, float j = float.Epsilon)
        {
        }

        public void ConstLoadDouble()
        {
            GenerateConstLoadInstructionsDouble();
        }

        private static void GenerateConstLoadInstructionsDouble(double a = 0, double b = 1, double c = -1, double d = double.MinValue, double e = double.MaxValue, double f = double.NaN, double g = double.NegativeInfinity, double i = double.PositiveInfinity, double z = double.Epsilon)
        {
        }

        public void ConstLoadString()
        {
            GenerateConstLoadInstructionsString();
        }

        private static void GenerateConstLoadInstructionsString(string a = null, string b = "", string c = "Str 1", string d = "Str 2")
        {
        }

        public class SomeClass
        {
        }

        public void ConstLoadClass()
        {
            GenerateConstLoadInstructionsClass();
        }

        private static void GenerateConstLoadInstructionsClass(SomeClass a = null)
        {
        }

        public enum SomeEnum
        {
            A, B
        }

        public enum SomeByteEnum : byte
        {
            A, B
        }

        public enum SomeLongEnum : long
        {
            A, B
        }

        public void ConstLoadEnum()
        {
            GenerateConstLoadInstructionsEnum();
        }

        private static void GenerateConstLoadInstructionsEnum(SomeEnum a = SomeEnum.A, SomeEnum b = SomeEnum.B)
        {
        }

        public void ConstLoadByteEnum()
        {
            GenerateConstLoadInstructionsByteEnum();
        }

        private static void GenerateConstLoadInstructionsByteEnum(SomeByteEnum a = SomeByteEnum.A, SomeByteEnum b = SomeByteEnum.B)
        {
        }

        public void ConstLoadLongEnum()
        {
            GenerateConstLoadInstructionsLongEnum();
        }

        private static void GenerateConstLoadInstructionsLongEnum(SomeLongEnum a = SomeLongEnum.A, SomeLongEnum b = SomeLongEnum.B)
        {
        }
    }
}
