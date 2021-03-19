using UnityEngine;
using UnityEngine.Serialization;
[System.Serializable]
public abstract class Base
{
    public int x;
}
[System.Serializable]
public class Derived : Base, IUnitySerializable
{
    public int y;

    public override void Unity_Serialize(int depth, bool cloningObject)
    {
        SerializedStateWriter.Instance.WriteInt32(this.x);
        SerializedStateWriter.Instance.WriteInt32(this.y);
    }

    public override void Unity_Deserialize(int depth, bool cloningObject)
    {
        this.x = SerializedStateReader.Instance.ReadInt32();
        this.y = SerializedStateReader.Instance.ReadInt32();
    }

    public override void Unity_RemapPPtrs(int depth, bool cloningObject)
    {
    }

    public unsafe override void Unity_NamedSerialize(int depth, bool cloningObject)
    {
        byte[] var_0_cp_0 = $FieldNamesStorage.$RuntimeNames;
        int var_0_cp_1 = 0;
        SerializedNamedStateWriter.Instance.WriteInt32(this.x, &var_0_cp_0[var_0_cp_1]);
        SerializedNamedStateWriter.Instance.WriteInt32(this.y, &var_0_cp_0[var_0_cp_1] + 2);
    }

    public unsafe override void Unity_NamedDeserialize(int depth, bool cloningObject)
    {
        byte[] var_0_cp_0 = $FieldNamesStorage.$RuntimeNames;
        int var_0_cp_1 = 0;
        this.x = SerializedNamedStateReader.Instance.ReadInt32(&var_0_cp_0[var_0_cp_1]);
        this.y = SerializedNamedStateReader.Instance.ReadInt32(&var_0_cp_0[var_0_cp_1] + 2);
    }
}
public class ArraysToObjectsSerializedProperly : MonoBehaviour, IUnitySerializable
{
    public Base[] bases;

    public Derived[] derivees;

    public override void Unity_Serialize(int depth, bool cloningObject)
    {
        if (depth <= 7)
        {
            if (this.derivees == null)
            {
                SerializedStateWriter.Instance.WriteInt32(0);
            }
            else
            {
                SerializedStateWriter.Instance.WriteInt32(this.derivees.Length);
                for (int i = 0; i < this.derivees.Length; i++)
                {
                    ((this.derivees[i] != null) ? this.derivees[i] : new Derived()).Unity_Serialize(depth + 1, cloningObject);
                }
            }
        }
    }

    public override void Unity_Deserialize(int depth, bool cloningObject)
    {
        if (depth <= 7)
        {
            this.derivees = new Derived[SerializedStateReader.Instance.ReadInt32()];
            for (int i = 0; i < this.derivees.Length; i++)
            {
                this.derivees[i] = new Derived();
                this.derivees[i].Unity_Deserialize(depth + 1, cloningObject);
            }
        }
    }

    public override void Unity_RemapPPtrs(int depth, bool cloningObject)
    {
        if (depth <= 7)
        {
            if (this.derivees != null)
            {
                for (int i = 0; i < this.derivees.Length; i++)
                {
                    if (this.derivees[i] != null)
                    {
                        this.derivees[i].Unity_RemapPPtrs(depth + 1, cloningObject);
                    }
                }
            }
        }
    }

    public unsafe override void Unity_NamedSerialize(int depth, bool cloningObject)
    {
        byte[] var_0_cp_0 = $FieldNamesStorage.$RuntimeNames;
        int var_0_cp_1 = 0;
        if (depth <= 7)
        {
            if (this.derivees == null)
            {
                SerializedNamedStateWriter.Instance.BeginSequenceGroup(&var_0_cp_0[var_0_cp_1] + 4, 0);
                SerializedNamedStateWriter.Instance.EndMetaGroup();
            }
            else
            {
                SerializedNamedStateWriter.Instance.BeginSequenceGroup(&var_0_cp_0[var_0_cp_1] + 4, this.derivees.Length);
                for (int i = 0; i < this.derivees.Length; i++)
                {
                    Derived arg_84_0 = (this.derivees[i] != null) ? this.derivees[i] : new Derived();
                    SerializedNamedStateWriter.Instance.BeginMetaGroup((System.IntPtr)0);
                    arg_84_0.Unity_NamedSerialize(depth + 1, cloningObject);
                    SerializedNamedStateWriter.Instance.EndMetaGroup();
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
            this.derivees = new Derived[SerializedNamedStateReader.Instance.BeginSequenceGroup(ref var_0_cp_0[var_0_cp_1] + 4)];
            for (int i = 0; i < this.derivees.Length; i++)
            {
                this.derivees[i] = new Derived();
                Derived arg_57_0 = this.derivees[i];
                SerializedNamedStateReader.Instance.BeginMetaGroup((System.IntPtr)0);
                arg_57_0.Unity_NamedDeserialize(depth + 1, cloningObject);
                SerializedNamedStateReader.Instance.EndMetaGroup();
            }
            SerializedNamedStateReader.Instance.EndMetaGroup();
        }
    }
}
namespace UnityEngine.Internal
{
    internal static class $FieldNamesStorage
    {
        [System.Runtime.InteropServices.StructLayout(LayoutKind.Explicit, Pack = 1, Size = 13)]
        internal struct $FieldNames
        {
        }

        internal static byte[] $RuntimeNames;

        internal static readonly $FieldNamesStorage.$FieldNames $RVAStaticNames;
    }
}
