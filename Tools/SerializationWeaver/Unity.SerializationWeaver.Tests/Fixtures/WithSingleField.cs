using UnityEngine;

namespace Unity.SerializationWeaver.Tests.Fixtures
{
    // ReSharper disable InconsistentNaming
    public class WithSingleField
    {
        public class Fixtures
        {
            public class MyMonoBehaviour : MonoBehaviour
            {
                public int Field;

                public MyMonoBehaviour()
                {
                }
            }
        }

        public class Expectations
        {
            public class MyMonoBehaviour : MonoBehaviour, IUnitySerializable
            {
                public int Field;

                public MyMonoBehaviour()
                {
                }

                public void Unity_Serialize()
                {
                    SerializedStateWriter.Instance.WriteInt32(Field);
                }

                public void Unity_Deserialize()
                {
                    Field = SerializedStateReader.Instance.ReadInt32();
                }

                public void Unity_RemapPPtrs()
                {
                }
            }
        }
    }
    // ReSharper restore InconsistentNaming
}
