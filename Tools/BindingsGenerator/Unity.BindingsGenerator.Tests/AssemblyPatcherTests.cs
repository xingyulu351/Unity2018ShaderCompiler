using System;
using System.IO;
using System.Linq;
using Mono.Cecil;
using Mono.Cecil.Cil;
using Mono.Collections.Generic;
using NUnit.Compatibility;
using NUnit.Framework;
using Unity.BindingsGenerator.AssemblyPatcherTestAssembly;
using Unity.BindingsGenerator.Core;
using Unity.BindingsGenerator.Core.AssemblyPatcher;
using UnityEngine.Bindings;
using Unity.BindingsGenerator.TestFramework;

namespace Unity.BindingsGenerator.Tests
{
    [TestFixture]
    class AssemblyPatcherTests : BindingsGeneratorTestsBase
    {
        #pragma warning disable 169

        private class AttributeChecker : Visitor
        {
            private static string[] OkAttributeNames =
            {
                nameof(SomeAttribute),
                nameof(ObsoleteAttribute),
                nameof(NativeTypeAttribute),
                nameof(NativeWritableSelfAttribute)
            };
            private static void AssertHasAttribute<T>(ICustomAttributeProvider customAttributeProvider)
            {
                Assert.True(customAttributeProvider.HasAttribute<T>(false));
            }

            private static void AssertDoesntHaveUnexpectedAttributes(Collection<CustomAttribute> collection)
            {
                var res = collection.FirstOrDefault(attr => !OkAttributeNames.Contains(attr.AttributeType.Name));
                Assert.IsNull(res, res == null ? "" : String.Format("Unexpected attribute '{0}'", res.AttributeType.Name));
            }

            public override void Visit(TypeDefinition typeDefinition)
            {
                if (!typeDefinition.FullName.Contains("ClassWithAttributes"))
                    return;

                AssertHasAttribute<SomeAttribute>(typeDefinition);
                AssertHasAttribute<ObsoleteAttribute>(typeDefinition);
                AssertHasAttribute<NativeTypeAttribute>(typeDefinition);

                AssertDoesntHaveUnexpectedAttributes(typeDefinition.CustomAttributes);
            }

            public override void Visit(MethodDefinition methodDefinition)
            {
                if (!methodDefinition.FullName.Contains("ClassWithAttributes"))
                    return;

                if (methodDefinition.FullName.Contains(Naming.ByRefInjectedMethod("")))
                    return;

                if (methodDefinition.Name == ".ctor")
                    return;

                AssertHasAttribute<SomeAttribute>(methodDefinition);
                AssertHasAttribute<NativeWritableSelfAttribute>(methodDefinition);
                AssertDoesntHaveUnexpectedAttributes(methodDefinition.CustomAttributes);
            }

            public override void Visit(PropertyDefinition propertyDefinition)
            {
                if (!propertyDefinition.FullName.Contains("ClassWithAttributes"))
                    return;

                AssertHasAttribute<SomeAttribute>(propertyDefinition);
                AssertDoesntHaveUnexpectedAttributes(propertyDefinition.CustomAttributes);
            }
        }

        [Test]
        [NUnit.Framework.Ignore("Attribute stripping is disabled")]
        public void AllAttributesExceptTestAreStripped()
        {
            #pragma warning disable 612
            var klass = TypeResolver.TypeDefinitionFor(typeof(ClassWithAttributes));

            new AssemblyPatcher(new NonFilteringVisitor(klass), ScriptingBackend.Mono).Patch();
            new NonFilteringVisitor(klass).Accept(new AttributeChecker());
        }

        const string TestAssemblyPath = @"Unity.BindingsGenerator.AssemblyPatcherTestAssembly.dll";

        AssemblyDefinition ReadTestAssembly()
        {
            return TypeResolver.LoadAssembly(TestAssemblyPath);
        }

        [TestCase("ClassWithInstanceStructUser")]
        [TestCase("ClassWithStaticStructUser")]
        [TestCase("ClassWithStaticMultipleStructUser")]
        [TestCase("ClassWithNoByValueStructUser")]
        [TestCase("ClassWithInstanceReturnStructUser")]
        [TestCase("ClassWithStaticReturnStructUser")]
        [TestCase("ClassWithInstancePropertyStructUser")]
        [TestCase("ClassWithMixedStructUser")]
        [TestCase("ClassWithInstanceStructUserAndAttributes")]
        [TestCase("ClassWithInstanceStructUserAndParameterWithDefaultValue")]
        [TestCase("ClassWithStructProperty")]
        [TestCase("ClassWithAttributedMethod")]
        [TestCase("BlittableStructWithExternInstanceMethod")]
        public void StructUsagePatching(string typeName)
        {
            var assembly = ReadTestAssembly();

            var patchedClass = GetType(assembly, String.Format("Unity.BindingsGenerator.AssemblyPatcherTestAssembly.{0}", typeName));
            var expectedClass = GetType(assembly, String.Format("Unity.BindingsGenerator.AssemblyPatcherTestAssembly.{0}Expected", typeName));

            new AssemblyPatcher(new FilteringVisitor(patchedClass), ScriptingBackend.Mono).Patch();

            AssemblyPatcherAssertEqual.AssertEqual(expectedClass, patchedClass);
        }

        private static TypeDefinition GetType(AssemblyDefinition assembly, string name)
        {
            return assembly.MainModule.Types.Single(t => t.FullName == name);
        }

        [TestCase("DefaultParameterOverloadSimple")]
        [TestCase("DefaultParameterOverloadTriple")]
        [TestCase("DefaultParameterOverloadMixed")]
        [TestCase("DefaultParameterOverloadWithAttribute")]
        [TestCase("DefaultParameterOverloadByte")]
        [TestCase("DefaultParameterOverloadSByte")]
        [TestCase("DefaultParameterOverloadChar")]
        [TestCase("DefaultParameterOverloadBoolean")]
        [TestCase("DefaultParameterOverloadInt32")]
        [TestCase("DefaultParameterOverloadUInt32")]
        [TestCase("DefaultParameterOverloadInt64")]
        [TestCase("DefaultParameterOverloadUInt64")]
        [TestCase("DefaultParameterOverloadSingle")]
        [TestCase("DefaultParameterOverloadDouble")]
        [TestCase("DefaultParameterOverloadString")]
        [TestCase("DefaultParameterOverloadClass")]
        [TestCase("DefaultParameterOverloadEnum")]
        [NUnit.Framework.Ignore("Default parameters expansion is now disabled in the bindings generator")]
        public void SimpleDefaultOverload(string typeName)
        {
            var assembly = ReadTestAssembly();

            var patchedClass = GetType(assembly, string.Format("Unity.BindingsGenerator.AssemblyPatcherTestAssembly.{0}", typeName));
            var expectedClass = GetType(assembly, string.Format("Unity.BindingsGenerator.AssemblyPatcherTestAssembly.{0}Expected", typeName));

            new AssemblyPatcher(new FilteringVisitor(patchedClass), ScriptingBackend.Mono).Patch();

            AssemblyPatcherAssertEqual.AssertEqual(expectedClass, patchedClass);
        }

        private void AssertConstantLoad(MethodDefinition method, object values, TypeDefinition typeDefinition)
        {
            int instructionIndex = 0;

            var valuesArray = values as Array;

            foreach (object value in valuesArray)
            {
                var instructions = CodeGenHelper.LoadConstantInstructionFor(typeDefinition, value);

                Assert.IsTrue(instructions.Count != 0);

                foreach (var instruction in instructions)
                {
                    AssertConstantLoadInstructionsAreEqual(method.Body.Instructions[instructionIndex], instruction);
                    instructionIndex++;
                }
            }

            // Only call and return instructions should be left
            Assert.AreEqual(method.Body.Instructions.Count, instructionIndex + 2);
        }

        private bool IsConstantLoadEmptyStringInstruction(Mono.Cecil.Cil.Instruction instruction)
        {
            if (instruction.OpCode == OpCodes.Ldstr && (string)instruction.Operand == "")
                return true;

            var operandAsMemberReference = instruction.Operand as MemberReference;

            if (instruction.OpCode == OpCodes.Ldsfld && operandAsMemberReference != null && operandAsMemberReference.FullName == "System.String System.String::Empty")
                return true;

            return false;
        }

        private void AssertConstantLoadInstructionsAreEqual(Mono.Cecil.Cil.Instruction expected, Mono.Cecil.Cil.Instruction actual)
        {
            // This is necessary because Mono and .NET compilers emit slightly different code for empty string constant loading
            // Mono emits ldsfld System.String::Empty, and .NET emits ldstr ""
            // If the differences in code generation become more pronounced, we should find another way to do this test
            if (IsConstantLoadEmptyStringInstruction(expected) && IsConstantLoadEmptyStringInstruction(actual))
                return;

            Assert.AreEqual(expected.OpCode, actual.OpCode);
            Assert.AreEqual(expected.Operand, actual.Operand);
        }

        [Test]
        [TestCase("ConstLoadBool", new bool[] {true, false}, typeof(bool))]
        [TestCase("ConstLoadByte", new byte[] {0, 1, 2, 3, 4, 5, 6, 7, 8, 127, 128, 0xFF}, typeof(byte))]
        [TestCase("ConstLoadSByte", new sbyte[] {-128, -127, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 127}, typeof(sbyte))]
        [TestCase("ConstLoadChar", new char[] {'a', '\u14D6'}, typeof(char))]
        [TestCase("ConstLoadInt16", new short[] { short.MinValue, short.MinValue + 1, -129, -128, -127, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 127, 128, 129, short.MaxValue - 1, short.MaxValue }, typeof(short))]
        [TestCase("ConstLoadUInt16", new ushort[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 127, 128, 129, (ushort)short.MaxValue - 1, (ushort)short.MaxValue, ushort.MaxValue - 1, ushort.MaxValue }, typeof(ushort))]
        [TestCase("ConstLoadInt32", new int[] { int.MinValue, int.MinValue + 1, -129, -128, -127, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 127, 128, 129, int.MaxValue - 1, int.MaxValue }, typeof(int))]
        [TestCase("ConstLoadUInt32", new uint[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 127, 128, 129, int.MaxValue - 1, int.MaxValue, (uint)int.MaxValue + 1, uint.MaxValue - 1, uint.MaxValue }, typeof(uint))]
        [TestCase("ConstLoadInt64", new long[] { long.MinValue, long.MinValue + 1, ((long)int.MinValue) - 1, int.MinValue, int.MinValue + 1, -129, -128, -127, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 127, 128, 129, int.MaxValue - 1, int.MaxValue, ((long)int.MaxValue) + 1, ((long)int.MaxValue) + 2, uint.MaxValue - 1, uint.MaxValue, ((long)uint.MaxValue) + 1, long.MaxValue - 1, long.MaxValue }, typeof(long))]
        [TestCase("ConstLoadUInt64", new ulong[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 127, 128, 129, int.MaxValue - 1, int.MaxValue, ((long)int.MaxValue) + 1, uint.MaxValue - 1, uint.MaxValue, ((long)uint.MaxValue) + 1, long.MaxValue - 1, long.MaxValue, ((ulong)long.MaxValue) + 1, ulong.MaxValue - 1, ulong.MaxValue }, typeof(ulong))]
        [TestCase("ConstLoadSingle", new float[] { 0, 1, -1, float.MinValue, float.MaxValue, float.NaN, float.NegativeInfinity, float.PositiveInfinity, float.Epsilon }, typeof(float))]
        [TestCase("ConstLoadDouble", new double[] { 0, 1, -1, double.MinValue, double.MaxValue, double.NaN, double.NegativeInfinity, double.PositiveInfinity, double.Epsilon }, typeof(double))]
        [TestCase("ConstLoadString", new string[] { null, "", "Str 1", "Str 2" }, typeof(string))]
        [TestCase("ConstLoadClass", new object[] { null }, typeof(object))]
        [TestCase("ConstLoadEnum", new ConstantLoading.SomeEnum[] { ConstantLoading.SomeEnum.A, ConstantLoading.SomeEnum.B }, typeof(ConstantLoading.SomeEnum))]
        [TestCase("ConstLoadByteEnum", new ConstantLoading.SomeByteEnum[] { ConstantLoading.SomeByteEnum.A, ConstantLoading.SomeByteEnum.B }, typeof(ConstantLoading.SomeByteEnum))]
        [TestCase("ConstLoadLongEnum", new ConstantLoading.SomeLongEnum[] { ConstantLoading.SomeLongEnum.A, ConstantLoading.SomeLongEnum.B }, typeof(ConstantLoading.SomeLongEnum))]
        public void OptimalConstantLoadInstructionFor(string methodName, object values, Type type)
        {
            var assembly = ReadTestAssembly();
            var @class = GetType(assembly, String.Format("Unity.BindingsGenerator.AssemblyPatcherTestAssembly.ConstantLoading"));

            var method = @class.Methods.SingleOrDefault(m => m.Name == methodName);

            AssertConstantLoad(method, values, TypeResolver.TypeDefinitionFor(type));
        }

        class ClassWithMethodWithAttribute
        {
            [NativeMethod]
            private void AttributeConflictingMethod() {}
        }

        class ClassWithMethodWithoutAttribute
        {
            private void AttributeConflictingMethod() {}
        }

        [Test]
        public void AssertEqualFailsOnAttribute()
        {
            var a = TypeResolver.TypeDefinitionFor(typeof(ClassWithMethodWithAttribute));
            var b = TypeResolver.TypeDefinitionFor(typeof(ClassWithMethodWithoutAttribute));

            Assert.Catch(delegate
            {
                AssemblyPatcherAssertEqual.AssertEqual(a, b);
            });
        }

        [TestCase("SimpleCombinationTest")]
        [NUnit.Framework.Ignore("Default parameters expansion is now disabled in the bindings generator")]
        public void CombinationTests(string typeName)
        {
            var assembly = ReadTestAssembly();

            var patchedClass = GetType(assembly, String.Format("Unity.BindingsGenerator.AssemblyPatcherTestAssembly.{0}", typeName));
            var expectedClass = GetType(assembly, String.Format("Unity.BindingsGenerator.AssemblyPatcherTestAssembly.{0}Expected", typeName));

            new AssemblyPatcher(new FilteringVisitor(patchedClass), ScriptingBackend.Mono).Patch();

            AssemblyPatcherAssertEqual.AssertEqual(expectedClass, patchedClass);
        }

        [Test]
        public void WriteToFile()
        {
            var assembly = ReadTestAssembly();
            new AssemblyPatcher(new FilteringVisitor(assembly), ScriptingBackend.Mono).Patch();

            var patchedAssemblyPath = Path.GetTempFileName();
            assembly.Write(patchedAssemblyPath);

            Console.WriteLine("Patched Assembly Saved At: {0}", patchedAssemblyPath);
        }

        [Test]
        public void Extern_Methods_Implementing_Interface_Methods_Get_MethodImplAttribute_Injected()
        {
            var testAssemblyPath = Path.Combine(Path.GetTempPath(), "Extern_Methods_Implementing_Interface_Methods_Get_MethodImplAttribute_Injected.dll");

            CompilerUtil.CompileAssembly(testAssemblyPath, @"
                                            interface IFoo { void Bar(); }
                                            class FooBar : IFoo {
                                                public extern void Bar();
                                            }");

            var assembly = TypeResolver.LoadAssembly(testAssemblyPath);
            new AssemblyPatcher(new FilteringVisitor(assembly), ScriptingBackend.Mono).Patch();

            var patchedType = assembly.MainModule.GetType("FooBar").Resolve();
            var patchedMethod = patchedType.Methods.SingleOrDefault(m => m.Name == "Bar" && m.Parameters.Count == 0);

            Assert.That(patchedMethod, Is.Not.Null);

            Assert.IsTrue(patchedMethod.IsInternalCall, "Attribute should have been injected into " + patchedMethod.Name);
        }
    }
}
