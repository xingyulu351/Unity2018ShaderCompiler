// ReSharper disable CheckNamespace

using System;
using System.Collections.Generic;

namespace UnityEngine
{
    public struct Vector2
    {
    }

    public struct Vector3
    {
        public float x;
        public float y;
        public float z;
    }

    public struct Vector4
    {
    }

    public struct Rect
    {
    }

    public struct Quaternion
    {
    }

    public struct Matrix4x4
    {
    }

    public struct Color
    {
    }

    public struct Color32
    {
    }

    public struct LayerMask
    {
    }

    public struct Ray
    {}

    public class Object
    {
        public static bool operator==(Object x, Object y)
        {
            return false;
        }

        public static bool operator!=(Object x, Object y)
        {
            return true;
        }
    }

    public class Component : Object
    {
        public T GetComponent<T>()
        {
            return default(T);
        }
    }

    public class GameObject : Object
    {
    }

    public class Plane : GameObject
    {}

    public class MonoBehaviour : Component
    {
    }

    public class ScriptableObject : Object
    {
    }

    public class AnimationCurve : Object
    {
    }

    public class Gradient : Object
    {
    }

    public class GUIStyle : Object
    {
    }

    public class RectOffset : Object
    {
    }

    public sealed class SerializeField : Attribute
    {
    }

    public class Texture : Object
    {
    }

    public class Texture2D : Texture
    {
    }

    public class Transform : Component
    {}

    public static class Debug
    {
        public static void LogError(string s)
        {}

        public static void LogWarning(string s)
        {}
    }

    public interface IUnitySerializable
    {
        void Unity_Serialize(int depth, bool cloningObject);
        void Unity_Deserialize(int depth, bool cloningObject);
        void Unity_RemapPPtrs(int depth, bool cloningObject);
    }

    public interface IPPtrRemapper
    {
        object GetNewInstanceToReplaceOldInstance(object value);
        bool CanExecuteSerializationCallbacks();
    }

    public interface ISerializedStateReader
    {
        void Align();
        sbyte ReadSByte();
        byte ReadByte();
        char ReadChar();
        Int16 ReadInt16();
        UInt16 ReadUInt16();
        int ReadInt32();
        uint ReadUInt32();
        Int64 ReadInt64();
        UInt64 ReadUInt64();
        float ReadSingle();
        double ReadDouble();
        bool ReadBoolean();
        string ReadString();
        object ReadUnityEngineObject();
        object ReadAnimationCurve();
        object ReadGradient();
        object ReadGUIStyle();
        object ReadRectOffset();
        Color32 ReadColor32();
        Matrix4x4 ReadMatrix4x4();
        byte[] ReadArrayOfByte();
        List<byte> ReadListOfByte();
    }

    public interface ISerializedStateWriter
    {
        void Align();
        void WriteBoolean(bool value);
        void WriteSByte(sbyte value);
        void WriteByte(byte value);
        void WriteChar(char value);
        void WriteInt16(Int16 value);
        void WriteUInt16(UInt16 value);
        void WriteInt32(int value);
        void WriteUInt32(uint value);
        void WriteInt64(Int64 value);
        void WriteUIn64(UInt64 value);
        void WriteSingle(float value);
        void WriteDouble(double value);
        void WriteString(string value);
        void WriteUnityEngineObject(object value);
        void WriteAnimationCurve(object value);
        void WriteGradient(object value);
        void WriteGUIStyle(object value);
        void WriteRectOffset(object value);
        void WriteColor32(Color32 value);
        void WriteMatrix4x4(ref Matrix4x4 value);
        void WriteArrayOfByte(byte[] value);
        void WriteListOfByte(List<byte> value);
    }

    public interface ISerializedNamedStateWriter
    {
        void Align();
        void WriteByte(byte value, IntPtr fieldName);
        void WriteSByte(sbyte value, IntPtr fieldName);
        void WriteChar(char value, IntPtr fieldName);
        void WriteInt16(short value, IntPtr fieldName);
        void WriteUInt16(ushort value, IntPtr fieldName);
        void WriteInt32(int value, IntPtr fieldName);
        void WriteUInt32(uint value, IntPtr fieldName);
        void WriteInt64(long value, IntPtr fieldName);
        void WriteUInt64(ulong value, IntPtr fieldName);
        void WriteSingle(float value, IntPtr fieldName);
        void WriteDouble(double value, IntPtr fieldName);
        void WriteBoolean(bool value, IntPtr fieldName);
        void WriteString(string value, IntPtr fieldName);
        void WriteUnityEngineObject(object value, IntPtr fieldName);
        void WriteAnimationCurve(object value, IntPtr fieldName);
        void WriteGradient(object value, IntPtr fieldName);
        void WriteGUIStyle(object value, IntPtr fieldName);
        void WriteRectOffset(object value, IntPtr fieldName);
        void WriteMatrix4x4(ref Matrix4x4 value, IntPtr fieldName);
        void WriteColor32(Color32 value, IntPtr fieldName);
        void WriteArrayOfByte(byte[] value, IntPtr fieldName);
        void WriteListOfByte(List<byte> value, IntPtr fieldName);
        void BeginMetaGroup(IntPtr fieldName);
        void EndMetaGroup();
        void BeginSequenceGroup(IntPtr fieldName, int sequenceLength);
    }

    public interface ISerializedNamedStateReader
    {
        void Align();
        sbyte ReadSByte(IntPtr fieldName);
        byte ReadByte(IntPtr fieldName);
        char ReadChar(IntPtr fieldName);
        short ReadInt16(IntPtr fieldName);
        ushort ReadUInt16(IntPtr fieldName);
        int ReadInt32(IntPtr fieldName);
        uint ReadUInt32(IntPtr fieldName);
        long ReadInt64(IntPtr fieldName);
        ulong ReadUInt64(IntPtr fieldName);
        float ReadSingle(IntPtr fieldName);
        double ReadDouble(IntPtr fieldName);
        bool ReadBoolean(IntPtr fieldName);
        string ReadString(IntPtr fieldName);
        object ReadUnityEngineObject(IntPtr fieldName);
        object ReadAnimationCurve(IntPtr fieldName);
        object ReadGradient(IntPtr fieldName);
        object ReadGUIStyle(IntPtr fieldName);
        object ReadRectOffset(IntPtr fieldName);
        Matrix4x4 ReadMatrix4x4(IntPtr fieldName);
        Color32 ReadColor32(IntPtr fieldName);
        byte[] ReadArrayOfByte(IntPtr fieldName);
        List<byte> ReadListOfByte(IntPtr fieldName);
        void BeginMetaGroup(IntPtr fieldName);
        void EndMetaGroup();
        int BeginSequenceGroup(IntPtr fieldName);
    }

    public interface ISerializationCallbackReceiver
    {
        void OnBeforeSerialize();
        void OnAfterDeserialize();
    }
}

namespace UnityEngine.Serialization
{
    public sealed class PPtrRemapper : IPPtrRemapper
    {
        public static IPPtrRemapper Instance = null;

        public object GetNewInstanceToReplaceOldInstance(object value)
        {
            return null;
        }

        public bool CanExecuteSerializationCallbacks()
        {
            return true;
        }
    }

    public class SerializedStateReader : ISerializedStateReader
    {
        public static ISerializedStateReader Instance = null;

        public void Align()
        {
        }

        public sbyte ReadSByte()
        {
            return 0;
        }

        public byte ReadByte()
        {
            return 0;
        }

        public char ReadChar()
        {
            return '\0';
        }

        public Int16 ReadInt16()
        {
            return 0;
        }

        public UInt16 ReadUInt16()
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

        public Int64 ReadInt64()
        {
            return 0;
        }

        public UInt64 ReadUInt64()
        {
            return 0;
        }

        public float ReadSingle()
        {
            return 0;
        }

        public double ReadDouble()
        {
            return 0;
        }

        public bool ReadBoolean()
        {
            return false;
        }

        public string ReadString()
        {
            return null;
        }

        public object ReadUnityEngineObject()
        {
            return null;
        }

        public object ReadAnimationCurve()
        {
            return null;
        }

        public object ReadGradient()
        {
            return null;
        }

        public object ReadGUIStyle()
        {
            return null;
        }

        public Color32 ReadColor32()
        {
            Color32 c;
            return c;
        }

        public Matrix4x4 ReadMatrix4x4()
        {
            Matrix4x4 c;
            return c;
        }

        public object ReadRectOffset()
        {
            return null;
        }

        public byte[] ReadArrayOfByte()
        {
            return null;
        }

        public List<byte> ReadListOfByte()
        {
            return null;
        }
    }

    public class SerializedStateWriter : ISerializedStateWriter
    {
        public static ISerializedStateWriter Instance = null;

        public void Align()
        {
        }

        public void WriteSByte(sbyte value)
        {
        }

        public void WriteByte(byte value)
        {
        }

        public void WriteChar(char value)
        {
        }

        public void WriteInt16(short value)
        {
        }

        public void WriteUInt16(ushort value)
        {
        }

        public void WriteInt32(int value)
        {
        }

        public void WriteUInt32(uint value)
        {
        }

        public void WriteInt64(long value)
        {
        }

        public void WriteUIn64(ulong value)
        {
        }

        public void WriteSingle(float value)
        {
        }

        public void WriteDouble(double value)
        {
        }

        public void WriteBoolean(bool value)
        {
        }

        public void WriteString(string value)
        {
        }

        public void WriteUnityEngineObject(object value)
        {
        }

        public void WriteAnimationCurve(object value)
        {
        }

        public void WriteGradient(object value)
        {
        }

        public void WriteGUIStyle(object value)
        {
        }

        public void WriteRectOffset(object value)
        {
        }

        public void WriteColor32(Color32 value)
        {
        }

        public void WriteMatrix4x4(ref Matrix4x4 value)
        {
        }

        public void WriteArrayOfByte(byte[] value)
        {
        }

        public void WriteListOfByte(List<byte> value)
        {
        }
    }

    public class SerializedNamedStateReader : ISerializedNamedStateReader
    {
        public static ISerializedNamedStateReader Instance = null;

        public void Align()
        {
        }

        public sbyte ReadSByte(IntPtr fieldName)
        {
            return 0;
        }

        public byte ReadByte(IntPtr fieldName)
        {
            return 0;
        }

        public char ReadChar(IntPtr fieldName)
        {
            return '\0';
        }

        public short ReadInt16(IntPtr fieldName)
        {
            return 0;
        }

        public ushort ReadUInt16(IntPtr fieldName)
        {
            return 0;
        }

        public int ReadInt32(IntPtr fieldName)
        {
            return 0;
        }

        public uint ReadUInt32(IntPtr fieldName)
        {
            return 0;
        }

        public long ReadInt64(IntPtr fieldName)
        {
            return 0;
        }

        public ulong ReadUInt64(IntPtr fieldName)
        {
            return 0;
        }

        public float ReadSingle(IntPtr fieldName)
        {
            return 0.0f;
        }

        public double ReadDouble(IntPtr fieldName)
        {
            return 0.0;
        }

        public bool ReadBoolean(IntPtr fieldName)
        {
            return false;
        }

        public string ReadString(IntPtr fieldName)
        {
            return null;
        }

        public object ReadUnityEngineObject(IntPtr fieldName)
        {
            return null;
        }

        public object ReadAnimationCurve(IntPtr fieldName)
        {
            return null;
        }

        public object ReadGradient(IntPtr fieldName)
        {
            return null;
        }

        public object ReadGUIStyle(IntPtr fieldName)
        {
            return null;
        }

        public object ReadRectOffset(IntPtr fieldName)
        {
            return null;
        }

        public Matrix4x4 ReadMatrix4x4(IntPtr fieldName)
        {
            return default(Matrix4x4);
        }

        public Color32 ReadColor32(IntPtr fieldName)
        {
            return default(Color32);
        }

        public byte[] ReadArrayOfByte(IntPtr fieldName)
        {
            return null;
        }

        public List<byte> ReadListOfByte(IntPtr fieldName)
        {
            return null;
        }

        public void BeginMetaGroup(IntPtr fieldName)
        {
        }

        public void EndMetaGroup()
        {
        }

        public int BeginSequenceGroup(IntPtr fieldName)
        {
            return 0;
        }
    }

    public class SerializedNamedStateWriter : ISerializedNamedStateWriter
    {
        public static ISerializedNamedStateWriter Instance = null;

        public void Align()
        {
        }

        public void WriteByte(byte value, IntPtr fieldName)
        {
        }

        public void WriteSByte(sbyte value, IntPtr fieldName)
        {
        }

        public void WriteChar(char value, IntPtr fieldName)
        {
        }

        public void WriteInt16(short value, IntPtr fieldName)
        {
        }

        public void WriteUInt16(ushort value, IntPtr fieldName)
        {
        }

        public void WriteInt32(int value, IntPtr fieldName)
        {
        }

        public void WriteUInt32(uint value, IntPtr fieldName)
        {
        }

        public void WriteInt64(long value, IntPtr fieldName)
        {
        }

        public void WriteUInt64(ulong value, IntPtr fieldName)
        {
        }

        public void WriteSingle(float value, IntPtr fieldName)
        {
        }

        public void WriteDouble(double value, IntPtr fieldName)
        {
        }

        public void WriteBoolean(bool value, IntPtr fieldName)
        {
        }

        public void WriteString(string value, IntPtr fieldName)
        {
        }

        public void WriteUnityEngineObject(object value, IntPtr fieldName)
        {
        }

        public void WriteAnimationCurve(object value, IntPtr fieldName)
        {
        }

        public void WriteGradient(object value, IntPtr fieldName)
        {
        }

        public void WriteGUIStyle(object value, IntPtr fieldName)
        {
        }

        public void WriteRectOffset(object value, IntPtr fieldName)
        {
        }

        public void WriteMatrix4x4(ref Matrix4x4 value, IntPtr fieldName)
        {
        }

        public void WriteColor32(Color32 value, IntPtr fieldName)
        {
        }

        public void WriteArrayOfByte(byte[] value, IntPtr fieldName)
        {
        }

        public void WriteListOfByte(List<byte> value, IntPtr fieldName)
        {
        }

        public void BeginMetaGroup(IntPtr fieldName)
        {
        }

        public void EndMetaGroup()
        {
        }

        public void BeginSequenceGroup(IntPtr fieldName, int sequenceLength)
        {
        }
    }
}

// ReSharper restore CheckNamespace
