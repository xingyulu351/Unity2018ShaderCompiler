using UnityEngine;
using UnityEngine.Serialization;
public class ExplicitSerializationCallbackInBaseClassBase : MonoBehaviour, ISerializationCallbackReceiver, IUnitySerializable
{
    public string mystring;

    public Component c;

    void ISerializationCallbackReceiver.OnBeforeSerialize()
    {
        this.mystring = "Two";
    }

    void ISerializationCallbackReceiver.OnAfterDeserialize()
    {
    }

    public override void Unity_Serialize(int depth, bool cloningObject)
    {
        if (!cloningObject)
        {
            ((ISerializationCallbackReceiver)this).OnBeforeSerialize();
        }
        SerializedStateWriter.Instance.WriteString(this.mystring);
        if (depth <= 7)
        {
            SerializedStateWriter.Instance.WriteUnityEngineObject(this.c);
        }
    }

    public override void Unity_Deserialize(int depth, bool cloningObject)
    {
        this.mystring = (SerializedStateReader.Instance.ReadString() as string);
        if (depth <= 7)
        {
            this.c = (SerializedStateReader.Instance.ReadUnityEngineObject() as Component);
        }
        if (!cloningObject)
        {
            ((ISerializationCallbackReceiver)this).OnAfterDeserialize();
        }
    }

    public override void Unity_RemapPPtrs(int depth, bool cloningObject)
    {
        if (PPtrRemapper.Instance.CanExecuteSerializationCallbacks() && !cloningObject)
        {
            ((ISerializationCallbackReceiver)this).OnBeforeSerialize();
        }
        if (this.c != null)
        {
            this.c = (PPtrRemapper.Instance.GetNewInstanceToReplaceOldInstance(this.c) as Component);
        }
        if (PPtrRemapper.Instance.CanExecuteSerializationCallbacks())
        {
            ((ISerializationCallbackReceiver)this).OnAfterDeserialize();
        }
    }

    public unsafe override void Unity_NamedSerialize(int depth, bool cloningObject)
    {
        byte[] var_0_cp_0 = $FieldNamesStorage.$RuntimeNames;
        int var_0_cp_1 = 0;
        if (!cloningObject)
        {
            ((ISerializationCallbackReceiver)this).OnBeforeSerialize();
        }
        SerializedNamedStateWriter.Instance.WriteString(this.mystring, &var_0_cp_0[var_0_cp_1]);
        if (depth <= 7)
        {
            SerializedNamedStateWriter.Instance.WriteUnityEngineObject(this.c, &var_0_cp_0[var_0_cp_1] + 9);
        }
    }

    public unsafe override void Unity_NamedDeserialize(int depth, bool cloningObject)
    {
        byte[] var_0_cp_0 = $FieldNamesStorage.$RuntimeNames;
        int var_0_cp_1 = 0;
        this.mystring = (SerializedNamedStateReader.Instance.ReadString(&var_0_cp_0[var_0_cp_1]) as string);
        if (depth <= 7)
        {
            this.c = (SerializedNamedStateReader.Instance.ReadUnityEngineObject(&var_0_cp_0[var_0_cp_1] + 9) as Component);
        }
        if (!cloningObject)
        {
            ((ISerializationCallbackReceiver)this).OnAfterDeserialize();
        }
    }
}
public class ExplictiSerializationCallbackInBaseClassScript : ExplicitSerializationCallbackInBaseClassBase, IUnitySerializable
{
    public bool firstPass = true;

    private void Awake()
    {
    }

    private void Test()
    {
        if (PPtrRemapper.Instance.CanExecuteSerializationCallbacks())
        {
            ((ISerializationCallbackReceiver)this).OnBeforeSerialize();
        }
    }

    public override void Unity_Serialize(int depth, bool cloningObject)
    {
        if (!cloningObject)
        {
            ((ISerializationCallbackReceiver)this).OnBeforeSerialize();
        }
        SerializedStateWriter.Instance.WriteString(this.mystring);
        if (depth <= 7)
        {
            SerializedStateWriter.Instance.WriteUnityEngineObject(this.c);
        }
        SerializedStateWriter.Instance.WriteBoolean(this.firstPass);
        SerializedStateWriter.Instance.Align();
    }

    public override void Unity_Deserialize(int depth, bool cloningObject)
    {
        this.mystring = (SerializedStateReader.Instance.ReadString() as string);
        if (depth <= 7)
        {
            this.c = (SerializedStateReader.Instance.ReadUnityEngineObject() as Component);
        }
        this.firstPass = SerializedStateReader.Instance.ReadBoolean();
        SerializedStateReader.Instance.Align();
        if (!cloningObject)
        {
            ((ISerializationCallbackReceiver)this).OnAfterDeserialize();
        }
    }

    public override void Unity_RemapPPtrs(int depth, bool cloningObject)
    {
        if (PPtrRemapper.Instance.CanExecuteSerializationCallbacks() && !cloningObject)
        {
            ((ISerializationCallbackReceiver)this).OnBeforeSerialize();
        }
        if (this.c != null)
        {
            this.c = (PPtrRemapper.Instance.GetNewInstanceToReplaceOldInstance(this.c) as Component);
        }
        if (PPtrRemapper.Instance.CanExecuteSerializationCallbacks())
        {
            ((ISerializationCallbackReceiver)this).OnAfterDeserialize();
        }
    }

    public unsafe override void Unity_NamedSerialize(int depth, bool cloningObject)
    {
        byte[] var_0_cp_0 = $FieldNamesStorage.$RuntimeNames;
        int var_0_cp_1 = 0;
        if (!cloningObject)
        {
            ((ISerializationCallbackReceiver)this).OnBeforeSerialize();
        }
        SerializedNamedStateWriter.Instance.WriteString(this.mystring, &var_0_cp_0[var_0_cp_1]);
        if (depth <= 7)
        {
            SerializedNamedStateWriter.Instance.WriteUnityEngineObject(this.c, &var_0_cp_0[var_0_cp_1] + 9);
        }
        SerializedNamedStateWriter.Instance.WriteBoolean(this.firstPass, &var_0_cp_0[var_0_cp_1] + 11);
        SerializedNamedStateWriter.Instance.Align();
    }

    public unsafe override void Unity_NamedDeserialize(int depth, bool cloningObject)
    {
        byte[] var_0_cp_0 = $FieldNamesStorage.$RuntimeNames;
        int var_0_cp_1 = 0;
        this.mystring = (SerializedNamedStateReader.Instance.ReadString(&var_0_cp_0[var_0_cp_1]) as string);
        if (depth <= 7)
        {
            this.c = (SerializedNamedStateReader.Instance.ReadUnityEngineObject(&var_0_cp_0[var_0_cp_1] + 9) as Component);
        }
        this.firstPass = SerializedNamedStateReader.Instance.ReadBoolean(&var_0_cp_0[var_0_cp_1] + 11);
        SerializedNamedStateReader.Instance.Align();
        if (!cloningObject)
        {
            ((ISerializationCallbackReceiver)this).OnAfterDeserialize();
        }
    }
}
namespace UnityEngine.Internal
{
    internal static class $FieldNamesStorage
    {
        [System.Runtime.InteropServices.StructLayout(LayoutKind.Explicit, Pack = 1, Size = 21)]
        internal struct $FieldNames
        {
        }

        internal static byte[] $RuntimeNames;

        internal static readonly $FieldNamesStorage.$FieldNames $RVAStaticNames;
    }
}
