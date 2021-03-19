using System;
using System.IO;
using InternalCallRegistration.Tests.Fixtures.UnityFaux;
using NUnit.Framework;
using Unity.InternalCallRegistrationWriter.Collector;
using Unity.InternalCallRegistrationWriter.CppWriter;

namespace Unity.InternalCallRegistration.Tests
{
    [TestFixture]
    class WriterTests
    {
        private static InternalCallCollector CreateCollectorForUnityTestFixtureAssembly()
        {
            return new InternalCallCollector(UnityFauxFixtureLocation.Location());
        }

        private static UnityInternalCallRegistrationsWriter CreateWriter()
        {
            return new UnityInternalCallRegistrationsWriter(new CodeWriter());
        }

        [Test]
        public void SingleUnityTypeSingleICallRegistration()
        {
            var icalls = CreateCollectorForUnityTestFixtureAssembly().CollectedCallsFor("UnityEngine.SimpleUnityBindingsType");
            var writer = CreateWriter();

            writer.MethodDeclarationAndInvocationFor(icalls[0]);
            VerifyOutputAgainstFile(writer.OutputWriter.Output(), "SingleUnityTypeSingleICallRegistration");
        }

        [Test]
        public void SingleUnityTypeAllICallRegistrations()
        {
            var collectorForUnityTestFixtureAssembly = CreateCollectorForUnityTestFixtureAssembly();
            var icalls = collectorForUnityTestFixtureAssembly.AllCollectedCalls;
            Assert.IsNotNull(icalls);

            var writer = CreateWriter();
            writer.AllMethodDeclarationsAndInvocationsForType(icalls);

            VerifyOutputAgainstFile(writer.OutputWriter.Output(), "SingleUnityTypeAllICallRegistrations");
        }

        [Test]
        public void AllUnityAssemblyCallRegistrations()
        {
            var icalls = CreateCollectorForUnityTestFixtureAssembly().InternalCallsByType;
            Assert.IsNotNull(icalls);

            var writer = CreateWriter();
            writer.AllRegistrationsForAssembly(icalls);

            VerifyOutputAgainstFile(writer.OutputWriter.Output(), "AllUnityAssemblyCallRegistrations");
        }

        [Test]
        public void FullUnityAssemblyCallRegistration()
        {
            var icalls = CreateCollectorForUnityTestFixtureAssembly().InternalCallsByType;
            Assert.IsNotNull(icalls);

            var writer = CreateWriter();
            writer.FullRegistrationCpp(icalls);

            VerifyOutputAgainstFile(writer.OutputWriter.Output(), "FullUnityAssemblyCallRegistration");
        }

        [Test]
        public void NestedTypeICallRegistration()
        {
            var icalls = CreateCollectorForUnityTestFixtureAssembly().CollectedCallsFor("UnityEngine.OuterType/InnerType");
            var writer = CreateWriter();

            writer.MethodDeclarationAndInvocationFor(icalls[0]);
            VerifyOutputAgainstFile(writer.OutputWriter.Output(), "NestedTypeAllICallRegistrations");
        }

        public string FolderForTxtFiles()
        {
            var seperator = Path.DirectorySeparatorChar.ToString();
            var txtFilePath = "InternalCallRegistration.Tests.Fixtures" + seperator + "TxtFiles" + seperator;
            return ProjectRootDirectory() + seperator + txtFilePath;
        }

        private static DirectoryInfo ProjectRootDirectory()
        {
            return Directory.GetParent(Directory.GetParent(Path.GetDirectoryName(AppDomain.CurrentDomain.BaseDirectory)).ToString());
        }

        public void VerifyOutputAgainstFile(string testOutput, string filename)
        {
            if (OverwriteTextFiles)
                File.WriteAllText(FullPathForTestFile(filename), testOutput);

            Assert.AreEqual(File.ReadAllText(FullPathForTestFile(filename)).Replace("\r\n", "\n"), testOutput.Replace("\r\n", "\n"));
        }

        private string FullPathForTestFile(string filename)
        {
            return FolderForTxtFiles() + filename + ".txt";
        }

        protected bool OverwriteTextFiles
        {
            get { return false;  }
        }
    }
}
