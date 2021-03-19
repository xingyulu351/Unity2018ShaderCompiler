using UnityEngine;
using UnityEngine.Serialization;
public class CanSerializeStruct : MonoBehaviour, IUnitySerializable
{
    public Matrix4x4 myStruct;

    public override void Unity_Serialize(int depth, bool cloningObject)
    {
        SerializedStateWriter.Instance.WriteMatrix4x4(ref this.myStruct);
        SerializedStateWriter.Instance.Align();
    }

    public override void Unity_Deserialize(int depth, bool cloningObject)
    {
        this.myStruct = SerializedStateReader.Instance.ReadMatrix4x4();
        SerializedStateReader.Instance.Align();
    }

    public override void Unity_RemapPPtrs(int depth, bool cloningObject)
    {
    }

    public override void Unity_NamedSerialize(int depth, bool cloningObject)
    {
        byte[] var_0_cp_0 = $FieldNamesStorage.$RuntimeNames;
        int var_0_cp_1 = 0;
        SerializedNamedStateWriter.Instance.WriteMatrix4x4(ref this.myStruct, ref var_0_cp_0[var_0_cp_1]);
        SerializedNamedStateWriter.Instance.Align();
    }

    public override void Unity_NamedDeserialize(int depth, bool cloningObject)
    {
        byte[] var_0_cp_0 = $FieldNamesStorage.$RuntimeNames;
        int var_0_cp_1 = 0;
        this.myStruct = SerializedNamedStateReader.Instance.ReadMatrix4x4(ref var_0_cp_0[var_0_cp_1]);
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
