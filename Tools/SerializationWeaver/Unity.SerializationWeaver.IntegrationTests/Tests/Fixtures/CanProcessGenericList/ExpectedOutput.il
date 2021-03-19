IL For method: System.Void SerializingEnumListWorksScript::.ctor()
// Method begins at RVA 0x206c
// Code size 7 (0x7)
.maxstack 8

IL_0000: ldarg.0
IL_0001: call instance void [UnityEngine]UnityEngine.MonoBehaviour::.ctor()
IL_0006: ret


IL For method: System.Void SerializingEnumListWorksScript::Start()
// Method begins at RVA 0x2074
// Code size 1 (0x1)
.maxstack 8

IL_0000: ret


IL For method: System.Void SerializingEnumListWorksScript::Unity_Serialize(System.Int32,System.Boolean)
// Method begins at RVA 0x2078
// Code size 99 (0x63)
.maxstack 3
.locals init (
    [0] int32
)

IL_0000: ldarg.1
IL_0001: ldc.i4.7
IL_0002: bgt.s IL_0061

IL_0004: ldarg.0
IL_0005: ldfld class [mscorlib]System.Collections.Generic.List`1<valuetype SerializingEnumListWorksScript/testEnum> SerializingEnumListWorksScript::enumList
IL_000a: brtrue.s IL_0019

IL_000c: ldsfld class [UnityEngine]UnityEngine.ISerializedStateWriter [UnityEngine]UnityEngine.Serialization.SerializedStateWriter::Instance
IL_0011: ldc.i4.0
IL_0012: callvirt instance void [UnityEngine]UnityEngine.ISerializedStateWriter::WriteInt32(int32)
IL_0017: br.s IL_0060

IL_0019: nop
IL_001a: ldsfld class [UnityEngine]UnityEngine.ISerializedStateWriter [UnityEngine]UnityEngine.Serialization.SerializedStateWriter::Instance
IL_001f: ldarg.0
IL_0020: ldfld class [mscorlib]System.Collections.Generic.List`1<valuetype SerializingEnumListWorksScript/testEnum> SerializingEnumListWorksScript::enumList
IL_0025: call instance int32 class [mscorlib]System.Collections.Generic.List`1<valuetype SerializingEnumListWorksScript/testEnum>::get_Count()
IL_002a: callvirt instance void [UnityEngine]UnityEngine.ISerializedStateWriter::WriteInt32(int32)
IL_002f: ldc.i4.0
IL_0030: stloc.0
IL_0031: br.s IL_004e
// loop start (head: IL_004e)
    IL_0033: nop
    IL_0034: ldsfld class [UnityEngine]UnityEngine.ISerializedStateWriter [UnityEngine]UnityEngine.Serialization.SerializedStateWriter::Instance
    IL_0039: ldarg.0
    IL_003a: ldfld class [mscorlib]System.Collections.Generic.List`1<valuetype SerializingEnumListWorksScript/testEnum> SerializingEnumListWorksScript::enumList
    IL_003f: ldloc.0
    IL_0040: call instance !0 class [mscorlib]System.Collections.Generic.List`1<valuetype SerializingEnumListWorksScript/testEnum>::get_Item(int32)
    IL_0045: callvirt instance void [UnityEngine]UnityEngine.ISerializedStateWriter::WriteInt32(int32)
    IL_004a: ldloc.0
    IL_004b: ldc.i4.1
    IL_004c: add
    IL_004d: stloc.0

    IL_004e: nop
    IL_004f: ldloc.0
    IL_0050: ldarg.0
    IL_0051: ldfld class [mscorlib]System.Collections.Generic.List`1<valuetype SerializingEnumListWorksScript/testEnum> SerializingEnumListWorksScript::enumList
    IL_0056: call instance int32 class [mscorlib]System.Collections.Generic.List`1<valuetype SerializingEnumListWorksScript/testEnum>::get_Count()
    IL_005b: conv.i4
    IL_005c: clt
    IL_005e: brtrue.s IL_0033
// end loop

IL_0060: nop

IL_0061: nop
IL_0062: ret


IL For method: System.Void SerializingEnumListWorksScript::Unity_Deserialize(System.Int32,System.Boolean)
// Method begins at RVA 0x20e8
// Code size 67 (0x43)
.maxstack 2
.locals init (
    [0] int32,
    [1] int32
)

IL_0000: ldarg.1
IL_0001: ldc.i4.7
IL_0002: bgt.s IL_0041

IL_0004: ldarg.0
IL_0005: ldsfld class [UnityEngine]UnityEngine.ISerializedStateReader [UnityEngine]UnityEngine.Serialization.SerializedStateReader::Instance
IL_000a: callvirt instance int32 [UnityEngine]UnityEngine.ISerializedStateReader::ReadInt32()
IL_000f: stloc.0
IL_0010: ldloc.0
IL_0011: newobj instance void class [mscorlib]System.Collections.Generic.List`1<valuetype SerializingEnumListWorksScript/testEnum>::.ctor(int32)
IL_0016: stfld class [mscorlib]System.Collections.Generic.List`1<valuetype SerializingEnumListWorksScript/testEnum> SerializingEnumListWorksScript::enumList
IL_001b: ldc.i4.0
IL_001c: stloc.1
IL_001d: br.s IL_0039
// loop start (head: IL_0039)
    IL_001f: nop
    IL_0020: ldarg.0
    IL_0021: ldfld class [mscorlib]System.Collections.Generic.List`1<valuetype SerializingEnumListWorksScript/testEnum> SerializingEnumListWorksScript::enumList
    IL_0026: ldsfld class [UnityEngine]UnityEngine.ISerializedStateReader [UnityEngine]UnityEngine.Serialization.SerializedStateReader::Instance
    IL_002b: callvirt instance int32 [UnityEngine]UnityEngine.ISerializedStateReader::ReadInt32()
    IL_0030: callvirt instance void class [mscorlib]System.Collections.Generic.List`1<valuetype SerializingEnumListWorksScript/testEnum>::Add(!0)
    IL_0035: ldloc.1
    IL_0036: ldc.i4.1
    IL_0037: add
    IL_0038: stloc.1

    IL_0039: nop
    IL_003a: ldloc.1
    IL_003b: ldloc.0
    IL_003c: conv.i4
    IL_003d: clt
    IL_003f: brtrue.s IL_001f
// end loop

IL_0041: nop
IL_0042: ret


IL For method: System.Void SerializingEnumListWorksScript::Unity_RemapPPtrs(System.Int32,System.Boolean)
// Method begins at RVA 0x2138
// Code size 1 (0x1)
.maxstack 8

IL_0000: ret


IL For method: System.Void SerializingEnumListWorksScript::Unity_NamedSerialize(System.Int32,System.Boolean)
// Method begins at RVA 0x213c
// Code size 137 (0x89)
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
IL_000e: bgt.s IL_0087

IL_0010: ldarg.0
IL_0011: ldfld class [mscorlib]System.Collections.Generic.List`1<valuetype SerializingEnumListWorksScript/testEnum> SerializingEnumListWorksScript::enumList
IL_0016: brtrue.s IL_0031

IL_0018: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateWriter [UnityEngine]UnityEngine.Serialization.SerializedNamedStateWriter::Instance
IL_001d: ldloc.0
IL_001e: conv.i
IL_001f: ldc.i4.0
IL_0020: callvirt instance void [UnityEngine]UnityEngine.ISerializedNamedStateWriter::BeginSequenceGroup(native int, int32)
IL_0025: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateWriter [UnityEngine]UnityEngine.Serialization.SerializedNamedStateWriter::Instance
IL_002a: callvirt instance void [UnityEngine]UnityEngine.ISerializedNamedStateWriter::EndMetaGroup()
IL_002f: br.s IL_0086

IL_0031: nop
IL_0032: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateWriter [UnityEngine]UnityEngine.Serialization.SerializedNamedStateWriter::Instance
IL_0037: ldloc.0
IL_0038: conv.i
IL_0039: ldarg.0
IL_003a: ldfld class [mscorlib]System.Collections.Generic.List`1<valuetype SerializingEnumListWorksScript/testEnum> SerializingEnumListWorksScript::enumList
IL_003f: call instance int32 class [mscorlib]System.Collections.Generic.List`1<valuetype SerializingEnumListWorksScript/testEnum>::get_Count()
IL_0044: callvirt instance void [UnityEngine]UnityEngine.ISerializedNamedStateWriter::BeginSequenceGroup(native int, int32)
IL_0049: ldc.i4.0
IL_004a: stloc.1
IL_004b: br.s IL_006a
// loop start (head: IL_006a)
    IL_004d: nop
    IL_004e: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateWriter [UnityEngine]UnityEngine.Serialization.SerializedNamedStateWriter::Instance
    IL_0053: ldarg.0
    IL_0054: ldfld class [mscorlib]System.Collections.Generic.List`1<valuetype SerializingEnumListWorksScript/testEnum> SerializingEnumListWorksScript::enumList
    IL_0059: ldloc.1
    IL_005a: call instance !0 class [mscorlib]System.Collections.Generic.List`1<valuetype SerializingEnumListWorksScript/testEnum>::get_Item(int32)
    IL_005f: ldc.i4.0
    IL_0060: conv.i
    IL_0061: callvirt instance void [UnityEngine]UnityEngine.ISerializedNamedStateWriter::WriteInt32(int32, native int)
    IL_0066: ldloc.1
    IL_0067: ldc.i4.1
    IL_0068: add
    IL_0069: stloc.1

    IL_006a: nop
    IL_006b: ldloc.1
    IL_006c: ldarg.0
    IL_006d: ldfld class [mscorlib]System.Collections.Generic.List`1<valuetype SerializingEnumListWorksScript/testEnum> SerializingEnumListWorksScript::enumList
    IL_0072: call instance int32 class [mscorlib]System.Collections.Generic.List`1<valuetype SerializingEnumListWorksScript/testEnum>::get_Count()
    IL_0077: conv.i4
    IL_0078: clt
    IL_007a: brtrue.s IL_004d
// end loop

IL_007c: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateWriter [UnityEngine]UnityEngine.Serialization.SerializedNamedStateWriter::Instance
IL_0081: callvirt instance void [UnityEngine]UnityEngine.ISerializedNamedStateWriter::EndMetaGroup()

IL_0086: nop

IL_0087: nop
IL_0088: ret


IL For method: System.Void SerializingEnumListWorksScript::Unity_NamedDeserialize(System.Int32,System.Boolean)
// Method begins at RVA 0x21d4
// Code size 93 (0x5d)
.maxstack 3
.locals init (
    [0] uint8& pinned,
    [1] int32,
    [2] int32
)

IL_0000: ldsfld uint8[] UnityEngine.Internal.$FieldNamesStorage::$RuntimeNames
IL_0005: ldc.i4.0
IL_0006: ldelema [mscorlib]System.Byte
IL_000b: stloc.0
IL_000c: ldarg.1
IL_000d: ldc.i4.7
IL_000e: bgt.s IL_005b

IL_0010: ldarg.0
IL_0011: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateReader [UnityEngine]UnityEngine.Serialization.SerializedNamedStateReader::Instance
IL_0016: ldloc.0
IL_0017: conv.i
IL_0018: callvirt instance int32 [UnityEngine]UnityEngine.ISerializedNamedStateReader::BeginSequenceGroup(native int)
IL_001d: stloc.1
IL_001e: ldloc.1
IL_001f: newobj instance void class [mscorlib]System.Collections.Generic.List`1<valuetype SerializingEnumListWorksScript/testEnum>::.ctor(int32)
IL_0024: stfld class [mscorlib]System.Collections.Generic.List`1<valuetype SerializingEnumListWorksScript/testEnum> SerializingEnumListWorksScript::enumList
IL_0029: ldc.i4.0
IL_002a: stloc.2
IL_002b: br.s IL_0049
// loop start (head: IL_0049)
    IL_002d: nop
    IL_002e: ldarg.0
    IL_002f: ldfld class [mscorlib]System.Collections.Generic.List`1<valuetype SerializingEnumListWorksScript/testEnum> SerializingEnumListWorksScript::enumList
    IL_0034: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateReader [UnityEngine]UnityEngine.Serialization.SerializedNamedStateReader::Instance
    IL_0039: ldc.i4.0
    IL_003a: conv.i
    IL_003b: callvirt instance int32 [UnityEngine]UnityEngine.ISerializedNamedStateReader::ReadInt32(native int)
    IL_0040: callvirt instance void class [mscorlib]System.Collections.Generic.List`1<valuetype SerializingEnumListWorksScript/testEnum>::Add(!0)
    IL_0045: ldloc.2
    IL_0046: ldc.i4.1
    IL_0047: add
    IL_0048: stloc.2

    IL_0049: nop
    IL_004a: ldloc.2
    IL_004b: ldloc.1
    IL_004c: conv.i4
    IL_004d: clt
    IL_004f: brtrue.s IL_002d
// end loop

IL_0051: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateReader [UnityEngine]UnityEngine.Serialization.SerializedNamedStateReader::Instance
IL_0056: callvirt instance void [UnityEngine]UnityEngine.ISerializedNamedStateReader::EndMetaGroup()

IL_005b: nop
IL_005c: ret


