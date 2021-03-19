using System;
using System.Collections.Generic;
using NiceIO;
using Unity.BuildSystem.NativeProgramSupport;

namespace Bee.Toolchain.ARM
{
    public class ArmDynamicLinker : DynamicLinker
    {
        public override bool DynamicLibrary { get; set; }
        protected override bool SupportsResponseFile { get; } = false;

        public ArmDynamicLinker(ToolChain toolChain) : base(toolChain) {}

        protected override IEnumerable<string> CommandLineFlagsFor(NPath destination, CodeGen codegen, IEnumerable<NPath> objectFiles,
            IEnumerable<PrecompiledLibrary> nonToolchainSpecificLibraries, IEnumerable<PrecompiledLibrary> allLibraries)
        {
            switch (codegen)
            {
                case CodeGen.Debug:
                    yield return "--debug";
                    yield return "--bestdebug";
                    break;
                case CodeGen.Release:
                    yield return "--debug";
                    yield return "--no_bestdebug";
                    yield return "--tailreorder";
                    yield return "--inline";
                    break;
                case CodeGen.Master:
                    yield return "--no_debug";
                    yield return "--no_bestdebug";
                    yield return "--tailreorder";
                    yield return "--inline";
                    break;
                default:
                    throw new Exception("Unsupported codegen: ");
            }

            // we like strict
            yield return "--strict";

            // padding value (byte)
            yield return "--pad=0xFF";

            // code stripping
            yield return "--vfemode=force";
            yield return "--remove";

            // should be default, but just in case
            yield return "--merge";

            // msvc style error codes
            yield return "--diag_style=ide";

            foreach (var objectFile in objectFiles)
                yield return objectFile.InQuotes();

            foreach (var lib in nonToolchainSpecificLibraries)
                yield return lib.System ? $"lib{lib.Name}.a" : lib.InQuotes();

            yield return "-o " + destination;
        }
    }
}
