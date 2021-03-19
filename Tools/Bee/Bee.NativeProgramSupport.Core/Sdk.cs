using System;
using System.Collections.Generic;
using System.Linq;
using Bee.Core;
using Unity.BuildSystem.NativeProgramSupport;
using NiceIO;

namespace Bee.NativeProgramSupport
{
    public static class SdkExtensions
    {
        public static T Latest<T>(this IEnumerable<T> _this) where T : Sdk
            => _this.OrderByDescending(s => s).FirstOrDefault();

        public static T LatestMatching<T>(this IEnumerable<T> _this, Version min, Version max) where T : Sdk
            => _this.OrderByDescending(s => s).FirstOrDefault(s => s.Matches(min, max));

        public static T LatestMatching<T>(this IEnumerable<T> _this, Version version) where T : Sdk
            => _this.OrderByDescending(s => s).FirstOrDefault(s => s.Matches(version));

        public static T LatestMatching<T>(this IEnumerable<T> _this, int? major = null, int? minor = null, int? build = null, int? revision = null) where T : Sdk
            => _this.OrderByDescending(s => s).FirstOrDefault(s => s.Matches(major, minor, build, revision));
    }

    public abstract class Sdk : IComparable<Sdk>, IEquatable<Sdk>, IBackendRegistrable
    {
        public abstract Platform Platform { get; }
        public abstract NPath Path { get; }
        public abstract Version Version { get; }

        // These are used create a dependency on the tool binaries.
        // We use this to trigger Stevedore downloads and trigger
        // recompilation on tool updates. They may be null.
        public abstract NPath CppCompilerPath { get; }
        public abstract NPath StaticLinkerPath { get; }
        public abstract NPath DynamicLinkerPath { get; }
        public virtual IEnumerable<NPath> TundraInputs => new[] { CppCompilerPath, StaticLinkerPath, DynamicLinkerPath };

        public virtual string CppCompilerCommand => CppCompilerPath?.InQuotes(SlashMode.Native);
        public virtual string StaticLinkerCommand => StaticLinkerPath?.InQuotes(SlashMode.Native);
        public virtual string DynamicLinkerCommand => DynamicLinkerPath?.InQuotes(SlashMode.Native);

        public virtual NPath SysRoot { get; }
        public virtual IEnumerable<string> Defines { get; } = Array.Empty<string>();
        public virtual IEnumerable<NPath> IncludePaths { get; } = Array.Empty<NPath>();
        public virtual IEnumerable<NPath> LibraryPaths { get; } = Array.Empty<NPath>();

        public bool IsDownloadable { get; protected set; }
        public abstract bool SupportedOnHostPlatform { get; }

        readonly IBackendRegistrable[] m_BackendRegistrables;

        protected Sdk(IEnumerable<IBackendRegistrable> backendRegistrables = null)
        {
            m_BackendRegistrables = backendRegistrables?.ToArray() ?? Array.Empty<IBackendRegistrable>();
        }

        public bool Matches(Version min, Version max) => min <= Version && Version <= max;
        public bool Matches(Version version) => Version == version;
        public bool Matches(int? major = null, int? minor = null, int? build = null, int? revision = null)
            => (major == null || major == Version.Major) && (minor == null || minor == Version.Minor) &&
            (build == null || build == Version.Build) && (revision == null || revision == Version.Revision);

        public int CompareTo(Sdk other)
        {
            if (ReferenceEquals(this, other)) return 0;
            if (ReferenceEquals(null, other)) return 1;
            var versionComparison = Comparer<Version>.Default.Compare(Version, other.Version);
            return versionComparison != 0 ? versionComparison : Comparer<NPath>.Default.Compare(Path, other.Path);
        }

        public bool Equals(Sdk other) => CompareTo(other) == 0;
        public override bool Equals(object obj)
        {
            if (ReferenceEquals(null, obj)) return false;
            if (ReferenceEquals(this, obj)) return true;
            return obj.GetType() == this.GetType() && Equals((Sdk)obj);
        }

        public override int GetHashCode() => (Version.GetHashCode() * 397) ^ (Path != null ? Path.GetHashCode() : 0);

        public virtual void Register(IBackend backend)
        {
            foreach (var br in m_BackendRegistrables)
                br?.Register(backend);
        }
    }
}
