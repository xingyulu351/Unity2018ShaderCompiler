IL For method: System.Void ExplicitSerializationCallbackInBaseClassBase::.ctor()
// Method begins at RVA 0x206c
// Code size 7 (0x7)
.maxstack 8

IL_0000: ldarg.0
IL_0001: call instance void [UnityEngine]UnityEngine.MonoBehaviour::.ctor()
IL_0006: ret


IL For method: System.Void ExplicitSerializationCallbackInBaseClassBase::UnityEngine.ISerializationCallbackReceiver.OnBeforeSerialize()
// Method begins at RVA 0x2074
// Code size 12 (0xc)
.maxstack 8

IL_0000: ldarg.0
IL_0001: ldstr "Two"
IL_0006: stfld string ExplicitSerializationCallbackInBaseClassBase::mystring
IL_000b: ret


IL For method: System.Void ExplicitSerializationCallbackInBaseClassBase::UnityEngine.ISerializationCallbackReceiver.OnAfterDeserialize()
// Method begins at RVA 0x2084
// Code size 1 (0x1)
.maxstack 8

IL_0000: ret


IL For method: System.Void ExplicitSerializationCallbackInBaseClassBase::Unity_Serialize(System.Int32,System.Boolean)
// Method begins at RVA 0x2088
// Code size 53 (0x35)
.maxstack 8

IL_0000: ldarg.2
IL_0001: brtrue.s IL_000e

IL_0003: ldarg.0
IL_0004: castclass [UnityEngine]UnityEngine.ISerializationCallbackReceiver
IL_0009: callvirt instance void [UnityEngine]UnityEngine.ISerializationCallbackReceiver::OnBeforeSerialize()

IL_000e: nop
IL_000f: ldsfld class [UnityEngine]UnityEngine.ISerializedStateWriter [UnityEngine]UnityEngine.Serialization.SerializedStateWriter::Instance
IL_0014: ldarg.0
IL_0015: ldfld string ExplicitSerializationCallbackInBaseClassBase::mystring
IL_001a: callvirt instance void [UnityEngine]UnityEngine.ISerializedStateWriter::WriteString(string)
IL_001f: ldarg.1
IL_0020: ldc.i4.7
IL_0021: bgt.s IL_0033

IL_0023: ldsfld class [UnityEngine]UnityEngine.ISerializedStateWriter [UnityEngine]UnityEngine.Serialization.SerializedStateWriter::Instance
IL_0028: ldarg.0
IL_0029: ldfld class [UnityEngine]UnityEngine.Component ExplicitSerializationCallbackInBaseClassBase::c
IL_002e: callvirt instance void [UnityEngine]UnityEngine.ISerializedStateWriter::WriteUnityEngineObject(object)

IL_0033: nop
IL_0034: ret


IL For method: System.Void ExplicitSerializationCallbackInBaseClassBase::Unity_Deserialize(System.Int32,System.Boolean)
// Method begins at RVA 0x20c0
// Code size 63 (0x3f)
.maxstack 8

IL_0000: ldarg.0
IL_0001: ldsfld class [UnityEngine]UnityEngine.ISerializedStateReader [UnityEngine]UnityEngine.Serialization.SerializedStateReader::Instance
IL_0006: callvirt instance string [UnityEngine]UnityEngine.ISerializedStateReader::ReadString()
IL_000b: isinst [mscorlib]System.String
IL_0010: stfld string ExplicitSerializationCallbackInBaseClassBase::mystring
IL_0015: ldarg.1
IL_0016: ldc.i4.7
IL_0017: bgt.s IL_002e

IL_0019: ldarg.0
IL_001a: ldsfld class [UnityEngine]UnityEngine.ISerializedStateReader [UnityEngine]UnityEngine.Serialization.SerializedStateReader::Instance
IL_001f: callvirt instance object [UnityEngine]UnityEngine.ISerializedStateReader::ReadUnityEngineObject()
IL_0024: isinst [UnityEngine]UnityEngine.Component
IL_0029: stfld class [UnityEngine]UnityEngine.Component ExplicitSerializationCallbackInBaseClassBase::c

IL_002e: nop
IL_002f: ldarg.2
IL_0030: brtrue.s IL_003d

IL_0032: ldarg.0
IL_0033: castclass [UnityEngine]UnityEngine.ISerializationCallbackReceiver
IL_0038: callvirt instance void [UnityEngine]UnityEngine.ISerializationCallbackReceiver::OnAfterDeserialize()

IL_003d: nop
IL_003e: ret


IL For method: System.Void ExplicitSerializationCallbackInBaseClassBase::Unity_RemapPPtrs(System.Int32,System.Boolean)
// Method begins at RVA 0x2100
// Code size 88 (0x58)
.maxstack 3

IL_0000: ldsfld class [UnityEngine]UnityEngine.IPPtrRemapper [UnityEngine]UnityEngine.Serialization.PPtrRemapper::Instance
IL_0005: callvirt instance bool [UnityEngine]UnityEngine.IPPtrRemapper::CanExecuteSerializationCallbacks()
IL_000a: brfalse.s IL_001a

IL_000c: ldarg.2
IL_000d: brtrue.s IL_001a

IL_000f: ldarg.0
IL_0010: castclass [UnityEngine]UnityEngine.ISerializationCallbackReceiver
IL_0015: callvirt instance void [UnityEngine]UnityEngine.ISerializationCallbackReceiver::OnBeforeSerialize()

IL_001a: nop
IL_001b: ldarg.0
IL_001c: ldfld class [UnityEngine]UnityEngine.Component ExplicitSerializationCallbackInBaseClassBase::c
IL_0021: brfalse.s IL_003e

IL_0023: ldarg.0
IL_0024: ldsfld class [UnityEngine]UnityEngine.IPPtrRemapper [UnityEngine]UnityEngine.Serialization.PPtrRemapper::Instance
IL_0029: ldarg.0
IL_002a: ldfld class [UnityEngine]UnityEngine.Component ExplicitSerializationCallbackInBaseClassBase::c
IL_002f: callvirt instance object [UnityEngine]UnityEngine.IPPtrRemapper::GetNewInstanceToReplaceOldInstance(object)
IL_0034: isinst [UnityEngine]UnityEngine.Component
IL_0039: stfld class [UnityEngine]UnityEngine.Component ExplicitSerializationCallbackInBaseClassBase::c

IL_003e: nop
IL_003f: ldsfld class [UnityEngine]UnityEngine.IPPtrRemapper [UnityEngine]UnityEngine.Serialization.PPtrRemapper::Instance
IL_0044: callvirt instance bool [UnityEngine]UnityEngine.IPPtrRemapper::CanExecuteSerializationCallbacks()
IL_0049: brfalse.s IL_0056

IL_004b: ldarg.0
IL_004c: castclass [UnityEngine]UnityEngine.ISerializationCallbackReceiver
IL_0051: callvirt instance void [UnityEngine]UnityEngine.ISerializationCallbackReceiver::OnAfterDeserialize()

IL_0056: nop
IL_0057: ret


IL For method: System.Void ExplicitSerializationCallbackInBaseClassBase::Unity_NamedSerialize(System.Int32,System.Boolean)
// Method begins at RVA 0x2164
// Code size 72 (0x48)
.maxstack 4
.locals init (
    [0] uint8& pinned
)

IL_0000: ldsfld uint8[] UnityEngine.Internal.$FieldNamesStorage::$RuntimeNames
IL_0005: ldc.i4.0
IL_0006: ldelema [mscorlib]System.Byte
IL_000b: stloc.0
IL_000c: ldarg.2
IL_000d: brtrue.s IL_001a

IL_000f: ldarg.0
IL_0010: castclass [UnityEngine]UnityEngine.ISerializationCallbackReceiver
IL_0015: callvirt instance void [UnityEngine]UnityEngine.ISerializationCallbackReceiver::OnBeforeSerialize()

IL_001a: nop
IL_001b: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateWriter [UnityEngine]UnityEngine.Serialization.SerializedNamedStateWriter::Instance
IL_0020: ldarg.0
IL_0021: ldfld string ExplicitSerializationCallbackInBaseClassBase::mystring
IL_0026: ldloc.0
IL_0027: conv.i
IL_0028: callvirt instance void [UnityEngine]UnityEngine.ISerializedNamedStateWriter::WriteString(string, native int)
IL_002d: ldarg.1
IL_002e: ldc.i4.7
IL_002f: bgt.s IL_0046

IL_0031: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateWriter [UnityEngine]UnityEngine.Serialization.SerializedNamedStateWriter::Instance
IL_0036: ldarg.0
IL_0037: ldfld class [UnityEngine]UnityEngine.Component ExplicitSerializationCallbackInBaseClassBase::c
IL_003c: ldloc.0
IL_003d: conv.i
IL_003e: ldc.i4.s 9
IL_0040: add
IL_0041: callvirt instance void [UnityEngine]UnityEngine.ISerializedNamedStateWriter::WriteUnityEngineObject(object, native int)

IL_0046: nop
IL_0047: ret


IL For method: System.Void ExplicitSerializationCallbackInBaseClassBase::Unity_NamedDeserialize(System.Int32,System.Boolean)
// Method begins at RVA 0x21b8
// Code size 82 (0x52)
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
IL_0014: callvirt instance string [UnityEngine]UnityEngine.ISerializedNamedStateReader::ReadString(native int)
IL_0019: isinst [mscorlib]System.String
IL_001e: stfld string ExplicitSerializationCallbackInBaseClassBase::mystring
IL_0023: ldarg.1
IL_0024: ldc.i4.7
IL_0025: bgt.s IL_0041

IL_0027: ldarg.0
IL_0028: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateReader [UnityEngine]UnityEngine.Serialization.SerializedNamedStateReader::Instance
IL_002d: ldloc.0
IL_002e: conv.i
IL_002f: ldc.i4.s 9
IL_0031: add
IL_0032: callvirt instance object [UnityEngine]UnityEngine.ISerializedNamedStateReader::ReadUnityEngineObject(native int)
IL_0037: isinst [UnityEngine]UnityEngine.Component
IL_003c: stfld class [UnityEngine]UnityEngine.Component ExplicitSerializationCallbackInBaseClassBase::c

IL_0041: nop
IL_0042: ldarg.2
IL_0043: brtrue.s IL_0050

IL_0045: ldarg.0
IL_0046: castclass [UnityEngine]UnityEngine.ISerializationCallbackReceiver
IL_004b: callvirt instance void [UnityEngine]UnityEngine.ISerializationCallbackReceiver::OnAfterDeserialize()

IL_0050: nop
IL_0051: ret


IL For method: System.Void ExplictiSerializationCallbackInBaseClassScript::.ctor()
// Method begins at RVA 0x2218
// Code size 14 (0xe)
.maxstack 8

IL_0000: ldarg.0
IL_0001: ldc.i4.1
IL_0002: stfld bool ExplictiSerializationCallbackInBaseClassScript::firstPass
IL_0007: ldarg.0
IL_0008: call instance void ExplicitSerializationCallbackInBaseClassBase::.ctor()
IL_000d: ret


IL For method: System.Void ExplictiSerializationCallbackInBaseClassScript::Awake()
// Method begins at RVA 0x2228
// Code size 1 (0x1)
.maxstack 8

IL_0000: ret


IL For method: System.Void ExplictiSerializationCallbackInBaseClassScript::Test()
// Method begins at RVA 0x222c
// Code size 22 (0x16)
.maxstack 8

IL_0000: ldsfld class [UnityEngine]UnityEngine.IPPtrRemapper [UnityEngine]UnityEngine.Serialization.PPtrRemapper::Instance
IL_0005: callvirt instance bool [UnityEngine]UnityEngine.IPPtrRemapper::CanExecuteSerializationCallbacks()
IL_000a: brfalse IL_0015

IL_000f: ldarg.0
IL_0010: callvirt instance void [UnityEngine]UnityEngine.ISerializationCallbackReceiver::OnBeforeSerialize()

IL_0015: ret


IL For method: System.Void ExplictiSerializationCallbackInBaseClassScript::Unity_Serialize(System.Int32,System.Boolean)
// Method begins at RVA 0x2244
// Code size 79 (0x4f)
.maxstack 2

IL_0000: ldarg.2
IL_0001: brtrue.s IL_000e

IL_0003: ldarg.0
IL_0004: castclass [UnityEngine]UnityEngine.ISerializationCallbackReceiver
IL_0009: callvirt instance void [UnityEngine]UnityEngine.ISerializationCallbackReceiver::OnBeforeSerialize()

IL_000e: nop
IL_000f: ldsfld class [UnityEngine]UnityEngine.ISerializedStateWriter [UnityEngine]UnityEngine.Serialization.SerializedStateWriter::Instance
IL_0014: ldarg.0
IL_0015: ldfld string ExplicitSerializationCallbackInBaseClassBase::mystring
IL_001a: callvirt instance void [UnityEngine]UnityEngine.ISerializedStateWriter::WriteString(string)
IL_001f: ldarg.1
IL_0020: ldc.i4.7
IL_0021: bgt.s IL_0033

IL_0023: ldsfld class [UnityEngine]UnityEngine.ISerializedStateWriter [UnityEngine]UnityEngine.Serialization.SerializedStateWriter::Instance
IL_0028: ldarg.0
IL_0029: ldfld class [UnityEngine]UnityEngine.Component ExplicitSerializationCallbackInBaseClassBase::c
IL_002e: callvirt instance void [UnityEngine]UnityEngine.ISerializedStateWriter::WriteUnityEngineObject(object)

IL_0033: nop
IL_0034: ldsfld class [UnityEngine]UnityEngine.ISerializedStateWriter [UnityEngine]UnityEngine.Serialization.SerializedStateWriter::Instance
IL_0039: ldarg.0
IL_003a: ldfld bool ExplictiSerializationCallbackInBaseClassScript::firstPass
IL_003f: callvirt instance void [UnityEngine]UnityEngine.ISerializedStateWriter::WriteBoolean(bool)
IL_0044: ldsfld class [UnityEngine]UnityEngine.ISerializedStateWriter [UnityEngine]UnityEngine.Serialization.SerializedStateWriter::Instance
IL_0049: callvirt instance void [UnityEngine]UnityEngine.ISerializedStateWriter::Align()
IL_004e: ret


IL For method: System.Void ExplictiSerializationCallbackInBaseClassScript::Unity_Deserialize(System.Int32,System.Boolean)
// Method begins at RVA 0x22a0
// Code size 89 (0x59)
.maxstack 2

IL_0000: ldarg.0
IL_0001: ldsfld class [UnityEngine]UnityEngine.ISerializedStateReader [UnityEngine]UnityEngine.Serialization.SerializedStateReader::Instance
IL_0006: callvirt instance string [UnityEngine]UnityEngine.ISerializedStateReader::ReadString()
IL_000b: isinst [mscorlib]System.String
IL_0010: stfld string ExplicitSerializationCallbackInBaseClassBase::mystring
IL_0015: ldarg.1
IL_0016: ldc.i4.7
IL_0017: bgt.s IL_002e

IL_0019: ldarg.0
IL_001a: ldsfld class [UnityEngine]UnityEngine.ISerializedStateReader [UnityEngine]UnityEngine.Serialization.SerializedStateReader::Instance
IL_001f: callvirt instance object [UnityEngine]UnityEngine.ISerializedStateReader::ReadUnityEngineObject()
IL_0024: isinst [UnityEngine]UnityEngine.Component
IL_0029: stfld class [UnityEngine]UnityEngine.Component ExplicitSerializationCallbackInBaseClassBase::c

IL_002e: nop
IL_002f: ldarg.0
IL_0030: ldsfld class [UnityEngine]UnityEngine.ISerializedStateReader [UnityEngine]UnityEngine.Serialization.SerializedStateReader::Instance
IL_0035: callvirt instance bool [UnityEngine]UnityEngine.ISerializedStateReader::ReadBoolean()
IL_003a: stfld bool ExplictiSerializationCallbackInBaseClassScript::firstPass
IL_003f: ldsfld class [UnityEngine]UnityEngine.ISerializedStateReader [UnityEngine]UnityEngine.Serialization.SerializedStateReader::Instance
IL_0044: callvirt instance void [UnityEngine]UnityEngine.ISerializedStateReader::Align()
IL_0049: ldarg.2
IL_004a: brtrue.s IL_0057

IL_004c: ldarg.0
IL_004d: castclass [UnityEngine]UnityEngine.ISerializationCallbackReceiver
IL_0052: callvirt instance void [UnityEngine]UnityEngine.ISerializationCallbackReceiver::OnAfterDeserialize()

IL_0057: nop
IL_0058: ret


IL For method: System.Void ExplictiSerializationCallbackInBaseClassScript::Unity_RemapPPtrs(System.Int32,System.Boolean)
// Method begins at RVA 0x2308
// Code size 88 (0x58)
.maxstack 3

IL_0000: ldsfld class [UnityEngine]UnityEngine.IPPtrRemapper [UnityEngine]UnityEngine.Serialization.PPtrRemapper::Instance
IL_0005: callvirt instance bool [UnityEngine]UnityEngine.IPPtrRemapper::CanExecuteSerializationCallbacks()
IL_000a: brfalse.s IL_001a

IL_000c: ldarg.2
IL_000d: brtrue.s IL_001a

IL_000f: ldarg.0
IL_0010: castclass [UnityEngine]UnityEngine.ISerializationCallbackReceiver
IL_0015: callvirt instance void [UnityEngine]UnityEngine.ISerializationCallbackReceiver::OnBeforeSerialize()

IL_001a: nop
IL_001b: ldarg.0
IL_001c: ldfld class [UnityEngine]UnityEngine.Component ExplicitSerializationCallbackInBaseClassBase::c
IL_0021: brfalse.s IL_003e

IL_0023: ldarg.0
IL_0024: ldsfld class [UnityEngine]UnityEngine.IPPtrRemapper [UnityEngine]UnityEngine.Serialization.PPtrRemapper::Instance
IL_0029: ldarg.0
IL_002a: ldfld class [UnityEngine]UnityEngine.Component ExplicitSerializationCallbackInBaseClassBase::c
IL_002f: callvirt instance object [UnityEngine]UnityEngine.IPPtrRemapper::GetNewInstanceToReplaceOldInstance(object)
IL_0034: isinst [UnityEngine]UnityEngine.Component
IL_0039: stfld class [UnityEngine]UnityEngine.Component ExplicitSerializationCallbackInBaseClassBase::c

IL_003e: nop
IL_003f: ldsfld class [UnityEngine]UnityEngine.IPPtrRemapper [UnityEngine]UnityEngine.Serialization.PPtrRemapper::Instance
IL_0044: callvirt instance bool [UnityEngine]UnityEngine.IPPtrRemapper::CanExecuteSerializationCallbacks()
IL_0049: brfalse.s IL_0056

IL_004b: ldarg.0
IL_004c: castclass [UnityEngine]UnityEngine.ISerializationCallbackReceiver
IL_0051: callvirt instance void [UnityEngine]UnityEngine.ISerializationCallbackReceiver::OnAfterDeserialize()

IL_0056: nop
IL_0057: ret


IL For method: System.Void ExplictiSerializationCallbackInBaseClassScript::Unity_NamedSerialize(System.Int32,System.Boolean)
// Method begins at RVA 0x236c
// Code size 103 (0x67)
.maxstack 4
.locals init (
    [0] uint8& pinned
)

IL_0000: ldsfld uint8[] UnityEngine.Internal.$FieldNamesStorage::$RuntimeNames
IL_0005: ldc.i4.0
IL_0006: ldelema [mscorlib]System.Byte
IL_000b: stloc.0
IL_000c: ldarg.2
IL_000d: brtrue.s IL_001a

IL_000f: ldarg.0
IL_0010: castclass [UnityEngine]UnityEngine.ISerializationCallbackReceiver
IL_0015: callvirt instance void [UnityEngine]UnityEngine.ISerializationCallbackReceiver::OnBeforeSerialize()

IL_001a: nop
IL_001b: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateWriter [UnityEngine]UnityEngine.Serialization.SerializedNamedStateWriter::Instance
IL_0020: ldarg.0
IL_0021: ldfld string ExplicitSerializationCallbackInBaseClassBase::mystring
IL_0026: ldloc.0
IL_0027: conv.i
IL_0028: callvirt instance void [UnityEngine]UnityEngine.ISerializedNamedStateWriter::WriteString(string, native int)
IL_002d: ldarg.1
IL_002e: ldc.i4.7
IL_002f: bgt.s IL_0046

IL_0031: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateWriter [UnityEngine]UnityEngine.Serialization.SerializedNamedStateWriter::Instance
IL_0036: ldarg.0
IL_0037: ldfld class [UnityEngine]UnityEngine.Component ExplicitSerializationCallbackInBaseClassBase::c
IL_003c: ldloc.0
IL_003d: conv.i
IL_003e: ldc.i4.s 9
IL_0040: add
IL_0041: callvirt instance void [UnityEngine]UnityEngine.ISerializedNamedStateWriter::WriteUnityEngineObject(object, native int)

IL_0046: nop
IL_0047: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateWriter [UnityEngine]UnityEngine.Serialization.SerializedNamedStateWriter::Instance
IL_004c: ldarg.0
IL_004d: ldfld bool ExplictiSerializationCallbackInBaseClassScript::firstPass
IL_0052: ldloc.0
IL_0053: conv.i
IL_0054: ldc.i4.s 11
IL_0056: add
IL_0057: callvirt instance void [UnityEngine]UnityEngine.ISerializedNamedStateWriter::WriteBoolean(bool, native int)
IL_005c: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateWriter [UnityEngine]UnityEngine.Serialization.SerializedNamedStateWriter::Instance
IL_0061: callvirt instance void [UnityEngine]UnityEngine.ISerializedNamedStateWriter::Align()
IL_0066: ret


IL For method: System.Void ExplictiSerializationCallbackInBaseClassScript::Unity_NamedDeserialize(System.Int32,System.Boolean)
// Method begins at RVA 0x23e0
// Code size 113 (0x71)
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
IL_0014: callvirt instance string [UnityEngine]UnityEngine.ISerializedNamedStateReader::ReadString(native int)
IL_0019: isinst [mscorlib]System.String
IL_001e: stfld string ExplicitSerializationCallbackInBaseClassBase::mystring
IL_0023: ldarg.1
IL_0024: ldc.i4.7
IL_0025: bgt.s IL_0041

IL_0027: ldarg.0
IL_0028: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateReader [UnityEngine]UnityEngine.Serialization.SerializedNamedStateReader::Instance
IL_002d: ldloc.0
IL_002e: conv.i
IL_002f: ldc.i4.s 9
IL_0031: add
IL_0032: callvirt instance object [UnityEngine]UnityEngine.ISerializedNamedStateReader::ReadUnityEngineObject(native int)
IL_0037: isinst [UnityEngine]UnityEngine.Component
IL_003c: stfld class [UnityEngine]UnityEngine.Component ExplicitSerializationCallbackInBaseClassBase::c

IL_0041: nop
IL_0042: ldarg.0
IL_0043: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateReader [UnityEngine]UnityEngine.Serialization.SerializedNamedStateReader::Instance
IL_0048: ldloc.0
IL_0049: conv.i
IL_004a: ldc.i4.s 11
IL_004c: add
IL_004d: callvirt instance bool [UnityEngine]UnityEngine.ISerializedNamedStateReader::ReadBoolean(native int)
IL_0052: stfld bool ExplictiSerializationCallbackInBaseClassScript::firstPass
IL_0057: ldsfld class [UnityEngine]UnityEngine.ISerializedNamedStateReader [UnityEngine]UnityEngine.Serialization.SerializedNamedStateReader::Instance
IL_005c: callvirt instance void [UnityEngine]UnityEngine.ISerializedNamedStateReader::Align()
IL_0061: ldarg.2
IL_0062: brtrue.s IL_006f

IL_0064: ldarg.0
IL_0065: castclass [UnityEngine]UnityEngine.ISerializationCallbackReceiver
IL_006a: callvirt instance void [UnityEngine]UnityEngine.ISerializationCallbackReceiver::OnAfterDeserialize()

IL_006f: nop
IL_0070: ret


