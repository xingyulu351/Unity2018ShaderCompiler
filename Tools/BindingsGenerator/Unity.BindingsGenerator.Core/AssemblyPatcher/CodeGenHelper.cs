using System;
using System.Collections.Generic;
using Mono.Cecil;
using Mono.Cecil.Cil;
using Mono.Cecil.Rocks;

namespace Unity.BindingsGenerator.Core.AssemblyPatcher
{
    public static class CodeGenHelper
    {
        public static List<Instruction> LoadConstantInstructionForPrimitive(int value)
        {
            switch (value)
            {
                case -1:
                    return new List<Instruction> { Instruction.Create(OpCodes.Ldc_I4_M1) };
                case 0:
                    return new List<Instruction> { Instruction.Create(OpCodes.Ldc_I4_0) };
                case 1:
                    return new List<Instruction> { Instruction.Create(OpCodes.Ldc_I4_1) };
                case 2:
                    return new List<Instruction> { Instruction.Create(OpCodes.Ldc_I4_2) };
                case 3:
                    return new List<Instruction> { Instruction.Create(OpCodes.Ldc_I4_3) };
                case 4:
                    return new List<Instruction> { Instruction.Create(OpCodes.Ldc_I4_4) };
                case 5:
                    return new List<Instruction> { Instruction.Create(OpCodes.Ldc_I4_5) };
                case 6:
                    return new List<Instruction> { Instruction.Create(OpCodes.Ldc_I4_6) };
                case 7:
                    return new List<Instruction> { Instruction.Create(OpCodes.Ldc_I4_7) };
                case 8:
                    return new List<Instruction> { Instruction.Create(OpCodes.Ldc_I4_8) };
            }

            if (value >= sbyte.MinValue && value <= sbyte.MaxValue)
                return new List<Instruction> { Instruction.Create(OpCodes.Ldc_I4_S, (sbyte)value) };
            else
                return new List<Instruction> { Instruction.Create(OpCodes.Ldc_I4, value) };
        }

        public static List<Instruction> LoadConstantInstructionForPrimitive(uint value)
        {
            switch (value)
            {
                case 0:
                    return new List<Instruction> { Instruction.Create(OpCodes.Ldc_I4_0) };
                case 1:
                    return new List<Instruction> { Instruction.Create(OpCodes.Ldc_I4_1) };
                case 2:
                    return new List<Instruction> { Instruction.Create(OpCodes.Ldc_I4_2) };
                case 3:
                    return new List<Instruction> { Instruction.Create(OpCodes.Ldc_I4_3) };
                case 4:
                    return new List<Instruction> { Instruction.Create(OpCodes.Ldc_I4_4) };
                case 5:
                    return new List<Instruction> { Instruction.Create(OpCodes.Ldc_I4_5) };
                case 6:
                    return new List<Instruction> { Instruction.Create(OpCodes.Ldc_I4_6) };
                case 7:
                    return new List<Instruction> { Instruction.Create(OpCodes.Ldc_I4_7) };
                case 8:
                    return new List<Instruction> { Instruction.Create(OpCodes.Ldc_I4_8) };
            }

            if (value <= 127)
                return new List<Instruction> {Instruction.Create(OpCodes.Ldc_I4_S, (sbyte)value)};
            else
            {
                // The cast here is pretty wrong, it'll cast unsigned to signed. Apparently expected way to handle in IL.
                int castValue = (int)value;

                return LoadConstantInstructionForPrimitive(castValue);
            }
        }

        public static List<Instruction> LoadConstantInstructionForPrimitive(long value)
        {
            if (value >= int.MinValue && value <= int.MaxValue)
            {
                var instructions = LoadConstantInstructionForPrimitive((int)value);
                instructions.Add(Instruction.Create(OpCodes.Conv_I8));
                return instructions;
            }
            else if (value >= 0 && value <= uint.MaxValue)
            {
                var instructions = LoadConstantInstructionForPrimitive((int)value);
                instructions.Add(Instruction.Create(OpCodes.Conv_U8));
                return instructions;
            }
            else
            {
                return new List<Instruction> { Instruction.Create(OpCodes.Ldc_I8, value) };
            }
        }

        public static List<Instruction> LoadConstantInstructionForPrimitive(ulong value)
        {
            if (value <= int.MaxValue)
            {
                var instructions = LoadConstantInstructionForPrimitive((int)value);
                instructions.Add(Instruction.Create(OpCodes.Conv_I8));
                return instructions;
            }
            else if (value <= uint.MaxValue)
            {
                var instructions = LoadConstantInstructionForPrimitive((int)value);
                instructions.Add(Instruction.Create(OpCodes.Conv_U8));
                return instructions;
            }
            else
            {
                long castValue = (long)value;

                if (castValue >= int.MinValue && castValue <= int.MaxValue)
                {
                    var instructions = LoadConstantInstructionForPrimitive((int)castValue);
                    instructions.Add(Instruction.Create(OpCodes.Conv_I8));
                    return instructions;
                }

                return new List<Instruction> { Instruction.Create(OpCodes.Ldc_I8, castValue) };
            }
        }

        public static List<Instruction> LoadConstantInstructionForPrimitive(byte value)
        {
            return LoadConstantInstructionForPrimitive((uint)value);
        }

        public static List<Instruction> LoadConstantInstructionForPrimitive(char value)
        {
            return LoadConstantInstructionForPrimitive((int)value);
        }

        public static List<Instruction> LoadConstantInstructionForPrimitive(sbyte value)
        {
            return LoadConstantInstructionForPrimitive((int)value);
        }

        public static List<Instruction> LoadConstantInstructionForPrimitive(bool value)
        {
            return LoadConstantInstructionForPrimitive(value ? 1 : 0);
        }

        public static List<Instruction> LoadConstantInstructionForPrimitive(float value)
        {
            return new List<Instruction> { Instruction.Create(OpCodes.Ldc_R4, value) };
        }

        public static List<Instruction> LoadConstantInstructionForPrimitive(double value)
        {
            return new List<Instruction> { Instruction.Create(OpCodes.Ldc_R8, value) };
        }

        public static List<Instruction> LoadConstantInstructionForString(string value)
        {
            if (value == null)
            {
                return new List<Instruction> {Instruction.Create(OpCodes.Ldnull)};
            }
            else
            {
                return new List<Instruction> {Instruction.Create(OpCodes.Ldstr, value)};
            }
        }

        public static List<Instruction> LoadConstantInstructionForObject(object value)
        {
            if (value == null)
            {
                return new List<Instruction> {Instruction.Create(OpCodes.Ldnull)};
            }
            else
            {
                throw new ArgumentException("Cannot assign non-null to object type");
            }
        }

        public static List<Instruction> LoadConstantInstructionFor(TypeDefinition typeDefinition, object value)
        {
            if (typeDefinition.IsEnum)
                return LoadConstantInstructionFor(typeDefinition.GetEnumUnderlyingType().Resolve(), value);
            if (typeDefinition.MetadataType == MetadataType.Boolean)
                return CodeGenHelper.LoadConstantInstructionForPrimitive((bool)value);
            if (typeDefinition.MetadataType == MetadataType.Byte)
                return CodeGenHelper.LoadConstantInstructionForPrimitive((byte)value);
            if (typeDefinition.MetadataType == MetadataType.SByte)
                return CodeGenHelper.LoadConstantInstructionForPrimitive((sbyte)value);
            if (typeDefinition.MetadataType == MetadataType.Char)
                return CodeGenHelper.LoadConstantInstructionForPrimitive((char)value);
            if (typeDefinition.MetadataType == MetadataType.Int16)
                return CodeGenHelper.LoadConstantInstructionForPrimitive((short)value);
            if (typeDefinition.MetadataType == MetadataType.UInt16)
                return CodeGenHelper.LoadConstantInstructionForPrimitive((ushort)value);
            if (typeDefinition.MetadataType == MetadataType.Int32)
                return CodeGenHelper.LoadConstantInstructionForPrimitive((int)value);
            if (typeDefinition.MetadataType == MetadataType.UInt32)
                return CodeGenHelper.LoadConstantInstructionForPrimitive((uint)value);
            if (typeDefinition.MetadataType == MetadataType.Int64)
                return CodeGenHelper.LoadConstantInstructionForPrimitive((long)value);
            if (typeDefinition.MetadataType == MetadataType.UInt64)
                return CodeGenHelper.LoadConstantInstructionForPrimitive((ulong)value);
            if (typeDefinition.MetadataType == MetadataType.Single)
                return CodeGenHelper.LoadConstantInstructionForPrimitive((float)value);
            if (typeDefinition.MetadataType == MetadataType.Double)
                return CodeGenHelper.LoadConstantInstructionForPrimitive((double)value);
            if (typeDefinition.MetadataType == MetadataType.String)
                return CodeGenHelper.LoadConstantInstructionForString((string)value);

            return CodeGenHelper.LoadConstantInstructionForObject(value);
        }
    }
}
