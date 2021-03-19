using Unity.BuildTools;

namespace Bee.DotNet
{
    public abstract class Framework
    {
        public abstract string Name { get; }
        public string MsBuildName => Name.StripStart("v");

        public static Framework20 Framework20 = Framework20.Singleton;
        public static Framework35 Framework35 = Framework35.Singleton;
        public static Framework40 Framework40 = Framework40.Singleton;
        public static Framework46 Framework46 = Framework46.Singleton;
        public static Framework461 Framework461 = Framework461.Singleton;
        public static Framework462 Framework462 = Framework462.Singleton;
        public static Framework47 Framework47 = Framework47.Singleton;
        public static Framework471 Framework471 = Framework471.Singleton;

        public static Netstandard13 NetStandard13 = Netstandard13.Singleton;
        public static Netstandard20 NetStandard20 = Netstandard20.Singleton;
        public static UAP UAP = UAP.Singleton;

        public bool IsDotNetFramework => this is DotNetFramework;
    }

    public abstract class DotNetFramework : Framework
    {
    }

    public class Framework20 : DotNetFramework
    {
        Framework20()
        {
        }

        internal static Framework20 Singleton = new Framework20();
        public override string Name => "v2.0";
    }

    public class Framework35 : DotNetFramework
    {
        Framework35()
        {
        }

        internal static Framework35 Singleton = new Framework35();
        public override string Name => "v3.5";
    }

    public class Framework40 : DotNetFramework
    {
        Framework40()
        {
        }

        internal static Framework40 Singleton = new Framework40();

        public override string Name => "v4.0";
    }
    class Framework45 : DotNetFramework
    {
        Framework45()
        {
        }

        internal static Framework45 Singleton = new Framework45();

        public override string Name => "v4.5";
    }

    public class Framework46 : DotNetFramework
    {
        Framework46()
        {
        }

        internal static Framework46 Singleton = new Framework46();

        public override string Name => "v4.6";
    }

    public class Framework461 : DotNetFramework
    {
        Framework461()
        {
        }

        internal static Framework461 Singleton = new Framework461();

        public override string Name => "v4.6.1";
    }

    public class Framework462 : DotNetFramework
    {
        Framework462()
        {
        }

        internal static Framework462 Singleton = new Framework462();

        public override string Name => "v4.6.2";
    }

    public class Framework47 : DotNetFramework
    {
        Framework47()
        {
        }

        internal static Framework47 Singleton = new Framework47();

        public override string Name => "v4.7";
    }

    public class Framework471 : DotNetFramework
    {
        Framework471()
        {
        }

        internal static Framework471 Singleton = new Framework471();

        public override string Name => "v4.7.1";
    }

    public class UAP : Framework
    {
        UAP()
        {
        }

        internal static UAP Singleton = new UAP();

        public override string Name => "UAP";
    }

    public class Netstandard13 : Framework
    {
        Netstandard13()
        {
        }

        internal static Netstandard13 Singleton = new Netstandard13();

        public override string Name => "netstandard1.3";
    }

    public class Netstandard20 : Framework
    {
        Netstandard20()
        {
        }

        internal static Netstandard20 Singleton = new Netstandard20();

        public override string Name => "netstandard2";
    }
}
