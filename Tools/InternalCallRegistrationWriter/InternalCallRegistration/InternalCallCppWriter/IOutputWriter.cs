namespace Unity.InternalCallRegistrationWriter.CppWriter
{
    public interface IOutputWriter
    {
        void AppendLine();
        void AppendLine(string singleMethodRegistrationInvocationFor);
        void AppendComment(string comment);
        void Append(string s);
        string Output();
        void BeginBlock();
        void EndBlock();
        void Indent();
        void Dedent();
    }
}
