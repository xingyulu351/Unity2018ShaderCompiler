using System;
using NiceIO;

namespace Unity.BuildSystem.NativeProgramSupport
{
    public abstract class ObjectFileProductionInstructions
    {
        public ObjectFileProducer ObjectFileProducer { get; }
        public NPath InputFile { get; }

        protected ObjectFileProductionInstructions(NPath inputFile, ObjectFileProducer objectFileProducer)
        {
            InputFile = inputFile;
            ObjectFileProducer = objectFileProducer;
        }

        public abstract void TellJamHowToProduce(NPath objectFile);
    }
}
