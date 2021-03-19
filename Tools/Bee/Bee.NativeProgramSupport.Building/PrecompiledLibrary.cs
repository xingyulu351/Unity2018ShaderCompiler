using System;
using NiceIO;

namespace Unity.BuildSystem.NativeProgramSupport
{
    interface ILibrary
    {
    }

    public abstract class PrecompiledLibrary : IEquatable<PrecompiledLibrary>, IComparable<PrecompiledLibrary>, ILibrary
    {
        protected NPath  _Path { get; set; }
        protected string _Name { private get; set; }

        public NPath  Path => _Path ?? throw new InvalidOperationException($"Path is not available for this type of library {GetType()}");
        public string Name => _Name ?? _Path.FileNameWithoutExtension;

        public virtual bool ToolchainSpecific => true;
        public virtual bool Static  => false;
        public virtual bool Dynamic => false;
        public virtual bool System  => false;

        public override string ToString() => ToString(SlashMode.Forward);
        public virtual  string ToString(SlashMode mode) => System ? Name : Path.ToString(mode);
        public virtual  string InQuotes(SlashMode mode = SlashMode.Forward) => System ? $"\"{Name}\"" : Path.InQuotes(mode);

        public virtual WhenMultiplePolicy WhenMultiplePolicy => WhenMultiplePolicy.KeepLast;

        public override int GetHashCode() => System? Name.GetHashCode() : Path.GetHashCode();
        public override bool Equals(Object obj) => obj is PrecompiledLibrary lib && Equals(lib);
        public bool Equals(PrecompiledLibrary other) => CompareTo(other) == 0;

        public int CompareTo(PrecompiledLibrary other)
        {
            int result;
            if (0 != (result = ToolchainSpecific.CompareTo(other.ToolchainSpecific))) return result;
            if (0 != (result = Static.CompareTo(other.Static))) return result;
            if (0 != (result = Dynamic.CompareTo(other.Dynamic))) return result;
            if (0 != (result = System.CompareTo(other.System))) return result;
            return System ? Name.CompareTo(other.Name) : Path.CompareTo(other.Path);
        }

        public virtual string[] PublicDefinesFor(NativeProgramConfiguration config) => Array.Empty<string>();
        public virtual NPath[] PublicIncludeDirectoriesFor(NativeProgramConfiguration config) => Array.Empty<NPath>();
    }

    public enum WhenMultiplePolicy
    {
        KeepLast,
        KeepFirst
    }

    public sealed class StaticLibrary : PrecompiledLibrary
    {
        public override bool ToolchainSpecific => false;
        public override bool Static => true;
        public StaticLibrary(NPath path) => _Path = path;
        public static implicit operator StaticLibrary(string path) => new StaticLibrary(path);
        public static implicit operator StaticLibrary(NPath path) => new StaticLibrary(path);
    }

    public sealed class DynamicLibrary : PrecompiledLibrary
    {
        public override bool ToolchainSpecific => false;
        public override bool Dynamic => true;
        public DynamicLibrary(NPath path) { _Path = path; }
        public static implicit operator DynamicLibrary(string path) => new DynamicLibrary(path);
        public static implicit operator DynamicLibrary(NPath path) => new DynamicLibrary(path);
    }

    public sealed class SystemLibrary : PrecompiledLibrary
    {
        public override bool ToolchainSpecific => false;
        public override bool System => true;
        public SystemLibrary(string name) => _Name = name;
        public static implicit operator SystemLibrary(string name) => new SystemLibrary(name);
    }

    internal class BagOfObjectFilesLibrary : ILibrary
    {
        public BagOfObjectFilesLibrary(NPath[] allObjectFiles)
        {
            AllObjectFiles = allObjectFiles;
        }

        public NPath[] AllObjectFiles { get; }
    }
}
