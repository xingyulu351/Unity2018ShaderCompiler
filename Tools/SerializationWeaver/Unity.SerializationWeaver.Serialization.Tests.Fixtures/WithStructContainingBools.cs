using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Unity.SerializationWeaver.Serialization.Tests.Fixtures
{
    [Serializable]
    public struct Struct : UnityEngine.ISerializationCallbackReceiver
    {
        public int intField1;
        public bool boolField1;
        public int intField2;
        public bool boolField2;
        public int intField3;

        public void OnAfterDeserialize()
        {}

        public void OnBeforeSerialize()
        {}
    }

    public class WithStructContainingBools
    {
        public Struct structField;

        public void Test()
        {
            structField.intField1 = 1;
            structField.boolField1 = true;
            structField.intField2 = 2;
            structField.boolField2 = true;
            structField.intField3 = 3;
        }
    }
}
