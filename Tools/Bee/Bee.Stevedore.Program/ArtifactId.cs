using System.Linq;
using System.Text.RegularExpressions;

namespace Bee.Stevedore.Program
{
    public class InvalidArtifactStringException : HumaneException
    {
        public InvalidArtifactStringException(string value, string requiredType)
            : base($"'{value}' is not a valid {requiredType}")
        {
        }
    }

    /// <summary>
    /// A valid Stevedore artifact name. Artifact names are guaranteed to be
    /// valid filenames on all platforms (though subject to path length limits
    /// on Windows), as well as URL safe.
    /// </summary>
    public struct ArtifactName
    {
        internal const int MaxLength = 28;

        public static readonly Regex ReservedFilenames = new Regex(@"^[-.]|\.$|^(CON|PRN|AUX|NUL|COM[1-9]|LPT[1-9])(\..*)?", RegexOptions.IgnoreCase);
        public static readonly Regex Regex = new Regex(@"^[-_a-zA-Z0-9]{1,28}$");

        public string Value { get; }

        public ArtifactName(string value)
        {
            if (!Regex.Match(value).Success || ReservedFilenames.Match(value).Success)
                throw new InvalidArtifactStringException(value, "artifact name");

            Value = value;
        }

        public override string ToString() => Value;
        public static implicit operator string(ArtifactName name) => name.Value;
    }

    /// <summary>
    /// A valid and parsed Stevedore artifact version. Artifact versions are
    /// guaranteed to be valid filenames on all platforms (though subject to
    /// path length limits on Windows). They are not guaranteed to be URL safe.
    /// </summary>
    public struct ArtifactVersion
    {
        // The longest string allowed by below regexes and the overall artifact
        // version structure, except reserve 7 chars (not 4) for the extension,
        // as there are loose plans for .tar.gz support.
        internal const int MaxLength = 40 + 1 + 64 + 7;

        static readonly string[] ValidFileExtensions = { ".zip", ".7z" };
        static readonly Regex ReservedFilenames = ArtifactName.ReservedFilenames;
        static readonly Regex VersionStringRegex = new Regex(@"^[-(),.=@[\]_0-9A-Za-z]{1,40}$");
        static readonly Regex HashRegex = new Regex(@"^[0-9a-f]{64}$");

        public string Value { get; }
        public string VersionString { get; }
        public string Hash { get; }
        public string Extension { get; }

        public ArtifactVersion(string value)
        {
            int i = value.LastIndexOf('_');
            int j = value.IndexOf('.', i + 1);
            if (i == -1 || j == -1)
                throw new InvalidArtifactStringException(value, "artifact version");

            VersionString = value.Substring(0, i);
            if (!VersionStringRegex.IsMatch(VersionString) || ReservedFilenames.IsMatch(VersionString))
                throw new InvalidArtifactStringException(VersionString, "artifact version string");

            Hash = value.Substring(i + 1, j - i - 1);
            if (!HashRegex.IsMatch(Hash))
                throw new InvalidArtifactStringException(Hash, "artifact hash");

            Extension = value.Substring(j);
            if (!ValidFileExtensions.Contains(Extension))
                throw new InvalidArtifactStringException(Extension, "artifact extension");

            Value = value;
        }

        public override string ToString() => Value;
        public static implicit operator string(ArtifactVersion name) => name.Value;

        /// <summary>
        /// Validate just the VersionString part of an ArtifactVersion.
        /// </summary>
        public static void ValidateVersionString(string versionString)
        {
            try
            {
                new ArtifactVersion(versionString + "_0000000000000000000000000000000000000000000000000000000000000000.7z");
            }
            catch (InvalidArtifactStringException)
            {
                throw new InvalidArtifactStringException(versionString, "artifact version string");
            }
        }
    }

    /// <summary>
    /// A valid and parsed Stevedore artifact ID.
    /// </summary>
    public struct ArtifactId
    {
        internal const int MaxLength = ArtifactName.MaxLength + 1 + ArtifactVersion.MaxLength;

        public ArtifactName Name;
        public ArtifactVersion Version;

        public ArtifactId(string value)
        {
            string[] split = value.Split(new char[] { '/' }, 3);
            if (split.Length != 2)
                throw new InvalidArtifactStringException(value, "artifact ID");

            Name = new ArtifactName(split[0]);
            Version = new ArtifactVersion(split[1]);
        }

        public override string ToString() => Name + '/' + Version;
    }
}
