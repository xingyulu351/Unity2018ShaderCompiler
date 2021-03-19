using System;
using System.Collections.Generic;
using Bee.Core;
using NiceIO;
using Unity.BuildSystem.NativeProgramSupport;
using Unity.BuildSystem.VisualStudio;
using Unity.BuildTools;

namespace Bee.Toolchain.VisualStudio
{
    public enum SubSystemType
    {
        Console,
        Windows
    }

    public static class MsvcDynamicLinkerExtensions
    {
        // NativeProgramFormat settings
        public static T WithPdbName<T>(this T _this, string value) where T : MsvcDynamicLinker =>
            _this.With<T>(l => l.PdbName = value);

        public static T WithStackSize<T>(this T _this, int value) where T : MsvcDynamicLinker =>
            _this.With<T>(c => c.StackSize = value);

        public static T WithGenerateWindowsMetadata<T>(this T _this) where T : MsvcDynamicLinker =>
            _this.With<T>(c => c.GenerateWindowsMetadata = true);

        public static T WithSubSystemType<T>(this T _this, SubSystemType subSystem) where T : MsvcDynamicLinker =>
            _this.With<T>(c => c.SubSystem = subSystem);

        public static T WithoutEntryPoint<T>(this T _this) where T : MsvcDynamicLinker =>
            _this.With<T>(c => c.HasEntryPoint = false);

        public static T WithDefinitionFile<T>(this T _this, NPath defFile) where T : MsvcDynamicLinker =>
            _this.With<T>(c => c.DefinitionFile = defFile);

        public static T WithEntryPoint<T>(this T _this, string entryPointName) where T : MsvcDynamicLinker =>
            _this.With<T>(c => c.EntryPointName = entryPointName);
    }

    public class MsvcDynamicLinker : DynamicLinker
    {
        public override bool DynamicLibrary { get; set; }

        public string PdbName { get; internal set; }
        public string EntryPointName { get; internal set; }
        public int StackSize { get; internal set; }
        public bool GenerateWindowsMetadata { get; internal set; }
        public bool HasEntryPoint { get; internal set; } = true;
        public SubSystemType SubSystem { get; internal set; } = SubSystemType.Console;
        public NPath DefinitionFile { get; internal set; }

        private const string ImportLibExtension = "dll.lib";
        private const string DummyDefExtension  = "dll.dummy.def";

        public override bool AllowUnwrittenOutputFiles => true;

        public static bool VerbosePerfOutput => false;

        public MsvcDynamicLinker(ToolChain toolchain) : base(toolchain)
        {
        }

        protected override IEnumerable<string> CommandLineFlagsFor(
            NPath destination,
            CodeGen codegen,
            IEnumerable<NPath> objectFiles,
            IEnumerable<PrecompiledLibrary> nonToolchainSpecificLibraries,
            IEnumerable<PrecompiledLibrary> allLibraries)
        {
            foreach (var linkDir in Toolchain.Sdk.LibraryPaths)
                yield return
                    $"/LIBPATH:{linkDir.InQuotes(SlashMode.Backward)}"; // note: incremental linker falls back to full link when given forward-slashed paths

            foreach (var prebuiltLib in nonToolchainSpecificLibraries)
            {
                if (prebuiltLib.System)
                {
                    var name = prebuiltLib.InQuotes();

                    /*
                     * if you pass e.g. "comsuppw" without an extension to the VS linker, it will silently append ".obj"
                     * and then complain that it can't find "comsuppw.obj", which is obviously wrong for a system
                     * library. So, we help it out.
                     */
                    if (prebuiltLib.Name.ToNPath().HasExtension(""))
                        name = prebuiltLib.InQuotes();

                    yield return name;
                }
                else
                {
                    // Kinda HACK? If a prebuilt library we're linking with is a DLL (e.g. an other NativeProgramSupport result);
                    // link to the counterpart import library that it produced.
                    var lib = prebuiltLib.Path;
                    if (lib.Extension == "dll")
                        lib = lib.ChangeExtension(ImportLibExtension);
                    yield return lib.InQuotes(SlashMode.Native);
                }
            }

            yield return "/NOLOGO";
            yield return "/LARGEADDRESSAWARE";
            yield return "/NXCOMPAT";
            yield return "/DYNAMICBASE";
            yield return "/TLBID:1";

            //This is a warning about missing pdb's for many of the static libraries we link to; it creates
            //a huge spew of useless information when everybody links, and hides more interesting stuff much
            //farther up
            //zilys points out that if something crashes in one of the static libraries, then it'll be harder
            //to examine a crash dump, but we have lived with this spew for years and done nothing about it,
            //so here we're codifying that
            yield return "/IGNORE:4099";

            // /DEBUG:FASTLINK results in a small .pdb that just indexes into the real
            // debugging information that's stored in the .obj files.
            // see https://msdn.microsoft.com/en-us/library/xe4t6fc1.aspx
            // We don't want to do that on build machines because that will make the resulting
            // binaries undebuggable (since we don't keep .obj files)
            if (BuildEnvironment.IsRunningOnBuildMachine())
            {
                yield return "/DEBUG:FULL";
            }
            else
            {
                yield return "/DEBUG:FASTLINK";
            }

            if (Toolchain.Architecture is x86Architecture)
            {
                yield return "/MACHINE:X86";
            }
            else if (Toolchain.Architecture is x64Architecture)
            {
                yield return "/MACHINE:X64";
                yield return "/HIGHENTROPYVA";
            }
            else if (Toolchain.Architecture is ARMv7Architecture)
            {
                yield return "/MACHINE:ARM";
            }
            else if (Toolchain.Architecture is Arm64Architecture)
            {
                yield return "/MACHINE:ARM64";
            }
            else
            {
                throw new Exception("Unknown architecture: " + Toolchain.Architecture.Name);
            }

            if (codegen != CodeGen.Debug)
            {
                yield return "/INCREMENTAL:NO";
                yield return "/OPT:REF";
                yield return "/OPT:ICF";

                if (Toolchain.Architecture is ArmArchitecture)
                    yield return "/OPT:LBR";

                // Master builds have link time code generation enabled
                if (codegen == CodeGen.Master)
                    yield return "/LTCG";
            }
            else
            {
                yield return "/INCREMENTAL";
            }

            if (StackSize != 0)
                yield return $"/STACK:{StackSize}";

            var pdb = GetPdbOutputPath(destination, PdbName ?? destination.FileNameWithoutExtension);
            yield return $"/PDB:\"{pdb}.pdb\"";

            // /PDBSTRIPPED isn't compatible with /DEBUG:FASTLINK
            // So only emit stripped PDB when we're running on a build machine and emit full debug info.
            // There's not much use for stripped PDBs locally anyway
            if (BuildEnvironment.IsRunningOnBuildMachine())
            {
                yield return $"/PDBSTRIPPED:\"{pdb}_s.pdb\"";
            }

            foreach (var objectFile in objectFiles)
                yield return objectFile.InQuotes();

            if (GenerateWindowsMetadata)
            {
                yield return "/WINMD";
                yield return $"/WINMDFILE:{destination.ChangeExtension(".winmd").InQuotes()}";
            }

            if (DynamicLibrary)
            {
                yield return "/DLL";
                if (DefinitionFile != null)
                    yield return $"/DEF:{DefinitionFile.InQuotes()}";
                else
                    yield return $"/DEF:{destination.ChangeExtension(DummyDefExtension).InQuotes()}";

                yield return $"/IMPLIB:{destination.ChangeExtension(ImportLibExtension)}";
            }

            switch (SubSystem)
            {
                case SubSystemType.Console:
                    yield return "/SUBSYSTEM:CONSOLE";
                    break;
                case SubSystemType.Windows:
                    yield return "/SUBSYSTEM:WINDOWS";
                    break;
            }

            if (EntryPointName != null)
                yield return $"/ENTRY:{EntryPointName.InQuotes()}";

            if (!HasEntryPoint)
                yield return "/NOENTRY";

            if (VerbosePerfOutput)
            {
                // https://blogs.msdn.microsoft.com/vcblog/2018/01/04/visual-studio-2017-throughput-improvements-and-advice/
                yield return "/VERBOSE:INCR";
                yield return "/time";
            }

            yield return $"/OUT:{destination.InQuotes()}";
        }

        protected override IEnumerable<NPath> InputFilesFor(NPath destination, CodeGen codegen, IEnumerable<NPath> objectFiles,
            IEnumerable<PrecompiledLibrary> nonToolchainSpecificLibraries, IEnumerable<PrecompiledLibrary> allLibraries)
        {
            foreach (var file in base.InputFilesFor(destination, codegen, objectFiles, nonToolchainSpecificLibraries, allLibraries))
                yield return file;

            if (DynamicLibrary)
            {
                var defFile = DefinitionFile != null ? DefinitionFile : destination.ChangeExtension(DummyDefExtension);
                if (DefinitionFile == null)
                    Backend.Current.AddWriteTextAction(defFile, "EXPORTS");

                yield return defFile;
            }
        }

        protected override IEnumerable<NPath> OutputFilesFor(
            NPath destination,
            CodeGen codegen,
            IEnumerable<NPath> objectFiles,
            IEnumerable<PrecompiledLibrary> nonToolchainSpecificLibraries,
            IEnumerable<PrecompiledLibrary> allLibraries)
        {
            foreach (var path in base.OutputFilesFor(
                destination,
                codegen,
                objectFiles,
                nonToolchainSpecificLibraries,
                allLibraries))
                yield return path;


            var pdb = GetPdbOutputPath(destination, PdbName ?? destination.FileNameWithoutExtension);
            var pdbFull = new NPath($"{pdb}.pdb");

            yield return pdbFull;

            if (BuildEnvironment.IsRunningOnBuildMachine())
            {
                var pdbStripped = new NPath($"{pdb}_s.pdb");
                yield return pdbStripped;
            }

            if (GenerateWindowsMetadata)
                yield return destination.ChangeExtension(".winmd");

            if (DynamicLibrary)
                yield return destination.ChangeExtension(ImportLibExtension);
        }

        string GetPdbOutputPath(NPath destination, string _pdbname)
        {
            string arch;
            var folder = destination.Parent;

            if (Toolchain.Architecture is x86Architecture)
            {
                arch = "x86";
            }
            else if (Toolchain.Architecture is x64Architecture)
            {
                arch = "x64";
            }
            else if (Toolchain.Architecture is ARMv7Architecture)
            {
                arch = "arm";
            }
            else if (Toolchain.Architecture is Arm64Architecture)
            {
                arch = "arm64";
            }
            else
            {
                throw new Exception("Unknown architecture: " + Toolchain.Architecture.Name);
            }

            return $"{folder}/{_pdbname}_{arch}";
        }
    }
}
