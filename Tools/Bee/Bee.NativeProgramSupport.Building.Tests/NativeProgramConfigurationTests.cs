using System.Collections.Generic;
using System.Linq;
using NUnit.Framework;
using Unity.BuildSystem.NativeProgramSupport;
using Unity.BuildTools;

namespace Bee.NativeProgramSupport.Building.Tests
{
    public class NativeProgramConfigurationTests
    {
        [Test]
        // IDE project generation code depends on first item on the list being chosen as "best" when multiple are
        // present, to pick the first scripting backend & platform specified for a NativeProgram when multiple
        // choices would be "just as good". E.g. in VS solution, "Debug" should map to "x64, Mono" for Standalone.
        public void FindClosestConfig_WhenMultipleSameScore_ReturnsFirstOneInList()
        {
            var toolchain = new GenericToolChain(new WindowsPlatform(), new x64Architecture(), "win64");
            var cfg1 = new NativeProgramConfiguration(CodeGen.Release, toolchain, true);
            var cfg2 = new NativeProgramConfiguration(CodeGen.Debug, toolchain, true); // this one should be found, since it's first
            var cfg3 = new NativeProgramConfiguration(CodeGen.Debug, toolchain, true); // even if this one is equivalent
            var cfg = new NativeProgramConfiguration(CodeGen.Debug, toolchain, true);
            NativeProgramConfiguration[] configs = { cfg1, cfg2, cfg3 };
            var bestConfig = cfg.FindBestMatching("win64", configs);
            Assert.IsTrue(bestConfig == cfg2);
        }

        [Test]
        public void FindBestMatching_PrefersPlatformMatchOverBitnessMatch()
        {
            var tcWin64 = new GenericToolChain(new WindowsPlatform(), new x64Architecture(), "win64");
            var tcWin32 = new GenericToolChain(new WindowsPlatform(), new x86Architecture(), "win32");
            var tcAndroidArm7 = new GenericToolChain(new AndroidPlatform(), new ARMv7Architecture(), "android-arm7");
            var tcAndroidArm8 = new GenericToolChain(new AndroidPlatform(), new Arm64Architecture(), "android-arm8");
            var tcAndroidx86 = new GenericToolChain(new AndroidPlatform(), new x86Architecture(), "android-x86");
            var configsWin = new[]
            {
                new NativeProgramConfiguration(CodeGen.Debug, tcWin64, true),
                new NativeProgramConfiguration(CodeGen.Debug, tcWin32, true),
            };
            var configsAndroid = new[]
            {
                new NativeProgramConfiguration(CodeGen.Debug, tcAndroidArm7, true),
                new NativeProgramConfiguration(CodeGen.Debug, tcAndroidArm8, true),
                new NativeProgramConfiguration(CodeGen.Debug, tcAndroidx86, true),
            };
            var cfg = new NativeProgramConfiguration(CodeGen.Debug, tcWin64, true);
            // Toolchain used in cfg uses 64 bit architecture, but some of the configs
            // we are searching through use 32 bits. We want to find those anyway, i.e. platform name match should
            // have more weight than toolchain bitness match.
            Assert.AreEqual(configsAndroid[0], cfg.FindBestMatching("android-arm7", configsAndroid));
            Assert.AreEqual(configsAndroid[1], cfg.FindBestMatching("android-arm8", configsAndroid));
            Assert.AreEqual(configsAndroid[2], cfg.FindBestMatching("android-x86", configsAndroid));
            Assert.AreEqual(configsWin[0], cfg.FindBestMatching("win64", configsWin));
            Assert.AreEqual(configsWin[1], cfg.FindBestMatching("win32", configsWin));
        }
    }
}
