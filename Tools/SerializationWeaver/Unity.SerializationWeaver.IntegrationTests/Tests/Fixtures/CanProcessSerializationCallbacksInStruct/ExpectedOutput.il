IL For method: System.Void ExplicitSerializationCallbackInStruct::OnBeforeSerialize()
// Method begins at RVA 0x206c
// Code size 12 (0xc)
.maxstack 8

IL_0000: ldarg.0
IL_0001: ldstr "Two"
IL_0006: stfld string ExplicitSerializationCallbackInStruct::mystring
IL_000b: ret


IL For method: System.Void ExplicitSerializationCallbackInStruct::OnAfterDeserialize()
// Method begins at RVA 0x207c
// Code size 1 (0x1)
.maxstack 8

IL_0000: ret


IL For method: System.Void ExplicitSerializationCallbackInStruct::.ctor(UnityEngine.IUnitySerializable)
// Method begins at RVA 0x2080
// Code size 1 (0x1)
.maxstack 8

IL_0000: ret


IL For method: System.Void ExplicitSerializationCallbackInStruct::Unity_Serialize(System.Int32,System.Boolean)
// Method begins at RVA 0x2084
// Code size 27 (0x1b)
.maxstack 8

IL_0000: ldarg.2
IL_0001: brtrue.s IL_0009

IL_0003: ldarg.0
IL_0004: call instance void ExplicitSerializationCallbackInStruct::OnBeforeSerialize()

IL_0009: nop
IL_000a: ldsfld class [UnityEngine]UnityEngine.ISerializedStateWriter [UnityEngine]UnityEngine.Serialization.SerializedStateWriter::Instance
IL_000f: ldarg.0
IL_0010: ldfld string ExplicitSerializationCallbackInStruct::mystring
IL_0015: callvirt instance void [UnityEngine]UnityEngine.ISerializedStateWriter::WriteString(string)
IL_001a: ret


IL For method: System.Void ExplicitSerializationCallbackInStruct::Unity_Deserialize(System.Int32,System.Boolean)
// Method begins at RVA 0x20a0
// Code size 32 (0x20)
.maxstack 8

IL_0000: ldarg.0
IL_0001: ldsfld class [UnityEngine]UnityEngine.ISerializedStateReader [UnityEngine]UnityEngine.Serialization.SerializedStateReader::Instance
IL_0006: callvirt instance string [UnityEngine]UnityEngine.ISerializedStateReader::ReadString()
IL_000b: isinst [mscorlib]System.String
IL_0010: stfld string ExplicitSerializationCallbackInStruct::mystring
IL_0015: ldarg.2
IL_0016: brtrue.s IL_001e

IL_0018: ldarg.0
IL_0019: call instance void ExplicitSerializationCallbackInStruct::OnAfterDeserialize()

IL_001e: nop
IL_001f: ret


IL For method: System.Void ExplicitSerializationCallbackInStruct::Unity_RemapPPtrs(System.Int32,System.Boolean)
// Method begins at RVA 0x20c4
// Code size 42 (0x2a)
.maxstack 8

IL_0000: ldsfld class [UnityEngine]UnityEngine.IPPtrRemapper [UnityEngine]UnityEngine.Serialization.PPtrRemapper::Instance
IL_0005: callvirt instance bool [UnityEngine]UnityEngine.IPPtrRemapper::CanExecuteSerializationCallbacks()
IL_000a: brfalse.s IL_0015

IL_000c: ldarg.2
IL_000d: brtrue.s IL_0015

IL_000f: ldarg.0
IL_0010: call instance void ExplicitSerializationCallbackInStruct::OnBeforeSerialize()

IL_0015: nop
IL_0016: ldsfld class [UnityEngine]UnityEngine.IPPtrRemapper [UnityEngine]UnityEngine.Serialization.PPtrRemapper::Instance
IL_001b: callvirt instance bool [UnityEngine]UnityEngine.IPPtrRemapper::CanExecuteSerializationCallbacks()
IL_0020: brfalse.s IL_0028

IL_0022: ldarg.0
IL_0023: call instance void ExplicitSerializationCallbackInStruct::OnAfterDeserialize()

IL_0028: nop
IL_0029: ret


IL For method: System.Void ExplicitSerializationCallbackInStruct::Unity_NamedSerialize(System.Int32,System.Boolean)
// Method begins at RVA 0x20f0
// Code size 41 (0x29)
.maxstack 3
.locals init (
    [0] uint8& pinned
)

IL_0000: ldsfld uint8[] UnityEngine.Internal.$FieldNamesStorage::$RuntimeNames
IL_0005: ldc.i4.0
IL_0006: ldelema [mscorlib]System.Byte
IL_000b: stloc.0
IL_000c: ldarg.2
IL_000d: brtrue.s IL_0015

IL_000f: ldarg.0
IL_0010: call instance void ExplicitSerializationCallbackInStruct::OnBeforeSerialize()

IL_0015: nop
IL_0016: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateWriter [UnityEngine]UnityEngine.Serialization.SerializedNamedStateWriter::Instance
IL_001b: ldarg.0
IL_001c: ldfld string ExplicitSerializationCallbackInStruct::mystring
IL_0021: ldloc.0
IL_0022: conv.i
IL_0023: callvirt instance void [UnityEngine]UnityEngine.ISerializedNamedStateWriter::WriteString(string, native int)
IL_0028: ret


IL For method: System.Void ExplicitSerializationCallbackInStruct::Unity_NamedDeserialize(System.Int32,System.Boolean)
// Method begins at RVA 0x2128
// Code size 46 (0x2e)
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
IL_0014: callvirt instance string [UnityEngine]UnityEngine.ISerializedNamedStateReader::ReadString(native int)
IL_0019: isinst [mscorlib]System.String
IL_001e: stfld string ExplicitSerializationCallbackInStruct::mystring
IL_0023: ldarg.2
IL_0024: brtrue.s IL_002c

IL_0026: ldarg.0
IL_0027: call instance void ExplicitSerializationCallbackInStruct::OnAfterDeserialize()

IL_002c: nop
IL_002d: ret


