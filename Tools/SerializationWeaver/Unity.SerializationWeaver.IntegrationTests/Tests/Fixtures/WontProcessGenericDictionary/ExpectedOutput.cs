using UnityEngine;
using UnityEngine.Serialization;
public class WontProcessGenericDictionaryScript : MonoBehaviour, IUnitySerializable
{
    public class SubDict : System.Collections.Generic.Dictionary<int, int>
    {
    }

    public WontProcessGenericDictionaryScript.SubDict subDict;

    public System.Collections.Generic.Dictionary<int, int> dict;

    [SerializeField]
    private WontProcessGenericDictionaryScript.SubDict subDict2;

    [SerializeField]
    private System.Collections.Generic.Dictionary<int, int> dict2;

    public void Start()
    {
    }

    public override void Unity_Serialize(int depth, bool cloningObject)
    {
    }

    public override void Unity_Deserialize(int depth, bool cloningObject)
    {
    }

    public override void Unity_RemapPPtrs(int depth, bool cloningObject)
    {
    }

    public override void Unity_NamedSerialize(int depth, bool cloningObject)
    {
    }

    public override void Unity_NamedDeserialize(int depth, bool cloningObject)
    {
    }
}
namespace UnityEngine.Internal
{
    internal static class $FieldNamesStorage
    {
        [System.Runtime.InteropServices.StructLayout(LayoutKind.Explicit, Pack = 1, Size = 1)]
        internal struct $FieldNames
        {
        }

        internal static byte[] $RuntimeNames;

        internal static readonly $FieldNamesStorage.$FieldNames $RVAStaticNames;
    }
}
