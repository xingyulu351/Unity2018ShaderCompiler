using System.Collections.Generic;
using NUnit.Framework;
using UnderlyingModel.MemDoc;
using UnderlyingModel.MemDoc.Comparers;

namespace UnderlyingModel.Tests.MemDoc2
{
    [TestFixture]
    public class MemDocComparisonTests
    {
        [Test]
        public void CompareDifferentNames()
        {
            var model1 = new MemDocModel
            {
                ItemNameFromMember = "AccelerationEvent",
                AssemblyKind = AssemblyType.Struct,
                NamespaceFromMember = "UnityEngine"
            };
            var model2 = new MemDocModel
            {
                ItemNameFromMember = "Accel2",
                AssemblyKind = AssemblyType.Struct,
                NamespaceFromMember = "UnityEngine"
            };
            var comparer = new MemDocModelComparer();
            Assert.AreNotEqual(0, comparer.Compare(model1, model2)); //0 means equal
        }

        [Test]
        public void CompareDifferentTypes()
        {
            var model1 = new MemDocModel
            {
                ItemNameFromMember = "AccelerationEvent",
                AssemblyKind = AssemblyType.Struct,
                NamespaceFromMember = "UnityEngine"
            };
            var model2 = new MemDocModel
            {
                ItemNameFromMember = "AccelerationEvent",
                AssemblyKind = AssemblyType.Class,
                NamespaceFromMember = "UnityEngine"
            };
            var comparer = new MemDocModelComparer();
            Assert.AreNotEqual(0, comparer.Compare(model1, model2)); //0 means equal
        }

        [Test]
        public void CompareDifferentNamespaces()
        {
            var model1 = new MemDocModel
            {
                ItemNameFromMember = "AccelerationEvent",
                AssemblyKind = AssemblyType.Struct,
                NamespaceFromMember = "UnityEngine"
            };
            var model2 = new MemDocModel
            {
                ItemNameFromMember = "AccelerationEvent",
                AssemblyKind = AssemblyType.Struct,
                NamespaceFromMember = "UnityEditor"
            };
            var comparer = new MemDocModelComparer();
            Assert.AreNotEqual(0, comparer.Compare(model1, model2)); //0 means equal
        }

        [Test]
        public void CompareIdentical()
        {
            var model1 = new MemDocModel
            {
                ItemNameFromMember = "AccelerationEvent",
                AssemblyKind = AssemblyType.Struct,
                NamespaceFromMember = "UnityEngine"
            };
            var model2 = new MemDocModel
            {
                ItemNameFromMember = "AccelerationEvent",
                AssemblyKind = AssemblyType.Struct,
                NamespaceFromMember = "UnityEngine"
            };
            var comparer = new MemDocModelComparer();
            Assert.IsTrue(comparer.Compare(model1, model2).Success);
        }

        [Test]
        public void CompareIdenticalEmpty()
        {
            var model1 = new MemDocModel();
            var model2 = new MemDocModel();

            var comparer = new MemDocModelComparer();
            Assert.IsTrue(comparer.Compare(model1, model2).Success);
        }

        [Test]
        public void CompareWithChildren2PropertiesDiffName()
        {
            var model1 = new MemDocModel();
            var model2 = new MemDocModel();
            model1.ChildModels = new List<MemDocModel>
            {
                new MemDocModel
                {
                    ItemNameFromMember = "AccelerationEvent._acceleration",
                    AssemblyKind = AssemblyType.Property,
                }
            };
            model2.ChildModels = new List<MemDocModel>
            {
                new MemDocModel
                {
                    ItemNameFromMember = "AccelerationEvent._deltaTime",
                    AssemblyKind = AssemblyType.Property,
                }
            };

            Assert.AreNotEqual(0, (new MemDocModelComparer()).Compare(model1, model2));
        }

        [Test]
        public void CompareWithChildrenDifferentLength10()
        {
            var model1 = new MemDocModel();
            var model2 = new MemDocModel();
            model1.ChildModels = new List<MemDocModel>
            {
                new MemDocModel
                {
                    ItemNameFromMember = "AccelerationEvent._acceleration",
                    AssemblyKind = AssemblyType.Property,
                }
            };
            model2.ChildModels = new List<MemDocModel>();

            Assert.AreNotEqual(0, (new MemDocModelComparer()).Compare(model1, model2));
        }

        [Test]
        public void CompareOneChildrenOneWithout()
        {
            var model1 = new MemDocModel();
            var model2 = new MemDocModel();
            model1.ChildModels = new List<MemDocModel>
            {
                new MemDocModel
                {
                    ItemNameFromMember = "AccelerationEvent._acceleration",
                    AssemblyKind = AssemblyType.Property,
                }
            };

            Assert.AreNotEqual(0, (new MemDocModelComparer()).Compare(model1, model2));
        }

        [Test]
        public void CompareWithChildrenDiffType()
        {
            var model1 = new MemDocModel();
            var model2 = new MemDocModel();
            model1.ChildModels = new List<MemDocModel>
            {
                new MemDocModel
                {
                    ItemNameFromMember = "AccelerationEvent._acceleration",
                    AssemblyKind = AssemblyType.Property,
                }
            };
            model2.ChildModels = new List<MemDocModel>
            {
                new MemDocModel
                {
                    ItemNameFromMember = "AccelerationEvent._acceleration",
                    AssemblyKind = AssemblyType.Enum
                }
            };

            Assert.AreNotEqual(0, (new MemDocModelComparer()).Compare(model1, model2));
        }

        [Test]
        public void CompareSameChildrenDiffSubsections()
        {
            var model1 = new MemDocModel();
            var model2 = new MemDocModel();
            var childModel1 = new MemDocModel
            {
                ItemNameFromMember = "AccelerationEvent._acceleration",
                AssemblyKind = AssemblyType.Property,
            };
            var childModel2 = new MemDocModel
            {
                ItemNameFromMember = "AccelerationEvent._acceleration",
                AssemblyKind = AssemblyType.Property,
            };
            childModel1.SubSections.Add(new MemberSubSection("howdy"));
            model1.ChildModels = new List<MemDocModel> {childModel1};
            model2.ChildModels = new List<MemDocModel> {childModel2};

            Assert.AreNotEqual(0, (new MemDocModelComparer()).Compare(model1, model2));
        }
    }
}
