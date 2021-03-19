namespace JamSharp.Runtime
{
    public interface IJamStateBuilder
    {
        void Depends(params JamList[] values);
        void Needs(params JamList[] values);
        void NotFile(JamList values);
        void Includes(params JamList[] values);

        void InvokeAction(
            string actionName,
            JamList targets,
            JamList secondArguments = null,
            bool allowUnwrittenOutputFiles = false);
        void RegisterAction(string actionName, string actionBody, ActionsFlags flags);
        void InvokeJamRule(JamList rules, JamList[] values);
    }
}
