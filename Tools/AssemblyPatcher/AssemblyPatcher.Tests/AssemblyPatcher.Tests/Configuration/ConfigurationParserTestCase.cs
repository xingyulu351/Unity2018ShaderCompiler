using System.Collections.Generic;
using System.Linq;
using AssemblyPatcher.Configuration;
using AssemblyPatcher.Tests.Extensions;
using NUnit.Framework;

namespace AssemblyPatcher.Tests.Configuration
{
    [TestFixture]
    public class ConfigurationParserTestCase
    {
        [Test]
        public void TestParserMethodBodyDitcher()
        {
            AssertRuleParsing(
                "R T::M() -> {}",
                new PatchRule("R T::M()", "{}") {  Operation =  OperationKind.RemoveBody });
        }

        [Test]
        public void TestParserAppendDllToAssemblyName()
        {
            AssertRuleParsing(
                "A -> [Target.Assembly] B",
                new PatchRule("A", "B") { Operation = OperationKind.Replace, DeclaringAssembly = "Target.Assembly.dll"});
        }

        [Test]
        public void TestTypeReplacement()
        {
            AssertRuleParsing(
                "A.B.Type -> A.B.Type",
                new PatchRule("A.B.Type", "A.B.Type") { Operation = OperationKind.Replace });
        }

        [Test]
        public void TestMemberReplacementOnDiferentAssembly()
        {
            AssertRuleParsing(
                "A.RT Type::Method() -> [SomeAssembly]",
                new PatchRule("A.RT Type::Method()", OperationKind.Replace) { DeclaringAssembly = "SomeAssembly.dll", Operation = OperationKind.Replace });
        }

        [Test]
        public void TestMemberReplacementOnSameAssembly()
        {
            AssertRuleParsing(
                "A.RT Type::Method() -> A.RT Type::Method2()",
                new PatchRule("A.RT Type::Method()", "A.RT Type::Method2()"));
        }

        [Test]
        public void TestUnsupportedMember()
        {
            AssertRuleParsing(
                "A.RT Type::Method() ->",
                new PatchRule("A.RT Type::Method()", OperationKind.Throw));
        }

        [Test]
        public void TestConditionalMemberReplacement()
        {
            AssertRuleParsing(
                "| A.B C::M1() | A.RT Type::Method() -> A.RT Type2::Method()",
                new PatchRule("A.RT Type::Method()", "A.RT Type2::Method()")
                {
                    Operation =  OperationKind.Replace,
                    ApplicableWith = "A.B C::M1()"
                });
        }

        [Test]
        public void TestMultipleConditionalMemberReplacement()
        {
            var rulesText = @"
|System.Boolean UnityScript.Lang.Extensions::op_Implicit(System.Enum)| System.Int32 System.IConvertible::ToInt32(System.IFormatProvider) -> [WinRTLegacy] System.Boolean Unity.WinRTLegacy.TypeExtensions::EnumToBoolean(System.Enum,System.Object)

|System.Reflection.MethodInfo Boo.Lang.Runtime.RuntimeServices::FindImplicitConversionOperator(System.Type,System.Type)| System.Reflection.MethodInfo[] System.Type::GetMethods(System.Reflection.BindingFlags) -> [WinRTLegacy] System.Reflection.MethodInfo[] Unity.WinRTLegacy.TypeExtensions::GetOpImplicitMethodsFromTypeHierarchy(System.Type,System.Reflection.BindingFlags)

System.Reflection.MethodInfo[] System.Type::GetMethods(System.Reflection.BindingFlags) ->";

            var rules = new Parser().Parse(rulesText.AsStream());

            Assert.AreEqual(3, rules.Count());
            Assert.AreEqual("System.Reflection.MethodInfo Boo.Lang.Runtime.RuntimeServices::FindImplicitConversionOperator(System.Type,System.Type)", rules.ElementAt(1).ApplicableWith);
        }

        [Test]
        public void TestSpacesAreIgnored()
        {
            AssertRuleParsing(
                "| A.B C::M1()|      A.RT Type::Method() ->         A.RT Type2::Method()  ",
                new PatchRule("A.RT Type::Method()", "A.RT Type2::Method()")
                {
                    Operation =  OperationKind.Replace,
                    ApplicableWith = "A.B C::M1()"
                });

            AssertRuleParsing(
                "     A.RT Type::Method() ->         A.RT Type2::Method()  ",
                new PatchRule("A.RT Type::Method()", "A.RT Type2::Method()")
                {
                    Operation =  OperationKind.Replace,
                });

            AssertRuleParsing("     A.Type->[  some_assembly] A.Type  ", new PatchRule("A.Type", "A.Type") { DeclaringAssembly = "some_assembly.dll" });
        }

        [Test]
        public void TestMultipleLines()
        {
            var rulesText =
@"
                A -> A.1
                B -> [B_1] B.1
                C -> C.1";

            var rules = new Parser().Parse(rulesText.AsStream());
            Assert.That(rules.Count(), Is.EqualTo(3));

            for (int i = 0; i < rules.Count(); i++)
            {
                var expectedName = (char)('A' + i);
                Assert.That(rules.ElementAt(i).SourceMemberFullyQualifiedName, Is.EqualTo(string.Format("{0}", expectedName)));
                Assert.That(rules.ElementAt(i).TargetMemberFullyQualifiedName , Is.EqualTo(string.Format("{0}.1", expectedName)));
            }
        }

        [Test]
        public void TestEmptyLinesAreIgnored()
        {
            var rulesText =
@"
                A -> A.1

                B -> B.1
             ";

            var rules = new Parser().Parse(rulesText.AsStream());
            Assert.That(rules.Count(), Is.EqualTo(2));

            for (int i = 0; i < rules.Count(); i++)
            {
                var expectedName = (char)('A' + i);

                Assert.That(rules.ElementAt(i).SourceMemberFullyQualifiedName, Is.EqualTo(string.Format("{0}", expectedName)));
                Assert.That(rules.ElementAt(i).TargetMemberFullyQualifiedName, Is.EqualTo(string.Format("{0}.1", expectedName)));
            }
        }

        [Test]
        public void TestLineNumber([ValueSource("LineNumberTestData")] LineNumberTestSpec spec)
        {
            try
            {
                new Parser().Parse(spec.rulesText.AsStream());
                Assert.Fail();
            }
            catch (PatchRuleException ex)
            {
                Assert.That(ex.LineNumber, Is.EqualTo(spec.lineWithError));
            }
        }

        private static void AssertRuleParsing(string ruleString, PatchRule expected)
        {
            var rules = new Parser().Parse(ruleString.AsStream());
            Assert.That(rules.Count(), Is.EqualTo(1));
            var rule = rules.Single();

            Assert.That(rule, Is.EqualTo(expected));
        }

        private static IEnumerable<LineNumberTestSpec> LineNumberTestData()
        {
            return new[]
            {
                new LineNumberTestSpec(@"INVALID RULE", 1),

                new LineNumberTestSpec(@"A -> B

                                             INVALID RULE", 3),

                new LineNumberTestSpec(@"A -> B
                                             C -> D
                                             INVALID RULE", 3),

                new LineNumberTestSpec(@"# line 1

                                             INVALID RULE", 3),
            };
        }

        public struct LineNumberTestSpec
        {
            public string rulesText;
            public int lineWithError;

            public LineNumberTestSpec(string contents, int line)
            {
                rulesText = contents;
                lineWithError = line;
            }

            public override string ToString()
            {
                return "#" + lineWithError;
            }
        }
    }
}
