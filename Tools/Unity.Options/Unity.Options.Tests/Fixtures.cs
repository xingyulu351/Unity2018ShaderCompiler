using System.Collections.Generic;

namespace Unity.Options.Tests
{
    [ProgramOptions]
    public sealed class SimpleOptions
    {
        public static int Value;
    }

    [ProgramOptions]
    public sealed class MultipleSimpleOptions
    {
        public static int Value;
        public static int SecondValue;
    }

    [ProgramOptions(Group = "custom")]
    public sealed class CustomGroupOptions
    {
        public static int Value;
        public static int SecondValue;
    }

    [ProgramOptions]
    public sealed class BasicTypesOptions
    {
        public static char CharValue;
        public static byte ByteValue;
        public static sbyte SByteValue;
        public static short ShortValue;
        public static ushort UShortValue;
        public static int IntValue;
        public static uint UIntValue;
        public static long LongValue;
        public static ulong ULongValue;
        public static float FloatValue;
        public static double DoubleValue;
        public static bool BoolValue;
        public static string StringValue;
    }

    [ProgramOptions]
    public sealed class ArrayOptions
    {
        public static int[] IntArray;
    }

    [ProgramOptions]
    public sealed class ArrayRepetitionOptions
    {
        public static int[] IntArray;
    }

    [ProgramOptions(CollectionSeparator = "|")]
    public sealed class CustomArrayOptions
    {
        public static int[] IntArray;
    }

    [ProgramOptions]
    public sealed class ArrayBasicTypesOptions
    {
        public static char[] CharValue;
        public static byte[] ByteValue;
        public static sbyte[] SByteValue;
        public static short[] ShortValue;
        public static ushort[] UShortValue;
        public static int[] IntValue;
        public static uint[] UIntValue;
        public static long[] LongValue;
        public static ulong[] ULongValue;
        public static float[] FloatValue;
        public static double[] DoubleValue;
        public static bool[] BoolValue;
        public static string[] StringValue;
    }

    [ProgramOptions]
    public sealed class ListOptions
    {
        public static List<int> IntList;
    }

    [ProgramOptions]
    public sealed class ListRepetitionOptions
    {
        public static List<int> IntList;
    }

    [ProgramOptions(CollectionSeparator = "|")]
    public sealed class CustomListOptions
    {
        public static List<int> IntList;
    }

    [ProgramOptions]
    public sealed class ListBasicTypesOptions
    {
        public static List<char> CharList;
        public static List<byte> ByteList;
        public static List<sbyte> SByteList;
        public static List<short> ShortList;
        public static List<ushort> UShortList;
        public static List<int> IntList;
        public static List<uint> UIntList;
        public static List<long> LongList;
        public static List<ulong> ULongList;
        public static List<float> FloatList;
        public static List<double> DoubleList;
        public static List<bool> BoolList;
        public static List<string> StringList;
    }

    public enum Values
    {
        First,
        Second
    }

    [ProgramOptions]
    public sealed class EnumOptions
    {
        public static Values EnumValue;
    }

    [ProgramOptions]
    public sealed class EnumArrayOptions
    {
        public static Values[] EnumArrayValue;
    }

    [ProgramOptions]
    public sealed class BoolOptions
    {
        public static bool BoolValue;
    }

    [ProgramOptions]
    public sealed class HelpOptions
    {
        internal const string OptionOneHelpText = "This is help information";
        internal const string OptionTwoHelpText = "I hope it helps you";
        internal const string CustomValueDescriptionHelpText = "Dont Care..normally";

        [HelpDetails(OptionOneHelpText)]
        public static string OptionOne;

        [HelpDetails(OptionTwoHelpText)]
        public static bool OptionTwo;

        public static string OptionMissingHelpDetails;

        [HelpDetails(CustomValueDescriptionHelpText, "path")]
        public static string CustomValueDescription;

        [HideFromHelp]
        public static bool OptionHidden;
    }

    [ProgramOptions]
    public sealed class HelpOptionsWithCollections
    {
        internal const string HelpText = "This is an array arg";

        [HelpDetails(HelpText)]
        public static string[] StringArray;

        [HelpDetails(HelpText, "custom")]
        public static List<string> StringList;
    }
}
