namespace Unity.BuildSystem.NativeProgramSupport
{
    public struct WarningAndPolicy
    {
        public readonly string Warning;
        public readonly WarningPolicy Policy;

        public WarningAndPolicy(string warning, WarningPolicy policy)
        {
            Policy = policy;
            Warning = warning;
        }
    }
}
