using NiceIO;
using Unity.BuildTools;

namespace Unity.BuildSystem.NativeProgramSupport
{
    public sealed class CLikeCompilationUnitInstruction : ObjectFileProductionInstructions
    {
        public CLikeCompilerSettings CompilerSettings { get; private set; }
        public bool CanLump { get; }
        public bool HasIncludeDependenciesSetupManually { get; }

        public CLikeCompilationUnitInstruction(CLikeCompilerSettings compilerSettings, NPath inputFile,
                                               bool hasIncludeDependenciesSetupManually = false, bool canLump = false) : base(inputFile, compilerSettings.Compiler)
        {
            CompilerSettings = compilerSettings;
            HasIncludeDependenciesSetupManually = hasIncludeDependenciesSetupManually;
            CanLump = canLump;
        }

        public override void TellJamHowToProduce(NPath objectFile)
        {
            CompilerSettings.Compiler.SetupInvocation(objectFile, this);
        }

        public ulong HashSettingsRelevantForCompilation()
        {
            //is this hash safe enough to protect against hashing collisions?
            //KnuthHash is a 64bit hash.  since we are using this hash for deciding
            //wether or not multiple files can be comined into a single lump, we only
            //care about hash collisions against other files in the same directory,
            //as the lumping algorithm first groups by parent directory.
            //Even in directories that have 200 files, using a 64bit hash brings
            //the likelyhood of a collision to one in 10^15, which is about 100 times
            //less likely than a meteor landing on your house, about 100 million times
            //less likely than dying in a shark attack, and 10 billion times less likely
            //than being struck by lighting.  (all these probabilities are per directory)
            //
            //tldr: i think we're good.
            //
            //see: http://preshing.com/20110504/hash-collision-probabilities/

            var hash = KnuthHash.Create();
            hash.Add(InputFile.Parent);
            hash.Add(InputFile.Extension);
            hash.Add(CompilerSettings.HashSettingsRelevantForCompilation());
            return hash.Value;
        }

        public CLikeCompilationUnitInstruction WithCompilerSettings(CLikeCompilerSettings s)
        {
            return new CLikeCompilationUnitInstruction(s, base.InputFile, HasIncludeDependenciesSetupManually, CanLump);
        }
    }
}
