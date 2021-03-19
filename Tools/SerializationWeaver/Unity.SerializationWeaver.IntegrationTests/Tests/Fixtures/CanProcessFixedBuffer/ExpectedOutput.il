IL For method: System.Void FixedBuffer::.ctor(UnityEngine.IUnitySerializable)
// Method begins at RVA 0x206c
// Code size 1 (0x1)
.maxstack 8

IL_0000: ret


IL For method: System.Void FixedBuffer::Unity_Serialize(System.Int32,System.Boolean)
// Method begins at RVA 0x2070
// Code size 77 (0x4d)
.maxstack 3
.locals init (
    [0] int32& pinned,
    [1] int32,
    [2] int32
)

IL_0000: ldarg.1
IL_0001: ldc.i4.7
IL_0002: bgt.s IL_0041

IL_0004: ldsfld class [UnityEngine]UnityEngine.ISerializedStateWriter [UnityEngine]UnityEngine.Serialization.SerializedStateWriter::Instance
IL_0009: ldc.i4.s 28
IL_000b: callvirt instance void [UnityEngine]UnityEngine.ISerializedStateWriter::WriteInt32(int32)
IL_0010: ldarg.0
IL_0011: ldflda valuetype FixedBuffer/'<buffer>__FixedBuffer0' FixedBuffer::buffer
IL_0016: ldflda int32 FixedBuffer/'<buffer>__FixedBuffer0'::FixedElementField
IL_001b: stloc.0
IL_001c: ldc.i4.0
IL_001d: stloc.2
IL_001e: br.s IL_0038
// loop start (head: IL_0038)
    IL_0020: nop
    IL_0021: ldloc.0
    IL_0022: ldloc.2
    IL_0023: conv.i
    IL_0024: ldc.i4.4
    IL_0025: mul
    IL_0026: add
    IL_0027: ldind.i4
    IL_0028: stloc.1
    IL_0029: ldsfld class [UnityEngine]UnityEngine.ISerializedStateWriter [UnityEngine]UnityEngine.Serialization.SerializedStateWriter::Instance
    IL_002e: ldloc.1
    IL_002f: callvirt instance void [UnityEngine]UnityEngine.ISerializedStateWriter::WriteInt32(int32)
    IL_0034: ldloc.2
    IL_0035: ldc.i4.1
    IL_0036: add
    IL_0037: stloc.2

    IL_0038: nop
    IL_0039: ldloc.2
    IL_003a: ldc.i4.s 28
    IL_003c: conv.i4
    IL_003d: clt
    IL_003f: brtrue.s IL_0020
// end loop

IL_0041: nop
IL_0042: ldsfld class [UnityEngine]UnityEngine.ISerializedStateWriter [UnityEngine]UnityEngine.Serialization.SerializedStateWriter::Instance
IL_0047: callvirt instance void [UnityEngine]UnityEngine.ISerializedStateWriter::Align()
IL_004c: ret


IL For method: System.Void FixedBuffer::Unity_Deserialize(System.Int32,System.Boolean)
// Method begins at RVA 0x20cc
// Code size 76 (0x4c)
.maxstack 3
.locals init (
    [0] int32& pinned,
    [1] int32,
    [2] int32
)

IL_0000: ldarg.1
IL_0001: ldc.i4.7
IL_0002: bgt.s IL_0040

IL_0004: ldsfld class [UnityEngine]UnityEngine.ISerializedStateReader [UnityEngine]UnityEngine.Serialization.SerializedStateReader::Instance
IL_0009: callvirt instance int32 [UnityEngine]UnityEngine.ISerializedStateReader::ReadInt32()
IL_000e: pop
IL_000f: ldarg.0
IL_0010: ldflda valuetype FixedBuffer/'<buffer>__FixedBuffer0' FixedBuffer::buffer
IL_0015: ldflda int32 FixedBuffer/'<buffer>__FixedBuffer0'::FixedElementField
IL_001a: stloc.0
IL_001b: ldc.i4.0
IL_001c: stloc.2
IL_001d: br.s IL_0037
// loop start (head: IL_0037)
    IL_001f: nop
    IL_0020: ldsfld class [UnityEngine]UnityEngine.ISerializedStateReader [UnityEngine]UnityEngine.Serialization.SerializedStateReader::Instance
    IL_0025: callvirt instance int32 [UnityEngine]UnityEngine.ISerializedStateReader::ReadInt32()
    IL_002a: stloc.1
    IL_002b: ldloc.0
    IL_002c: ldloc.2
    IL_002d: conv.i
    IL_002e: ldc.i4.4
    IL_002f: mul
    IL_0030: add
    IL_0031: ldloc.1
    IL_0032: stind.i4
    IL_0033: ldloc.2
    IL_0034: ldc.i4.1
    IL_0035: add
    IL_0036: stloc.2

    IL_0037: nop
    IL_0038: ldloc.2
    IL_0039: ldc.i4.s 28
    IL_003b: conv.i4
    IL_003c: clt
    IL_003e: brtrue.s IL_001f
// end loop

IL_0040: nop
IL_0041: ldsfld class [UnityEngine]UnityEngine.ISerializedStateReader [UnityEngine]UnityEngine.Serialization.SerializedStateReader::Instance
IL_0046: callvirt instance void [UnityEngine]UnityEngine.ISerializedStateReader::Align()
IL_004b: ret


IL For method: System.Void FixedBuffer::Unity_RemapPPtrs(System.Int32,System.Boolean)
// Method begins at RVA 0x2124
// Code size 1 (0x1)
.maxstack 8

IL_0000: ret


IL For method: System.Void FixedBuffer::Unity_NamedSerialize(System.Int32,System.Boolean)
// Method begins at RVA 0x2128
// Code size 103 (0x67)
.maxstack 3
.locals init (
    [0] uint8& pinned,
    [1] int32& pinned,
    [2] int32,
    [3] int32
)

IL_0000: ldsfld uint8[] UnityEngine.Internal.$FieldNamesStorage::$RuntimeNames
IL_0005: ldc.i4.0
IL_0006: ldelema [mscorlib]System.Byte
IL_000b: stloc.0
IL_000c: ldarg.1
IL_000d: ldc.i4.7
IL_000e: bgt.s IL_005b

IL_0010: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateWriter [UnityEngine]UnityEngine.Serialization.SerializedNamedStateWriter::Instance
IL_0015: ldloc.0
IL_0016: conv.i
IL_0017: ldc.i4.s 28
IL_0019: callvirt instance void [UnityEngine]UnityEngine.ISerializedNamedStateWriter::BeginSequenceGroup(native int, int32)
IL_001e: ldarg.0
IL_001f: ldflda valuetype FixedBuffer/'<buffer>__FixedBuffer0' FixedBuffer::buffer
IL_0024: ldflda int32 FixedBuffer/'<buffer>__FixedBuffer0'::FixedElementField
IL_0029: stloc.1
IL_002a: ldc.i4.0
IL_002b: stloc.3
IL_002c: br.s IL_0048
// loop start (head: IL_0048)
    IL_002e: nop
    IL_002f: ldloc.1
    IL_0030: ldloc.3
    IL_0031: conv.i
    IL_0032: ldc.i4.4
    IL_0033: mul
    IL_0034: add
    IL_0035: ldind.i4
    IL_0036: stloc.2
    IL_0037: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateWriter [UnityEngine]UnityEngine.Serialization.SerializedNamedStateWriter::Instance
    IL_003c: ldloc.2
    IL_003d: ldc.i4.0
    IL_003e: conv.i
    IL_003f: callvirt instance void [UnityEngine]UnityEngine.ISerializedNamedStateWriter::WriteInt32(int32, native int)
    IL_0044: ldloc.3
    IL_0045: ldc.i4.1
    IL_0046: add
    IL_0047: stloc.3

    IL_0048: nop
    IL_0049: ldloc.3
    IL_004a: ldc.i4.s 28
    IL_004c: conv.i4
    IL_004d: clt
    IL_004f: brtrue.s IL_002e
// end loop

IL_0051: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateWriter [UnityEngine]UnityEngine.Serialization.SerializedNamedStateWriter::Instance
IL_0056: callvirt instance void [UnityEngine]UnityEngine.ISerializedNamedStateWriter::EndMetaGroup()

IL_005b: nop
IL_005c: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateWriter [UnityEngine]UnityEngine.Serialization.SerializedNamedStateWriter::Instance
IL_0061: callvirt instance void [UnityEngine]UnityEngine.ISerializedNamedStateWriter::Align()
IL_0066: ret


IL For method: System.Void FixedBuffer::Unity_NamedDeserialize(System.Int32,System.Boolean)
// Method begins at RVA 0x219c
// Code size 102 (0x66)
.maxstack 3
.locals init (
    [0] uint8& pinned,
    [1] int32& pinned,
    [2] int32,
    [3] int32
)

IL_0000: ldsfld uint8[] UnityEngine.Internal.$FieldNamesStorage::$RuntimeNames
IL_0005: ldc.i4.0
IL_0006: ldelema [mscorlib]System.Byte
IL_000b: stloc.0
IL_000c: ldarg.1
IL_000d: ldc.i4.7
IL_000e: bgt.s IL_005a

IL_0010: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateReader [UnityEngine]UnityEngine.Serialization.SerializedNamedStateReader::Instance
IL_0015: ldloc.0
IL_0016: conv.i
IL_0017: callvirt instance int32 [UnityEngine]UnityEngine.ISerializedNamedStateReader::BeginSequenceGroup(native int)
IL_001c: pop
IL_001d: ldarg.0
IL_001e: ldflda valuetype FixedBuffer/'<buffer>__FixedBuffer0' FixedBuffer::buffer
IL_0023: ldflda int32 FixedBuffer/'<buffer>__FixedBuffer0'::FixedElementField
IL_0028: stloc.1
IL_0029: ldc.i4.0
IL_002a: stloc.3
IL_002b: br.s IL_0047
// loop start (head: IL_0047)
    IL_002d: nop
    IL_002e: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateReader [UnityEngine]UnityEngine.Serialization.SerializedNamedStateReader::Instance
    IL_0033: ldc.i4.0
    IL_0034: conv.i
    IL_0035: callvirt instance int32 [UnityEngine]UnityEngine.ISerializedNamedStateReader::ReadInt32(native int)
    IL_003a: stloc.2
    IL_003b: ldloc.1
    IL_003c: ldloc.3
    IL_003d: conv.i
    IL_003e: ldc.i4.4
    IL_003f: mul
    IL_0040: add
    IL_0041: ldloc.2
    IL_0042: stind.i4
    IL_0043: ldloc.3
    IL_0044: ldc.i4.1
    IL_0045: add
    IL_0046: stloc.3

    IL_0047: nop
    IL_0048: ldloc.3
    IL_0049: ldc.i4.s 28
    IL_004b: conv.i4
    IL_004c: clt
    IL_004e: brtrue.s IL_002d
// end loop

IL_0050: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateReader [UnityEngine]UnityEngine.Serialization.SerializedNamedStateReader::Instance
IL_0055: callvirt instance void [UnityEngine]UnityEngine.ISerializedNamedStateReader::EndMetaGroup()

IL_005a: nop
IL_005b: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateReader [UnityEngine]UnityEngine.Serialization.SerializedNamedStateReader::Instance
IL_0060: callvirt instance void [UnityEngine]UnityEngine.ISerializedNamedStateReader::Align()
IL_0065: ret


IL For method: System.Void CanSerializeFixedBuffer::.ctor()
// Method begins at RVA 0x2210
// Code size 7 (0x7)
.maxstack 8

IL_0000: ldarg.0
IL_0001: call instance void [UnityEngine]UnityEngine.MonoBehaviour::.ctor()
IL_0006: ret


IL For method: System.Void CanSerializeFixedBuffer::Unity_Serialize(System.Int32,System.Boolean)
// Method begins at RVA 0x2218
// Code size 31 (0x1f)
.maxstack 8

IL_0000: ldarg.1
IL_0001: ldc.i4.7
IL_0002: bgt.s IL_0013

IL_0004: ldarg.0
IL_0005: ldflda valuetype FixedBuffer CanSerializeFixedBuffer::fixedBuffer
IL_000a: ldarg.1
IL_000b: ldc.i4.1
IL_000c: add
IL_000d: ldarg.2
IL_000e: call instance void ['Assembly-CSharp.dll']FixedBuffer::Unity_Serialize(int32, bool)

IL_0013: nop
IL_0014: ldsfld class [UnityEngine]UnityEngine.ISerializedStateWriter [UnityEngine]UnityEngine.Serialization.SerializedStateWriter::Instance
IL_0019: callvirt instance void [UnityEngine]UnityEngine.ISerializedStateWriter::Align()
IL_001e: ret


IL For method: System.Void CanSerializeFixedBuffer::Unity_Deserialize(System.Int32,System.Boolean)
// Method begins at RVA 0x2238
// Code size 31 (0x1f)
.maxstack 8

IL_0000: ldarg.1
IL_0001: ldc.i4.7
IL_0002: bgt.s IL_0013

IL_0004: ldarg.0
IL_0005: ldflda valuetype FixedBuffer CanSerializeFixedBuffer::fixedBuffer
IL_000a: ldarg.1
IL_000b: ldc.i4.1
IL_000c: add
IL_000d: ldarg.2
IL_000e: call instance void ['Assembly-CSharp.dll']FixedBuffer::Unity_Deserialize(int32, bool)

IL_0013: nop
IL_0014: ldsfld class [UnityEngine]UnityEngine.ISerializedStateReader [UnityEngine]UnityEngine.Serialization.SerializedStateReader::Instance
IL_0019: callvirt instance void [UnityEngine]UnityEngine.ISerializedStateReader::Align()
IL_001e: ret


IL For method: System.Void CanSerializeFixedBuffer::Unity_RemapPPtrs(System.Int32,System.Boolean)
// Method begins at RVA 0x2258
// Code size 21 (0x15)
.maxstack 8

IL_0000: ldarg.1
IL_0001: ldc.i4.7
IL_0002: bgt.s IL_0013

IL_0004: ldarg.0
IL_0005: ldflda valuetype FixedBuffer CanSerializeFixedBuffer::fixedBuffer
IL_000a: ldarg.1
IL_000b: ldc.i4.1
IL_000c: add
IL_000d: ldarg.2
IL_000e: call instance void ['Assembly-CSharp.dll']FixedBuffer::Unity_RemapPPtrs(int32, bool)

IL_0013: nop
IL_0014: ret


IL For method: System.Void CanSerializeFixedBuffer::Unity_NamedSerialize(System.Int32,System.Boolean)
// Method begins at RVA 0x2270
// Code size 67 (0x43)
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
IL_000e: bgt.s IL_0037

IL_0010: ldarg.0
IL_0011: ldflda valuetype FixedBuffer CanSerializeFixedBuffer::fixedBuffer
IL_0016: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateWriter [UnityEngine]UnityEngine.Serialization.SerializedNamedStateWriter::Instance
IL_001b: ldloc.0
IL_001c: conv.i
IL_001d: ldc.i4.7
IL_001e: add
IL_001f: callvirt instance void [UnityEngine]UnityEngine.ISerializedNamedStateWriter::BeginMetaGroup(native int)
IL_0024: ldarg.1
IL_0025: ldc.i4.1
IL_0026: add
IL_0027: ldarg.2
IL_0028: call instance void ['Assembly-CSharp.dll']FixedBuffer::Unity_NamedSerialize(int32, bool)
IL_002d: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateWriter [UnityEngine]UnityEngine.Serialization.SerializedNamedStateWriter::Instance
IL_0032: callvirt instance void [UnityEngine]UnityEngine.ISerializedNamedStateWriter::EndMetaGroup()

IL_0037: nop
IL_0038: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateWriter [UnityEngine]UnityEngine.Serialization.SerializedNamedStateWriter::Instance
IL_003d: callvirt instance void [UnityEngine]UnityEngine.ISerializedNamedStateWriter::Align()
IL_0042: ret


IL For method: System.Void CanSerializeFixedBuffer::Unity_NamedDeserialize(System.Int32,System.Boolean)
// Method begins at RVA 0x22c0
// Code size 67 (0x43)
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
IL_000e: bgt.s IL_0037

IL_0010: ldarg.0
IL_0011: ldflda valuetype FixedBuffer CanSerializeFixedBuffer::fixedBuffer
IL_0016: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateReader [UnityEngine]UnityEngine.Serialization.SerializedNamedStateReader::Instance
IL_001b: ldloc.0
IL_001c: conv.i
IL_001d: ldc.i4.7
IL_001e: add
IL_001f: callvirt instance void [UnityEngine]UnityEngine.ISerializedNamedStateReader::BeginMetaGroup(native int)
IL_0024: ldarg.1
IL_0025: ldc.i4.1
IL_0026: add
IL_0027: ldarg.2
IL_0028: call instance void ['Assembly-CSharp.dll']FixedBuffer::Unity_NamedDeserialize(int32, bool)
IL_002d: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateReader [UnityEngine]UnityEngine.Serialization.SerializedNamedStateReader::Instance
IL_0032: callvirt instance void [UnityEngine]UnityEngine.ISerializedNamedStateReader::EndMetaGroup()

IL_0037: nop
IL_0038: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateReader [UnityEngine]UnityEngine.Serialization.SerializedNamedStateReader::Instance
IL_003d: callvirt instance void [UnityEngine]UnityEngine.ISerializedNamedStateReader::Align()
IL_0042: ret


