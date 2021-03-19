using UnityEngine;

namespace Unity.SerializationWeaver.Serialization.Tests.Fixtures
{
    public class BaseClass<T> : MonoBehaviour
    {
        public T GenericField;
    }

    public class WithGenericBaseClass : BaseClass<float>
    {
        public float Field;

        public void Test()
        {
            Field = 1;
            GenericField = 2;
        }
    }
}
