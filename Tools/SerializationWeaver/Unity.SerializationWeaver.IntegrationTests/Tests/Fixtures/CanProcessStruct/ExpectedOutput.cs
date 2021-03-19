using UnityEngine;
using UnityEngine.Serialization;
public class CanSerializeStruct : MonoBehaviour, IUnitySerializable
{
    [System.Serializable]
    public struct MyStruct : IUnitySerializable
    {
        public byte myByte;

        public int t;

        public ushort myUInt16;

        public char myChar;

        public MyStruct(int i)
        {
            this.t = 0;
            this.myChar = 'a';
            this.myByte = 127;
            this.myUInt16 = 320;
        }

        public MyStruct(IUnitySerializable self)
        {
        }

        public override void Unity_Serialize(int depth, bool cloningObject)
        {
            SerializedStateWriter.Instance.WriteByte(this.myByte);
            SerializedStateWriter.Instance.Align();
            SerializedStateWriter.Instance.WriteInt32(this.t);
            SerializedStateWriter.Instance.WriteUInt16(this.myUInt16);
            SerializedStateWriter.Instance.Align();
            SerializedStateWriter.Instance.WriteChar(this.myChar);
            SerializedStateWriter.Instance.Align();
        }

        public override void Unity_Deserialize(int depth, bool cloningObject)
        {
            this.myByte = SerializedStateReader.Instance.ReadByte();
            SerializedStateReader.Instance.Align();
            this.t = SerializedStateReader.Instance.ReadInt32();
            this.myUInt16 = SerializedStateReader.Instance.ReadUInt16();
            SerializedStateReader.Instance.Align();
            this.myChar = SerializedStateReader.Instance.ReadChar();
            SerializedStateReader.Instance.Align();
        }

        public override void Unity_RemapPPtrs(int depth, bool cloningObject)
        {
        }

        public unsafe override void Unity_NamedSerialize(int depth, bool cloningObject)
        {
            byte[] var_0_cp_0 = $FieldNamesStorage.$RuntimeNames;
            int var_0_cp_1 = 0;
            SerializedNamedStateWriter.Instance.WriteByte(this.myByte, &var_0_cp_0[var_0_cp_1]);
            SerializedNamedStateWriter.Instance.Align();
            SerializedNamedStateWriter.Instance.WriteInt32(this.t, &var_0_cp_0[var_0_cp_1] + 7);
            SerializedNamedStateWriter.Instance.WriteUInt16(this.myUInt16, &var_0_cp_0[var_0_cp_1] + 9);
            SerializedNamedStateWriter.Instance.Align();
            SerializedNamedStateWriter.Instance.WriteChar(this.myChar, &var_0_cp_0[var_0_cp_1] + 18);
            SerializedNamedStateWriter.Instance.Align();
        }

        public unsafe override void Unity_NamedDeserialize(int depth, bool cloningObject)
        {
            byte[] var_0_cp_0 = $FieldNamesStorage.$RuntimeNames;
            int var_0_cp_1 = 0;
            this.myByte = SerializedNamedStateReader.Instance.ReadByte(&var_0_cp_0[var_0_cp_1]);
            SerializedNamedStateReader.Instance.Align();
            this.t = SerializedNamedStateReader.Instance.ReadInt32(&var_0_cp_0[var_0_cp_1] + 7);
            this.myUInt16 = SerializedNamedStateReader.Instance.ReadUInt16(&var_0_cp_0[var_0_cp_1] + 9);
            SerializedNamedStateReader.Instance.Align();
            this.myChar = SerializedNamedStateReader.Instance.ReadChar(&var_0_cp_0[var_0_cp_1] + 18);
            SerializedNamedStateReader.Instance.Align();
        }
    }

    public CanSerializeStruct.MyStruct myStruct;

    public override void Unity_Serialize(int depth, bool cloningObject)
    {
        if (depth <= 7)
        {
            this.myStruct.Unity_Serialize(depth + 1, cloningObject);
        }
        SerializedStateWriter.Instance.Align();
    }

    public override void Unity_Deserialize(int depth, bool cloningObject)
    {
        if (depth <= 7)
        {
            this.myStruct.Unity_Deserialize(depth + 1, cloningObject);
        }
        SerializedStateReader.Instance.Align();
    }

    public override void Unity_RemapPPtrs(int depth, bool cloningObject)
    {
        if (depth <= 7)
        {
            this.myStruct.Unity_RemapPPtrs(depth + 1, cloningObject);
        }
    }

    public override void Unity_NamedSerialize(int depth, bool cloningObject)
    {
        byte[] var_0_cp_0 = $FieldNamesStorage.$RuntimeNames;
        int var_0_cp_1 = 0;
        if (depth <= 7)
        {
            SerializedNamedStateWriter.Instance.BeginMetaGroup(ref var_0_cp_0[var_0_cp_1] + 25);
            this.myStruct.Unity_NamedSerialize(depth + 1, cloningObject);
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
            SerializedNamedStateReader.Instance.BeginMetaGroup(ref var_0_cp_0[var_0_cp_1] + 25);
            this.myStruct.Unity_NamedDeserialize(depth + 1, cloningObject);
            SerializedNamedStateReader.Instance.EndMetaGroup();
        }
        SerializedNamedStateReader.Instance.Align();
    }
}
namespace UnityEngine.Internal
{
    internal static class $FieldNamesStorage
    {
        [System.Runtime.InteropServices.StructLayout(LayoutKind.Explicit, Pack = 1, Size = 34)]
        internal struct $FieldNames
        {
        }

        internal static byte[] $RuntimeNames;

        internal static readonly $FieldNamesStorage.$FieldNames $RVAStaticNames;
    }
}
