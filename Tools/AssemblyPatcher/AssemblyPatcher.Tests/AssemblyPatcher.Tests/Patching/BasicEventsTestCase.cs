using System.Text;
using Mono.Cecil;
using NUnit.Framework;

namespace AssemblyPatcher.Tests
{
    [TestFixture]
    public class BasicEventsTestCase : AssemblyPatcherTestBase
    {
        [Test]
        public void Test()
        {
            Test("TestScenarios/BasicEvents.cs");

            Assert.That(sb.ToString().AsUnixString(), Is.EqualTo(
@"[FieldType] fldA:B
[VarType] System.String BasicEvents::Run() V_0 A/B
[MethodRef] System.String BasicEvents::Run() System.Void B::Method1()/System.Void <Module>::UnsupportedMember_Void(System.String)
[FieldRef] System.String BasicEvents::Run() System.String A::Value/System.String B::Value
[MethodRef] System.String BasicEvents::Run() System.String A::Method()/System.String B::Method()
[CatchType] System.String BasicEvents::Run() System.MissingMethodException/System.MissingMemberException
[ParamType] ReturnA(a) A/B
[ReturnType] ReturnA A/B
[MethodRef] System.Void BasicEvents::.ctor() System.Void A::.ctor()/System.Void B::.ctor()
[BaseType] DerivedFromA B
[MethodRef] System.Void DerivedFromA::.ctor() System.Void A::.ctor()/System.Void B::.ctor()
".AsUnixString()), sb.ToString());
        }

        internal override void Configure(AssemblyPatcher patcher)
        {
            patcher.BaseTypePatchingEvent += (sender, args) => sb.AppendFormat("[BaseType] {0} {1}\r\n", args.PatchedItem.Name, args.Item.Name);
            patcher.CatchTypePatchingEvent += (sender, args) => sb.AppendFormat("[CatchType] {0} {1}/{2}\r\n", args.Method.FullName, args.Item.CatchType.FullName, args.NewType.FullName);
            patcher.FieldReferencePatching += (sender, args) => sb.AppendFormat("[FieldRef] {0} {1}/{2}\r\n", args.PatchedItem.FullName, args.Instruction.Operand, args.Item.FullName);
            patcher.FieldTypePatchingEvent += (sender, args) => sb.AppendFormat("[FieldType] {0}:{1}\r\n", args.PatchedItem.Name, args.Item.Name);
            patcher.LocalVariableTypePatching += (sender, args) => sb.AppendFormat("[VarType] {0} {1} {2}/{3}\r\n", args.DeclaringMethod.FullName, AssemblyPatcher.GetVariableName(args.Item, args.DeclaringMethod), args.Item.VariableType.Name, args.NewType.Name);
            patcher.MethodReferencePatching += (sender, args) => sb.AppendFormat("[MethodRef] {0} {1}/{2}\r\n", args.PatchedItem.FullName, args.Instruction.Operand, args.Item.FullName);
            patcher.MethodReturnTypePatching += (sender, args) => sb.AppendFormat("[ReturnType] {0} {1}/{2}\r\n", args.PatchedItem.Name, MethodReference(args.PatchedItem).ReturnType.Name, args.Item.Name);
            patcher.ParameterTypePatching += (sender, args) => sb.AppendFormat("[ParamType] {0}({1}) {2}/{3}\r\n", args.Method.Name, args.Item.Name, args.Item.ParameterType.FullName, args.NewType.FullName);
        }

        private MethodReference MethodReference(MemberReference memberReference)
        {
            return (MethodReference)memberReference;
        }

        private StringBuilder sb = new StringBuilder();
    }

    static class StringExtensions
    {
        public static string AsUnixString(this string str)
        {
            return str.Replace("\r\n", "\n");
        }
    }
}
