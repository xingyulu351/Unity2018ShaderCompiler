using System;
using System.Linq;
using NUnit.Framework;

namespace JamSharp.Runtime.Tests
{
    [TestFixture]
    public class CSharpVariableStorageTests
    {
        [SetUp]
        public void Setup()
        {
            m_Storage = new CSharpVariableStorage();
        }

        CSharpVariableStorage m_Storage;

        [Test]
        public void CanStoreGlobal()
        {
            m_Storage.GlobalVariable("Harry").Assign("henk");
            Assert.AreEqual("henk", m_Storage.GlobalVariable("Harry").Elements.Single());
        }

        [Test]
        public void PushAndPopSettings()
        {
            var a = m_Storage.GlobalVariable("henk");
            a.Assign("one");

            m_Storage.VariableOnTarget("henk", "mytarget").Assign("two");
            m_Storage.PushSettingsFor("mytarget");

            Assert.AreEqual("two", a.Elements.Single());
            m_Storage.PopSettingsFor("mytarget");
            Assert.AreEqual("one", a.Elements.Single());
        }

        [Test]
        public void PushNonExistingTarget()
        {
            m_Storage.PushSettingsFor("mytarget");
            m_Storage.PopSettingsFor("mytarget");
        }

        [Test]
        public void PushTargetThatHasVariableWhichHasNoGlobal()
        {
            m_Storage.VariableOnTarget("henk", "mytarget").Assign("two");
            m_Storage.PushSettingsFor("mytarget");
            m_Storage.PopSettingsFor("mytarget");
        }

        [Test]
        public void QueryingTwiceAndComparing()
        {
            m_Storage.GlobalVariable("Harry").Assign("henk");

            var one = m_Storage.GlobalVariable("Harry");
            var two = m_Storage.GlobalVariable("Harry");

            Assert.AreEqual(one, two);

            one.Append("Pieter");
            Assert.AreEqual("Pieter", two.Elements.Last());
        }

        [Test]
        public void VariableOnTarget()
        {
            m_Storage.VariableOnTarget("Harry", "targetname").Assign("henk");
            Assert.AreEqual("henk", m_Storage.VariableOnTarget("Harry", "targetname").Elements.Single());
        }
    }
}
