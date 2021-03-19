IL For method: System.Void CanProcessListOfStructs::.ctor()
// Method begins at RVA 0x2068
// Code size 7 (0x7)
.maxstack 8

IL_0000: ldarg.0
IL_0001: call instance void [UnityEngine]UnityEngine.MonoBehaviour::.ctor()
IL_0006: ret


IL For method: System.Void CanProcessListOfStructs::Unity_Serialize(System.Int32,System.Boolean)
// Method begins at RVA 0x2070
// Code size 101 (0x65)
.maxstack 3
.locals init (
    [0] int32,
    [1] valuetype CanProcessListOfStructs/TestStruct
)

IL_0000: ldarg.1
IL_0001: ldc.i4.7
IL_0002: bgt.s IL_0063

IL_0004: ldarg.0
IL_0005: ldfld class [mscorlib]System.Collections.Generic.List`1<valuetype CanProcessListOfStructs/TestStruct> CanProcessListOfStructs::list
IL_000a: brtrue.s IL_0019

IL_000c: ldsfld class [UnityEngine]UnityEngine.ISerializedStateWriter [UnityEngine]UnityEngine.Serialization.SerializedStateWriter::Instance
IL_0011: ldc.i4.0
IL_0012: callvirt instance void [UnityEngine]UnityEngine.ISerializedStateWriter::WriteInt32(int32)
IL_0017: br.s IL_0062

IL_0019: nop
IL_001a: ldsfld class [UnityEngine]UnityEngine.ISerializedStateWriter [UnityEngine]UnityEngine.Serialization.SerializedStateWriter::Instance
IL_001f: ldarg.0
IL_0020: ldfld class [mscorlib]System.Collections.Generic.List`1<valuetype CanProcessListOfStructs/TestStruct> CanProcessListOfStructs::list
IL_0025: call instance int32 class [mscorlib]System.Collections.Generic.List`1<valuetype CanProcessListOfStructs/TestStruct>::get_Count()
IL_002a: callvirt instance void [UnityEngine]UnityEngine.ISerializedStateWriter::WriteInt32(int32)
IL_002f: ldc.i4.0
IL_0030: stloc.0
IL_0031: br.s IL_0050
// loop start (head: IL_0050)
    IL_0033: nop
    IL_0034: ldarg.0
    IL_0035: ldfld class [mscorlib]System.Collections.Generic.List`1<valuetype CanProcessListOfStructs/TestStruct> CanProcessListOfStructs::list
    IL_003a: ldloc.0
    IL_003b: call instance !0 class [mscorlib]System.Collections.Generic.List`1<valuetype CanProcessListOfStructs/TestStruct>::get_Item(int32)
    IL_0040: stloc.1
    IL_0041: ldloca.s 1
    IL_0043: ldarg.1
    IL_0044: ldc.i4.1
    IL_0045: add
    IL_0046: ldarg.2
    IL_0047: call instance void ['Assembly-CSharp.dll']CanProcessListOfStructs/TestStruct::Unity_Serialize(int32, bool)
    IL_004c: ldloc.0
    IL_004d: ldc.i4.1
    IL_004e: add
    IL_004f: stloc.0

    IL_0050: nop
    IL_0051: ldloc.0
    IL_0052: ldarg.0
    IL_0053: ldfld class [mscorlib]System.Collections.Generic.List`1<valuetype CanProcessListOfStructs/TestStruct> CanProcessListOfStructs::list
    IL_0058: call instance int32 class [mscorlib]System.Collections.Generic.List`1<valuetype CanProcessListOfStructs/TestStruct>::get_Count()
    IL_005d: conv.i4
    IL_005e: clt
    IL_0060: brtrue.s IL_0033
// end loop

IL_0062: nop

IL_0063: nop
IL_0064: ret


IL For method: System.Void CanProcessListOfStructs::Unity_Deserialize(System.Int32,System.Boolean)
// Method begins at RVA 0x20e4
// Code size 77 (0x4d)
.maxstack 3
.locals init (
    [0] int32,
    [1] int32,
    [2] valuetype CanProcessListOfStructs/TestStruct
)

IL_0000: ldarg.1
IL_0001: ldc.i4.7
IL_0002: bgt.s IL_004b

IL_0004: ldarg.0
IL_0005: ldsfld class [UnityEngine]UnityEngine.ISerializedStateReader [UnityEngine]UnityEngine.Serialization.SerializedStateReader::Instance
IL_000a: callvirt instance int32 [UnityEngine]UnityEngine.ISerializedStateReader::ReadInt32()
IL_000f: stloc.0
IL_0010: ldloc.0
IL_0011: newobj instance void class [mscorlib]System.Collections.Generic.List`1<valuetype CanProcessListOfStructs/TestStruct>::.ctor(int32)
IL_0016: stfld class [mscorlib]System.Collections.Generic.List`1<valuetype CanProcessListOfStructs/TestStruct> CanProcessListOfStructs::list
IL_001b: ldc.i4.0
IL_001c: stloc.1
IL_001d: br.s IL_0043
// loop start (head: IL_0043)
    IL_001f: nop
    IL_0020: ldloca.s 2
    IL_0022: initobj CanProcessListOfStructs/TestStruct
    IL_0028: ldloca.s 2
    IL_002a: ldarg.1
    IL_002b: ldc.i4.1
    IL_002c: add
    IL_002d: ldarg.2
    IL_002e: call instance void ['Assembly-CSharp.dll']CanProcessListOfStructs/TestStruct::Unity_Deserialize(int32, bool)
    IL_0033: ldarg.0
    IL_0034: ldfld class [mscorlib]System.Collections.Generic.List`1<valuetype CanProcessListOfStructs/TestStruct> CanProcessListOfStructs::list
    IL_0039: ldloc.2
    IL_003a: callvirt instance void class [mscorlib]System.Collections.Generic.List`1<valuetype CanProcessListOfStructs/TestStruct>::Add(!0)
    IL_003f: ldloc.1
    IL_0040: ldc.i4.1
    IL_0041: add
    IL_0042: stloc.1

    IL_0043: nop
    IL_0044: ldloc.1
    IL_0045: ldloc.0
    IL_0046: conv.i4
    IL_0047: clt
    IL_0049: brtrue.s IL_001f
// end loop

IL_004b: nop
IL_004c: ret


IL For method: System.Void CanProcessListOfStructs::Unity_RemapPPtrs(System.Int32,System.Boolean)
// Method begins at RVA 0x2140
// Code size 80 (0x50)
.maxstack 3
.locals init (
    [0] int32,
    [1] valuetype CanProcessListOfStructs/TestStruct
)

IL_0000: ldarg.1
IL_0001: ldc.i4.7
IL_0002: bgt.s IL_004e

IL_0004: ldarg.0
IL_0005: ldfld class [mscorlib]System.Collections.Generic.List`1<valuetype CanProcessListOfStructs/TestStruct> CanProcessListOfStructs::list
IL_000a: brfalse.s IL_004d

IL_000c: ldc.i4.0
IL_000d: stloc.0
IL_000e: br.s IL_003b
// loop start (head: IL_003b)
    IL_0010: nop
    IL_0011: ldarg.0
    IL_0012: ldfld class [mscorlib]System.Collections.Generic.List`1<valuetype CanProcessListOfStructs/TestStruct> CanProcessListOfStructs::list
    IL_0017: ldloc.0
    IL_0018: callvirt instance !0 class [mscorlib]System.Collections.Generic.List`1<valuetype CanProcessListOfStructs/TestStruct>::get_Item(int32)
    IL_001d: stloc.1
    IL_001e: ldloca.s 1
    IL_0020: ldarg.1
    IL_0021: ldc.i4.1
    IL_0022: add
    IL_0023: ldarg.2
    IL_0024: call instance void ['Assembly-CSharp.dll']CanProcessListOfStructs/TestStruct::Unity_RemapPPtrs(int32, bool)
    IL_0029: ldarg.0
    IL_002a: ldfld class [mscorlib]System.Collections.Generic.List`1<valuetype CanProcessListOfStructs/TestStruct> CanProcessListOfStructs::list
    IL_002f: ldloc.0
    IL_0030: ldloc.1
    IL_0031: callvirt instance void class [mscorlib]System.Collections.Generic.List`1<valuetype CanProcessListOfStructs/TestStruct>::set_Item(int32, !0)
    IL_0036: nop
    IL_0037: ldloc.0
    IL_0038: ldc.i4.1
    IL_0039: add
    IL_003a: stloc.0

    IL_003b: nop
    IL_003c: ldloc.0
    IL_003d: ldarg.0
    IL_003e: ldfld class [mscorlib]System.Collections.Generic.List`1<valuetype CanProcessListOfStructs/TestStruct> CanProcessListOfStructs::list
    IL_0043: call instance int32 class [mscorlib]System.Collections.Generic.List`1<valuetype CanProcessListOfStructs/TestStruct>::get_Count()
    IL_0048: conv.i4
    IL_0049: clt
    IL_004b: brtrue.s IL_0010
// end loop

IL_004d: nop

IL_004e: nop
IL_004f: ret


IL For method: System.Void CanProcessListOfStructs::Unity_NamedSerialize(System.Int32,System.Boolean)
// Method begins at RVA 0x219c
// Code size 166 (0xa6)
.maxstack 3
.locals init (
    [0] uint8& pinned,
    [1] int32,
    [2] valuetype CanProcessListOfStructs/TestStruct
)

IL_0000: ldsfld uint8[] UnityEngine.Internal.$FieldNamesStorage::$RuntimeNames
IL_0005: ldc.i4.0
IL_0006: ldelema [mscorlib]System.Byte
IL_000b: stloc.0
IL_000c: ldarg.1
IL_000d: ldc.i4.7
IL_000e: bgt IL_00a4

IL_0013: ldarg.0
IL_0014: ldfld class [mscorlib]System.Collections.Generic.List`1<valuetype CanProcessListOfStructs/TestStruct> CanProcessListOfStructs::list
IL_0019: brtrue.s IL_0036

IL_001b: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateWriter [UnityEngine]UnityEngine.Serialization.SerializedNamedStateWriter::Instance
IL_0020: ldloc.0
IL_0021: conv.i
IL_0022: ldc.i4.2
IL_0023: add
IL_0024: ldc.i4.0
IL_0025: callvirt instance void [UnityEngine]UnityEngine.ISerializedNamedStateWriter::BeginSequenceGroup(native int, int32)
IL_002a: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateWriter [UnityEngine]UnityEngine.Serialization.SerializedNamedStateWriter::Instance
IL_002f: callvirt instance void [UnityEngine]UnityEngine.ISerializedNamedStateWriter::EndMetaGroup()
IL_0034: br.s IL_00a3

IL_0036: nop
IL_0037: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateWriter [UnityEngine]UnityEngine.Serialization.SerializedNamedStateWriter::Instance
IL_003c: ldloc.0
IL_003d: conv.i
IL_003e: ldc.i4.2
IL_003f: add
IL_0040: ldarg.0
IL_0041: ldfld class [mscorlib]System.Collections.Generic.List`1<valuetype CanProcessListOfStructs/TestStruct> CanProcessListOfStructs::list
IL_0046: call instance int32 class [mscorlib]System.Collections.Generic.List`1<valuetype CanProcessListOfStructs/TestStruct>::get_Count()
IL_004b: callvirt instance void [UnityEngine]UnityEngine.ISerializedNamedStateWriter::BeginSequenceGroup(native int, int32)
IL_0050: ldc.i4.0
IL_0051: stloc.1
IL_0052: br.s IL_0087
// loop start (head: IL_0087)
    IL_0054: nop
    IL_0055: ldarg.0
    IL_0056: ldfld class [mscorlib]System.Collections.Generic.List`1<valuetype CanProcessListOfStructs/TestStruct> CanProcessListOfStructs::list
    IL_005b: ldloc.1
    IL_005c: call instance !0 class [mscorlib]System.Collections.Generic.List`1<valuetype CanProcessListOfStructs/TestStruct>::get_Item(int32)
    IL_0061: stloc.2
    IL_0062: ldloca.s 2
    IL_0064: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateWriter [UnityEngine]UnityEngine.Serialization.SerializedNamedStateWriter::Instance
    IL_0069: ldc.i4.0
    IL_006a: conv.i
    IL_006b: callvirt instance void [UnityEngine]UnityEngine.ISerializedNamedStateWriter::BeginMetaGroup(native int)
    IL_0070: ldarg.1
    IL_0071: ldc.i4.1
    IL_0072: add
    IL_0073: ldarg.2
    IL_0074: call instance void ['Assembly-CSharp.dll']CanProcessListOfStructs/TestStruct::Unity_NamedSerialize(int32, bool)
    IL_0079: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateWriter [UnityEngine]UnityEngine.Serialization.SerializedNamedStateWriter::Instance
    IL_007e: callvirt instance void [UnityEngine]UnityEngine.ISerializedNamedStateWriter::EndMetaGroup()
    IL_0083: ldloc.1
    IL_0084: ldc.i4.1
    IL_0085: add
    IL_0086: stloc.1

    IL_0087: nop
    IL_0088: ldloc.1
    IL_0089: ldarg.0
    IL_008a: ldfld class [mscorlib]System.Collections.Generic.List`1<valuetype CanProcessListOfStructs/TestStruct> CanProcessListOfStructs::list
    IL_008f: call instance int32 class [mscorlib]System.Collections.Generic.List`1<valuetype CanProcessListOfStructs/TestStruct>::get_Count()
    IL_0094: conv.i4
    IL_0095: clt
    IL_0097: brtrue.s IL_0054
// end loop

IL_0099: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateWriter [UnityEngine]UnityEngine.Serialization.SerializedNamedStateWriter::Instance
IL_009e: callvirt instance void [UnityEngine]UnityEngine.ISerializedNamedStateWriter::EndMetaGroup()

IL_00a3: nop

IL_00a4: nop
IL_00a5: ret


IL For method: System.Void CanProcessListOfStructs::Unity_NamedDeserialize(System.Int32,System.Boolean)
// Method begins at RVA 0x2250
// Code size 125 (0x7d)
.maxstack 4
.locals init (
    [0] uint8& pinned,
    [1] int32,
    [2] int32,
    [3] valuetype CanProcessListOfStructs/TestStruct
)

IL_0000: ldsfld uint8[] UnityEngine.Internal.$FieldNamesStorage::$RuntimeNames
IL_0005: ldc.i4.0
IL_0006: ldelema [mscorlib]System.Byte
IL_000b: stloc.0
IL_000c: ldarg.1
IL_000d: ldc.i4.7
IL_000e: bgt.s IL_007b

IL_0010: ldarg.0
IL_0011: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateReader [UnityEngine]UnityEngine.Serialization.SerializedNamedStateReader::Instance
IL_0016: ldloc.0
IL_0017: conv.i
IL_0018: ldc.i4.2
IL_0019: add
IL_001a: callvirt instance int32 [UnityEngine]UnityEngine.ISerializedNamedStateReader::BeginSequenceGroup(native int)
IL_001f: stloc.1
IL_0020: ldloc.1
IL_0021: newobj instance void class [mscorlib]System.Collections.Generic.List`1<valuetype CanProcessListOfStructs/TestStruct>::.ctor(int32)
IL_0026: stfld class [mscorlib]System.Collections.Generic.List`1<valuetype CanProcessListOfStructs/TestStruct> CanProcessListOfStructs::list
IL_002b: ldc.i4.0
IL_002c: stloc.2
IL_002d: br.s IL_0069
// loop start (head: IL_0069)
    IL_002f: nop
    IL_0030: ldloca.s 3
    IL_0032: initobj CanProcessListOfStructs/TestStruct
    IL_0038: ldloca.s 3
    IL_003a: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateReader [UnityEngine]UnityEngine.Serialization.SerializedNamedStateReader::Instance
    IL_003f: ldc.i4.0
    IL_0040: conv.i
    IL_0041: callvirt instance void [UnityEngine]UnityEngine.ISerializedNamedStateReader::BeginMetaGroup(native int)
    IL_0046: ldarg.1
    IL_0047: ldc.i4.1
    IL_0048: add
    IL_0049: ldarg.2
    IL_004a: call instance void ['Assembly-CSharp.dll']CanProcessListOfStructs/TestStruct::Unity_NamedDeserialize(int32, bool)
    IL_004f: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateReader [UnityEngine]UnityEngine.Serialization.SerializedNamedStateReader::Instance
    IL_0054: callvirt instance void [UnityEngine]UnityEngine.ISerializedNamedStateReader::EndMetaGroup()
    IL_0059: ldarg.0
    IL_005a: ldfld class [mscorlib]System.Collections.Generic.List`1<valuetype CanProcessListOfStructs/TestStruct> CanProcessListOfStructs::list
    IL_005f: ldloc.3
    IL_0060: callvirt instance void class [mscorlib]System.Collections.Generic.List`1<valuetype CanProcessListOfStructs/TestStruct>::Add(!0)
    IL_0065: ldloc.2
    IL_0066: ldc.i4.1
    IL_0067: add
    IL_0068: stloc.2

    IL_0069: nop
    IL_006a: ldloc.2
    IL_006b: ldloc.1
    IL_006c: conv.i4
    IL_006d: clt
    IL_006f: brtrue.s IL_002f
// end loop

IL_0071: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateReader [UnityEngine]UnityEngine.Serialization.SerializedNamedStateReader::Instance
IL_0076: callvirt instance void [UnityEngine]UnityEngine.ISerializedNamedStateReader::EndMetaGroup()

IL_007b: nop
IL_007c: ret


IL For method: System.Void CanProcessListOfStructs/TestStruct::.ctor(UnityEngine.IUnitySerializable)
// Method begins at RVA 0x22dc
// Code size 1 (0x1)
.maxstack 8

IL_0000: ret


IL For method: System.Void CanProcessListOfStructs/TestStruct::Unity_Serialize(System.Int32,System.Boolean)
// Method begins at RVA 0x22e0
// Code size 22 (0x16)
.maxstack 8

IL_0000: ldarg.1
IL_0001: ldc.i4.7
IL_0002: bgt.s IL_0014

IL_0004: ldsfld class [UnityEngine]UnityEngine.ISerializedStateWriter [UnityEngine]UnityEngine.Serialization.SerializedStateWriter::Instance
IL_0009: ldarg.0
IL_000a: ldfld class [UnityEngine]UnityEngine.Transform CanProcessListOfStructs/TestStruct::t
IL_000f: callvirt instance void [UnityEngine]UnityEngine.ISerializedStateWriter::WriteUnityEngineObject(object)

IL_0014: nop
IL_0015: ret


IL For method: System.Void CanProcessListOfStructs/TestStruct::Unity_Deserialize(System.Int32,System.Boolean)
// Method begins at RVA 0x22f8
// Code size 27 (0x1b)
.maxstack 8

IL_0000: ldarg.1
IL_0001: ldc.i4.7
IL_0002: bgt.s IL_0019

IL_0004: ldarg.0
IL_0005: ldsfld class [UnityEngine]UnityEngine.ISerializedStateReader [UnityEngine]UnityEngine.Serialization.SerializedStateReader::Instance
IL_000a: callvirt instance object [UnityEngine]UnityEngine.ISerializedStateReader::ReadUnityEngineObject()
IL_000f: isinst [UnityEngine]UnityEngine.Transform
IL_0014: stfld class [UnityEngine]UnityEngine.Transform CanProcessListOfStructs/TestStruct::t

IL_0019: nop
IL_001a: ret


IL For method: System.Void CanProcessListOfStructs/TestStruct::Unity_RemapPPtrs(System.Int32,System.Boolean)
// Method begins at RVA 0x2314
// Code size 37 (0x25)
.maxstack 8

IL_0000: ldarg.0
IL_0001: ldfld class [UnityEngine]UnityEngine.Transform CanProcessListOfStructs/TestStruct::t
IL_0006: brfalse.s IL_0023

IL_0008: ldarg.0
IL_0009: ldsfld class [UnityEngine]UnityEngine.IPPtrRemapper [UnityEngine]UnityEngine.Serialization.PPtrRemapper::Instance
IL_000e: ldarg.0
IL_000f: ldfld class [UnityEngine]UnityEngine.Transform CanProcessListOfStructs/TestStruct::t
IL_0014: callvirt instance object [UnityEngine]UnityEngine.IPPtrRemapper::GetNewInstanceToReplaceOldInstance(object)
IL_0019: isinst [UnityEngine]UnityEngine.Transform
IL_001e: stfld class [UnityEngine]UnityEngine.Transform CanProcessListOfStructs/TestStruct::t

IL_0023: nop
IL_0024: ret


IL For method: System.Void CanProcessListOfStructs/TestStruct::Unity_NamedSerialize(System.Int32,System.Boolean)
// Method begins at RVA 0x233c
// Code size 36 (0x24)
.maxstack 3
.locals init (
    [0] uint8& pinned
)

IL_0000: ldsfld uint8[] UnityEngine.Internal.$FieldNamesStorage::$RuntimeNames
IL_0005: ldc.i4.0
IL_0006: ldelema [mscorlib]System.Byte
IL_000b: stloc.0
IL_000c: ldarg.1
IL_000d: ldc.i4.7
IL_000e: bgt.s IL_0022

IL_0010: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateWriter [UnityEngine]UnityEngine.Serialization.SerializedNamedStateWriter::Instance
IL_0015: ldarg.0
IL_0016: ldfld class [UnityEngine]UnityEngine.Transform CanProcessListOfStructs/TestStruct::t
IL_001b: ldloc.0
IL_001c: conv.i
IL_001d: callvirt instance void [UnityEngine]UnityEngine.ISerializedNamedStateWriter::WriteUnityEngineObject(object, native int)

IL_0022: nop
IL_0023: ret


IL For method: System.Void CanProcessListOfStructs/TestStruct::Unity_NamedDeserialize(System.Int32,System.Boolean)
// Method begins at RVA 0x236c
// Code size 41 (0x29)
.maxstack 3
.locals init (
    [0] uint8& pinned
)

IL_0000: ldsfld uint8[] UnityEngine.Internal.$FieldNamesStorage::$RuntimeNames
IL_0005: ldc.i4.0
IL_0006: ldelema [mscorlib]System.Byte
IL_000b: stloc.0
IL_000c: ldarg.1
IL_000d: ldc.i4.7
IL_000e: bgt.s IL_0027

IL_0010: ldarg.0
IL_0011: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateReader [UnityEngine]UnityEngine.Serialization.SerializedNamedStateReader::Instance
IL_0016: ldloc.0
IL_0017: conv.i
IL_0018: callvirt instance object [UnityEngine]UnityEngine.ISerializedNamedStateReader::ReadUnityEngineObject(native int)
IL_001d: isinst [UnityEngine]UnityEngine.Transform
IL_0022: stfld class [UnityEngine]UnityEngine.Transform CanProcessListOfStructs/TestStruct::t

IL_0027: nop
IL_0028: ret


