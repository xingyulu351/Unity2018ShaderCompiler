IL For method: System.Void CanSerializeStruct::.ctor()
// Method begins at RVA 0x206c
// Code size 7 (0x7)
.maxstack 8

IL_0000: ldarg.0
IL_0001: call instance void [UnityEngine]UnityEngine.MonoBehaviour::.ctor()
IL_0006: ret


IL For method: System.Void CanSerializeStruct::Unity_Serialize(System.Int32,System.Boolean)
// Method begins at RVA 0x2074
// Code size 31 (0x1f)
.maxstack 8

IL_0000: ldarg.1
IL_0001: ldc.i4.7
IL_0002: bgt.s IL_0013

IL_0004: ldarg.0
IL_0005: ldflda valuetype CanSerializeStruct/MyStruct CanSerializeStruct::myStruct
IL_000a: ldarg.1
IL_000b: ldc.i4.1
IL_000c: add
IL_000d: ldarg.2
IL_000e: call instance void ['Assembly-CSharp.dll']CanSerializeStruct/MyStruct::Unity_Serialize(int32, bool)

IL_0013: nop
IL_0014: ldsfld class [UnityEngine]UnityEngine.ISerializedStateWriter [UnityEngine]UnityEngine.Serialization.SerializedStateWriter::Instance
IL_0019: callvirt instance void [UnityEngine]UnityEngine.ISerializedStateWriter::Align()
IL_001e: ret


IL For method: System.Void CanSerializeStruct::Unity_Deserialize(System.Int32,System.Boolean)
// Method begins at RVA 0x2094
// Code size 31 (0x1f)
.maxstack 8

IL_0000: ldarg.1
IL_0001: ldc.i4.7
IL_0002: bgt.s IL_0013

IL_0004: ldarg.0
IL_0005: ldflda valuetype CanSerializeStruct/MyStruct CanSerializeStruct::myStruct
IL_000a: ldarg.1
IL_000b: ldc.i4.1
IL_000c: add
IL_000d: ldarg.2
IL_000e: call instance void ['Assembly-CSharp.dll']CanSerializeStruct/MyStruct::Unity_Deserialize(int32, bool)

IL_0013: nop
IL_0014: ldsfld class [UnityEngine]UnityEngine.ISerializedStateReader [UnityEngine]UnityEngine.Serialization.SerializedStateReader::Instance
IL_0019: callvirt instance void [UnityEngine]UnityEngine.ISerializedStateReader::Align()
IL_001e: ret


IL For method: System.Void CanSerializeStruct::Unity_RemapPPtrs(System.Int32,System.Boolean)
// Method begins at RVA 0x20b4
// Code size 21 (0x15)
.maxstack 8

IL_0000: ldarg.1
IL_0001: ldc.i4.7
IL_0002: bgt.s IL_0013

IL_0004: ldarg.0
IL_0005: ldflda valuetype CanSerializeStruct/MyStruct CanSerializeStruct::myStruct
IL_000a: ldarg.1
IL_000b: ldc.i4.1
IL_000c: add
IL_000d: ldarg.2
IL_000e: call instance void ['Assembly-CSharp.dll']CanSerializeStruct/MyStruct::Unity_RemapPPtrs(int32, bool)

IL_0013: nop
IL_0014: ret


IL For method: System.Void CanSerializeStruct::Unity_NamedSerialize(System.Int32,System.Boolean)
// Method begins at RVA 0x20cc
// Code size 68 (0x44)
.maxstack 4
.locals init (
    [0] uint8& pinned
)

IL_0000: ldsfld uint8[] UnityEngine.Internal.$FieldNamesStorage::$RuntimeNames
IL_0005: ldc.i4.0
IL_0006: ldelema [mscorlib]System.Byte
IL_000b: stloc.0
IL_000c: ldarg.1
IL_000d: ldc.i4.7
IL_000e: bgt.s IL_0038

IL_0010: ldarg.0
IL_0011: ldflda valuetype CanSerializeStruct/MyStruct CanSerializeStruct::myStruct
IL_0016: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateWriter [UnityEngine]UnityEngine.Serialization.SerializedNamedStateWriter::Instance
IL_001b: ldloc.0
IL_001c: conv.i
IL_001d: ldc.i4.s 25
IL_001f: add
IL_0020: callvirt instance void [UnityEngine]UnityEngine.ISerializedNamedStateWriter::BeginMetaGroup(native int)
IL_0025: ldarg.1
IL_0026: ldc.i4.1
IL_0027: add
IL_0028: ldarg.2
IL_0029: call instance void ['Assembly-CSharp.dll']CanSerializeStruct/MyStruct::Unity_NamedSerialize(int32, bool)
IL_002e: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateWriter [UnityEngine]UnityEngine.Serialization.SerializedNamedStateWriter::Instance
IL_0033: callvirt instance void [UnityEngine]UnityEngine.ISerializedNamedStateWriter::EndMetaGroup()

IL_0038: nop
IL_0039: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateWriter [UnityEngine]UnityEngine.Serialization.SerializedNamedStateWriter::Instance
IL_003e: callvirt instance void [UnityEngine]UnityEngine.ISerializedNamedStateWriter::Align()
IL_0043: ret


IL For method: System.Void CanSerializeStruct::Unity_NamedDeserialize(System.Int32,System.Boolean)
// Method begins at RVA 0x211c
// Code size 68 (0x44)
.maxstack 4
.locals init (
    [0] uint8& pinned
)

IL_0000: ldsfld uint8[] UnityEngine.Internal.$FieldNamesStorage::$RuntimeNames
IL_0005: ldc.i4.0
IL_0006: ldelema [mscorlib]System.Byte
IL_000b: stloc.0
IL_000c: ldarg.1
IL_000d: ldc.i4.7
IL_000e: bgt.s IL_0038

IL_0010: ldarg.0
IL_0011: ldflda valuetype CanSerializeStruct/MyStruct CanSerializeStruct::myStruct
IL_0016: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateReader [UnityEngine]UnityEngine.Serialization.SerializedNamedStateReader::Instance
IL_001b: ldloc.0
IL_001c: conv.i
IL_001d: ldc.i4.s 25
IL_001f: add
IL_0020: callvirt instance void [UnityEngine]UnityEngine.ISerializedNamedStateReader::BeginMetaGroup(native int)
IL_0025: ldarg.1
IL_0026: ldc.i4.1
IL_0027: add
IL_0028: ldarg.2
IL_0029: call instance void ['Assembly-CSharp.dll']CanSerializeStruct/MyStruct::Unity_NamedDeserialize(int32, bool)
IL_002e: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateReader [UnityEngine]UnityEngine.Serialization.SerializedNamedStateReader::Instance
IL_0033: callvirt instance void [UnityEngine]UnityEngine.ISerializedNamedStateReader::EndMetaGroup()

IL_0038: nop
IL_0039: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateReader [UnityEngine]UnityEngine.Serialization.SerializedNamedStateReader::Instance
IL_003e: callvirt instance void [UnityEngine]UnityEngine.ISerializedNamedStateReader::Align()
IL_0043: ret


IL For method: System.Void CanSerializeStruct/MyStruct::.ctor(System.Int32)
// Method begins at RVA 0x216c
// Code size 35 (0x23)
.maxstack 2

IL_0000: ldarg.0
IL_0001: ldc.i4.0
IL_0002: stfld int32 CanSerializeStruct/MyStruct::t
IL_0007: ldarg.0
IL_0008: ldc.i4.s 97
IL_000a: stfld char CanSerializeStruct/MyStruct::myChar
IL_000f: ldarg.0
IL_0010: ldc.i4.s 127
IL_0012: stfld uint8 CanSerializeStruct/MyStruct::myByte
IL_0017: ldarg.0
IL_0018: ldc.i4 320
IL_001d: stfld uint16 CanSerializeStruct/MyStruct::myUInt16
IL_0022: ret


IL For method: System.Void CanSerializeStruct/MyStruct::.ctor(UnityEngine.IUnitySerializable)
// Method begins at RVA 0x219c
// Code size 1 (0x1)
.maxstack 8

IL_0000: ret


IL For method: System.Void CanSerializeStruct/MyStruct::Unity_Serialize(System.Int32,System.Boolean)
// Method begins at RVA 0x21a0
// Code size 95 (0x5f)
.maxstack 2

IL_0000: ldsfld class [UnityEngine]UnityEngine.ISerializedStateWriter [UnityEngine]UnityEngine.Serialization.SerializedStateWriter::Instance
IL_0005: ldarg.0
IL_0006: ldfld uint8 CanSerializeStruct/MyStruct::myByte
IL_000b: callvirt instance void [UnityEngine]UnityEngine.ISerializedStateWriter::WriteByte(uint8)
IL_0010: ldsfld class [UnityEngine]UnityEngine.ISerializedStateWriter [UnityEngine]UnityEngine.Serialization.SerializedStateWriter::Instance
IL_0015: callvirt instance void [UnityEngine]UnityEngine.ISerializedStateWriter::Align()
IL_001a: ldsfld class [UnityEngine]UnityEngine.ISerializedStateWriter [UnityEngine]UnityEngine.Serialization.SerializedStateWriter::Instance
IL_001f: ldarg.0
IL_0020: ldfld int32 CanSerializeStruct/MyStruct::t
IL_0025: callvirt instance void [UnityEngine]UnityEngine.ISerializedStateWriter::WriteInt32(int32)
IL_002a: ldsfld class [UnityEngine]UnityEngine.ISerializedStateWriter [UnityEngine]UnityEngine.Serialization.SerializedStateWriter::Instance
IL_002f: ldarg.0
IL_0030: ldfld uint16 CanSerializeStruct/MyStruct::myUInt16
IL_0035: callvirt instance void [UnityEngine]UnityEngine.ISerializedStateWriter::WriteUInt16(uint16)
IL_003a: ldsfld class [UnityEngine]UnityEngine.ISerializedStateWriter [UnityEngine]UnityEngine.Serialization.SerializedStateWriter::Instance
IL_003f: callvirt instance void [UnityEngine]UnityEngine.ISerializedStateWriter::Align()
IL_0044: ldsfld class [UnityEngine]UnityEngine.ISerializedStateWriter [UnityEngine]UnityEngine.Serialization.SerializedStateWriter::Instance
IL_0049: ldarg.0
IL_004a: ldfld char CanSerializeStruct/MyStruct::myChar
IL_004f: callvirt instance void [UnityEngine]UnityEngine.ISerializedStateWriter::WriteChar(char)
IL_0054: ldsfld class [UnityEngine]UnityEngine.ISerializedStateWriter [UnityEngine]UnityEngine.Serialization.SerializedStateWriter::Instance
IL_0059: callvirt instance void [UnityEngine]UnityEngine.ISerializedStateWriter::Align()
IL_005e: ret


IL For method: System.Void CanSerializeStruct/MyStruct::Unity_Deserialize(System.Int32,System.Boolean)
// Method begins at RVA 0x220c
// Code size 95 (0x5f)
.maxstack 2

IL_0000: ldarg.0
IL_0001: ldsfld class [UnityEngine]UnityEngine.ISerializedStateReader [UnityEngine]UnityEngine.Serialization.SerializedStateReader::Instance
IL_0006: callvirt instance uint8 [UnityEngine]UnityEngine.ISerializedStateReader::ReadByte()
IL_000b: stfld uint8 CanSerializeStruct/MyStruct::myByte
IL_0010: ldsfld class [UnityEngine]UnityEngine.ISerializedStateReader [UnityEngine]UnityEngine.Serialization.SerializedStateReader::Instance
IL_0015: callvirt instance void [UnityEngine]UnityEngine.ISerializedStateReader::Align()
IL_001a: ldarg.0
IL_001b: ldsfld class [UnityEngine]UnityEngine.ISerializedStateReader [UnityEngine]UnityEngine.Serialization.SerializedStateReader::Instance
IL_0020: callvirt instance int32 [UnityEngine]UnityEngine.ISerializedStateReader::ReadInt32()
IL_0025: stfld int32 CanSerializeStruct/MyStruct::t
IL_002a: ldarg.0
IL_002b: ldsfld class [UnityEngine]UnityEngine.ISerializedStateReader [UnityEngine]UnityEngine.Serialization.SerializedStateReader::Instance
IL_0030: callvirt instance uint16 [UnityEngine]UnityEngine.ISerializedStateReader::ReadUInt16()
IL_0035: stfld uint16 CanSerializeStruct/MyStruct::myUInt16
IL_003a: ldsfld class [UnityEngine]UnityEngine.ISerializedStateReader [UnityEngine]UnityEngine.Serialization.SerializedStateReader::Instance
IL_003f: callvirt instance void [UnityEngine]UnityEngine.ISerializedStateReader::Align()
IL_0044: ldarg.0
IL_0045: ldsfld class [UnityEngine]UnityEngine.ISerializedStateReader [UnityEngine]UnityEngine.Serialization.SerializedStateReader::Instance
IL_004a: callvirt instance char [UnityEngine]UnityEngine.ISerializedStateReader::ReadChar()
IL_004f: stfld char CanSerializeStruct/MyStruct::myChar
IL_0054: ldsfld class [UnityEngine]UnityEngine.ISerializedStateReader [UnityEngine]UnityEngine.Serialization.SerializedStateReader::Instance
IL_0059: callvirt instance void [UnityEngine]UnityEngine.ISerializedStateReader::Align()
IL_005e: ret


IL For method: System.Void CanSerializeStruct/MyStruct::Unity_RemapPPtrs(System.Int32,System.Boolean)
// Method begins at RVA 0x2278
// Code size 1 (0x1)
.maxstack 8

IL_0000: ret


IL For method: System.Void CanSerializeStruct/MyStruct::Unity_NamedSerialize(System.Int32,System.Boolean)
// Method begins at RVA 0x227c
// Code size 123 (0x7b)
.maxstack 4
.locals init (
    [0] uint8& pinned
)

IL_0000: ldsfld uint8[] UnityEngine.Internal.$FieldNamesStorage::$RuntimeNames
IL_0005: ldc.i4.0
IL_0006: ldelema [mscorlib]System.Byte
IL_000b: stloc.0
IL_000c: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateWriter [UnityEngine]UnityEngine.Serialization.SerializedNamedStateWriter::Instance
IL_0011: ldarg.0
IL_0012: ldfld uint8 CanSerializeStruct/MyStruct::myByte
IL_0017: ldloc.0
IL_0018: conv.i
IL_0019: callvirt instance void [UnityEngine]UnityEngine.ISerializedNamedStateWriter::WriteByte(uint8, native int)
IL_001e: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateWriter [UnityEngine]UnityEngine.Serialization.SerializedNamedStateWriter::Instance
IL_0023: callvirt instance void [UnityEngine]UnityEngine.ISerializedNamedStateWriter::Align()
IL_0028: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateWriter [UnityEngine]UnityEngine.Serialization.SerializedNamedStateWriter::Instance
IL_002d: ldarg.0
IL_002e: ldfld int32 CanSerializeStruct/MyStruct::t
IL_0033: ldloc.0
IL_0034: conv.i
IL_0035: ldc.i4.7
IL_0036: add
IL_0037: callvirt instance void [UnityEngine]UnityEngine.ISerializedNamedStateWriter::WriteInt32(int32, native int)
IL_003c: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateWriter [UnityEngine]UnityEngine.Serialization.SerializedNamedStateWriter::Instance
IL_0041: ldarg.0
IL_0042: ldfld uint16 CanSerializeStruct/MyStruct::myUInt16
IL_0047: ldloc.0
IL_0048: conv.i
IL_0049: ldc.i4.s 9
IL_004b: add
IL_004c: callvirt instance void [UnityEngine]UnityEngine.ISerializedNamedStateWriter::WriteUInt16(uint16, native int)
IL_0051: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateWriter [UnityEngine]UnityEngine.Serialization.SerializedNamedStateWriter::Instance
IL_0056: callvirt instance void [UnityEngine]UnityEngine.ISerializedNamedStateWriter::Align()
IL_005b: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateWriter [UnityEngine]UnityEngine.Serialization.SerializedNamedStateWriter::Instance
IL_0060: ldarg.0
IL_0061: ldfld char CanSerializeStruct/MyStruct::myChar
IL_0066: ldloc.0
IL_0067: conv.i
IL_0068: ldc.i4.s 18
IL_006a: add
IL_006b: callvirt instance void [UnityEngine]UnityEngine.ISerializedNamedStateWriter::WriteChar(char, native int)
IL_0070: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateWriter [UnityEngine]UnityEngine.Serialization.SerializedNamedStateWriter::Instance
IL_0075: callvirt instance void [UnityEngine]UnityEngine.ISerializedNamedStateWriter::Align()
IL_007a: ret


IL For method: System.Void CanSerializeStruct/MyStruct::Unity_NamedDeserialize(System.Int32,System.Boolean)
// Method begins at RVA 0x2304
// Code size 123 (0x7b)
.maxstack 4
.locals init (
    [0] uint8& pinned
)

IL_0000: ldsfld uint8[] UnityEngine.Internal.$FieldNamesStorage::$RuntimeNames
IL_0005: ldc.i4.0
IL_0006: ldelema [mscorlib]System.Byte
IL_000b: stloc.0
IL_000c: ldarg.0
IL_000d: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateReader [UnityEngine]UnityEngine.Serialization.SerializedNamedStateReader::Instance
IL_0012: ldloc.0
IL_0013: conv.i
IL_0014: callvirt instance uint8 [UnityEngine]UnityEngine.ISerializedNamedStateReader::ReadByte(native int)
IL_0019: stfld uint8 CanSerializeStruct/MyStruct::myByte
IL_001e: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateReader [UnityEngine]UnityEngine.Serialization.SerializedNamedStateReader::Instance
IL_0023: callvirt instance void [UnityEngine]UnityEngine.ISerializedNamedStateReader::Align()
IL_0028: ldarg.0
IL_0029: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateReader [UnityEngine]UnityEngine.Serialization.SerializedNamedStateReader::Instance
IL_002e: ldloc.0
IL_002f: conv.i
IL_0030: ldc.i4.7
IL_0031: add
IL_0032: callvirt instance int32 [UnityEngine]UnityEngine.ISerializedNamedStateReader::ReadInt32(native int)
IL_0037: stfld int32 CanSerializeStruct/MyStruct::t
IL_003c: ldarg.0
IL_003d: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateReader [UnityEngine]UnityEngine.Serialization.SerializedNamedStateReader::Instance
IL_0042: ldloc.0
IL_0043: conv.i
IL_0044: ldc.i4.s 9
IL_0046: add
IL_0047: callvirt instance uint16 [UnityEngine]UnityEngine.ISerializedNamedStateReader::ReadUInt16(native int)
IL_004c: stfld uint16 CanSerializeStruct/MyStruct::myUInt16
IL_0051: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateReader [UnityEngine]UnityEngine.Serialization.SerializedNamedStateReader::Instance
IL_0056: callvirt instance void [UnityEngine]UnityEngine.ISerializedNamedStateReader::Align()
IL_005b: ldarg.0
IL_005c: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateReader [UnityEngine]UnityEngine.Serialization.SerializedNamedStateReader::Instance
IL_0061: ldloc.0
IL_0062: conv.i
IL_0063: ldc.i4.s 18
IL_0065: add
IL_0066: callvirt instance char [UnityEngine]UnityEngine.ISerializedNamedStateReader::ReadChar(native int)
IL_006b: stfld char CanSerializeStruct/MyStruct::myChar
IL_0070: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateReader [UnityEngine]UnityEngine.Serialization.SerializedNamedStateReader::Instance
IL_0075: callvirt instance void [UnityEngine]UnityEngine.ISerializedNamedStateReader::Align()
IL_007a: ret


