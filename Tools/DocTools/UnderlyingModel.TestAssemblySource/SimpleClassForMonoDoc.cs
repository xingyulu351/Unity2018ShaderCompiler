using System;
using System.Collections.Generic;
using uei = UnityEngine.Internal;

public class SimpleClassForMonoDoc
{
    public SimpleClassForMonoDoc()
    {
    }

    public float FieldA;
    public int FieldB;
    public bool fieldC;
    public float PropertyA { get; set; }
    public int PropertyB { get { return 5; } }

    public void Foo() {}
    public void BarInt(int arg) {}
    public void BarBool(bool arg) {}

    public void Baz(int a) {}
    public void Baz(float b) {}

    public void Qux(int a) {}
    public void Qux(float b) {}

    public int FooBarBaz(int foo, float bar, bool baz) {return 5; }

    public void OptionalParameters(int a = 8, int b = 0, int c = 4) {}

    public void DefaultValues([uei.DefaultValue("2")] int a, [uei.DefaultValue("4")] int b) {}

    public void DocsWithMarkup() {}

    public void GenericParameter(int a, IDictionary<int, int> dic) {}

    public void MultiDimArray(int a, int[,,,] b) {}
}

public struct SimpleStructForMonoDoc
{
    public float FieldA;
}

public enum UEnum
{
    One,
    Two,
    Three
}

namespace UnityEngine.Internal
{
    [AttributeUsage(AttributeTargets.Parameter | AttributeTargets.GenericParameter)]
    public class DefaultValueAttribute : Attribute
    {
        private object DefaultValue;

        public DefaultValueAttribute(string value)
        {
            DefaultValue = value;
        }

        public object Value
        {
            get { return DefaultValue; }
        }

        public override bool Equals(object obj)
        {
            DefaultValueAttribute dva = (obj as DefaultValueAttribute);
            if (dva == null)
                return false;

            if (DefaultValue == null)
                return (dva.Value == null);

            return DefaultValue.Equals(dva.Value);
        }

        public override int GetHashCode()
        {
            if (DefaultValue == null)
                return base.GetHashCode();
            return DefaultValue.GetHashCode();
        }
    }
}


namespace UnityEngine
{
    public class UClass
    {
        public delegate void MyDelegate();
        public UClass() {}
        public UClass(int para) {}

        public void Foo() {}

        public int Prop { get { return 5; } }
        public float fieldA;
    };
}

namespace UnityEngine.UI
{
    public class UIDummyClass
    {
        public enum UIDummyEnum
        {
            Val0,
            Val1
        }
    };
}
