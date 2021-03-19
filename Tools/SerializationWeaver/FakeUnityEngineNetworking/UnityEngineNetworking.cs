// ReSharper disable CheckNamespace

using System;
using System.Collections;
using System.Collections.Generic;

namespace UnityEngine.Networking
{
    public abstract class MessageBase
    {
        public virtual void Deserialize(NetworkReader reader) {}
        public virtual void Serialize(NetworkWriter writer) {}
    }

    [Serializable]
    public struct NetworkHash128
    {}

    [Serializable]
    public struct NetworkInstanceId
    {
        public bool IsEmpty()
        {
            return true;
        }
    }

    [Serializable]
    public struct NetworkSceneId
    {}

    public class ClientScene
    {
        public static NetworkConnection readyConnection { get { return null; } }

        public static GameObject FindLocalObject(NetworkInstanceId netId)
        {
            return null;
        }
    }

    public class NetworkCRC
    {
        public static void RegisterBehaviour(string name, int channel)
        {}
    }

    public class NetworkIdentity : MonoBehaviour
    {
        public NetworkInstanceId netId { get { return new NetworkInstanceId(); } }
    }

    public class NetworkBehaviour : MonoBehaviour
    {
        protected uint syncVarDirtyBits { get { return 0; } }
        protected bool syncVarHookGuard { get; set; }
        public bool isServer { get { return false; } }
        public short playerControllerId { get { return 0; } }

        public virtual bool InvokeCommand(int cmdHash, NetworkReader reader)
        {
            return false;
        }

        public virtual bool InvokeRPC(int cmdHash, NetworkReader reader)
        {
            return false;
        }

        public virtual bool InvokeSyncEvent(int cmdHash, NetworkReader reader)
        {
            return false;
        }

        public virtual bool InvokeSyncList(int cmdHash, NetworkReader reader)
        {
            return false;
        }

        public virtual bool OnSerialize(NetworkWriter writer, bool initialState)
        {
            return false;
        }

        public virtual void OnDeserialize(NetworkReader reader, bool initialState)
        {
        }

        protected void SetSyncVarGameObject(GameObject newGameObject, ref GameObject gameObjectField, uint dirtyBit, ref NetworkInstanceId netIdField)
        {}

        protected void SetSyncVar<T>(T value, ref T fieldValue, uint dirtyBit)
        {}

        public delegate void CmdDelegate(NetworkBehaviour obj, NetworkReader reader);
        protected static void RegisterCommandDelegate(Type invokeClass, int cmdHash, CmdDelegate func)
        {}

        protected static void RegisterRpcDelegate(Type invokeClass, int cmdHash, CmdDelegate func)
        {}

        protected static void RegisterEventDelegate(Type invokeClass, int cmdHash, CmdDelegate func)
        {}

        static protected void RegisterSyncListDelegate(Type invokeClass, int cmdHash, CmdDelegate func)
        {}

        protected void SendCommandInternal(NetworkWriter writer, int channelId, string cmdName)
        {}

        protected void SendRPCInternal(NetworkWriter writer, int channelId, string rpcName)
        {}

        protected void SendTargetRPCInternal(NetworkConnection conn, NetworkWriter writer, int channelId, string rpcName)
        {}

        protected void SendEventInternal(NetworkWriter writer, int channelId, string eventName)
        {}
    }


    public class NetworkConnection : IDisposable
    {
        public void Dispose()
        {
        }
    }


    public class NetworkReader
    {
        public NetworkReader()
        {
        }

        public NetworkReader(NetworkWriter writer)
        {
        }

        public NetworkReader(byte[] buffer)
        {
        }

        public UInt32 ReadPackedUInt32()
        {
            return 0;
        }

        public UInt64 ReadPackedUInt64()
        {
            return 0;
        }

        public byte ReadByte()
        {
            return 0;
        }

        public sbyte ReadSByte()
        {
            return 0;
        }

        public short ReadInt16()
        {
            return 0;
        }

        public ushort ReadUInt16()
        {
            return 0;
        }

        public int ReadInt32()
        {
            return 0;
        }

        public uint ReadUInt32()
        {
            return 0;
        }

        public long ReadInt64()
        {
            return 0;
        }

        public ulong ReadUInt64()
        {
            return 0;
        }

        public decimal ReadDecimal()
        {
            return new decimal();
        }

        public float ReadSingle()
        {
            return 0;
        }

        public double ReadDouble()
        {
            return 0;
        }

        public string ReadString()
        {
            return "";
        }

        public char ReadChar()
        {
            return (char)0;
        }

        public bool ReadBoolean()
        {
            return false;
        }

        public byte[] ReadBytes(int count)
        {
            return null;
        }

        public byte[] ReadBytesAndSize()
        {
            return null;
        }

        public Vector2 ReadVector2()
        {
            return new Vector2();
        }

        public Vector3 ReadVector3()
        {
            return new Vector3();
        }

        public Vector4 ReadVector4()
        {
            return new Vector4();
        }

        public Color ReadColor()
        {
            return new Color();
        }

        public Color32 ReadColor32()
        {
            return new Color32();
        }

        public Quaternion ReadQuaternion()
        {
            return new Quaternion();
        }

        public Rect ReadRect()
        {
            return new Rect();
        }

        public Matrix4x4 ReadMatrix4x4()
        {
            return new Matrix4x4();
        }

        public TMsg ReadMessage<TMsg>() where TMsg : MessageBase, new()
        {
            var msg = new TMsg();
            msg.Deserialize(this);
            return msg;
        }

        public NetworkInstanceId ReadNetworkId()
        {
            return new NetworkInstanceId();
        }

        public NetworkSceneId ReadSceneId()
        {
            return new NetworkSceneId();
        }

        public Plane ReadPlane()
        {
            return new Plane();
        }

        public Ray ReadRay()
        {
            return new Ray();
        }

        public NetworkHash128 ReadNetworkHash128()
        {
            return new NetworkHash128();
        }

        public GameObject ReadGameObject()
        {
            return null;
        }

        public NetworkIdentity ReadNetworkIdentity()
        {
            return null;
        }

        public Transform ReadTransform()
        {
            return null;
        }
    }

    public class NetworkWriter
    {
        public NetworkWriter()
        {
        }

        public NetworkWriter(byte[] buffer)
        {
        }

        public void WritePackedUInt32(UInt32 value)
        {
        }

        public void WritePackedUInt64(UInt64 value)
        {
        }

        public void Write(char value)
        {
        }

        public void Write(byte value)
        {
        }

        public void Write(sbyte value)
        {
        }

        public void Write(short value)
        {
        }

        public void Write(ushort value)
        {
        }

        public void Write(int value)
        {
        }

        public void Write(uint value)
        {
        }

        public void Write(long value)
        {
        }

        public void Write(ulong value)
        {
        }

        public void Write(float value)
        {
        }

        public void Write(double value)
        {
        }

        public void Write(decimal value)
        {
        }

        public void Write(string value)
        {
        }

        public void Write(bool value)
        {
        }

        public void Write(byte[] buffer, int count)
        {
        }

        public void Write(byte[] buffer, int offset, int count)
        {
        }

        public void Write(NetworkInstanceId value)
        {
        }

        public void Write(NetworkSceneId value)
        {
        }

        public void Write(Plane value)
        {}

        public void Write(Ray value)
        {}

        public void Write(NetworkHash128 value)
        {}

        public void Write(GameObject value)
        {}

        public void Write(NetworkIdentity value)
        {}

        public void Write(Transform value)
        {}

        public void WriteBytesAndSize(byte[] buffer, int count)
        {
        }

        public void WriteBytesFull(byte[] buffer)
        {
        }

        public void Write(Vector2 value)
        {
        }

        public void Write(Vector3 value)
        {
        }

        public void Write(Vector4 value)
        {
        }

        public void Write(Color value)
        {
        }

        public void Write(Color32 value)
        {
        }

        public void Write(Quaternion value)
        {
        }

        public void Write(Rect value)
        {
        }

        public void Write(Matrix4x4 value)
        {
        }

        public void Write(MessageBase msg)
        {
        }

        public void StartMessage(short msgType)
        {
        }

        public void FinishMessage()
        {
        }
    }

    public class NetworkClient
    {
        public static bool active { get { return false; } }
    }

    public sealed class NetworkServer
    {
        public static bool active { get { return false; } }
        public static bool localClientActive { get { return false; } }
    }

    abstract public class SyncList<T> : IList<T>
    {
        public void Add(T t)
        {}

        public void Clear()
        {}

        public bool Contains(T t)
        {
            return false;
        }

        public void CopyTo(T[] a, int i)
        {}

        public int Count { get { return 0; } }

        public bool IsReadOnly { get { return false; } }

        public bool Remove(T t)
        {
            return false;
        }

        IEnumerator<T> System.Collections.Generic.IEnumerable<T>.GetEnumerator()
        {
            return new List<T>().GetEnumerator();
        }

        IEnumerator System.Collections.IEnumerable.GetEnumerator()
        {
            return new List<T>().GetEnumerator();
        }

        public int IndexOf(T t)
        {
            return -1;
        }

        public void Insert(int i, T t)
        {}

        public void RemoveAt(int i)
        {}

        public T this[int i] { get { return default(T); } set {} }

        public void InitializeBehaviour(NetworkBehaviour beh, int cmdHash)
        {}

        public void HandleMsg(NetworkReader reader)
        {}
    }

    public class SyncListBool : SyncList<bool>
    {
        public static void ReadReference(NetworkReader reader, SyncListBool syncList)
        {}

        public static void WriteInstance(NetworkWriter writer, SyncListBool syncList)
        {}
    }

    public class SyncListFloat : SyncList<float>
    {
        public static void ReadReference(NetworkReader reader, SyncListFloat syncList)
        {}

        public static void WriteInstance(NetworkWriter writer, SyncListFloat syncList)
        {}
    }

    public class SyncListInt : SyncList<int>
    {
        public static void ReadReference(NetworkReader reader, SyncListInt syncList)
        {}

        public static void WriteInstance(NetworkWriter writer, SyncListInt syncList)
        {}
    }

    public class SyncListUInt : SyncList<uint>
    {
        public static void ReadReference(NetworkReader reader, SyncListUInt syncList)
        {}

        public static void WriteInstance(NetworkWriter writer, SyncListInt syncList)
        {}
    }

    public class SyncListString : SyncList<string>
    {
        public static void ReadReference(NetworkReader reader, SyncListString syncList)
        {}

        public static void WriteInstance(NetworkWriter writer, SyncListString syncList)
        {}
    }

    public class SyncListStruct<T> : SyncList<T> where T : struct
    {}

    [AttributeUsage(AttributeTargets.Field)]
    public class SyncVarAttribute : Attribute
    {}

    [AttributeUsage(AttributeTargets.Method)]
    public class CommandAttribute : Attribute
    {}

    [AttributeUsage(AttributeTargets.Method)]
    public class ClientRpcAttribute : Attribute
    {
    }

    [AttributeUsage(AttributeTargets.Method)]
    public class TargetRpcAttribute : Attribute
    {
    }

    [AttributeUsage(AttributeTargets.Class)]
    public class NetworkSettingsAttribute : Attribute
    {}
}

// ReSharper restore CheckNamespace
