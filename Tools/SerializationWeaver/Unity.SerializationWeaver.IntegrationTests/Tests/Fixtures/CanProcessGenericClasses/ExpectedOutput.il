IL For method: System.Void CanSerializeGenericInstanceScript::.ctor()
// Method begins at RVA 0x206c
// Code size 7 (0x7)
.maxstack 8

IL_0000: ldarg.0
IL_0001: call instance void [UnityEngine]UnityEngine.MonoBehaviour::.ctor()
IL_0006: ret


IL For method: System.Void CanSerializeGenericInstanceScript::Start()
// Method begins at RVA 0x2074
// Code size 1 (0x1)
.maxstack 8

IL_0000: ret


IL For method: System.Void CanSerializeGenericInstanceScript::Unity_Serialize(System.Int32,System.Boolean)
// Method begins at RVA 0x2078
// Code size 41 (0x29)
.maxstack 8

IL_0000: ldarg.1
IL_0001: ldc.i4.7
IL_0002: bgt.s IL_0027

IL_0004: ldarg.0
IL_0005: ldfld class CanSerializeGenericInstanceScript/GenericClassOfString CanSerializeGenericInstanceScript::_genericOfString
IL_000a: brtrue.s IL_0017

IL_000c: ldarg.0
IL_000d: newobj instance void CanSerializeGenericInstanceScript/GenericClassOfString::.ctor()
IL_0012: stfld class CanSerializeGenericInstanceScript/GenericClassOfString CanSerializeGenericInstanceScript::_genericOfString

IL_0017: nop
IL_0018: ldarg.0
IL_0019: ldfld class CanSerializeGenericInstanceScript/GenericClassOfString CanSerializeGenericInstanceScript::_genericOfString
IL_001e: ldarg.1
IL_001f: ldc.i4.1
IL_0020: add
IL_0021: ldarg.2
IL_0022: callvirt instance void ['Assembly-CSharp.dll']CanSerializeGenericInstanceScript/GenericClassOfString::Unity_Serialize(int32, bool)

IL_0027: nop
IL_0028: ret


IL For method: System.Void CanSerializeGenericInstanceScript::Unity_Deserialize(System.Int32,System.Boolean)
// Method begins at RVA 0x20a4
// Code size 41 (0x29)
.maxstack 8

IL_0000: ldarg.1
IL_0001: ldc.i4.7
IL_0002: bgt.s IL_0027

IL_0004: ldarg.0
IL_0005: ldfld class CanSerializeGenericInstanceScript/GenericClassOfString CanSerializeGenericInstanceScript::_genericOfString
IL_000a: brtrue.s IL_0017

IL_000c: ldarg.0
IL_000d: newobj instance void CanSerializeGenericInstanceScript/GenericClassOfString::.ctor()
IL_0012: stfld class CanSerializeGenericInstanceScript/GenericClassOfString CanSerializeGenericInstanceScript::_genericOfString

IL_0017: nop
IL_0018: ldarg.0
IL_0019: ldfld class CanSerializeGenericInstanceScript/GenericClassOfString CanSerializeGenericInstanceScript::_genericOfString
IL_001e: ldarg.1
IL_001f: ldc.i4.1
IL_0020: add
IL_0021: ldarg.2
IL_0022: callvirt instance void ['Assembly-CSharp.dll']CanSerializeGenericInstanceScript/GenericClassOfString::Unity_Deserialize(int32, bool)

IL_0027: nop
IL_0028: ret


IL For method: System.Void CanSerializeGenericInstanceScript::Unity_RemapPPtrs(System.Int32,System.Boolean)
// Method begins at RVA 0x20d0
// Code size 30 (0x1e)
.maxstack 8

IL_0000: ldarg.1
IL_0001: ldc.i4.7
IL_0002: bgt.s IL_001c

IL_0004: ldarg.0
IL_0005: ldfld class CanSerializeGenericInstanceScript/GenericClassOfString CanSerializeGenericInstanceScript::_genericOfString
IL_000a: brfalse.s IL_001b

IL_000c: ldarg.0
IL_000d: ldfld class CanSerializeGenericInstanceScript/GenericClassOfString CanSerializeGenericInstanceScript::_genericOfString
IL_0012: ldarg.1
IL_0013: ldc.i4.1
IL_0014: add
IL_0015: ldarg.2
IL_0016: callvirt instance void ['Assembly-CSharp.dll']CanSerializeGenericInstanceScript/GenericClassOfString::Unity_RemapPPtrs(int32, bool)

IL_001b: nop

IL_001c: nop
IL_001d: ret


IL For method: System.Void CanSerializeGenericInstanceScript::Unity_NamedSerialize(System.Int32,System.Boolean)
// Method begins at RVA 0x20f0
// Code size 77 (0x4d)
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
IL_000e: bgt.s IL_004b

IL_0010: ldarg.0
IL_0011: ldfld class CanSerializeGenericInstanceScript/GenericClassOfString CanSerializeGenericInstanceScript::_genericOfString
IL_0016: brtrue.s IL_0023

IL_0018: ldarg.0
IL_0019: newobj instance void CanSerializeGenericInstanceScript/GenericClassOfString::.ctor()
IL_001e: stfld class CanSerializeGenericInstanceScript/GenericClassOfString CanSerializeGenericInstanceScript::_genericOfString

IL_0023: nop
IL_0024: ldarg.0
IL_0025: ldfld class CanSerializeGenericInstanceScript/GenericClassOfString CanSerializeGenericInstanceScript::_genericOfString
IL_002a: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateWriter [UnityEngine]UnityEngine.Serialization.SerializedNamedStateWriter::Instance
IL_002f: ldloc.0
IL_0030: conv.i
IL_0031: ldc.i4.2
IL_0032: add
IL_0033: callvirt instance void [UnityEngine]UnityEngine.ISerializedNamedStateWriter::BeginMetaGroup(native int)
IL_0038: ldarg.1
IL_0039: ldc.i4.1
IL_003a: add
IL_003b: ldarg.2
IL_003c: callvirt instance void ['Assembly-CSharp.dll']CanSerializeGenericInstanceScript/GenericClassOfString::Unity_NamedSerialize(int32, bool)
IL_0041: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateWriter [UnityEngine]UnityEngine.Serialization.SerializedNamedStateWriter::Instance
IL_0046: callvirt instance void [UnityEngine]UnityEngine.ISerializedNamedStateWriter::EndMetaGroup()

IL_004b: nop
IL_004c: ret


IL For method: System.Void CanSerializeGenericInstanceScript::Unity_NamedDeserialize(System.Int32,System.Boolean)
// Method begins at RVA 0x214c
// Code size 77 (0x4d)
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
IL_000e: bgt.s IL_004b

IL_0010: ldarg.0
IL_0011: ldfld class CanSerializeGenericInstanceScript/GenericClassOfString CanSerializeGenericInstanceScript::_genericOfString
IL_0016: brtrue.s IL_0023

IL_0018: ldarg.0
IL_0019: newobj instance void CanSerializeGenericInstanceScript/GenericClassOfString::.ctor()
IL_001e: stfld class CanSerializeGenericInstanceScript/GenericClassOfString CanSerializeGenericInstanceScript::_genericOfString

IL_0023: nop
IL_0024: ldarg.0
IL_0025: ldfld class CanSerializeGenericInstanceScript/GenericClassOfString CanSerializeGenericInstanceScript::_genericOfString
IL_002a: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateReader [UnityEngine]UnityEngine.Serialization.SerializedNamedStateReader::Instance
IL_002f: ldloc.0
IL_0030: conv.i
IL_0031: ldc.i4.2
IL_0032: add
IL_0033: callvirt instance void [UnityEngine]UnityEngine.ISerializedNamedStateReader::BeginMetaGroup(native int)
IL_0038: ldarg.1
IL_0039: ldc.i4.1
IL_003a: add
IL_003b: ldarg.2
IL_003c: callvirt instance void ['Assembly-CSharp.dll']CanSerializeGenericInstanceScript/GenericClassOfString::Unity_NamedDeserialize(int32, bool)
IL_0041: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateReader [UnityEngine]UnityEngine.Serialization.SerializedNamedStateReader::Instance
IL_0046: callvirt instance void [UnityEngine]UnityEngine.ISerializedNamedStateReader::EndMetaGroup()

IL_004b: nop
IL_004c: ret


IL For method: System.Void CanSerializeGenericInstanceScript/GenericBaseClass`1::.ctor()
// Method begins at RVA 0x21a8
// Code size 7 (0x7)
.maxstack 8

IL_0000: ldarg.0
IL_0001: call instance void [mscorlib]System.Object::.ctor()
IL_0006: ret


IL For method: System.Void CanSerializeGenericInstanceScript/GenericClassOfString::.ctor()
// Method begins at RVA 0x21b0
// Code size 7 (0x7)
.maxstack 8

IL_0000: ldarg.0
IL_0001: call instance void class CanSerializeGenericInstanceScript/GenericBaseClass`1<string>::.ctor()
IL_0006: ret


IL For method: System.Void CanSerializeGenericInstanceScript/GenericClassOfString::Unity_Serialize(System.Int32,System.Boolean)
// Method begins at RVA 0x21b8
// Code size 17 (0x11)
.maxstack 8

IL_0000: ldsfld class [UnityEngine]UnityEngine.ISerializedStateWriter [UnityEngine]UnityEngine.Serialization.SerializedStateWriter::Instance
IL_0005: ldarg.0
IL_0006: ldfld !0 class CanSerializeGenericInstanceScript/GenericBaseClass`1<string>::t
IL_000b: callvirt instance void [UnityEngine]UnityEngine.ISerializedStateWriter::WriteString(string)
IL_0010: ret


IL For method: System.Void CanSerializeGenericInstanceScript/GenericClassOfString::Unity_Deserialize(System.Int32,System.Boolean)
// Method begins at RVA 0x21cc
// Code size 22 (0x16)
.maxstack 8

IL_0000: ldarg.0
IL_0001: ldsfld class [UnityEngine]UnityEngine.ISerializedStateReader [UnityEngine]UnityEngine.Serialization.SerializedStateReader::Instance
IL_0006: callvirt instance string [UnityEngine]UnityEngine.ISerializedStateReader::ReadString()
IL_000b: isinst [mscorlib]System.String
IL_0010: stfld !0 class CanSerializeGenericInstanceScript/GenericBaseClass`1<string>::t
IL_0015: ret


IL For method: System.Void CanSerializeGenericInstanceScript/GenericClassOfString::Unity_RemapPPtrs(System.Int32,System.Boolean)
// Method begins at RVA 0x21e4
// Code size 1 (0x1)
.maxstack 8

IL_0000: ret


IL For method: System.Void CanSerializeGenericInstanceScript/GenericClassOfString::Unity_NamedSerialize(System.Int32,System.Boolean)
// Method begins at RVA 0x21e8
// Code size 31 (0x1f)
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
IL_0012: ldfld !0 class CanSerializeGenericInstanceScript/GenericBaseClass`1<string>::t
IL_0017: ldloc.0
IL_0018: conv.i
IL_0019: callvirt instance void [UnityEngine]UnityEngine.ISerializedNamedStateWriter::WriteString(string, native int)
IL_001e: ret


IL For method: System.Void CanSerializeGenericInstanceScript/GenericClassOfString::Unity_NamedDeserialize(System.Int32,System.Boolean)
// Method begins at RVA 0x2214
// Code size 36 (0x24)
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
IL_001e: stfld !0 class CanSerializeGenericInstanceScript/GenericBaseClass`1<string>::t
IL_0023: ret


