using UnityEngine;
using UnityEngine.Serialization;
[System.Serializable]
public struct FixedBuffer : IUnitySerializable
{
    [System.Runtime.CompilerServices.CompilerGenerated, System.Runtime.CompilerServices.UnsafeValueType]
    [System.Runtime.InteropServices.StructLayout(LayoutKind.Sequential, Size = 28)]
    public struct <buffer>__FixedBuffer0
    {
        public int FixedElementField;
    }

    [System.Runtime.CompilerServices.FixedBuffer(typeof(int), 28)]
    public FixedBuffer.<buffer>__FixedBuffer0 buffer;

    public FixedBuffer(IUnitySerializable self)
    {
    }

    public unsafe override void Unity_Serialize(int depth, bool cloningObject)
    {
        if (depth <= 7)
        {
            SerializedStateWriter.Instance.WriteInt32(28);
            for (int i = 0; i < 28; i++)
            {
                int value = *(ref this.buffer.FixedElementField + (System.IntPtr)i * 4);
                SerializedStateWriter.Instance.WriteInt32(value);
            }
        }
        SerializedStateWriter.Instance.Align();
    }

    public unsafe override void Unity_Deserialize(int depth, bool cloningObject)
    {
        if (depth <= 7)
        {
            SerializedStateReader.Instance.ReadInt32();
            for (int i = 0; i < 28; i++)
            {
                int num = SerializedStateReader.Instance.ReadInt32();
                *(ref this.buffer.FixedElementField + (System.IntPtr)i * 4) = num;
            }
        }
        SerializedStateReader.Instance.Align();
    }

    public override void Unity_RemapPPtrs(int depth, bool cloningObject)
    {
    }

    public unsafe override void Unity_NamedSerialize(int depth, bool cloningObject)
    {
        byte[] var_0_cp_0 = $FieldNamesStorage.$RuntimeNames;
        int var_0_cp_1 = 0;
        if (depth <= 7)
        {
            SerializedNamedStateWriter.Instance.BeginSequenceGroup(ref var_0_cp_0[var_0_cp_1], 28);
            for (int i = 0; i < 28; i++)
            {
                int value = *(ref this.buffer.FixedElementField + (System.IntPtr)i * 4);
                SerializedNamedStateWriter.Instance.WriteInt32(value, (System.IntPtr)0);
            }
            SerializedNamedStateWriter.Instance.EndMetaGroup();
        }
        SerializedNamedStateWriter.Instance.Align();
    }

    public unsafe override void Unity_NamedDeserialize(int depth, bool cloningObject)
    {
        byte[] var_0_cp_0 = $FieldNamesStorage.$RuntimeNames;
        int var_0_cp_1 = 0;
        if (depth <= 7)
        {
            SerializedNamedStateReader.Instance.BeginSequenceGroup(ref var_0_cp_0[var_0_cp_1]);
            for (int i = 0; i < 28; i++)
            {
                int num = SerializedNamedStateReader.Instance.ReadInt32((System.IntPtr)0);
                *(ref this.buffer.FixedElementField + (System.IntPtr)i * 4) = num;
            }
            SerializedNamedStateReader.Instance.EndMetaGroup();
        }
        SerializedNamedStateReader.Instance.Align();
    }
}
public class CanSerializeFixedBuffer : MonoBehaviour, IUnitySerializable
{
    public FixedBuffer fixedBuffer;

    public override void Unity_Serialize(int depth, bool cloningObject)
    {
        if (depth <= 7)
        {
            this.fixedBuffer.Unity_Serialize(depth + 1, cloningObject);
        }
        SerializedStateWriter.Instance.Align();
    }

    public override void Unity_Deserialize(int depth, bool cloningObject)
    {
        if (depth <= 7)
        {
            this.fixedBuffer.Unity_Deserialize(depth + 1, cloningObject);
        }
        SerializedStateReader.Instance.Align();
    }

    public override void Unity_RemapPPtrs(int depth, bool cloningObject)
    {
        if (depth <= 7)
        {
            this.fixedBuffer.Unity_RemapPPtrs(depth + 1, cloningObject);
        }
    }

    public override void Unity_NamedSerialize(int depth, bool cloningObject)
    {
        byte[] var_0_cp_0 = $FieldNamesStorage.$RuntimeNames;
        int var_0_cp_1 = 0;
        if (depth <= 7)
        {
            SerializedNamedStateWriter.Instance.BeginMetaGroup(ref var_0_cp_0[var_0_cp_1] + 7);
            this.fixedBuffer.Unity_NamedSerialize(depth + 1, cloningObject);
            SerializedNamedStateWriter.Instance.EndMetaGroup();
        }
        SerializedNamedStateWriter.Instance.Align();
    }

    public override void Unity_NamedDeserialize(int depth, bool cloningObject)
    {
        byte[] var_0_cp_0 = $FieldNamesStorage.$RuntimeNames;
        int var_0_cp_1 = 0;
        if (depth <= 7)
        {
            SerializedNamedStateReader.Instance.BeginMetaGroup(ref var_0_cp_0[var_0_cp_1] + 7);
            this.fixedBuffer.Unity_NamedDeserialize(depth + 1, cloningObject);
            SerializedNamedStateReader.Instance.EndMetaGroup();
        }
        SerializedNamedStateReader.Instance.Align();
    }
}
namespace UnityEngine.Internal
{
    internal static class $FieldNamesStorage
    {
        [System.Runtime.InteropServices.StructLayout(LayoutKind.Explicit, Pack = 1, Size = 19)]
        internal struct $FieldNames
        {
        }

        internal static byte[] $RuntimeNames;

        internal static readonly $FieldNamesStorage.$FieldNames $RVAStaticNames;
    }
}
