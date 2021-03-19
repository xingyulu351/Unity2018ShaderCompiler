IL For method: System.Void Base::.ctor()
// Method begins at RVA 0x206c
// Code size 7 (0x7)
.maxstack 8

IL_0000: ldarg.0
IL_0001: call instance void [mscorlib]System.Object::.ctor()
IL_0006: ret


IL For method: System.Void Derived::.ctor()
// Method begins at RVA 0x2074
// Code size 7 (0x7)
.maxstack 8

IL_0000: ldarg.0
IL_0001: call instance void Base::.ctor()
IL_0006: ret


IL For method: System.Void Derived::Unity_Serialize(System.Int32,System.Boolean)
// Method begins at RVA 0x207c
// Code size 33 (0x21)
.maxstack 8

IL_0000: ldsfld class [UnityEngine]UnityEngine.ISerializedStateWriter [UnityEngine]UnityEngine.Serialization.SerializedStateWriter::Instance
IL_0005: ldarg.0
IL_0006: ldfld int32 Base::x
IL_000b: callvirt instance void [UnityEngine]UnityEngine.ISerializedStateWriter::WriteInt32(int32)
IL_0010: ldsfld class [UnityEngine]UnityEngine.ISerializedStateWriter [UnityEngine]UnityEngine.Serialization.SerializedStateWriter::Instance
IL_0015: ldarg.0
IL_0016: ldfld int32 Derived::y
IL_001b: callvirt instance void [UnityEngine]UnityEngine.ISerializedStateWriter::WriteInt32(int32)
IL_0020: ret


IL For method: System.Void Derived::Unity_Deserialize(System.Int32,System.Boolean)
// Method begins at RVA 0x20a0
// Code size 33 (0x21)
.maxstack 8

IL_0000: ldarg.0
IL_0001: ldsfld class [UnityEngine]UnityEngine.ISerializedStateReader [UnityEngine]UnityEngine.Serialization.SerializedStateReader::Instance
IL_0006: callvirt instance int32 [UnityEngine]UnityEngine.ISerializedStateReader::ReadInt32()
IL_000b: stfld int32 Base::x
IL_0010: ldarg.0
IL_0011: ldsfld class [UnityEngine]UnityEngine.ISerializedStateReader [UnityEngine]UnityEngine.Serialization.SerializedStateReader::Instance
IL_0016: callvirt instance int32 [UnityEngine]UnityEngine.ISerializedStateReader::ReadInt32()
IL_001b: stfld int32 Derived::y
IL_0020: ret


IL For method: System.Void Derived::Unity_RemapPPtrs(System.Int32,System.Boolean)
// Method begins at RVA 0x20c4
// Code size 1 (0x1)
.maxstack 8

IL_0000: ret


IL For method: System.Void Derived::Unity_NamedSerialize(System.Int32,System.Boolean)
// Method begins at RVA 0x20c8
// Code size 51 (0x33)
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
IL_0012: ldfld int32 Base::x
IL_0017: ldloc.0
IL_0018: conv.i
IL_0019: callvirt instance void [UnityEngine]UnityEngine.ISerializedNamedStateWriter::WriteInt32(int32, native int)
IL_001e: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateWriter [UnityEngine]UnityEngine.Serialization.SerializedNamedStateWriter::Instance
IL_0023: ldarg.0
IL_0024: ldfld int32 Derived::y
IL_0029: ldloc.0
IL_002a: conv.i
IL_002b: ldc.i4.2
IL_002c: add
IL_002d: callvirt instance void [UnityEngine]UnityEngine.ISerializedNamedStateWriter::WriteInt32(int32, native int)
IL_0032: ret


IL For method: System.Void Derived::Unity_NamedDeserialize(System.Int32,System.Boolean)
// Method begins at RVA 0x2108
// Code size 51 (0x33)
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
IL_0014: callvirt instance int32 [UnityEngine]UnityEngine.ISerializedNamedStateReader::ReadInt32(native int)
IL_0019: stfld int32 Base::x
IL_001e: ldarg.0
IL_001f: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateReader [UnityEngine]UnityEngine.Serialization.SerializedNamedStateReader::Instance
IL_0024: ldloc.0
IL_0025: conv.i
IL_0026: ldc.i4.2
IL_0027: add
IL_0028: callvirt instance int32 [UnityEngine]UnityEngine.ISerializedNamedStateReader::ReadInt32(native int)
IL_002d: stfld int32 Derived::y
IL_0032: ret


IL For method: System.Void ArraysToObjectsSerializedProperly::.ctor()
// Method begins at RVA 0x2148
// Code size 7 (0x7)
.maxstack 8

IL_0000: ldarg.0
IL_0001: call instance void [UnityEngine]UnityEngine.MonoBehaviour::.ctor()
IL_0006: ret


IL For method: System.Void ArraysToObjectsSerializedProperly::Unity_Serialize(System.Int32,System.Boolean)
// Method begins at RVA 0x2150
// Code size 113 (0x71)
.maxstack 3
.locals init (
    [0] int32
)

IL_0000: ldarg.1
IL_0001: ldc.i4.7
IL_0002: bgt.s IL_006f

IL_0004: ldarg.0
IL_0005: ldfld class Derived[] ArraysToObjectsSerializedProperly::derivees
IL_000a: brtrue.s IL_0019

IL_000c: ldsfld class [UnityEngine]UnityEngine.ISerializedStateWriter [UnityEngine]UnityEngine.Serialization.SerializedStateWriter::Instance
IL_0011: ldc.i4.0
IL_0012: callvirt instance void [UnityEngine]UnityEngine.ISerializedStateWriter::WriteInt32(int32)
IL_0017: br.s IL_006e

IL_0019: nop
IL_001a: ldsfld class [UnityEngine]UnityEngine.ISerializedStateWriter [UnityEngine]UnityEngine.Serialization.SerializedStateWriter::Instance
IL_001f: ldarg.0
IL_0020: ldfld class Derived[] ArraysToObjectsSerializedProperly::derivees
IL_0025: ldlen
IL_0026: callvirt instance void [UnityEngine]UnityEngine.ISerializedStateWriter::WriteInt32(int32)
IL_002b: ldc.i4.0
IL_002c: stloc.0
IL_002d: br.s IL_0060
// loop start (head: IL_0060)
    IL_002f: nop
    IL_0030: ldarg.0
    IL_0031: ldfld class Derived[] ArraysToObjectsSerializedProperly::derivees
    IL_0036: ldloc.0
    IL_0037: ldelem.any Derived
    IL_003c: brtrue.s IL_0045

    IL_003e: newobj instance void Derived::.ctor()
    IL_0043: br.s IL_0052

    IL_0045: nop
    IL_0046: ldarg.0
    IL_0047: ldfld class Derived[] ArraysToObjectsSerializedProperly::derivees
    IL_004c: ldloc.0
    IL_004d: ldelem.any Derived

    IL_0052: nop
    IL_0053: ldarg.1
    IL_0054: ldc.i4.1
    IL_0055: add
    IL_0056: ldarg.2
    IL_0057: callvirt instance void ['Assembly-CSharp.dll']Derived::Unity_Serialize(int32, bool)
    IL_005c: ldloc.0
    IL_005d: ldc.i4.1
    IL_005e: add
    IL_005f: stloc.0

    IL_0060: nop
    IL_0061: ldloc.0
    IL_0062: ldarg.0
    IL_0063: ldfld class Derived[] ArraysToObjectsSerializedProperly::derivees
    IL_0068: ldlen
    IL_0069: conv.i4
    IL_006a: clt
    IL_006c: brtrue.s IL_002f
// end loop

IL_006e: nop

IL_006f: nop
IL_0070: ret


IL For method: System.Void ArraysToObjectsSerializedProperly::Unity_Deserialize(System.Int32,System.Boolean)
// Method begins at RVA 0x21d0
// Code size 84 (0x54)
.maxstack 3
.locals init (
    [0] int32
)

IL_0000: ldarg.1
IL_0001: ldc.i4.7
IL_0002: bgt.s IL_0052

IL_0004: ldarg.0
IL_0005: ldsfld class [UnityEngine]UnityEngine.ISerializedStateReader [UnityEngine]UnityEngine.Serialization.SerializedStateReader::Instance
IL_000a: callvirt instance int32 [UnityEngine]UnityEngine.ISerializedStateReader::ReadInt32()
IL_000f: newarr Derived
IL_0014: stfld class Derived[] ArraysToObjectsSerializedProperly::derivees
IL_0019: ldc.i4.0
IL_001a: stloc.0
IL_001b: br.s IL_0044
// loop start (head: IL_0044)
    IL_001d: nop
    IL_001e: ldarg.0
    IL_001f: ldfld class Derived[] ArraysToObjectsSerializedProperly::derivees
    IL_0024: ldloc.0
    IL_0025: newobj instance void Derived::.ctor()
    IL_002a: stelem.ref
    IL_002b: ldarg.0
    IL_002c: ldfld class Derived[] ArraysToObjectsSerializedProperly::derivees
    IL_0031: ldloc.0
    IL_0032: ldelem.any Derived
    IL_0037: ldarg.1
    IL_0038: ldc.i4.1
    IL_0039: add
    IL_003a: ldarg.2
    IL_003b: callvirt instance void ['Assembly-CSharp.dll']Derived::Unity_Deserialize(int32, bool)
    IL_0040: ldloc.0
    IL_0041: ldc.i4.1
    IL_0042: add
    IL_0043: stloc.0

    IL_0044: nop
    IL_0045: ldloc.0
    IL_0046: ldarg.0
    IL_0047: ldfld class Derived[] ArraysToObjectsSerializedProperly::derivees
    IL_004c: ldlen
    IL_004d: conv.i4
    IL_004e: clt
    IL_0050: brtrue.s IL_001d
// end loop

IL_0052: nop
IL_0053: ret


IL For method: System.Void ArraysToObjectsSerializedProperly::Unity_RemapPPtrs(System.Int32,System.Boolean)
// Method begins at RVA 0x2230
// Code size 74 (0x4a)
.maxstack 3
.locals init (
    [0] int32
)

IL_0000: ldarg.1
IL_0001: ldc.i4.7
IL_0002: bgt.s IL_0048

IL_0004: ldarg.0
IL_0005: ldfld class Derived[] ArraysToObjectsSerializedProperly::derivees
IL_000a: brfalse.s IL_0047

IL_000c: ldc.i4.0
IL_000d: stloc.0
IL_000e: br.s IL_0039
// loop start (head: IL_0039)
    IL_0010: nop
    IL_0011: ldarg.0
    IL_0012: ldfld class Derived[] ArraysToObjectsSerializedProperly::derivees
    IL_0017: ldloc.0
    IL_0018: ldelem.any Derived
    IL_001d: brfalse.s IL_0034

    IL_001f: ldarg.0
    IL_0020: ldfld class Derived[] ArraysToObjectsSerializedProperly::derivees
    IL_0025: ldloc.0
    IL_0026: ldelem.any Derived
    IL_002b: ldarg.1
    IL_002c: ldc.i4.1
    IL_002d: add
    IL_002e: ldarg.2
    IL_002f: callvirt instance void ['Assembly-CSharp.dll']Derived::Unity_RemapPPtrs(int32, bool)

    IL_0034: nop
    IL_0035: ldloc.0
    IL_0036: ldc.i4.1
    IL_0037: add
    IL_0038: stloc.0

    IL_0039: nop
    IL_003a: ldloc.0
    IL_003b: ldarg.0
    IL_003c: ldfld class Derived[] ArraysToObjectsSerializedProperly::derivees
    IL_0041: ldlen
    IL_0042: conv.i4
    IL_0043: clt
    IL_0045: brtrue.s IL_0010
// end loop

IL_0047: nop

IL_0048: nop
IL_0049: ret


IL For method: System.Void ArraysToObjectsSerializedProperly::Unity_NamedSerialize(System.Int32,System.Boolean)
// Method begins at RVA 0x2288
// Code size 178 (0xb2)
.maxstack 3
.locals init (
    [0] uint8& pinned,
    [1] int32
)

IL_0000: ldsfld uint8[] UnityEngine.Internal.$FieldNamesStorage::$RuntimeNames
IL_0005: ldc.i4.0
IL_0006: ldelema [mscorlib]System.Byte
IL_000b: stloc.0
IL_000c: ldarg.1
IL_000d: ldc.i4.7
IL_000e: bgt IL_00b0

IL_0013: ldarg.0
IL_0014: ldfld class Derived[] ArraysToObjectsSerializedProperly::derivees
IL_0019: brtrue.s IL_0036

IL_001b: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateWriter [UnityEngine]UnityEngine.Serialization.SerializedNamedStateWriter::Instance
IL_0020: ldloc.0
IL_0021: conv.i
IL_0022: ldc.i4.4
IL_0023: add
IL_0024: ldc.i4.0
IL_0025: callvirt instance void [UnityEngine]UnityEngine.ISerializedNamedStateWriter::BeginSequenceGroup(native int, int32)
IL_002a: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateWriter [UnityEngine]UnityEngine.Serialization.SerializedNamedStateWriter::Instance
IL_002f: callvirt instance void [UnityEngine]UnityEngine.ISerializedNamedStateWriter::EndMetaGroup()
IL_0034: br.s IL_00af

IL_0036: nop
IL_0037: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateWriter [UnityEngine]UnityEngine.Serialization.SerializedNamedStateWriter::Instance
IL_003c: ldloc.0
IL_003d: conv.i
IL_003e: ldc.i4.4
IL_003f: add
IL_0040: ldarg.0
IL_0041: ldfld class Derived[] ArraysToObjectsSerializedProperly::derivees
IL_0046: ldlen
IL_0047: callvirt instance void [UnityEngine]UnityEngine.ISerializedNamedStateWriter::BeginSequenceGroup(native int, int32)
IL_004c: ldc.i4.0
IL_004d: stloc.1
IL_004e: br.s IL_0097
// loop start (head: IL_0097)
    IL_0050: nop
    IL_0051: ldarg.0
    IL_0052: ldfld class Derived[] ArraysToObjectsSerializedProperly::derivees
    IL_0057: ldloc.1
    IL_0058: ldelem.any Derived
    IL_005d: brtrue.s IL_0066

    IL_005f: newobj instance void Derived::.ctor()
    IL_0064: br.s IL_0073

    IL_0066: nop
    IL_0067: ldarg.0
    IL_0068: ldfld class Derived[] ArraysToObjectsSerializedProperly::derivees
    IL_006d: ldloc.1
    IL_006e: ldelem.any Derived

    IL_0073: nop
    IL_0074: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateWriter [UnityEngine]UnityEngine.Serialization.SerializedNamedStateWriter::Instance
    IL_0079: ldc.i4.0
    IL_007a: conv.i
    IL_007b: callvirt instance void [UnityEngine]UnityEngine.ISerializedNamedStateWriter::BeginMetaGroup(native int)
    IL_0080: ldarg.1
    IL_0081: ldc.i4.1
    IL_0082: add
    IL_0083: ldarg.2
    IL_0084: callvirt instance void ['Assembly-CSharp.dll']Derived::Unity_NamedSerialize(int32, bool)
    IL_0089: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateWriter [UnityEngine]UnityEngine.Serialization.SerializedNamedStateWriter::Instance
    IL_008e: callvirt instance void [UnityEngine]UnityEngine.ISerializedNamedStateWriter::EndMetaGroup()
    IL_0093: ldloc.1
    IL_0094: ldc.i4.1
    IL_0095: add
    IL_0096: stloc.1

    IL_0097: nop
    IL_0098: ldloc.1
    IL_0099: ldarg.0
    IL_009a: ldfld class Derived[] ArraysToObjectsSerializedProperly::derivees
    IL_009f: ldlen
    IL_00a0: conv.i4
    IL_00a1: clt
    IL_00a3: brtrue.s IL_0050
// end loop

IL_00a5: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateWriter [UnityEngine]UnityEngine.Serialization.SerializedNamedStateWriter::Instance
IL_00aa: callvirt instance void [UnityEngine]UnityEngine.ISerializedNamedStateWriter::EndMetaGroup()

IL_00af: nop

IL_00b0: nop
IL_00b1: ret


IL For method: System.Void ArraysToObjectsSerializedProperly::Unity_NamedDeserialize(System.Int32,System.Boolean)
// Method begins at RVA 0x2348
// Code size 132 (0x84)
.maxstack 4
.locals init (
    [0] uint8& pinned,
    [1] int32
)

IL_0000: ldsfld uint8[] UnityEngine.Internal.$FieldNamesStorage::$RuntimeNames
IL_0005: ldc.i4.0
IL_0006: ldelema [mscorlib]System.Byte
IL_000b: stloc.0
IL_000c: ldarg.1
IL_000d: ldc.i4.7
IL_000e: bgt.s IL_0082

IL_0010: ldarg.0
IL_0011: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateReader [UnityEngine]UnityEngine.Serialization.SerializedNamedStateReader::Instance
IL_0016: ldloc.0
IL_0017: conv.i
IL_0018: ldc.i4.4
IL_0019: add
IL_001a: callvirt instance int32 [UnityEngine]UnityEngine.ISerializedNamedStateReader::BeginSequenceGroup(native int)
IL_001f: newarr Derived
IL_0024: stfld class Derived[] ArraysToObjectsSerializedProperly::derivees
IL_0029: ldc.i4.0
IL_002a: stloc.1
IL_002b: br.s IL_006a
// loop start (head: IL_006a)
    IL_002d: nop
    IL_002e: ldarg.0
    IL_002f: ldfld class Derived[] ArraysToObjectsSerializedProperly::derivees
    IL_0034: ldloc.1
    IL_0035: newobj instance void Derived::.ctor()
    IL_003a: stelem.ref
    IL_003b: ldarg.0
    IL_003c: ldfld class Derived[] ArraysToObjectsSerializedProperly::derivees
    IL_0041: ldloc.1
    IL_0042: ldelem.any Derived
    IL_0047: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateReader [UnityEngine]UnityEngine.Serialization.SerializedNamedStateReader::Instance
    IL_004c: ldc.i4.0
    IL_004d: conv.i
    IL_004e: callvirt instance void [UnityEngine]UnityEngine.ISerializedNamedStateReader::BeginMetaGroup(native int)
    IL_0053: ldarg.1
    IL_0054: ldc.i4.1
    IL_0055: add
    IL_0056: ldarg.2
    IL_0057: callvirt instance void ['Assembly-CSharp.dll']Derived::Unity_NamedDeserialize(int32, bool)
    IL_005c: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateReader [UnityEngine]UnityEngine.Serialization.SerializedNamedStateReader::Instance
    IL_0061: callvirt instance void [UnityEngine]UnityEngine.ISerializedNamedStateReader::EndMetaGroup()
    IL_0066: ldloc.1
    IL_0067: ldc.i4.1
    IL_0068: add
    IL_0069: stloc.1

    IL_006a: nop
    IL_006b: ldloc.1
    IL_006c: ldarg.0
    IL_006d: ldfld class Derived[] ArraysToObjectsSerializedProperly::derivees
    IL_0072: ldlen
    IL_0073: conv.i4
    IL_0074: clt
    IL_0076: brtrue.s IL_002d
// end loop

IL_0078: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateReader [UnityEngine]UnityEngine.Serialization.SerializedNamedStateReader::Instance
IL_007d: callvirt instance void [UnityEngine]UnityEngine.ISerializedNamedStateReader::EndMetaGroup()

IL_0082: nop
IL_0083: ret


