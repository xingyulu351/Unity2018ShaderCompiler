using UnityEngine;
using UnityEngine.Serialization;
[System.Serializable]
public struct ExplicitSerializationCallbackInStruct : ISerializationCallbackReceiver, IUnitySerializable
{
    public string mystring;

    public void OnBeforeSerialize()
    {
        this.mystring = "Two";
    }

    public void OnAfterDeserialize()
    {
    }

    public ExplicitSerializationCallbackInStruct(IUnitySerializable self)
    {
    }

    public override void Unity_Serialize(int depth, bool cloningObject)
    {
        if (!cloningObject)
        {
            this.OnBeforeSerialize();
        }
        SerializedStateWriter.Instance.WriteString(this.mystring);
    }

    public override void Unity_Deserialize(int depth, bool cloningObject)
    {
        this.mystring = (SerializedStateReader.Instance.ReadString() as string);
        if (!cloningObject)
        {
            this.OnAfterDeserialize();
        }
    }

    public override void Unity_RemapPPtrs(int depth, bool cloningObject)
    {
        if (PPtrRemapper.Instance.CanExecuteSerializationCallbacks() && !cloningObject)
        {
            this.OnBeforeSerialize();
        }
        if (PPtrRemapper.Instance.CanExecuteSerializationCallbacks())
        {
            this.OnAfterDeserialize();
        }
    }

    public override void Unity_NamedSerialize(int depth, bool cloningObject)
    {
        byte[] var_0_cp_0 = $FieldNamesStorage.$RuntimeNames;
        int var_0_cp_1 = 0;
        if (!cloningObject)
        {
            this.OnBeforeSerialize();
        }
        SerializedNamedStateWriter.Instance.WriteString(this.mystring, ref var_0_cp_0[var_0_cp_1]);
    }

    public override void Unity_NamedDeserialize(int depth, bool cloningObject)
    {
        byte[] var_0_cp_0 = $FieldNamesStorage.$RuntimeNames;
        int var_0_cp_1 = 0;
        this.mystring = (SerializedNamedStateReader.Instance.ReadString(ref var_0_cp_0[var_0_cp_1]) as string);
        if (!cloningObject)
        {
            this.OnAfterDeserialize();
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
