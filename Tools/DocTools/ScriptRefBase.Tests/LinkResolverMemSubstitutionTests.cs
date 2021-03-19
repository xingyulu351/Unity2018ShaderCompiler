using System.Collections.Generic;
using System.IO;
using NUnit.Framework;
using UnderlyingModel;
using UnderlyingModel.Tests;

namespace ScriptRefBase.Tests
{
    [TestFixture]
    public class LinkResolverMemSubstitutionTests
    {
        private LinkResolverWithMemSubstitutions m_LinkResolver;
        private IMemberGetter m_NewDataItemProject;
        private string m_OldCurDir;

        [OneTimeSetUp]
        public void Init()
        {
            Directory.SetCurrentDirectory(TestContext.CurrentContext.TestDirectory);
            m_OldCurDir = Directory.GetCurrentDirectory();
            Directory.SetCurrentDirectory(DirectoryCalculator.RootDirName + "/Tools/DocTools");
            m_NewDataItemProject = TestUtils.CreateNewDataItemProjectFromLegacyDlls();
            m_LinkResolver = new LinkResolverWithMemSubstitutions(m_NewDataItemProject) {ResolveLinkWithoutDoc = true};
        }

        [OneTimeTearDown]
        public void Uninit()
        {
            //restore cur dir
            Directory.SetCurrentDirectory(m_OldCurDir);
        }

        [Test]
        public void ArrayFound()
        {
            const string input = "left Array right";
            var expectedOutput =
                new MemDocElementList(new List<IMemDocElement>
                {
                    new MemDocElementPlainText("left "),
                    new MemDocElementLink("Array", "Array"),
                    new MemDocElementPlainText(" right"),
                });
            ResolveAndVerify(m_LinkResolver, input, expectedOutput);
        }

        [TestCase("whatever", new[] { "whatever" })]
        [TestCase("what ever", new[] { "what", " ", "ever" })]
        [TestCase("what, .ever", new[] { "what", ", .", "ever" })]
        [TestCase("what.ever", new[] { "what.ever" })]
        [TestCase("var lerpedColor : Color", new[] { "var", " ", "lerpedColor", " : ", "Color" })]
        [TestCase("what+{ever)", new[] { "what", "+{", "ever", ")" })]
        public void SplitWithPunctuationTest(string input, string[] expectedSplitResult)
        {
            var actualSplitResult = RecognizedClassSubstitutor.SplitWithPunctuation(input);
            Assert.AreEqual(expectedSplitResult, actualSplitResult);
        }

        [Test]
        public void LerpedColor()
        {
            const string input = "var lerpedColor : Color = something";
            var expectedOutput =
                new MemDocElementList(new List<IMemDocElement>
                {
                    new MemDocElementPlainText("var lerpedColor : "),
                    new MemDocElementLink("Color", "Color"),
                    new MemDocElementPlainText(" = something"),
                });
            ResolveAndVerify(m_LinkResolver, input, expectedOutput);
        }

        [Test]
        public void UndocumentedClassNoLink()
        {
            m_LinkResolver.ResolveLinkWithoutDoc = false;
            const string input = "left Perlin right";
            var expectedOutput =
                new MemDocElementList(new List<IMemDocElement>
                {
                    new MemDocElementPlainText("left Perlin right")
                });
            ResolveAndVerify(m_LinkResolver, input, expectedOutput);
        }

        [Test]
        public void ArrayNotFoundLowercase()
        {
            const string input = "left array right";
            var expectedOutput =
                new MemDocElementList(new List<IMemDocElement>
                {
                    new MemDocElementPlainText("left array right"),
                });
            ResolveAndVerify(m_LinkResolver, input, expectedOutput);
        }

        [Test]
        public void MonoBehaviourFound()
        {
            const string input = "left MonoBehaviour right";
            var expectedOutput =
                new MemDocElementList(new List<IMemDocElement>
                {
                    new MemDocElementPlainText("left "),
                    new MemDocElementLink("MonoBehaviour", "MonoBehaviour"),
                    new MemDocElementPlainText(" right"),
                });
            ResolveAndVerify(m_LinkResolver, input, expectedOutput);
        }

        [Test]
        public void AndroidJavaClass()
        {
            const string input = "AndroidJavaClass jc = new AndroidJavaClass(java.lang.System)";
            var expectedOutput =
                new MemDocElementList(new List<IMemDocElement>
                {
                    new MemDocElementLink("AndroidJavaClass", "AndroidJavaClass"),
                    new MemDocElementPlainText(" jc = new "),
                    new MemDocElementLink("AndroidJavaClass", "AndroidJavaClass"),
                    new MemDocElementPlainText("(java.lang.System)")
                });
            ResolveAndVerify(m_LinkResolver, input, expectedOutput);
        }

        [Test]
        public void AudioVelocityUpdateMode()
        {
            const string input = "audio.velocityUpdateMode = AudioVelocityUpdateMode.Fixed";
            var expectedOutput =
                new MemDocElementList(new List<IMemDocElement>
                {
                    new MemDocElementPlainText("audio.velocityUpdateMode = "),
                    new MemDocElementLink("AudioVelocityUpdateMode.Fixed", "AudioVelocityUpdateMode.Fixed"),
                });
            ResolveAndVerify(m_LinkResolver, input, expectedOutput);
        }

        [Test]
        public void AudioVelocityUpdateModeSemicolon()
        {
            const string input = "audio.velocityUpdateMode = AudioVelocityUpdateMode.Fixed;";

            var expectedOutput =
                new MemDocElementList(new List<IMemDocElement>
                {
                    new MemDocElementPlainText("audio.velocityUpdateMode = "),
                    new MemDocElementLink("AudioVelocityUpdateMode.Fixed", "AudioVelocityUpdateMode.Fixed"),
                    new MemDocElementPlainText(";")
                });
            ResolveAndVerify(m_LinkResolver, input, expectedOutput);
        }

        [Test]
        public void Transform()
        {
            const string input = "child.position += Vector3.up * 10.0;";
            var expectedOutput =
                new MemDocElementList(new List<IMemDocElement>
                {
                    new MemDocElementPlainText("child.position += "),
                    new MemDocElementLink("Vector3-up", "Vector3.up"),
                    new MemDocElementPlainText(" * 10.0;")
                });
            ResolveAndVerify(m_LinkResolver, input, expectedOutput);
        }

        [Test]
        public void TripleName()
        {
            const string input = "left [[PlayerSettings.Android._preferredInstallLocation]] right";
            var expectedOutput =
                new MemDocElementList(new List<IMemDocElement>
                {
                    new MemDocElementPlainText("left "),
                    new MemDocElementLink("PlayerSettings.Android-preferredInstallLocation", "PlayerSettings.Android.preferredInstallLocation"),
                    new MemDocElementPlainText(" right")
                });
            ResolveAndVerify(m_LinkResolver, input, expectedOutput);
        }

        [Test]
        public void RefToMovedClassFunction_ResolveLink_ToFunctionOfMovedClass()
        {
            var project = TestUtils.CreateMemberGetterFromTestAssembly();
            var linkResolver = new LinkResolverWithMemSubstitutions(project)
            {
                CurClass = "NamespaceA.ClassMovedToNamespace"
            };
            const string input = "blah ::ref::Foo";
            var expectedOutput =
                new MemDocElementList(new List<IMemDocElement>
                {
                    new MemDocElementPlainText("blah "),
                    new MemDocElementLink("NamespaceA.ClassMovedToNamespace.Foo", "Foo")
                });
            ResolveAndVerify(linkResolver, input, expectedOutput);
        }

        [Test]
        public void LinkWithTextToMovedClassFunction_ResolveLink_ToFunctionOfMovedClass()
        {
            var project = TestUtils.CreateMemberGetterFromTestAssembly();
            var linkResolver = new LinkResolverWithMemSubstitutions(project);
            const string input = "blah [[ClassMovedToNamespace.Foo|whatever]]";
            var expectedOutput =
                new MemDocElementList(new List<IMemDocElement>
                {
                    new MemDocElementPlainText("blah "),
                    new MemDocElementLink("NamespaceA.ClassMovedToNamespace.Foo", "whatever")
                });
            ResolveAndVerify(linkResolver, input, expectedOutput);
        }

        [Test]
        public void RefToMovedClassFunctionNoNamespaceSpecified_ResolveLink_ToFunctionOfMovedClass()
        {
            var project = TestUtils.CreateMemberGetterFromTestAssembly();
            var linkResolver = new LinkResolverWithMemSubstitutions(project)
            {
                CurClass = "ClassMovedToNamespace"
            };
            const string input = "blah ::ref::Foo";
            var expectedOutput =
                new MemDocElementList(new List<IMemDocElement>
                {
                    new MemDocElementPlainText("blah "),
                    new MemDocElementLink("NamespaceA.ClassMovedToNamespace.Foo", "Foo")
                });
            ResolveAndVerify(linkResolver, input, expectedOutput);
        }

        [Test]
        public void MovedClass_ResolveLink_ToClassInNamespace()
        {
            var project = TestUtils.CreateMemberGetterFromTestAssembly();
            var linkResolver = new LinkResolverWithMemSubstitutions(project);
            const string input = "blah [[ClassMovedToNamespace.Foo]]";
            var expectedOutput =
                new MemDocElementList(new List<IMemDocElement>
                {
                    new MemDocElementPlainText("blah "),
                    new MemDocElementLink("NamespaceA.ClassMovedToNamespace.Foo", "ClassMovedToNamespace.Foo")
                });
            ResolveAndVerify(linkResolver, input, expectedOutput);
        }

        [Test]
        public void PropertyNoUnderscore_ResolveLink_ProperHtmlName()
        {
            var project = TestUtils.CreateNewDataItemProjectFromLegacyDlls();
            var linkResolver = new LinkResolverWithMemSubstitutions(project);
            const string input = "blah [[RectTransform.drivenByObject]]";
            var expectedOutput =
                new MemDocElementList(new List<IMemDocElement>
                {
                    new MemDocElementPlainText("blah "),
                    new MemDocElementLink("RectTransform-drivenByObject", "RectTransform.drivenByObject")
                });
            ResolveAndVerify(linkResolver, input, expectedOutput);
        }

        [Test]
        public void MovedClass_ResolveLink_PointsToClassInNamespace()
        {
            var project = TestUtils.CreateMemberGetterFromTestAssembly();
            var linkResolver = new LinkResolverWithMemSubstitutions(project);
            const string input = "blah [[ClassMovedToNamespace]]";
            var expectedOutput =
                new MemDocElementList(new List<IMemDocElement>
                {
                    new MemDocElementPlainText("blah "),
                    new MemDocElementLink("NamespaceA.ClassMovedToNamespace", "ClassMovedToNamespace")
                });
            ResolveAndVerify(linkResolver, input, expectedOutput);
        }

        private static void ResolveAndVerify(LinkResolverWithMemSubstitutions linkResolver, string input, MemDocElementList expectedOutput)
        {
            var actualOutput = linkResolver.ParseDescriptionWithMemSubstitutions(input);

            if (expectedOutput.m_Elements.Count != actualOutput.m_Elements.Count)
                Assert.Fail("the two descriptions have different numbers of elements exp = {0}, act = {1}",
                    expectedOutput.m_Elements.Count, actualOutput.m_Elements.Count);
            for (int i = 0; i < expectedOutput.m_Elements.Count; i++)
            {
                var expectedSt = expectedOutput.m_Elements[i].ToString();
                var actualSt = actualOutput.m_Elements[i].ToString();
                if (expectedSt != actualSt)
                    Assert.Fail("Descriptions differ at index {0}:\nexp = {1},\nact = {2}", i, expectedSt, actualSt);
            }
        }
    }
}
