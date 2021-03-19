using System.Linq;
using Bee.Core;
using Unity.BuildSystem.NativeProgramSupport;

namespace Bee.NativeProgramSupport.Building.FluentSyntaxHelpers
{
    public sealed class FluentPlatforms
    {
        internal FluentPlatforms()
        {
        }
    }

    public sealed class FluentPlatform<T> where T : Platform
    {
    }

    public static class FluentPlatformExtensions
    {
        public static FluentPlatform<MacOSXPlatform> Mac(this FluentPlatforms _) => new FluentPlatform<MacOSXPlatform>();
        public static FluentPlatform<WindowsPlatform> Windows(this FluentPlatforms _) => new FluentPlatform<WindowsPlatform>();
        public static FluentPlatform<IosPlatform> Ios(this FluentPlatforms _) => new FluentPlatform<IosPlatform>();
        public static FluentPlatform<TvosPlatform> Tvos(this FluentPlatforms _) => new FluentPlatform<TvosPlatform>();
        public static FluentPlatform<WebGLPlatform> Emscripten(this FluentPlatforms _) => new FluentPlatform<WebGLPlatform>();
        public static FluentPlatform<AndroidPlatform> Android(this FluentPlatforms _) => new FluentPlatform<AndroidPlatform>();
        public static FluentPlatform<LuminPlatform> Lumin(this FluentPlatforms _) => new FluentPlatform<LuminPlatform>();
        public static FluentPlatform<LinuxPlatform> Linux(this FluentPlatforms _) => new FluentPlatform<LinuxPlatform>();
        public static FluentPlatform<UniversalWindowsPlatform> UniversalWindows(this FluentPlatforms _) => new FluentPlatform<UniversalWindowsPlatform>();
        public static FluentPlatform<SwitchPlatform> Switch(this FluentPlatforms _) => new FluentPlatform<SwitchPlatform>();
        public static FluentPlatform<PS4Platform> PS4(this FluentPlatforms _) => new FluentPlatform<PS4Platform>();
        public static FluentPlatform<XboxOnePlatform> XboxOne(this FluentPlatforms _) => new FluentPlatform<XboxOnePlatform>();

        public static ToolChain Host(this FluentPlatforms _)
        {
            return DynamicLoader.FindAndCreateInstanceOfEachInAllAssemblies<ToolChainForHostProvider>("Bee.*.dll").Select(p => p.Provide()).First(t => t != null);
        }
    }
}
