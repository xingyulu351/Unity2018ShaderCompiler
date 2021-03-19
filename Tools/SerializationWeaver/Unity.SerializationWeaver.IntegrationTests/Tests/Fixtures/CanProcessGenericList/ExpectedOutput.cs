using UnityEngine;
using UnityEngine.Serialization;
public class SerializingEnumListWorksScript : MonoBehaviour, IUnitySerializable
{
    public enum testEnum
    {
        NONE,
        Something
    }

    public System.Collections.Generic.List<SerializingEnumListWorksScript.testEnum> enumList;

    public void Start()
    {
    }

    public override void Unity_Serialize(int depth, bool cloningObject)
    {
        if (depth <= 7)
        {
            if (this.enumList == null)
            {
                SerializedStateWriter.Instance.WriteInt32(0);
            }
            else
            {
                SerializedStateWriter.Instance.WriteInt32(this.enumList.Count);
                for (int i = 0; i < this.enumList.Count; i++)
                {
                    SerializedStateWriter.Instance.WriteInt32((int)this.enumList[i]);
                }
            }
        }
    }

    public override void Unity_Deserialize(int depth, bool cloningObject)
    {
        if (depth <= 7)
        {
            int num = SerializedStateReader.Instance.ReadInt32();
            this.enumList = new System.Collections.Generic.List<SerializingEnumListWorksScript.testEnum>(num);
            for (int i = 0; i < num; i++)
            {
                this.enumList.Add((SerializingEnumListWorksScript.testEnum)SerializedStateReader.Instance.ReadInt32());
            }
        }
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
            if (this.enumList == null)
            {
                SerializedNamedStateWriter.Instance.BeginSequenceGroup(&var_0_cp_0[var_0_cp_1], 0);
                SerializedNamedStateWriter.Instance.EndMetaGroup();
            }
            else
            {
                SerializedNamedStateWriter.Instance.BeginSequenceGroup(&var_0_cp_0[var_0_cp_1], this.enumList.Count);
                for (int i = 0; i < this.enumList.Count; i++)
                {
                    SerializedNamedStateWriter.Instance.WriteInt32((int)this.enumList[i], (System.IntPtr)0);
                }
                SerializedNamedStateWriter.Instance.EndMetaGroup();
            }
        }
    }

    public override void Unity_NamedDeserialize(int depth, bool cloningObject)
    {
        byte[] var_0_cp_0 = $FieldNamesStorage.$RuntimeNames;
        int var_0_cp_1 = 0;
        if (depth <= 7)
        {
            int num = SerializedNamedStateReader.Instance.BeginSequenceGroup(ref var_0_cp_0[var_0_cp_1]);
            this.enumList = new System.Collections.Generic.List<SerializingEnumListWorksScript.testEnum>(num);
            for (int i = 0; i < num; i++)
            {
                this.enumList.Add((SerializingEnumListWorksScript.testEnum)SerializedNamedStateReader.Instance.ReadInt32((System.IntPtr)0));
            }
            SerializedNamedStateReader.Instance.EndMetaGroup();
        }
    }
}
namespace UnityEngine.Internal
{
    internal static class $FieldNamesStorage
    {
        [System.Runtime.InteropServices.StructLayout(LayoutKind.Explicit, Pack = 1, Size = 9)]
        internal struct $FieldNames
        {
        }

        internal static byte[] $RuntimeNames;

        internal static readonly $FieldNamesStorage.$FieldNames $RVAStaticNames;
    }
}
