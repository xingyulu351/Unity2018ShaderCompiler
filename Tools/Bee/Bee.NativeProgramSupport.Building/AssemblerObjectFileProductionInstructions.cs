using NiceIO;

namespace Unity.BuildSystem.NativeProgramSupport
{
    public class AssemblerObjectFileProductionInstructions : ObjectFileProductionInstructions
    {
        public AssemblerBase AssemblerBase { get; }

        public AssemblerObjectFileProductionInstructions(NPath inputFile, AssemblerBase assemblerBase) : base(inputFile, assemblerBase)
        {
            AssemblerBase = assemblerBase;
        }

        public override void TellJamHowToProduce(NPath objectFile)
        {
            AssemblerBase.SetupInvocation(objectFile, this);
        }
    }
}
