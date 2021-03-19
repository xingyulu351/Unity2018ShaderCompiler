using System.Collections.Generic;
using NUnit.Framework;

namespace AssemblyPatcher.Tests.Misc
{
    [TestFixture]
    public class SimpleMemberReferenceTestCase
    {
        [Test]
        public void TestTypeFullyQualifiedNames([ValueSource("ValidFieldReferenceFQN")] string fieldRefFQNSpec)
        {
            //var separatorIndex = typeRefFQNSpec.IndexOf('|');
            //var fqnParts = typeRefFQNSpec.Substring(separatorIndex + 1).Split(',');

            //var typeRefFQN = typeRefFQNSpec.Substring(0, separatorIndex);

            //var smr = SimpleMemberReference.Parse(typeRefFQN);
            //Assert.AreEqual(fqnParts[0], smr.AssemblyName ?? string.Empty);
            //Assert.AreEqual(fqnParts[1], smr.FullName);
            AssertValidSimpleMemberReference(fieldRefFQNSpec);
        }

        [Test]
        public void TestInvalidMethodFullyQualifiedNames([ValueSource("InvalidMethodReferenceFQN")] string methodRefFQN)
        {
            Assert.Throws<PatchingException>(() => SimpleMemberReference.Parse(methodRefFQN));
        }

        [Test]
        public void TestMethodFullyQualifiedNames([ValueSource("ValidMethodReferenceFQN")] string methodRefFQN)
        {
            AssertValidSimpleMemberReference(methodRefFQN);
        }

        private static void AssertValidSimpleMemberReference(string memberRefFQNSpec)
        {
            var expectationSeparatorIndex = memberRefFQNSpec.IndexOf("|");
            var smr = SimpleMemberReference.Parse(memberRefFQNSpec.Substring(0, expectationSeparatorIndex));

            var fqnParts = memberRefFQNSpec.Substring(expectationSeparatorIndex + 1).Split(',');

            Assert.AreEqual(fqnParts[0], smr.AssemblyName ?? string.Empty, "AssemblyName not match");
            Assert.AreEqual(fqnParts[1], smr.MemberType, "Member type not match");
            Assert.AreEqual(fqnParts[2], smr.FullName, "FullName not match");
            Assert.AreEqual(fqnParts[3], smr.Name, "Name not match");
        }

        static IEnumerable<string> ValidFieldReferenceFQN()
        {
            return new[]
            {
                "FT Type::Fld|,FT,Type,Fld",
                "[Assembly] FT Type::Fld|[Assembly],FT,Type,Fld",
            };
        }

        static IEnumerable<string> ValidMethodReferenceFQN()
        {
            return new[]
            {
                "RT NS.Type::M2()|,RT,NS.Type,M2,",
                "[SomeAssemlby] RT NS.Type::M2()|[SomeAssemlby],RT,NS.Type,M2,",
                "[SomeAssemlby] RT NS.Type::M2(P1T)|[SomeAssemlby],RT,NS.Type,M2,P1T",
            };
        }

        static IEnumerable<string> InvalidMethodReferenceFQN()
        {
            return new[]
            {
                "[SomeAssemlby] NS.Type::M2()",
                "NS.Type::M2()",
            };
        }
    }
}
