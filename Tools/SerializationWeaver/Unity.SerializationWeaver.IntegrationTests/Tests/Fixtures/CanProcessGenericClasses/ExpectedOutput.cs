using UnityEngine;
using UnityEngine.Serialization;
public class CanSerializeGenericInstanceScript : MonoBehaviour, IUnitySerializable
{
    [System.Serializable]
    public class GenericBaseClass<T>
    {
        public T t;
    }

    [System.Serializable]
    public class GenericClassOfString : CanSerializeGenericInstanceScript.GenericBaseClass<string>, IUnitySerializable
    {
        public override void Unity_Serialize(int depth, bool cloningObject)
        {
            SerializedStateWriter.Instance.WriteString(this.t);
        }

        public override void Unity_Deserialize(int depth, bool cloningObject)
        {
            this.t = (SerializedStateReader.Instance.ReadString() as string);
        }

        public override void Unity_RemapPPtrs(int depth, bool cloningObject)
        {
        }

        public override void Unity_NamedSerialize(int depth, bool cloningObject)
        {
            byte[] var_0_cp_0 = $FieldNamesStorage.$RuntimeNames;
            int var_0_cp_1 = 0;
            SerializedNamedStateWriter.Instance.WriteString(this.t, ref var_0_cp_0[var_0_cp_1]);
        }

        public override void Unity_NamedDeserialize(int depth, bool cloningObject)
        {
            byte[] var_0_cp_0 = $FieldNamesStorage.$RuntimeNames;
            int var_0_cp_1 = 0;
            this.t = (SerializedNamedStateReader.Instance.ReadString(ref var_0_cp_0[var_0_cp_1]) as string);
        }
    }

    public CanSerializeGenericInstanceScript.GenericClassOfString _genericOfString;

    public CanSerializeGenericInstanceScript.GenericBaseClass<int> _genericOfInt;

    public void Start()
    {
    }

    public override void Unity_Serialize(int depth, bool cloningObject)
    {
        if (depth <= 7)
        {
            if (this._genericOfString == null)
            {
                this._genericOfString = new CanSerializeGenericInstanceScript.GenericClassOfString();
            }
            this._genericOfString.Unity_Serialize(depth + 1, cloningObject);
        }
    }

    public override void Unity_Deserialize(int depth, bool cloningObject)
    {
        if (depth <= 7)
        {
            if (this._genericOfString == null)
            {
                this._genericOfString = new CanSerializeGenericInstanceScript.GenericClassOfString();
            }
            this._genericOfString.Unity_Deserialize(depth + 1, cloningObject);
        }
    }

    public override void Unity_RemapPPtrs(int depth, bool cloningObject)
    {
        if (depth <= 7)
        {
            if (this._genericOfString != null)
            {
                this._genericOfString.Unity_RemapPPtrs(depth + 1, cloningObject);
            }
        }
    }

    public override void Unity_NamedSerialize(int depth, bool cloningObject)
    {
        byte[] var_0_cp_0 = $FieldNamesStorage.$RuntimeNames;
        int var_0_cp_1 = 0;
        if (depth <= 7)
        {
            if (this._genericOfString == null)
            {
                this._genericOfString = new CanSerializeGenericInstanceScript.GenericClassOfString();
            }
            CanSerializeGenericInstanceScript.GenericClassOfString arg_3C_0 = this._genericOfString;
            SerializedNamedStateWriter.Instance.BeginMetaGroup(ref var_0_cp_0[var_0_cp_1] + 2);
            arg_3C_0.Unity_NamedSerialize(depth + 1, cloningObject);
            SerializedNamedStateWriter.Instance.EndMetaGroup();
        }
    }

    public override void Unity_NamedDeserialize(int depth, bool cloningObject)
    {
        byte[] var_0_cp_0 = $FieldNamesStorage.$RuntimeNames;
        int var_0_cp_1 = 0;
        if (depth <= 7)
        {
            if (this._genericOfString == null)
            {
                this._genericOfString = new CanSerializeGenericInstanceScript.GenericClassOfString();
            }
            CanSerializeGenericInstanceScript.GenericClassOfString arg_3C_0 = this._genericOfString;
            SerializedNamedStateReader.Instance.BeginMetaGroup(ref var_0_cp_0[var_0_cp_1] + 2);
            arg_3C_0.Unity_NamedDeserialize(depth + 1, cloningObject);
            SerializedNamedStateReader.Instance.EndMetaGroup();
        }
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
