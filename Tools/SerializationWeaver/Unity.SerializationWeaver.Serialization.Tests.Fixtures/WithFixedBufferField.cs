using UnityEngine;

namespace Unity.SerializationWeaver.Serialization.Tests.Fixtures
{
    public unsafe struct FixedBuffer
    {
        public const int bufferSize = 4;
        public fixed int buffer[bufferSize];
    }

    public class WithFixedBufferField : MonoBehaviour
    {
        public FixedBuffer fixedBuffer;
    }
}
