IL For method: System.Void CanProcessPrivateOrInternalField::.ctor(System.Int32,System.Int32)
// Method begins at RVA 0x206c
// Code size 21 (0x15)
.maxstack 8

IL_0000: ldarg.0
IL_0001: call instance void [UnityEngine]UnityEngine.MonoBehaviour::.ctor()
IL_0006: ldarg.0
IL_0007: ldarg.1
IL_0008: stfld int32 CanProcessPrivateOrInternalField::privField
IL_000d: ldarg.0
IL_000e: ldarg.2
IL_000f: stfld int32 CanProcessPrivateOrInternalField::internField
IL_0014: ret


IL For method: System.Void CanProcessPrivateOrInternalField::.ctor(UnityEngine.IUnitySerializable)
// Method begins at RVA 0x2084
// Code size 7 (0x7)
.maxstack 8

IL_0000: ldarg.0
IL_0001: call instance void [UnityEngine]UnityEngine.MonoBehaviour::.ctor()
IL_0006: ret


IL For method: System.Void CanProcessPrivateOrInternalField::Unity_Serialize(System.Int32,System.Boolean)
// Method begins at RVA 0x208c
// Code size 33 (0x21)
.maxstack 8

IL_0000: ldsfld class [UnityEngine]UnityEngine.ISerializedStateWriter [UnityEngine]UnityEngine.Serialization.SerializedStateWriter::Instance
IL_0005: ldarg.0
IL_0006: ldfld int32 CanProcessPrivateOrInternalField::privField
IL_000b: callvirt instance void [UnityEngine]UnityEngine.ISerializedStateWriter::WriteInt32(int32)
IL_0010: ldsfld class [UnityEngine]UnityEngine.ISerializedStateWriter [UnityEngine]UnityEngine.Serialization.SerializedStateWriter::Instance
IL_0015: ldarg.0
IL_0016: ldfld int32 CanProcessPrivateOrInternalField::internField
IL_001b: callvirt instance void [UnityEngine]UnityEngine.ISerializedStateWriter::WriteInt32(int32)
IL_0020: ret


IL For method: System.Void CanProcessPrivateOrInternalField::Unity_Deserialize(System.Int32,System.Boolean)
// Method begins at RVA 0x20b0
// Code size 33 (0x21)
.maxstack 8

IL_0000: ldarg.0
IL_0001: ldsfld class [UnityEngine]UnityEngine.ISerializedStateReader [UnityEngine]UnityEngine.Serialization.SerializedStateReader::Instance
IL_0006: callvirt instance int32 [UnityEngine]UnityEngine.ISerializedStateReader::ReadInt32()
IL_000b: stfld int32 CanProcessPrivateOrInternalField::privField
IL_0010: ldarg.0
IL_0011: ldsfld class [UnityEngine]UnityEngine.ISerializedStateReader [UnityEngine]UnityEngine.Serialization.SerializedStateReader::Instance
IL_0016: callvirt instance int32 [UnityEngine]UnityEngine.ISerializedStateReader::ReadInt32()
IL_001b: stfld int32 CanProcessPrivateOrInternalField::internField
IL_0020: ret


IL For method: System.Void CanProcessPrivateOrInternalField::Unity_RemapPPtrs(System.Int32,System.Boolean)
// Method begins at RVA 0x20d4
// Code size 1 (0x1)
.maxstack 8

IL_0000: ret


IL For method: System.Void CanProcessPrivateOrInternalField::Unity_NamedSerialize(System.Int32,System.Boolean)
// Method begins at RVA 0x20d8
// Code size 52 (0x34)
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
IL_0012: ldfld int32 CanProcessPrivateOrInternalField::privField
IL_0017: ldloc.0
IL_0018: conv.i
IL_0019: callvirt instance void [UnityEngine]UnityEngine.ISerializedNamedStateWriter::WriteInt32(int32, native int)
IL_001e: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateWriter [UnityEngine]UnityEngine.Serialization.SerializedNamedStateWriter::Instance
IL_0023: ldarg.0
IL_0024: ldfld int32 CanProcessPrivateOrInternalField::internField
IL_0029: ldloc.0
IL_002a: conv.i
IL_002b: ldc.i4.s 10
IL_002d: add
IL_002e: callvirt instance void [UnityEngine]UnityEngine.ISerializedNamedStateWriter::WriteInt32(int32, native int)
IL_0033: ret


IL For method: System.Void CanProcessPrivateOrInternalField::Unity_NamedDeserialize(System.Int32,System.Boolean)
// Method begins at RVA 0x2118
// Code size 52 (0x34)
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
IL_0019: stfld int32 CanProcessPrivateOrInternalField::privField
IL_001e: ldarg.0
IL_001f: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateReader [UnityEngine]UnityEngine.Serialization.SerializedNamedStateReader::Instance
IL_0024: ldloc.0
IL_0025: conv.i
IL_0026: ldc.i4.s 10
IL_0028: add
IL_0029: callvirt instance int32 [UnityEngine]UnityEngine.ISerializedNamedStateReader::ReadInt32(native int)
IL_002e: stfld int32 CanProcessPrivateOrInternalField::internField
IL_0033: ret


