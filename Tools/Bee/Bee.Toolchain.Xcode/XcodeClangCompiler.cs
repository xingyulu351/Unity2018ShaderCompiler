using System;
using System.Collections.Generic;
using System.Linq;
using Bee.Toolchain.LLVM;
using NiceIO;
using Unity.BuildSystem.NativeProgramSupport;
using Unity.BuildTools;

namespace Bee.Toolchain.Xcode
{
    public class XcodeClangCompiler : ClangCompiler
    {
        // only use CCache on iOS/tvOS for time being
        protected override bool SupportsCCache => Toolchain.Platform is IosPlatform || Toolchain.Platform is TvosPlatform;

        public override string[] SupportedExtensions => base.SupportedExtensions.Append("mm", "m").ToArray();

        public XcodeClangCompiler(XcodeToolchain toolchain) : base(toolchain)
        {
            DefaultSettings = new XcodeClangCompilerSettings(this);
        }
    }

    public class XcodeClangCompilerSettings : ClangCompilerSettings
    {
        public XcodeClangCompilerSettings(XcodeClangCompiler compiler) : base(compiler)
        {
        }

        public override IEnumerable<string> CommandLineFlagsFor(NPath target)
        {
            if (!RTTI && Exceptions)
                throw new Exception("Enabling RTTI requires C++ exceptions to be enabled as well for the XcodeClangCompiler. Code would still compile but some exceptions would silently not be handled.");

            foreach (var flag in base.CommandLineFlagsFor(target))
                yield return flag;

            var toolchain = (XcodeToolchain)Toolchain;
            yield return FlagForStdLib(toolchain.StandardLibrary);
            yield return FlagForArch(Toolchain.Architecture);
            if (toolchain.Architecture is ARMv7Architecture)
            {
                yield return "-mfpu=neon-fp16";
                yield return "-mno-thumb";
            }

            // iOS/tvOS Simulator specific flags
            if (toolchain.Architecture is IntelArchitecture)
            {
                var ios = toolchain.Platform is IosPlatform;
                var tvos = toolchain.Platform is TvosPlatform;
                if (ios)
                    yield return "-DTARGET_IPHONE_SIMULATOR=1";
                if (tvos)
                    yield return "-DTARGET_TVOS_SIMULATOR=1";
                if (ios || tvos)
                {
                    // not exactly sure why these are needed, but Xcode 9.3 for newly created
                    // simulator projects does emit these flags, so play it safe and
                    // follow what Xcode does
                    yield return "-fobjc-abi-version=2";
                    yield return "-fobjc-legacy-dispatch";
                }
            }

            if (toolchain.MinOSVersion != null)
                yield return FlagForMinVersion(toolchain.MinOSVersion, toolchain.Platform);
        }

        public static string FlagForStdLib(StdLib stdLib)
        {
            switch (stdLib)
            {
                case StdLib.libcpp:
                    return "-stdlib=libc++";
                case StdLib.libstdcpp:
                    return "-stdlib=libstdc++";
                default:
                    throw new ArgumentOutOfRangeException();
            }
        }

        public static string FlagForArch(Architecture architecture)
        {
            return "-arch " + NamingHelpers.AppleNameForArch(architecture);
        }

        public static string FlagForMinVersion(string version, Platform platform)
        {
            if (platform is MacOSXPlatform)
                return $"-mmacosx-version-min={version}";
            if (platform is IosPlatform)
                return $"-mios-version-min={version}";
            if (platform is TvosPlatform)
                return $"-mtvos-version-min={version}";

            throw new NotSupportedException("Not supported platform: " + platform);
        }
    }
}
