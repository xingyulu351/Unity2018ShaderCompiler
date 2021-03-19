using System;
using System.Collections.Generic;
using NiceIO;
using NUnit.Framework;
using Unity.BuildSystem.NativeProgramSupport;

namespace Bee.NativeProgramSupport.Building.Tests
{
    [TestFixture]
    public class LocatorTests
    {
        class TestSdk : Sdk
        {
            public TestSdk(bool isDownloadable) => IsDownloadable = isDownloadable;
            public override Platform Platform { get; } = new MacOSXPlatform();
            public override NPath Path { get; } = "mypath";
            public override Version Version { get; } = new Version();
            public override NPath CppCompilerPath { get; }
            public override NPath StaticLinkerPath { get; }
            public override NPath DynamicLinkerPath { get; }
            public override bool SupportedOnHostPlatform { get; }
        }

        private class TestDownloadableSdkProvider : DownloadableSdkProvider
        {
            private Sdk[] Sdks { get; }
            public static TestSdk MockSdk { get; } = new TestSdk(true);

            public TestDownloadableSdkProvider() => Sdks = new[] {MockSdk};
            public TestDownloadableSdkProvider(Sdk[] sdks) => Sdks = sdks;
            public override IEnumerable<Sdk> Provide() => Sdks;
        }

        class TestLocator : SdkLocator<TestSdk>
        {
            public TestLocator(DownloadableSdkProvider[] downloadableSdkProviders = null) : base(downloadableSdkProviders)
            {
            }

            protected override Platform Platform { get; } = new MacOSXPlatform();
            public IEnumerable<TestSdk> MockSystemSdks { get; set; } = Array.Empty<TestSdk>();
            protected override IEnumerable<TestSdk> ScanForSystemInstalledSdks() => MockSystemSdks;
            protected override TestSdk NewSdkInstance(NPath path, Version version, bool isDownloadable) => new TestSdk(isDownloadable);
            public override TestSdk FromPath(NPath path) => throw new NotImplementedException();
            public override TestSdk UserDefault { get; }
        }

        [Test]
        public void LocatorUsesDownloadableSdkProvider()
        {
            CollectionAssert.Contains(new TestLocator().DownloadableSdks, TestDownloadableSdkProvider.MockSdk);
        }

        [Test]
        public void DownloadableSdkProviderThatProvidesNonDownloadableSdkThrows()
        {
            var provider = new TestDownloadableSdkProvider(new[] {new TestSdk(isDownloadable: false)});
            Assert.Throws<ArgumentException>(() =>
            {
                var a = new TestLocator(new[] {provider}).DownloadableSdks;
            });
        }
    }
}
