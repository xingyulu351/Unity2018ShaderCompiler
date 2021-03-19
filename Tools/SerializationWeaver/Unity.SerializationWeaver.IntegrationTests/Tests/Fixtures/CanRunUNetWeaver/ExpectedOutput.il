IL For method: System.Void Unity.SerializationWeaver.IntegrationTests.Tests.Fixtures.CanRunUNetWeaver.CanRunUNetWeaver::.ctor()
// Method begins at RVA 0x2068
// Code size 7 (0x7)
.maxstack 8

IL_0000: ldarg.0
IL_0001: call instance void [UnityEngine.Networking]UnityEngine.Networking.NetworkBehaviour::.ctor()
IL_0006: ret


IL For method: System.Void Unity.SerializationWeaver.IntegrationTests.Tests.Fixtures.CanRunUNetWeaver.CanRunUNetWeaver::UNetVersion()
// Method begins at RVA 0x2070
// Code size 1 (0x1)
.maxstack 8

IL_0000: ret


IL For method: Unity.SerializationWeaver.IntegrationTests.Tests.Fixtures.CanRunUNetWeaver.CubeState Unity.SerializationWeaver.IntegrationTests.Tests.Fixtures.CanRunUNetWeaver.CanRunUNetWeaver::get_NetworkserverState()
// Method begins at RVA 0x2074
// Code size 7 (0x7)
.maxstack 1
.locals init (
    [0] valuetype Unity.SerializationWeaver.IntegrationTests.Tests.Fixtures.CanRunUNetWeaver.CubeState
)

IL_0000: ldarg.0
IL_0001: ldfld valuetype Unity.SerializationWeaver.IntegrationTests.Tests.Fixtures.CanRunUNetWeaver.CubeState Unity.SerializationWeaver.IntegrationTests.Tests.Fixtures.CanRunUNetWeaver.CanRunUNetWeaver::serverState
IL_0006: ret


IL For method: System.Void Unity.SerializationWeaver.IntegrationTests.Tests.Fixtures.CanRunUNetWeaver.CanRunUNetWeaver::set_NetworkserverState(Unity.SerializationWeaver.IntegrationTests.Tests.Fixtures.CanRunUNetWeaver.CubeState)
// Method begins at RVA 0x2088
// Code size 19 (0x13)
.maxstack 8

IL_0000: ldarg.0
IL_0001: ldarg.1
IL_0002: ldarg.0
IL_0003: ldflda valuetype Unity.SerializationWeaver.IntegrationTests.Tests.Fixtures.CanRunUNetWeaver.CubeState Unity.SerializationWeaver.IntegrationTests.Tests.Fixtures.CanRunUNetWeaver.CanRunUNetWeaver::serverState
IL_0008: ldc.i4 1
IL_000d: call instance void [UnityEngine.Networking]UnityEngine.Networking.NetworkBehaviour::SetSyncVar<valuetype Unity.SerializationWeaver.IntegrationTests.Tests.Fixtures.CanRunUNetWeaver.CubeState>(!!0, !!0&, uint32)
IL_0012: ret


IL For method: System.Boolean Unity.SerializationWeaver.IntegrationTests.Tests.Fixtures.CanRunUNetWeaver.CanRunUNetWeaver::OnSerialize(UnityEngine.Networking.NetworkWriter,System.Boolean)
// Method begins at RVA 0x209c
// Code size 95 (0x5f)
.maxstack 2
.locals init (
    [0] bool
)

IL_0000: ldarg.2
IL_0001: brfalse IL_0014

IL_0006: ldarg.1
IL_0007: ldarg.0
IL_0008: ldfld valuetype Unity.SerializationWeaver.IntegrationTests.Tests.Fixtures.CanRunUNetWeaver.CubeState Unity.SerializationWeaver.IntegrationTests.Tests.Fixtures.CanRunUNetWeaver.CanRunUNetWeaver::serverState
IL_000d: call void Unity.GeneratedNetworkCode::_WriteCubeState_None(class [UnityEngine.Networking]UnityEngine.Networking.NetworkWriter, valuetype Unity.SerializationWeaver.IntegrationTests.Tests.Fixtures.CanRunUNetWeaver.CubeState)
IL_0012: ldc.i4.1
IL_0013: ret

IL_0014: nop
IL_0015: ldc.i4.0
IL_0016: stloc.0
IL_0017: ldarg.0
IL_0018: call instance uint32 [UnityEngine.Networking]UnityEngine.Networking.NetworkBehaviour::get_syncVarDirtyBits()
IL_001d: ldc.i4 1
IL_0022: and
IL_0023: brfalse IL_0049

IL_0028: ldloc.0
IL_0029: brtrue IL_003c

IL_002e: ldarg.1
IL_002f: ldarg.0
IL_0030: call instance uint32 [UnityEngine.Networking]UnityEngine.Networking.NetworkBehaviour::get_syncVarDirtyBits()
IL_0035: callvirt instance void [UnityEngine.Networking]UnityEngine.Networking.NetworkWriter::WritePackedUInt32(uint32)
IL_003a: ldc.i4.1
IL_003b: stloc.0

IL_003c: nop
IL_003d: ldarg.1
IL_003e: ldarg.0
IL_003f: ldfld valuetype Unity.SerializationWeaver.IntegrationTests.Tests.Fixtures.CanRunUNetWeaver.CubeState Unity.SerializationWeaver.IntegrationTests.Tests.Fixtures.CanRunUNetWeaver.CanRunUNetWeaver::serverState
IL_0044: call void Unity.GeneratedNetworkCode::_WriteCubeState_None(class [UnityEngine.Networking]UnityEngine.Networking.NetworkWriter, valuetype Unity.SerializationWeaver.IntegrationTests.Tests.Fixtures.CanRunUNetWeaver.CubeState)

IL_0049: nop
IL_004a: ldloc.0
IL_004b: brtrue IL_005c

IL_0050: ldarg.1
IL_0051: ldarg.0
IL_0052: call instance uint32 [UnityEngine.Networking]UnityEngine.Networking.NetworkBehaviour::get_syncVarDirtyBits()
IL_0057: callvirt instance void [UnityEngine.Networking]UnityEngine.Networking.NetworkWriter::WritePackedUInt32(uint32)

IL_005c: nop
IL_005d: ldloc.0
IL_005e: ret


IL For method: System.Void Unity.SerializationWeaver.IntegrationTests.Tests.Fixtures.CanRunUNetWeaver.CanRunUNetWeaver::OnDeserialize(UnityEngine.Networking.NetworkReader,System.Boolean)
// Method begins at RVA 0x2108
// Code size 53 (0x35)
.maxstack 2
.locals init (
    [0] int32
)

IL_0000: ldarg.2
IL_0001: brfalse IL_0013

IL_0006: ldarg.0
IL_0007: ldarg.1
IL_0008: call valuetype Unity.SerializationWeaver.IntegrationTests.Tests.Fixtures.CanRunUNetWeaver.CubeState Unity.GeneratedNetworkCode::_ReadCubeState_None(class [UnityEngine.Networking]UnityEngine.Networking.NetworkReader)
IL_000d: stfld valuetype Unity.SerializationWeaver.IntegrationTests.Tests.Fixtures.CanRunUNetWeaver.CubeState Unity.SerializationWeaver.IntegrationTests.Tests.Fixtures.CanRunUNetWeaver.CanRunUNetWeaver::serverState
IL_0012: ret

IL_0013: nop
IL_0014: ldarg.1
IL_0015: callvirt instance uint32 [UnityEngine.Networking]UnityEngine.Networking.NetworkReader::ReadPackedUInt32()
IL_001a: stloc.0
IL_001b: ldloc.0
IL_001c: ldc.i4 1
IL_0021: and
IL_0022: brfalse IL_0033

IL_0027: ldarg.0
IL_0028: ldarg.1
IL_0029: call valuetype Unity.SerializationWeaver.IntegrationTests.Tests.Fixtures.CanRunUNetWeaver.CubeState Unity.GeneratedNetworkCode::_ReadCubeState_None(class [UnityEngine.Networking]UnityEngine.Networking.NetworkReader)
IL_002e: stfld valuetype Unity.SerializationWeaver.IntegrationTests.Tests.Fixtures.CanRunUNetWeaver.CubeState Unity.SerializationWeaver.IntegrationTests.Tests.Fixtures.CanRunUNetWeaver.CanRunUNetWeaver::serverState

IL_0033: nop
IL_0034: ret


IL For method: System.Void Unity.SerializationWeaver.IntegrationTests.Tests.Fixtures.CanRunUNetWeaver.CanRunUNetWeaver::Unity_Serialize(System.Int32,System.Boolean)
// Method begins at RVA 0x214c
// Code size 1 (0x1)
.maxstack 8

IL_0000: ret


IL For method: System.Void Unity.SerializationWeaver.IntegrationTests.Tests.Fixtures.CanRunUNetWeaver.CanRunUNetWeaver::Unity_Deserialize(System.Int32,System.Boolean)
// Method begins at RVA 0x2150
// Code size 1 (0x1)
.maxstack 8

IL_0000: ret


IL For method: System.Void Unity.SerializationWeaver.IntegrationTests.Tests.Fixtures.CanRunUNetWeaver.CanRunUNetWeaver::Unity_RemapPPtrs(System.Int32,System.Boolean)
// Method begins at RVA 0x2154
// Code size 1 (0x1)
.maxstack 8

IL_0000: ret


IL For method: System.Void Unity.SerializationWeaver.IntegrationTests.Tests.Fixtures.CanRunUNetWeaver.CanRunUNetWeaver::Unity_NamedSerialize(System.Int32,System.Boolean)
// Method begins at RVA 0x2158
// Code size 1 (0x1)
.maxstack 8

IL_0000: ret


IL For method: System.Void Unity.SerializationWeaver.IntegrationTests.Tests.Fixtures.CanRunUNetWeaver.CanRunUNetWeaver::Unity_NamedDeserialize(System.Int32,System.Boolean)
// Method begins at RVA 0x215c
// Code size 1 (0x1)
.maxstack 8

IL_0000: ret


IL For method: System.Void Unity.GeneratedNetworkCode::.ctor()
// Method begins at RVA 0x2160
// Code size 7 (0x7)
.maxstack 8

IL_0000: ldarg.0
IL_0001: call instance void [mscorlib]System.Object::.ctor()
IL_0006: ret


IL For method: System.Void Unity.GeneratedNetworkCode::_WriteCubeState_None(UnityEngine.Networking.NetworkWriter,Unity.SerializationWeaver.IntegrationTests.Tests.Fixtures.CanRunUNetWeaver.CubeState)
// Method begins at RVA 0x2168
// Code size 25 (0x19)
.maxstack 8

IL_0000: ldarg.0
IL_0001: ldarg.1
IL_0002: ldfld int32 Unity.SerializationWeaver.IntegrationTests.Tests.Fixtures.CanRunUNetWeaver.CubeState::x
IL_0007: call instance void [UnityEngine.Networking]UnityEngine.Networking.NetworkWriter::WritePackedUInt32(uint32)
IL_000c: ldarg.0
IL_000d: ldarg.1
IL_000e: ldfld int32 Unity.SerializationWeaver.IntegrationTests.Tests.Fixtures.CanRunUNetWeaver.CubeState::y
IL_0013: call instance void [UnityEngine.Networking]UnityEngine.Networking.NetworkWriter::WritePackedUInt32(uint32)
IL_0018: ret


IL For method: Unity.SerializationWeaver.IntegrationTests.Tests.Fixtures.CanRunUNetWeaver.CubeState Unity.GeneratedNetworkCode::_ReadCubeState_None(UnityEngine.Networking.NetworkReader)
// Method begins at RVA 0x2184
// Code size 42 (0x2a)
.maxstack 2
.locals init (
    [0] valuetype Unity.SerializationWeaver.IntegrationTests.Tests.Fixtures.CanRunUNetWeaver.CubeState
)

IL_0000: ldloca 0
IL_0004: initobj Unity.SerializationWeaver.IntegrationTests.Tests.Fixtures.CanRunUNetWeaver.CubeState
IL_000a: ldloca 0
IL_000e: ldarg.0
IL_000f: call instance uint32 [UnityEngine.Networking]UnityEngine.Networking.NetworkReader::ReadPackedUInt32()
IL_0014: stfld int32 Unity.SerializationWeaver.IntegrationTests.Tests.Fixtures.CanRunUNetWeaver.CubeState::x
IL_0019: ldloca 0
IL_001d: ldarg.0
IL_001e: call instance uint32 [UnityEngine.Networking]UnityEngine.Networking.NetworkReader::ReadPackedUInt32()
IL_0023: stfld int32 Unity.SerializationWeaver.IntegrationTests.Tests.Fixtures.CanRunUNetWeaver.CubeState::y
IL_0028: ldloc.0
IL_0029: ret


