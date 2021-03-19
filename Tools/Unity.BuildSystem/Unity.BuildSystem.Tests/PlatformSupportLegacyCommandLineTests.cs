using JamSharp.Runtime;
using NUnit.Framework;
using Unity.BuildSystem.NativeProgramSupport;
using Unity.BuildSystem.V2;

namespace Unity.BuildSystem.Tests
{
    [TestFixture]
    public class PlatformSupportLegacyCommandLineTests
    {
        JamList _backupCommandLine;
        JamList _backupPlatformsDefines;

        [SetUp]
        public void Setup()
        {
            _backupCommandLine = ConvertedJamFile.Vars.JAM_COMMAND_LINE_TARGETS;
            _backupPlatformsDefines = ConvertedJamFile.Vars.UNITY_TARGET_DEFINES_PLATFORM;
            ConvertedJamFile.Vars.UNITY_TARGET_DEFINES_PLATFORM.Assign(
                "PLATFORM_STANDALONE_WIN",
                "PLATFORM_STANDALONE_OSX",
                "PLATFORM_STANDALONE_LINUX",
                "PLATFORM_ANDROID",
                "PLATFORM_IOS",
                "PLATFORM_TVOS",
                "PLATFORM_METRO",
                "PLATFORM_PS4",
                "PLATFORM_XBOXONE",
                "PLATFORM_SWITCH");
        }

        [TearDown]
        public void TearDown()
        {
            ConvertedJamFile.Vars.JAM_COMMAND_LINE_TARGETS.Assign(_backupCommandLine);
            ConvertedJamFile.Vars.PLATFORM.Assign(new JamList());
            ConvertedJamFile.Vars.CONFIG.Assign(new JamList());
            ConvertedJamFile.Vars.SCRIPTING_BACKEND.Assign(new JamList());
            ConvertedJamFile.Vars.DEVELOPMENT_PLAYER.Assign(new JamList());
            GlobalVariables.Singleton["LUMP"].Assign(new JamList());
            ConvertedJamFile.Vars.UNITY_TARGET_DEFINES_PLATFORM.Assign(_backupPlatformsDefines);
        }

        void SetBuildCommand(string target, string platform = null, string config = "debug", string scripting = "mono", string dev = "1", string lump = "1")
        {
            ConvertedJamFile.Vars.JAM_COMMAND_LINE_TARGETS.Assign(target);
            if (platform != null) ConvertedJamFile.Vars.PLATFORM.Assign(platform);
            if (config != null) ConvertedJamFile.Vars.CONFIG.Assign(config);
            if (scripting != null) ConvertedJamFile.Vars.SCRIPTING_BACKEND.Assign(scripting);
            if (dev != null) ConvertedJamFile.Vars.DEVELOPMENT_PLAYER.Assign(dev);
            if (lump != null) GlobalVariables.Singleton["LUMP"].Assign(lump);
        }

        void CheckVariation(UnityPlayerConfiguration cfg, CodeGen codegen, ScriptingBackend scripting, Architecture arch, bool development = true, bool lump = true)
        {
            Assert.AreEqual(codegen, cfg.CodeGen);
            Assert.AreEqual(scripting, cfg.ScriptingBackend);
            Assert.AreEqual(arch, cfg.ToolChain.Architecture);
            Assert.AreEqual(development, cfg.DevelopmentPlayer);
            Assert.AreEqual(lump, cfg.Lump);
        }

        [Test]
        public void WinStandalone_Default()
        {
            SetBuildCommand("StandalonePlayer");
            var s = new WinStandaloneSupport();
            Assert.AreEqual(1, s.VariationsToSetup.Length);
            CheckVariation(s.VariationsToSetup[0], CodeGen.Debug, ScriptingBackend.Mono, Architecture.x64);
        }

        [Test]
        public void WinStandalone_Release()
        {
            SetBuildCommand("StandalonePlayer", config: "release");
            var s = new WinStandaloneSupport();
            Assert.AreEqual(1, s.VariationsToSetup.Length);
            CheckVariation(s.VariationsToSetup[0], CodeGen.Release, ScriptingBackend.Mono, Architecture.x64);
        }

        [Test]
        public void WinStandalone_IL2CPPNonDev32Release()
        {
            SetBuildCommand("StandalonePlayer", config: "release", scripting: "il2cpp", dev: "0", platform: "win32");
            var s = new WinStandaloneSupport();
            Assert.AreEqual(2, s.VariationsToSetup.Length);
            CheckVariation(s.VariationsToSetup[0], CodeGen.Release, ScriptingBackend.IL2CPP, Architecture.x86, development: false);
            Assert.AreEqual(s.DefaultDeveloperConfiguration, s.VariationsToSetup[1]);
        }

        [Test]
        public void MacStandalone_Default()
        {
            SetBuildCommand("MacStandalonePlayer");
            var s = new MacStandaloneSupport();
            Assert.AreEqual(1, s.VariationsToSetup.Length);
            CheckVariation(s.VariationsToSetup[0], CodeGen.Debug, ScriptingBackend.Mono, Architecture.x64);
        }

        [Test]
        public void MacStandalone_Release()
        {
            SetBuildCommand("MacStandalonePlayer", config: "release");
            var s = new MacStandaloneSupport();
            Assert.AreEqual(1, s.VariationsToSetup.Length);
            CheckVariation(s.VariationsToSetup[0], CodeGen.Release, ScriptingBackend.Mono, Architecture.x64);
        }

        [Test]
        public void MacStandalone_NoLump()
        {
            SetBuildCommand("MacStandalonePlayer", lump: "0");
            var s = new MacStandaloneSupport();
            Assert.AreEqual(2, s.VariationsToSetup.Length);
            CheckVariation(s.VariationsToSetup[0], CodeGen.Debug, ScriptingBackend.Mono, Architecture.x64, lump: false);
            Assert.AreEqual(s.DefaultDeveloperConfiguration, s.VariationsToSetup[1]);
        }

        [Test]
        public void MacStandalone_IL2CPPNonDevRelease()
        {
            SetBuildCommand("MacStandalonePlayer", config: "release", scripting: "il2cpp", dev: "0");
            var s = new MacStandaloneSupport();
            Assert.AreEqual(2, s.VariationsToSetup.Length);
            CheckVariation(s.VariationsToSetup[0], CodeGen.Release, ScriptingBackend.IL2CPP, Architecture.x64, development: false);
            Assert.AreEqual(s.DefaultDeveloperConfiguration, s.VariationsToSetup[1]);
        }

        #if PLATFORM_IN_SOURCESTREE_ANDROID
        [Test]
        public void Android_Default()
        {
            SetBuildCommand("AndroidSupport");
            var s = new PlatformDependent.AndroidPlayer.Jam.AndroidSupport();
            // Mono, 32 bit (ARM & Intel)
            Assert.AreEqual(2, s.VariationsToSetup.Length);
            CheckVariation(s.VariationsToSetup[0], CodeGen.Debug, ScriptingBackend.Mono, Architecture.Armv7);
            CheckVariation(s.VariationsToSetup[1], CodeGen.Debug, ScriptingBackend.Mono, Architecture.x86);
        }

        [Test]
        public void Android_IL2CPP()
        {
            SetBuildCommand("AndroidSupport", scripting: "il2cpp");
            var s = new PlatformDependent.AndroidPlayer.Jam.AndroidSupport();
            // IL2CPP, all archs
            Assert.AreEqual(4, s.VariationsToSetup.Length);
            CheckVariation(s.VariationsToSetup[0], CodeGen.Debug, ScriptingBackend.IL2CPP, Architecture.Armv7);
            CheckVariation(s.VariationsToSetup[1], CodeGen.Debug, ScriptingBackend.IL2CPP, Architecture.Arm64);
            CheckVariation(s.VariationsToSetup[2], CodeGen.Debug, ScriptingBackend.IL2CPP, Architecture.x86);
            Assert.AreEqual(s.DefaultDeveloperConfiguration, s.VariationsToSetup[3]);
        }

        #endif // #if PLATFORM_IN_SOURCESTREE_ANDROID

        #if PLATFORM_IN_SOURCESTREE_IOS
        [Test]
        public void Ios_Default()
        {
            // note, this actually sets up V2 since the new name matches legacy name
            SetBuildCommand("iOSPlayer");
            var s = new PlatformDependent.iPhonePlayer.Jam.iOSSupport();
            Assert.AreEqual(1, s.VariationsToSetup.Length);
            CheckVariation(s.VariationsToSetup[0], CodeGen.Debug, ScriptingBackend.Mono, Architecture.Armv7);
        }

        [Test]
        public void IosSim_Default()
        {
            SetBuildCommand("iOSSimulatorPlayer");
            var s = new PlatformDependent.iPhonePlayer.Jam.iOSSupport();
            Assert.AreEqual(2, s.VariationsToSetup.Length);
            CheckVariation(s.VariationsToSetup[0], CodeGen.Debug, ScriptingBackend.Mono, Architecture.x86);
            Assert.AreEqual(s.DefaultDeveloperConfiguration, s.VariationsToSetup[1]);
        }

        [Test]
        public void Tvos_Default()
        {
            SetBuildCommand("AppleTVPlayer");
            var s = new PlatformDependent.iPhonePlayer.Jam.tvOSSupport();
            Assert.AreEqual(1, s.VariationsToSetup.Length);
            CheckVariation(s.VariationsToSetup[0], CodeGen.Debug, ScriptingBackend.IL2CPP, Architecture.Arm64);
        }

        [Test]
        public void TvosSim_Default()
        {
            SetBuildCommand("AppleTVSimulatorPlayer");
            var s = new PlatformDependent.iPhonePlayer.Jam.tvOSSupport();
            Assert.AreEqual(2, s.VariationsToSetup.Length);
            CheckVariation(s.VariationsToSetup[0], CodeGen.Debug, ScriptingBackend.IL2CPP, Architecture.x64);
            Assert.AreEqual(s.DefaultDeveloperConfiguration, s.VariationsToSetup[1]);
        }

        [Test]
        public void Ios_ReleaseNonDev()
        {
            SetBuildCommand("iOSPlayer", config: "release", dev: "0");
            var s = new PlatformDependent.iPhonePlayer.Jam.iOSSupport();
            Assert.AreEqual(2, s.VariationsToSetup.Length);
            CheckVariation(s.VariationsToSetup[0], CodeGen.Release, ScriptingBackend.Mono, Architecture.Armv7, development: false);
            Assert.AreEqual(s.DefaultDeveloperConfiguration, s.VariationsToSetup[1]);
        }

        [Test]
        public void Ios_IL2CPP()
        {
            SetBuildCommand("iOSPlayer", scripting: "il2cpp");
            var s = new PlatformDependent.iPhonePlayer.Jam.iOSSupport();
            Assert.AreEqual(3, s.VariationsToSetup.Length);
            // legacy build does both 32 & 64 bit ARM archs
            CheckVariation(s.VariationsToSetup[0], CodeGen.Debug, ScriptingBackend.IL2CPP, Architecture.Armv7);
            CheckVariation(s.VariationsToSetup[1], CodeGen.Debug, ScriptingBackend.IL2CPP, Architecture.Arm64);
            Assert.AreEqual(s.DefaultDeveloperConfiguration, s.VariationsToSetup[2]);
        }

        #endif // #if PLATFORM_IN_SOURCESTREE_IOS
    }
}
