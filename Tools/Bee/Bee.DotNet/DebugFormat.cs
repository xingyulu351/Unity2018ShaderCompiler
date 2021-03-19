using Bee.Core;

namespace Bee.DotNet
{
    public abstract class DebugFormat : ClassInsteadOfEnumPatternBase
    {
        public abstract string Extension { get; }

        public static Mdb Mdb { get; } = new Mdb();
        public static PortablePdb PortablePdb { get; } = new PortablePdb();
        public static Pdb Pdb { get; } = new Pdb();
    }

    public class Mdb : DebugFormat
    {
        public override string Extension { get; } = "mdb";
    }

    public class PortablePdb : DebugFormat
    {
        public override string Extension { get; } = "pdb";
    }

    public class Pdb : DebugFormat
    {
        public override string Extension { get; } = "pdb";
    }
}
