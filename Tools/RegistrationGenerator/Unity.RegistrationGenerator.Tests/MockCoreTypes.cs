using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace UnityEngine
{
    public partial class Object
    {
    }

    public partial class Component : Object
    {
    }

    public partial class Behaviour : Component
    {
    }

    public partial class MonoBehaviour : Behaviour
    {
    }

    public class MonoBehaviourDerivedTestClass : MonoBehaviour
    {
    }

    public struct TestClass1
    {
    }

    [NativeClass("NativeClass2")]
    public struct TestClass2
    {
    }

    [NativeClass("MyNamespace::InnerNamespace::NativeClass2")]
    public class TestClass3
    {
    }

    [NativeClass("MyNamespace::InnerNamespace::NativeClassObjectDerived")]
    public class TestClassObjectDerived : Object
    {
    }

    public struct TestStruct1
    {
    }

    [NativeClass("NativeStruct2")]
    public struct TestStruct2
    {
    }

    [NativeClass("MyNamespace::InnerNamespace::NativeStruct2")]
    public struct TestStruct3
    {
    }
}
