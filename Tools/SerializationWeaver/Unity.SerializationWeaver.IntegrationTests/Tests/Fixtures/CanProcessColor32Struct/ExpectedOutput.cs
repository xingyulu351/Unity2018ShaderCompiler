using UnityEngine;
using UnityEngine.Serialization;
public class CanSerializeStruct : MonoBehaviour, IUnitySerializable
{
    public Color32 myStruct;

    public override void Unity_Serialize(int depth, bool cloningObject)
    {
        SerializedStateWriter.Instance.WriteColor32(this.myStruct);
        SerializedStateWriter.Instance.Align();
    }

    public override void Unity_Deserialize(int depth, bool cloningObject)
    {
        this.myStruct = SerializedStateReader.Instance.ReadColor32();
        SerializedStateReader.Instance.Align();
    }

    public override void Unity_RemapPPtrs(int depth, bool cloningObject)
    {
    }

    public override void Unity_NamedSerialize(int depth, bool cloningObject)
    {
        byte[] var_0_cp_0 = $FieldNamesStorage.$RuntimeNames;
        int var_0_cp_1 = 0;
        SerializedNamedStateWriter.Instance.WriteColor32(this.myStruct, ref var_0_cp_0[var_0_cp_1]);
        SerializedNamedStateWriter.Instance.Align();
    }

    public override void Unity_NamedDeserialize(int depth, bool cloningObject)
    {
        byte[] var_0_cp_0 = $FieldNamesStorage.$RuntimeNames;
        int var_0_cp_1 = 0;
        this.myStruct = SerializedNamedStateReader.Instance.ReadColor32(ref var_0_cp_0[var_0_cp_1]);
        SerializedNamedStateReader.Instance.Align();
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
