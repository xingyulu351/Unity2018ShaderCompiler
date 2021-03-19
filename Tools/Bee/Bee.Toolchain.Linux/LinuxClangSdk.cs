using System;
using System.Collections.Generic;
using NiceIO;
using System.Linq;
using Bee.NativeProgramSupport;
using Unity.BuildSystem.NativeProgramSupport;

namespace Bee.Toolchain.Linux
{
    public class LinuxClangSdk : LinuxSdk
    {
        public LinuxGccSdk GccSdk { get; }
        public NPath GccToolchain => GccSdk.Path;

        public override NPath CppCompilerPath { get; }
        public override NPath DynamicLinkerPath { get; }
        public override NPath StaticLinkerPath { get; }

        public override bool IsValid => base.IsValid && GccSdk.IsValid;

        public LinuxClangSdk(LinuxGccSdk gccSdk, NPath clangPath)
            : base(gccSdk.SysRoot, clangPath, gccSdk.Architecture, gccSdk.TargetDistribution)
        {
            CppCompilerPath = GetToolchainPathFor("clang++");
            StaticLinkerPath = GetToolchainPathFor("llvm-ar");
            DynamicLinkerPath = GetToolchainPathFor("clang++");
            GccSdk = gccSdk;
        }

        // Clang executables are named as-is and are located in clangPath/bin
        public override NPath GetToolchainPathFor(string binary) => Path.Combine("bin").Combine(binary);
    }
}
