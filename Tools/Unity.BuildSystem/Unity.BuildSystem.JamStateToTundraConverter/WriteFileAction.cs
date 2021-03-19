using NiceIO;

namespace Unity.BuildSystem.JamStateToTundraConverter
{
    class WriteFileAction : Action
    {
        public virtual string TypeString { get; } = "WriteFile";
        public string Contents { get; }
        public NPath m_TargetFile;

        public WriteFileAction(string contents, NPath targetFile)
        {
            m_TargetFile = targetFile;
            Contents = contents;
        }
    }
}
