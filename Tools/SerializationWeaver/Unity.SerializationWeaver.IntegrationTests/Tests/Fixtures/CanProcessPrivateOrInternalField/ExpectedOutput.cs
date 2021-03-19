using UnityEngine;
using UnityEngine.Serialization;
public class CanProcessPrivateOrInternalField : MonoBehaviour, IUnitySerializable
{
    [SerializeField]
    protected internal int privField;

    [SerializeField]
    protected internal int internField;

    public CanProcessPrivateOrInternalField(int x, int y)
    {
        this.privField = x;
        this.internField = y;
    }

    public CanProcessPrivateOrInternalField(IUnitySerializable self)
    {
    }

    public override void Unity_Serialize(int depth, bool cloningObject)
    {
        SerializedStateWriter.Instance.WriteInt32(this.privField);
        SerializedStateWriter.Instance.WriteInt32(this.internField);
    }

    public override void Unity_Deserialize(int depth, bool cloningObject)
    {
        this.privField = SerializedStateReader.Instance.ReadInt32();
        this.internField = SerializedStateReader.Instance.ReadInt32();
    }

    public override void Unity_RemapPPtrs(int depth, bool cloningObject)
    {
    }

    public unsafe override void Unity_NamedSerialize(int depth, bool cloningObject)
    {
        byte[] var_0_cp_0 = $FieldNamesStorage.$RuntimeNames;
        int var_0_cp_1 = 0;
        SerializedNamedStateWriter.Instance.WriteInt32(this.privField, &var_0_cp_0[var_0_cp_1]);
        SerializedNamedStateWriter.Instance.WriteInt32(this.internField, &var_0_cp_0[var_0_cp_1] + 10);
    }

    public unsafe override void Unity_NamedDeserialize(int depth, bool cloningObject)
    {
        byte[] var_0_cp_0 = $FieldNamesStorage.$RuntimeNames;
        int var_0_cp_1 = 0;
        this.privField = SerializedNamedStateReader.Instance.ReadInt32(&var_0_cp_0[var_0_cp_1]);
        this.internField = SerializedNamedStateReader.Instance.ReadInt32(&var_0_cp_0[var_0_cp_1] + 10);
    }
}
namespace UnityEngine.Internal
{
    internal static class $FieldNamesStorage
    {
        [System.Runtime.InteropServices.StructLayout(LayoutKind.Explicit, Pack = 1, Size = 22)]
        internal struct $FieldNames
        {
        }

        internal static byte[] $RuntimeNames;

        internal static readonly $FieldNamesStorage.$FieldNames $RVAStaticNames;
    }
}
