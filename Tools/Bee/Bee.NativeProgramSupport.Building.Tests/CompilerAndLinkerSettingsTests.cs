using System.Linq;
using Bee.Core;
using Moq;
using NiceIO;
using NUnit.Framework;
using Unity.BuildSystem.NativeProgramSupport;

namespace Bee.NativeProgramSupport.Building.Tests
{
    static class CompilerAndLinkerSettingsTestsExtensions
    {
        public static ICustomizationFor<CompilerAndLinkerSettingsTests.TestCompilerSettings> CompilerSettingsForTest(this NativeProgram nativeProgram)
        {
            return CLikeCompiler.TypedCompilerCustomizationsFor<CompilerAndLinkerSettingsTests.TestCompilerSettings>(nativeProgram, c => c.Platform is MockPlatform);
        }

        public static ICustomizationFor<CompilerAndLinkerSettingsTests.TestObjectLinker> DynamicLinkerSettingsForTest(this NativeProgram nativeProgram)
        {
            return ObjectFileLinker.TypedLinkerCustomizationsFor<CompilerAndLinkerSettingsTests.TestObjectLinker>(nativeProgram, c => c.Platform is MockPlatform);
        }
    }

    [TestFixture]
    public class CompilerAndLinkerSettingsTests
    {
        private class TestToolchain : MockToolchain
        {
            public override CLikeCompiler CppCompiler => new TestCompiler(this);
        }

        private class TestCompiler : MockCompiler
        {
            public TestCompiler(ToolChain toolchain, string toolchainSpecificTestPropertyValue = null) : base(toolchain)
            {
                DefaultSettings = new TestCompilerSettings(this, toolchainSpecificTestPropertyValue);
            }
        }

        internal class TestCompilerSettings : MockCompilerSettings
        {
            private readonly string _toolchainSpecificTestPropertyValue;
            public string TestProperty { get; private set; } = DefaultValue;

            public TestCompilerSettings(CLikeCompiler compiler, string toolchainSpecificTestPropertyValue = null) : base(compiler)
            {
                _toolchainSpecificTestPropertyValue = toolchainSpecificTestPropertyValue;
            }

            public TestCompilerSettings WithTestProperty(string value) => this.With(c => c.TestProperty = value);

            protected override CLikeCompilerSettings CompilerSpecificCompilerSettingsFor(
                NativeProgram nativeProgram,
                NativeProgramConfiguration config,
                NPath file,
                NPath artifactPath)
            {
                var settings = base.CompilerSpecificCompilerSettingsFor(nativeProgram, config, file, artifactPath) as TestCompilerSettings;
                return _toolchainSpecificTestPropertyValue != null ? settings.WithTestProperty(_toolchainSpecificTestPropertyValue) : settings;
            }
        }

        internal class TestObjectLinker : MockObjectLinker
        {
            private readonly string _toolchainSpecificTestPropertyValue;
            public string TestProperty { get; private set; } = DefaultValue;
            public TestCompilerSettings TestCompilerSettings { get; private set; }

            public TestObjectLinker(ToolChain toolchain, string toolchainSpecificTestPropertyValue = null) : base(toolchain)
            {
                _toolchainSpecificTestPropertyValue = toolchainSpecificTestPropertyValue;
            }

            protected override ObjectFileLinker LinkerSpecificSettingsFor(NativeProgram nativeProgram, NativeProgramConfiguration config,
                CLikeCompilerSettings settings, NPath file, NPath artifacts)
            {
                TestCompilerSettings = settings as TestCompilerSettings;
                var linker = base.LinkerSpecificSettingsFor(nativeProgram, config, settings, file, artifacts) as TestObjectLinker;
                return _toolchainSpecificTestPropertyValue != null ? linker.WithTestProperty(_toolchainSpecificTestPropertyValue) : linker;
            }

            public TestObjectLinker WithTestProperty(string value) => With<TestObjectLinker>(l => l.TestProperty = value);
        }


        private CLikeCompilerSettings SetupCompilerSettings(CLikeCompiler compiler = null)
        {
            var format = Toolchain.ExecutableFormat;
            var config = new NativeProgramConfiguration(CodeGen.Debug, Toolchain, true);
            compiler = compiler ?? new TestCompiler(Toolchain);
            return
                NativeProgram.ProduceObjectFileProductionInstructions("test-program", config, format, "artifacts", new ObjectFileProducer[] {compiler}, out var _)
                    .OfType<CLikeCompilationUnitInstruction>()
                    .First()
                    .CompilerSettings;
        }

        private TestCompilerSettings SetupTestCompilerSettings(CLikeCompiler compiler = null) =>
            SetupCompilerSettings(compiler) as TestCompilerSettings;

        private ObjectFileLinker SetupObjectFileLinker(ObjectFileLinker linker = null)
        {
            linker = linker ?? new TestObjectLinker(Toolchain);
            var config = new NativeProgramConfiguration(CodeGen.Debug, Toolchain, true);
            return new MockFormat(linker).SetupLinker(NativeProgram, config, TargetBinary, "artifacts");
        }

        private TestObjectLinker SetupTestObjectFileLinker(TestObjectLinker linker = null) =>
            SetupObjectFileLinker(linker) as TestObjectLinker;

        private NativeProgram NativeProgram { get; set; }
        private ToolChain Toolchain { get; set; }

        private const string DefaultValue = "default-value";
        private const string TargetBinary = "test-program.bin";
        private const string CppSourceFile = "test-program.cpp";
        private const string UnusedCppSourceFile = "unused-test-program.cpp";

        [SetUp]
        public void SetUp()
        {
            Backend.Current = new Mock<IBackend>().Object;
            Toolchain = new TestToolchain();
            NativeProgram = new NativeProgram("TestProgram");
            NativeProgram.Sources.Add(CppSourceFile);
        }

        // CompilerSettings
        // -------------------------------------------------------------------
        [Test]
        public void Compiler_DefaultSettings_ArePickedUp_ByCompiler()
        {
            Assert.That(SetupCompilerSettings(), Is.TypeOf<TestCompilerSettings>());
            Assert.That(SetupTestCompilerSettings().TestProperty, Is.EqualTo(DefaultValue));
        }

        [Test]
        public void NativeProgram_CompilerSettings_ArePickedUp_ByCompiler()
        {
            const string value = "expected-value";
            NativeProgram.CompilerSettings().Add(c => c is TestCompilerSettings settings ? settings.WithTestProperty(value) : c);
            Assert.That(SetupTestCompilerSettings().TestProperty, Is.EqualTo(value));
        }

        [Test]
        public void PlatformSpecifc_NativeProgram_CompilerSettings_ArePickedUp_ByCompiler()
        {
            const string value = "expected-value";
            NativeProgram.CompilerSettingsForTest().Add(c => c.WithTestProperty(value));
            Assert.That(SetupTestCompilerSettings().TestProperty, Is.EqualTo(value));
        }

        [Test]
        public void ToolchainSpecific_CompilerSettings_ArePickedUp_ByCompiler()
        {
            const string toolchainSpecificValue = "toolchain-specific-value";
            var compiler = new TestCompiler(Toolchain, toolchainSpecificValue);
            Assert.That(SetupTestCompilerSettings(compiler).TestProperty, Is.EqualTo(toolchainSpecificValue));
        }

        [Test]
        public void NativeProgram_CompilerSettings_TakePrecedenceToToolchainSpecific_CompilerSettings()
        {
            const string nativeProgramValue = "nativeprogram-value";
            const string toolchainSpecificValue = "toolchain-specific-value";
            var compiler = new TestCompiler(Toolchain, toolchainSpecificValue);
            NativeProgram.CompilerSettingsForTest().Add(c => c.WithTestProperty(nativeProgramValue));
            Assert.That(SetupTestCompilerSettings(compiler).TestProperty, Is.EqualTo(nativeProgramValue));
        }

        [Test]
        public void NativeProgram_CompilerSettings_OnUnusedSourceFile_AreNotPickedUp_ByCompiler()
        {
            const string value = "nativeprogram-value";
            NativeProgram.CompilerSettingsForTest().Add(c => c.WithTestProperty(value), UnusedCppSourceFile);
            Assert.That(SetupTestCompilerSettings().TestProperty, Is.EqualTo(DefaultValue));
        }

        [Test]
        public void NativeProgram_CompilerSettings_OnSourceFile_ArePickedUp_ByCompiler()
        {
            const string value = "nativeprogram-value";
            NativeProgram.CompilerSettingsForTest().Add(c => c.WithTestProperty(value), CppSourceFile);
            Assert.That(SetupTestCompilerSettings().TestProperty, Is.EqualTo(value));
        }

        [Test]
        public void NativeProgram_CompilerSettings_AreAppliedInTheOrderTheyAreListedInTheBuildProgram()
        {
            const string cppSpecificValue = "cpp-specific";
            const string platformSpecificValue = "platform-specific";
            const string nonSpecificValue = "non-specific";
            NativeProgram.CompilerSettingsForTest().Add(c => c.WithTestProperty(cppSpecificValue), CppSourceFile);
            NativeProgram.CompilerSettingsForTest().Add(c => c.WithTestProperty(platformSpecificValue));
            NativeProgram.CompilerSettings().Add(c => c is TestCompilerSettings settings ? settings.WithTestProperty(nonSpecificValue) : c);
            Assert.That(SetupTestCompilerSettings().TestProperty, Is.EqualTo(nonSpecificValue));
        }

        // LinkerSettings
        // -------------------------------------------------------------------
        [Test]
        public void Correct_Linker_IsUsed_ByNativeProgram()
        {
            Assert.That(SetupObjectFileLinker(), Is.TypeOf<TestObjectLinker>());
            Assert.That(SetupTestObjectFileLinker().TestProperty, Is.EqualTo(DefaultValue));
        }

        [Test]
        public void NativeProgram_LinkerSettings_ArePickedUp_ByLinker()
        {
            const string value = "expected-value";
            NativeProgram.DynamicLinkerSettings().Add(c => c is TestObjectLinker settings ? settings.WithTestProperty(value) : c);
            Assert.That(SetupTestObjectFileLinker().TestProperty, Is.EqualTo(value));
        }

        [Test]
        public void PlatformSpecifc_NativeProgram_LinkerSettings_ArePickedUp_ByLinker()
        {
            const string value = "expected-value";
            NativeProgram.DynamicLinkerSettingsForTest().Add(c => c.WithTestProperty(value));
            Assert.That(SetupTestObjectFileLinker().TestProperty, Is.EqualTo(value));
        }

        [Test]
        public void ToolchainSpecific_LinkerSettings_ArePickedUp_ByLinker()
        {
            const string toolchainSpecificValue = "toolchain-specific-value";
            var linker = new TestObjectLinker(Toolchain, toolchainSpecificValue);
            Assert.That(SetupTestObjectFileLinker(linker).TestProperty, Is.EqualTo(toolchainSpecificValue));
        }

        [Test]
        public void NativeProgram_LinkerSettings_TakePrecedenceToToolchainSpecific_LinkerSettings()
        {
            const string nativeProgramValue = "nativeprogram-value";
            const string toolchainSpecificValue = "toolchain-specific-value";
            var linker = new TestObjectLinker(Toolchain, toolchainSpecificValue);
            NativeProgram.DynamicLinkerSettingsForTest().Add(c => c.WithTestProperty(nativeProgramValue));
            Assert.That(SetupTestObjectFileLinker(linker).TestProperty, Is.EqualTo(nativeProgramValue));
        }

        [Test]
        public void NativeProgram_LinkerSettings_OnFileThatIsNotTargetBinary_AreNotPickedUp_ByLinker()
        {
            const string value = "nativeprogram-value";
            NativeProgram.DynamicLinkerSettingsForTest().Add(c => c.WithTestProperty(value), CppSourceFile);
            Assert.That(SetupTestObjectFileLinker().TestProperty, Is.EqualTo(DefaultValue));
        }

        [Test]
        public void NativeProgram_LinkerSettings_OnTargetBinary_ArePickedUp_ByLinker()
        {
            const string value = "nativeprogram-value";
            NativeProgram.DynamicLinkerSettingsForTest().Add(c => c.WithTestProperty(value), TargetBinary);
            Assert.That(SetupTestObjectFileLinker().TestProperty, Is.EqualTo(value));
        }

        [Test]
        public void NativeProgram_LinkerSettings_AreAppliedInTheOrderTheyAreListedInTheBuildProgram()
        {
            const string targetSpecificValue = "target-specific";
            const string platformSpecificValue = "platform-specific";
            const string nonSpecificValue = "non-specific";
            NativeProgram.DynamicLinkerSettingsForTest().Add(c => c.WithTestProperty(targetSpecificValue), TargetBinary);
            NativeProgram.DynamicLinkerSettingsForTest().Add(c => c.WithTestProperty(platformSpecificValue));
            NativeProgram.DynamicLinkerSettings().Add(c => c is TestObjectLinker settings ? settings.WithTestProperty(nonSpecificValue) : c);
            Assert.That(SetupTestObjectFileLinker().TestProperty, Is.EqualTo(nonSpecificValue));
        }

        // CompilerSettings for the linker
        // -------------------------------------------------------------------
        [Test]
        public void Toolchain_CompilerSettings_ArePickedUp_ByLinker()
        {
            Assert.That(SetupTestObjectFileLinker().TestCompilerSettings, Is.Not.Null);
            Assert.That(SetupTestObjectFileLinker().TestCompilerSettings.TestProperty, Is.EqualTo(DefaultValue));
        }

        [Test]
        public void NativeProgram_CompilerSettings_ArePickedUp_ByLinker()
        {
            const string nativeProgramValue = "nativeprogram-value";
            NativeProgram.CompilerSettingsForTest().Add(c => c.WithTestProperty(nativeProgramValue));
            Assert.That(SetupTestObjectFileLinker().TestCompilerSettings.TestProperty, Is.EqualTo(nativeProgramValue));
        }

        [Test]
        public void NativeProgram_CompilerSettings_OnFileThatIsNotTheTargetBinary_AreNotPickedUp_ByLinker()
        {
            const string value = "nativeprogram-value";
            NativeProgram.CompilerSettingsForTest().Add(c => c.WithTestProperty(value), CppSourceFile);
            Assert.That(SetupTestObjectFileLinker().TestCompilerSettings.TestProperty, Is.EqualTo(DefaultValue));
        }

        [Test]
        public void NativeProgram_CompilerSettings_OnTargetBinary_ArePickedUp_ByCompiler()
        {
            const string value = "nativeprogram-value";
            NativeProgram.CompilerSettingsForTest().Add(c => c.WithTestProperty(value), TargetBinary);
            Assert.That(SetupTestObjectFileLinker().TestCompilerSettings.TestProperty, Is.EqualTo(value));
        }
    }
}
