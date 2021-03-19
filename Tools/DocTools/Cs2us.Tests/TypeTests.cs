using NUnit.Framework;

namespace Cs2us.Tests
{
    [TestFixture]
    public class TypeTests : ClassBasedTests
    {
        [TestCase("EmptyClass", @"class C{};", @"class C {
}")]
        [TestCase("ClassOneSimpleMethod",
@"class C {
    void Foo() {
    }
}",
@"class C {
    function Foo() {
    }
}")]
        [TestCase("ClassOneSimpleMethodWithReturn",
@"class C {
    int Foo() {
        return 5;
    }
}",
@"class C {
    function Foo() {
        return 5;
    }
}")]
        [TestCase("NestedClass",
@"class C {
    class D {
    }
}",
@"class C {
    class D {
    }
}")]
        [TestCase("ExampleWontEmitAClass",
@"class ExampleClass {
    void Example() {
        int k = 0;
    }
}",
            @"var k = 0;")]

        [TestCase("PublicClassModifier",
@"public class C {
}",
@"public class C {
}")]

        [TestCase("SealedClassModifier",
@"public sealed class C {
}",
@"public final class C {
}")]

        [TestCase("StaticClassModifier",
@"internal static class C {
}",
@"internal static class C {
}")]

        [TestCase("ClassExtendsAnother",
@"class C : D {
}",
@"class C extends D {
}")]
        [TestCase("ClassExtendsMonobehaviour",
@"class C : MonoBehaviour {
}",
@"class C extends MonoBehaviour {
}")]

        [TestCase("SimpleInterface",
            @"interface I {};",
@"interface I {
}")]

        [TestCase("ClassImplementsInterface",
@"interface IImplementable {};
class C : IImplementable {
}",
@"interface IImplementable {
}
class C implements IImplementable {
}")]

        [TestCase("SimpleNamespace",
@"namespace UnityEditor {
    class A {}
};",
@"class A {
}")]

        [TestCase("SimpleasStatement",
@"class ExampleClass {
    void Example() {
         var texture = Instantiate(rend.material.mainTexture) as Texture2D;
    }
}",
            @"var texture = Instantiate(rend.material.mainTexture) as Texture2D;")]

        [TestCase("StructBareBones",
@"public struct S {
}",
@"public class S extends System.ValueType {
}")]

        [TestCase("StructOneMember",
@"public struct S
{
    int score;
}",
@"public class S extends System.ValueType {
    var score;
}")]

        [TestCase("StructOneMemberWithModifier",
@"public struct S
{
    internal int score;
}",
@"public class S extends System.ValueType {
    internal var score;
}")]

        [TestCase("StructTwoMembers",
@"public struct S
{
    int score;
    bool flag;
}",
@"public class S extends System.ValueType {
    var score;
    var flag;
}")]

        public void TypeTest(string title, string input, string expected)
        {
            TypeTest(title, input, expected, false);
        }

        [TestCase("Unetwork.Userver.SentToReady",
@"public class MyMsgTypes
        {
            public static short MSG_LOGIN_RESPONSE = 1000;
            public static short MSG_SCORE = 1005;
        };

        public struct ScoreMessage
        {
            public int score;
            public Vector3 scorePos;
        }

        class GameServer
        {
            Vector3 myServer;

            void SendScore(int score, Vector3 scorePos)
            {
                ScoreMessage msg;
                msg.score = score;
                msg.scorePos = scorePos;

                myServer.x = 4.0;
                myServer.y = 5.0;
            }
        }",

@"public class MyMsgTypes {
    public static var MSG_LOGIN_RESPONSE: short = 1000;
    public static var MSG_SCORE: short = 1005;
}
public class ScoreMessage extends System.ValueType {
    public var score: int;
    public var scorePos: Vector3;
}
class GameServer {
    var myServer: Vector3;
    function SendScore(score: int, scorePos: Vector3) {
        var msg: ScoreMessage;
        msg.score = score;
        msg.scorePos = scorePos;
        myServer.x = 4.0;
        myServer.y = 5.0;
    }
}", true)]
        public void TypeTest(string title, string input, string expected, bool explicitlyTyped)
        {
            AssertConversion(input, expected, explicitlyTyped);
        }
    }
}
