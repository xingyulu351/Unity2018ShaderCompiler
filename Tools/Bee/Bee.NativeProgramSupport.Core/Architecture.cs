using System;
using System.Collections.Generic;
using System.Linq;
using Bee.Core;

namespace Unity.BuildSystem.NativeProgramSupport
{
    public abstract class Architecture : ClassInsteadOfEnumPatternBase
    {
        public abstract int Bits { get; }
        public abstract string Name { get; }
        public abstract string DisplayName { get; }

        public static IntelArchitecture OfCurrentProcess => IntPtr.Size == 4 ? (IntelArchitecture) new x86Architecture() : new x64Architecture();
        public static IntelArchitecture BestThisMachineCanRun => new x64Architecture();

        public static x86Architecture x86 { get; } = new x86Architecture();
        public static x64Architecture x64 { get; } = new x64Architecture();
        public static ARMv7Architecture Armv7 { get; } = new ARMv7Architecture();
        public static Arm64Architecture Arm64 { get; } = new Arm64Architecture();

        private static readonly Lazy<Architecture[]> _allArchitectures = new Lazy<Architecture[]>(() => DynamicLoader.FindAndCreateInstanceOfEachInAllAssemblies<Architecture>("Bee.Toolchain.*.dll", true).ToArray());
        public static IEnumerable<Architecture> All => _allArchitectures.Value;
    }

    public abstract class IntelArchitecture : Architecture
    {
    }

    public sealed class x64Architecture : IntelArchitecture
    {
        public override int Bits => 64;
        public override string Name => "x86_64";
        public override string DisplayName => "x64";
    }

    public sealed class x86Architecture : IntelArchitecture
    {
        public override int Bits => 32;
        public override string Name => "x86";
        public override string DisplayName => "x86";
    }

    public abstract class ArmArchitecture : Architecture
    {
    }

    public sealed class ARMv6Architecture : ArmArchitecture
    {
        public override int Bits => 32;
        public override string Name => "armv6";
        public override string DisplayName => "arm32";
    }

    public sealed class ARMv7Architecture : ArmArchitecture
    {
        public override int Bits => 32;
        public override string Name => "armv7";
        public override string DisplayName => "arm32";
    }

    public sealed class Arm64Architecture : ArmArchitecture
    {
        public override int Bits => 64;
        public override string Name => "aarch64";
        public override string DisplayName => "arm64";
    }
}
