using UnityEngine;
using UnityEngine.Serialization;
namespace Unity.SerializationWeaver.IntegrationTests.Tests.Fixtures.CanRunUNetWeaver
{
    internal struct CubeState
    {
        public int x;

        public int y;
    }
    public class CanRunUNetWeaver : NetworkBehaviour, IUnitySerializable
    {
        [SyncVar]
        private CubeState serverState;

        public CubeState NetworkserverState
        {
            get
            {
                return this.serverState;
            }
            set
            {
                base.SetSyncVar<CubeState>(value, ref this.serverState, 1u);
            }
        }

        private void UNetVersion()
        {
        }

        public override bool OnSerialize(NetworkWriter writer, bool forceAll)
        {
            if (forceAll)
            {
                GeneratedNetworkCode._WriteCubeState_None(writer, this.serverState);
                return true;
            }
            bool flag = false;
            if ((base.syncVarDirtyBits & 1u) != 0u)
            {
                if (!flag)
                {
                    writer.WritePackedUInt32(base.syncVarDirtyBits);
                    flag = true;
                }
                GeneratedNetworkCode._WriteCubeState_None(writer, this.serverState);
            }
            if (!flag)
            {
                writer.WritePackedUInt32(base.syncVarDirtyBits);
            }
            return flag;
        }

        public override void OnDeserialize(NetworkReader reader, bool initialState)
        {
            if (initialState)
            {
                this.serverState = GeneratedNetworkCode._ReadCubeState_None(reader);
                return;
            }
            int num = (int)reader.ReadPackedUInt32();
            if ((num & 1) != 0)
            {
                this.serverState = GeneratedNetworkCode._ReadCubeState_None(reader);
            }
        }

        public override void Unity_Serialize(int depth, bool cloningObject)
        {
        }

        public override void Unity_Deserialize(int depth, bool cloningObject)
        {
        }

        public override void Unity_RemapPPtrs(int depth, bool cloningObject)
        {
        }

        public override void Unity_NamedSerialize(int depth, bool cloningObject)
        {
        }

        public override void Unity_NamedDeserialize(int depth, bool cloningObject)
        {
        }
    }
}
namespace Unity
{
    [System.Runtime.InteropServices.StructLayout(LayoutKind.Auto, CharSet = CharSet.Auto)]
    public class GeneratedNetworkCode
    {
        public static void _WriteCubeState_None(NetworkWriter writer, CubeState value)
        {
            writer.WritePackedUInt32((uint)value.x);
            writer.WritePackedUInt32((uint)value.y);
        }

        public static CubeState _ReadCubeState_None(NetworkReader reader)
        {
            return new CubeState
            {
                x = (int)reader.ReadPackedUInt32(),
                y = (int)reader.ReadPackedUInt32()
            };
        }
    }
}
namespace UnityEngine.Internal
{
    internal static class $FieldNamesStorage
    {
        [System.Runtime.InteropServices.StructLayout(LayoutKind.Explicit, Pack = 1, Size = 1)]
        internal struct $FieldNames
        {
        }

        internal static byte[] $RuntimeNames;

        internal static readonly $FieldNamesStorage.$FieldNames $RVAStaticNames;
    }
}
