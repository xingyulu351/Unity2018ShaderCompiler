using System.Collections.Generic;
using System.IO;
using NUnit.Framework;
using UnderlyingModel.MemDoc;
using UnderlyingModel.MemDoc.Comparers;
using UnderlyingModel.MemDoc.Serializers;

namespace UnderlyingModel.Tests.MemDoc2
{
    /* These tests load mem.xml files into MemDocModel
     * and verify the contents have been properly parsed
     * No NewDataItemProject or Asm info is used here
     */
    [TestFixture]
    public class Mem2Tests
    {
        private string m_OldCurDir;

        [OneTimeSetUp]
        public void Initialize()
        {
            Directory.SetCurrentDirectory(TestContext.CurrentContext.TestDirectory);
            m_OldCurDir = Directory.GetCurrentDirectory();
            Directory.SetCurrentDirectory("../..");
        }

        [OneTimeTearDown]
        public void Uninit()
        {
            //restore cur dir
            Directory.SetCurrentDirectory(m_OldCurDir);
        }

        [Test]
        public void SimpleClassNoMembers()
        {
            const string fname = "Mem2Source/AccelerationEventNoMembers.mem.xml";
            var ser = new MemDocModelXmlSerializer();
            var actualModel = ser.Deserialize(fname);
            var expectedModel = new MemDocModel
            {
                ItemNameFromMember = "AccelerationEvent",
                AssemblyKind = AssemblyType.Struct,
                NamespaceFromMember = "UnityEngine"
            };
            expectedModel.AddNewSectionWithSummary("Structure describing acceleration status of the device.");
            AssertMemDocModelsEqual(expectedModel, actualModel);
        }

        [Test]
        public void SimpleClassUndocMembers()
        {
            const string fname = "Mem2Source/RenderBuffer.mem.xml";
            var ser = new MemDocModelXmlSerializer();
            var actualModel = ser.Deserialize(fname);
            var expectedModel = new MemDocModel
            {
                ItemNameFromMember = "RenderBuffer",
                AssemblyKind = AssemblyType.Struct,
                NamespaceFromMember = "UnityEngine"
            };
            expectedModel.SubSections.Add(
                new MemberSubSection
                {
                    Summary = "RenderBuffer summary",
                    TextBlocks = {new DescriptionBlock("RenderBuffer description")}
                });
            var child1 = new MemDocModel
            {
                ItemNameFromMember = "RenderBuffer._m_BufferPtr",
                AssemblyKind = AssemblyType.Unknown,
                SubSections = {new MemberSubSection {IsUndoc = true}}
            };
            var child2 = new MemDocModel
            {
                ItemNameFromMember = "RenderBuffer._m_RenderTextureInstanceID",
                AssemblyKind = AssemblyType.Unknown,
                SubSections = {new MemberSubSection {IsUndoc = true}}
            };
            expectedModel.ChildModels = new List<MemDocModel> {child1, child2};
            AssertMemDocModelsEqual(expectedModel, actualModel);
        }

        [Test]
        public void SimpleClass()
        {
            const string fname = "Mem2Source/AccelerationEvent.mem.xml";
            var ser = new MemDocModelXmlSerializer();
            var actualDocModel = ser.Deserialize(fname);
            var expectedModel = new MemDocModel
            {
                ItemNameFromMember = "AccelerationEvent",
                AssemblyKind = AssemblyType.Struct,
                NamespaceFromMember = "UnityEngine"
            };
            expectedModel.AddNewSectionWithSummary("Structure describing acceleration status of the device.");
            var child1 = new MemDocModel
            {
                ItemNameFromMember = "AccelerationEvent._acceleration",
                AssemblyKind = AssemblyType.Property,
            };
            child1.AddNewSectionWithSummary("Value of acceleration.");
            var child2 = new MemDocModel
            {
                ItemNameFromMember = "AccelerationEvent._deltaTime",
                AssemblyKind = AssemblyType.Property,
            };
            child2.AddNewSectionWithSummary("Amount of time passed since last accelerometer measurement.");
            expectedModel.ChildModels = new List<MemDocModel> {child1, child2};
            AssertMemDocModelsEqual(expectedModel, actualDocModel);
        }

        [Test] //method with multiple sections
        public void EditorGuiSlider()
        {
            const string fname = "Mem2Source/EditorGUISlider.mem.xml";

            var expectedModel = new MemDocModel
            {
                ItemNameFromMember = "EditorGUI",
                AssemblyKind = AssemblyType.Class,
                NamespaceFromMember = "UnityEditor"
            };
            expectedModel.AddNewSectionWithSummary("top summary");
            var child = new MemDocModel
            {
                ItemNameFromMember = "EditorGUI.Slider",
                AssemblyKind = AssemblyType.Method,
            };

            //populate section1 of Slider
            var section1 = new MemberSubSection
            {
                SignatureList = new List<string>
                {
                    "sig1",
                    "sig2",
                    "sig3"
                },
                Summary = "top summary",
                Parameters =
                {
                    new ParameterWithDoc("position", "position doc"),
                    new ParameterWithDoc("label", "label doc"),
                    new ParameterWithDoc("value", "value doc"),
                },
                ReturnDoc = new ReturnWithDoc("return doc"),
                TextBlocks =
                {
                    new DescriptionBlock("section description"),
                    new ExampleBlock("example doc")
                }
            };

            //populate section2 of Slider
            var section2 = new MemberSubSection
            {
                SignatureList = {"sig4", "sig5", "sig6"},
                Summary = "summary slider2",
                Parameters = {new ParameterWithDoc("position", "position doc")}
            };

            //add slider to EditorGUI
            child.SubSections = new List<MemberSubSection> {section1, section2};
            expectedModel.ChildModels = new List<MemDocModel> {child};

            var ser = new MemDocModelXmlSerializer {Version = 1};
            var actualDocModel = ser.Deserialize(fname);
            AssertMemDocModelsEqual(expectedModel, actualDocModel);
        }

        [Test] //testing of example
        public void ColorRed()
        {
            const string fname = "Mem2Source/ColorRed.mem.xml";
            var expectedModel = new MemDocModel
            {
                ItemNameFromMember = "Color",
                AssemblyKind = AssemblyType.Struct,
                NamespaceFromMember = "UnityEngine",
                SubSections =
                {
                    new MemberSubSection
                    {
                        Summary = "color summary",
                        TextBlocks = {new DescriptionBlock("color description")}
                    }
                }
            };

            //populate section1 of Slider
            var redChild = new MemDocModel
            {
                ItemNameFromMember = "Color._red",
                AssemblyKind = AssemblyType.Property,
                NamespaceFromMember = "",
                SubSections =
                {
                    new MemberSubSection
                    {
                        SignatureList = {"red"},
                        Summary = "red summary",
                        TextBlocks =
                        {
                            new ExampleBlock("red example")
                            {
                                IsConvertExample = true,
                                IsNoCheck = false
                            }
                        }
                    }
                }
            };
            expectedModel.ChildModels.Add(redChild);

            var ser = new MemDocModelXmlSerializer() {Version = 1};
            var actualModel = ser.Deserialize(fname);

            AssertMemDocModelsEqual(expectedModel, actualModel);
        }

        [Test]
        //method with multiple sections
        public void Matrix4X4This()
        {
            const string fname = "Mem2Source/Matrix4x4this.mem.xml";
            var expectedModel = new MemDocModel
            {
                ItemNameFromMember = "Matrix4x4",
                AssemblyKind = AssemblyType.Struct,
                NamespaceFromMember = "UnityEngine",
                SubSections =
                {
                    new MemberSubSection
                    {
                        Summary = "Matrix summary",
                        TextBlocks = new List<TextBlock>
                        {
                            new DescriptionBlock("Matrix description")
                        }
                    }
                }
            };
            var child = new MemDocModel
            {
                ItemNameFromMember = "Matrix4x4.this",
                AssemblyKind = AssemblyType.Property,
                SubSections =
                {
                    new MemberSubSection
                    {
                        SignatureList = new List<string> {"this(int,int)"},
                        Summary = "two args summary",
                        TextBlocks = {new DescriptionBlock("two args description")}
                    },
                    new MemberSubSection
                    {
                        SignatureList = new List<string> {"this(int)"},
                        Summary = "one arg summary",
                        TextBlocks = {new DescriptionBlock("one arg description")}
                    }
                }
            };
            expectedModel.ChildModels.Add(child);

            var ser = new MemDocModelXmlSerializer();
            var actualDocModel = ser.Deserialize(fname);
            AssertMemDocModelsEqual(expectedModel, actualDocModel);
        }

        private void AssertMemDocModelsEqual(MemDocModel expectedModel, MemDocModel actualModel)
        {
            Assert.IsTrue(new MemDocModelComparer().Compare(expectedModel, actualModel).Success);
        }
    }
}
