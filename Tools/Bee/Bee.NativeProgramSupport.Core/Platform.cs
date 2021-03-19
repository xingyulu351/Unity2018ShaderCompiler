using System;
using System.Collections.Generic;
using System.Linq;
using Bee.Core;
using Unity.BuildTools;

namespace Unity.BuildSystem.NativeProgramSupport
{
    public abstract class Platform : ClassInsteadOfEnumPatternBase
    {
        public abstract bool HasPosix { get; }
        public virtual string Name => GetType().Name.StripEnd("Platform");
        public virtual string DisplayName => Name;

        private static readonly Lazy<Platform[]> _allPlatforms = new Lazy<Platform[]>(() =>
            DynamicLoader.FindAndCreateInstanceOfEachInAllAssemblies<Platform>("Bee.Toolchain.*.dll", true)
                .Concat(DynamicLoader.FindAndCreateInstanceOfEachInAllAssemblies<Platform>("Bee.*.Tests.dll")).ToArray());
        public static IEnumerable<Platform> All => _allPlatforms.Value;

        public static Platform HostPlatform
        {
            get
            {
                if (Unity.BuildTools.HostPlatform.IsLinux)
                    return new LinuxPlatform();
                if (Unity.BuildTools.HostPlatform.IsOSX)
                    return new MacOSXPlatform();
                else
                    return new WindowsPlatform();
            }
        }
    }

    public class WindowsPlatform : Platform
    {
        public override bool HasPosix => false;
        public override string DisplayName => "Win";
    }

    public class LinuxPlatform : Platform
    {
        public override bool HasPosix => true;
    }

    public class MacOSXPlatform : Platform
    {
        public override bool HasPosix => true;
        public override string Name => "OSX";
        public override string DisplayName => "Mac";
    }

    public class IosPlatform : Platform
    {
        public override bool HasPosix => true;
        public override String Name => "IOS";
        public override string DisplayName => "iOS";
    }

    public class TvosPlatform : Platform
    {
        public override bool HasPosix => true;
        public override string DisplayName => "tvOS";
    }

    public class UniversalWindowsPlatform : Platform
    {
        public override bool HasPosix => false;
        public override string DisplayName => "UWP";
    }

    public class SwitchPlatform : Platform
    {
        public override bool HasPosix => false;
    }

    public class XboxOnePlatform : Platform
    {
        public override bool HasPosix => false;
    }

    public class PS4Platform : Platform
    {
        public override bool HasPosix => true;
    }

    public class PSVitaPlatform : Platform
    {
        public override bool HasPosix => true;
        public override string DisplayName => "Vita";
    }

    public class AndroidPlatform : Platform
    {
        public override bool HasPosix => true;
    }

    public class LuminPlatform : Platform
    {
        public override bool HasPosix => true;
    }

    public class WebGLPlatform : Platform
    {
        public override bool HasPosix { get; } = true;
    }
}
