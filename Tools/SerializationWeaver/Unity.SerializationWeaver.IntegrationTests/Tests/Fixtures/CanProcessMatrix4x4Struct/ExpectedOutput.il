IL For method: System.Void CanSerializeStruct::.ctor()
// Method begins at RVA 0x206c
// Code size 7 (0x7)
.maxstack 8

IL_0000: ldarg.0
IL_0001: call instance void [UnityEngine]UnityEngine.MonoBehaviour::.ctor()
IL_0006: ret


IL For method: System.Void CanSerializeStruct::Unity_Serialize(System.Int32,System.Boolean)
// Method begins at RVA 0x2074
// Code size 27 (0x1b)
.maxstack 8

IL_0000: ldsfld class [UnityEngine]UnityEngine.ISerializedStateWriter [UnityEngine]UnityEngine.Serialization.SerializedStateWriter::Instance
IL_0005: ldarg.0
IL_0006: ldflda valuetype [UnityEngine]UnityEngine.Matrix4x4 CanSerializeStruct::myStruct
IL_000b: callvirt instance void [UnityEngine]UnityEngine.ISerializedStateWriter::WriteMatrix4x4(valuetype [UnityEngine]UnityEngine.Matrix4x4&)
IL_0010: ldsfld class [UnityEngine]UnityEngine.ISerializedStateWriter [UnityEngine]UnityEngine.Serialization.SerializedStateWriter::Instance
IL_0015: callvirt instance void [UnityEngine]UnityEngine.ISerializedStateWriter::Align()
IL_001a: ret


IL For method: System.Void CanSerializeStruct::Unity_Deserialize(System.Int32,System.Boolean)
// Method begins at RVA 0x2090
// Code size 27 (0x1b)
.maxstack 8

IL_0000: ldarg.0
IL_0001: ldsfld class [UnityEngine]UnityEngine.ISerializedStateReader [UnityEngine]UnityEngine.Serialization.SerializedStateReader::Instance
IL_0006: callvirt instance valuetype [UnityEngine]UnityEngine.Matrix4x4 [UnityEngine]UnityEngine.ISerializedStateReader::ReadMatrix4x4()
IL_000b: stfld valuetype [UnityEngine]UnityEngine.Matrix4x4 CanSerializeStruct::myStruct
IL_0010: ldsfld class [UnityEngine]UnityEngine.ISerializedStateReader [UnityEngine]UnityEngine.Serialization.SerializedStateReader::Instance
IL_0015: callvirt instance void [UnityEngine]UnityEngine.ISerializedStateReader::Align()
IL_001a: ret


IL For method: System.Void CanSerializeStruct::Unity_RemapPPtrs(System.Int32,System.Boolean)
// Method begins at RVA 0x20ac
// Code size 1 (0x1)
.maxstack 8

IL_0000: ret


IL For method: System.Void CanSerializeStruct::Unity_NamedSerialize(System.Int32,System.Boolean)
// Method begins at RVA 0x20b0
// Code size 41 (0x29)
.maxstack 3
.locals init (
    [0] uint8& pinned
)

IL_0000: ldsfld uint8[] UnityEngine.Internal.$FieldNamesStorage::$RuntimeNames
IL_0005: ldc.i4.0
IL_0006: ldelema [mscorlib]System.Byte
IL_000b: stloc.0
IL_000c: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateWriter [UnityEngine]UnityEngine.Serialization.SerializedNamedStateWriter::Instance
IL_0011: ldarg.0
IL_0012: ldflda valuetype [UnityEngine]UnityEngine.Matrix4x4 CanSerializeStruct::myStruct
IL_0017: ldloc.0
IL_0018: conv.i
IL_0019: callvirt instance void [UnityEngine]UnityEngine.ISerializedNamedStateWriter::WriteMatrix4x4(valuetype [UnityEngine]UnityEngine.Matrix4x4&, native int)
IL_001e: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateWriter [UnityEngine]UnityEngine.Serialization.SerializedNamedStateWriter::Instance
IL_0023: callvirt instance void [UnityEngine]UnityEngine.ISerializedNamedStateWriter::Align()
IL_0028: ret


IL For method: System.Void CanSerializeStruct::Unity_NamedDeserialize(System.Int32,System.Boolean)
// Method begins at RVA 0x20e8
// Code size 41 (0x29)
.maxstack 3
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
IL_0014: callvirt instance valuetype [UnityEngine]UnityEngine.Matrix4x4 [UnityEngine]UnityEngine.ISerializedNamedStateReader::ReadMatrix4x4(native int)
IL_0019: stfld valuetype [UnityEngine]UnityEngine.Matrix4x4 CanSerializeStruct::myStruct
IL_001e: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateReader [UnityEngine]UnityEngine.Serialization.SerializedNamedStateReader::Instance
IL_0023: callvirt instance void [UnityEngine]UnityEngine.ISerializedNamedStateReader::Align()
IL_0028: ret


