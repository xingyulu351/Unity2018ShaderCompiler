using System.Collections.Generic;

namespace Unity.BuildSystem.JamStateToTundraConverter
{
    class ShellAction : Action
    {
        readonly ShellCommand[] _shellCommands;
        public virtual string TypeString { get; }
        public bool AllowUnwrittenOutputFiles { get; }

        public IEnumerable<ShellCommand> ShellCommands => _shellCommands;

        public ShellAction(ShellCommand[] shellCommands, string typeString, bool allowUnwritten = false) : base()
        {
            TypeString = typeString;
            _shellCommands = shellCommands;
            AllowUnwrittenOutputFiles = allowUnwritten;
        }
    }
}
