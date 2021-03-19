using UnityEngine;
using UnityEngine.Serialization;
public class CanProcessListOfStructs : MonoBehaviour, IUnitySerializable
{
    [System.Serializable]
    public struct TestStruct : IUnitySerializable
    {
        public Transform t;

        public TestStruct(IUnitySerializable self)
        {
        }

        public override void Unity_Serialize(int depth, bool cloningObject)
        {
            if (depth <= 7)
            {
                SerializedStateWriter.Instance.WriteUnityEngineObject(this.t);
            }
        }

        public override void Unity_Deserialize(int depth, bool cloningObject)
        {
            if (depth <= 7)
            {
                this.t = (SerializedStateReader.Instance.ReadUnityEngineObject() as Transform);
            }
        }

        public override void Unity_RemapPPtrs(int depth, bool cloningObject)
        {
            if (this.t != null)
            {
                this.t = (PPtrRemapper.Instance.GetNewInstanceToReplaceOldInstance(this.t) as Transform);
            }
        }

        public override void Unity_NamedSerialize(int depth, bool cloningObject)
        {
            byte[] var_0_cp_0 = $FieldNamesStorage.$RuntimeNames;
            int var_0_cp_1 = 0;
            if (depth <= 7)
            {
                SerializedNamedStateWriter.Instance.WriteUnityEngineObject(this.t, ref var_0_cp_0[var_0_cp_1]);
            }
        }

        public override void Unity_NamedDeserialize(int depth, bool cloningObject)
        {
            byte[] var_0_cp_0 = $FieldNamesStorage.$RuntimeNames;
            int var_0_cp_1 = 0;
            if (depth <= 7)
            {
                this.t = (SerializedNamedStateReader.Instance.ReadUnityEngineObject(ref var_0_cp_0[var_0_cp_1]) as Transform);
            }
        }
    }

    public System.Collections.Generic.List<CanProcessListOfStructs.TestStruct> list;

    public override void Unity_Serialize(int depth, bool cloningObject)
    {
        if (depth <= 7)
        {
            if (this.list == null)
            {
                SerializedStateWriter.Instance.WriteInt32(0);
            }
            else
            {
                SerializedStateWriter.Instance.WriteInt32(this.list.Count);
                for (int i = 0; i < this.list.Count; i++)
                {
                    this.list[i].Unity_Serialize(depth + 1, cloningObject);
                }
            }
        }
    }

    public override void Unity_Deserialize(int depth, bool cloningObject)
    {
        if (depth <= 7)
        {
            int num = SerializedStateReader.Instance.ReadInt32();
            this.list = new System.Collections.Generic.List<CanProcessListOfStructs.TestStruct>(num);
            for (int i = 0; i < num; i++)
            {
                CanProcessListOfStructs.TestStruct item = default(CanProcessListOfStructs.TestStruct);
                item.Unity_Deserialize(depth + 1, cloningObject);
                this.list.Add(item);
            }
        }
    }

    public override void Unity_RemapPPtrs(int depth, bool cloningObject)
    {
        if (depth <= 7)
        {
            if (this.list != null)
            {
                for (int i = 0; i < this.list.Count; i++)
                {
                    CanProcessListOfStructs.TestStruct value = this.list[i];
                    value.Unity_RemapPPtrs(depth + 1, cloningObject);
                    this.list[i] = value;
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
            if (this.list == null)
            {
                SerializedNamedStateWriter.Instance.BeginSequenceGroup(&var_0_cp_0[var_0_cp_1] + 2, 0);
                SerializedNamedStateWriter.Instance.EndMetaGroup();
            }
            else
            {
                SerializedNamedStateWriter.Instance.BeginSequenceGroup(&var_0_cp_0[var_0_cp_1] + 2, this.list.Count);
                for (int i = 0; i < this.list.Count; i++)
                {
                    CanProcessListOfStructs.TestStruct testStruct = this.list[i];
                    SerializedNamedStateWriter.Instance.BeginMetaGroup((System.IntPtr)0);
                    testStruct.Unity_NamedSerialize(depth + 1, cloningObject);
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
            int num = SerializedNamedStateReader.Instance.BeginSequenceGroup(ref var_0_cp_0[var_0_cp_1] + 2);
            this.list = new System.Collections.Generic.List<CanProcessListOfStructs.TestStruct>(num);
            for (int i = 0; i < num; i++)
            {
                CanProcessListOfStructs.TestStruct item = default(CanProcessListOfStructs.TestStruct);
                SerializedNamedStateReader.Instance.BeginMetaGroup((System.IntPtr)0);
                item.Unity_NamedDeserialize(depth + 1, cloningObject);
                SerializedNamedStateReader.Instance.EndMetaGroup();
                this.list.Add(item);
            }
            SerializedNamedStateReader.Instance.EndMetaGroup();
        }
    }
}
namespace UnityEngine.Internal
{
    internal static class $FieldNamesStorage
    {
        [System.Runtime.InteropServices.StructLayout(LayoutKind.Explicit, Pack = 1, Size = 7)]
        internal struct $FieldNames
        {
        }

        internal static byte[] $RuntimeNames;

        internal static readonly $FieldNamesStorage.$FieldNames $RVAStaticNames;
    }
}
