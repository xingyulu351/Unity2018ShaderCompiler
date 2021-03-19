using System;
using System.IO;
using System.IO.Compression;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Security.Cryptography;
using Bee.Tools;
using JamSharp.Runtime;
using NiceIO;
using Unity.BuildTools;

[assembly: InternalsVisibleTo("Bee.Stevedore.Program.Tests")]
[assembly: InternalsVisibleTo("Bee.Stevedore.Tests")]

namespace Bee.Stevedore.Program
{
    public class StevedoreProgram
    {
        public static string StevedoreVersionFileName => ".StevedoreVersion";
        public static string SevenZipPathEnvironmentVariable => "BEE_INTERNAL_STEVEDORE_7ZA";
        public static string LocalArtifactPathEnvironmentVariable => "BEE_INTERNAL_STEVEDORE_LOCALPATH";

        /// <summary>
        /// Entrypoint for invoking Stevedore when not embedded into Bee.
        /// </summary>
        public static void Main(string[] args)
        {
            if (args.FirstOrDefault() == "steve")
                SteveMain(args.Skip(1).ToArray());
            else
                throw new Exception("First argument must be 'steve' (for 'bee' compatibility)");
        }

        public static void SteveMain(string[] args)
        {
            try
            {
                var arg0 = args.FirstOrDefault();

                var config = new StevedoreConfig();
                // Only show config warnings for "steve config" command.
                // In particular, we don't show warnings for internal-unpack,
                // as we already show warnings when Bee builds the depgraph.
                config.ReadEnvironmentDefaults(reportParseError: arg0 == "config" ? (Action<string>)EmitWarning : null);

                switch (arg0)
                {
                    case "cache":
                        throw new NotImplementedException();
                    case "config":
                        ShowConfig(config, Console.Out);
                        return;

                    case "internal-unpack":
                        GetArtifact(
                            config,
                            repoName: args[1],
                            artifactId: new ArtifactId(args[2]),
                            unpackPath: new NPath(args[3]).MakeAbsolute()
                        );
                        return;

                    case "new":
                        NewArtifact(config, Console.Out, new NPath(args[1]), new ArtifactName(args[2]), args[3]);
                        return;

                    case null:
                    case "-h":
                    case "--help":
                        Console.Write($@"Stevedore artifact manager

list of commands:

 config

  Show configuration information.

 internal-unpack REPO-NAME ARTIFACT-ID TARGET-PATH

  Fetches the specified artifact from the specified repository (if not
  already cached), and extracts to TARGET-PATH. By default unpacked using
  .NET's internal unzipper. Set the {SevenZipPathEnvironmentVariable} environment
  variable to a 7za executable path to use 7z instead. To skip downloading
  and use a local file, specify it with {LocalArtifactPathEnvironmentVariable}.

 new FILENAME ARTIFACT-NAME VERSION-STRING

  Hashes contents of FILENAME and prints full artifact ID based on the
  provided ARTIFACT-NAME and VERSION-STRING.
");
                        return;

                    default:
                        throw new HumaneException("unrecognized argument: " + arg0);
                }
            }
            catch (HumaneException e)
            {
                Console.Write($"stevedore: {e.Message}\n");
                Environment.Exit(1);
            }
        }

        internal static void GetArtifact(StevedoreConfig config, string repoName, ArtifactId artifactId, NPath unpackPath)
        {
            var unpackPathString = unpackPath.MakeAbsolute().ToString(SlashMode.Native);

            // Windows: Calculate the length of the longest filename we can
            // access inside the unpack path (without jumping through hoops).
            int unpackPathBudget = 259 - 1 - unpackPathString.Length; // -1 to count the '/' following unpackPath.

            DownloadCache cache;
            try
            {
                cache = new DownloadCache(config);

                if (HostPlatform.IsWindows)
                {
                    // Ensure we at least have path budget for writing the .StevedoreVersion file.
                    // (We don't know how long the paths inside the zip will be, so cannot check those.)
                    if (unpackPathBudget < StevedoreVersionFileName.Length)
                        throw new HumaneException($"The unpack path exceeds {259 - 1 - StevedoreVersionFileName.Length} characters, which is too long for Windows: {unpackPathString}");
                }
            }
            catch (HumaneException e)
            {
                throw new HumaneException($"Cannot download artifacts. {e.Message}");
            }

            var versionFilePath = unpackPath.Combine(StevedoreVersionFileName);

            try
            {
                if (versionFilePath.ReadAllText().TrimEnd() == artifactId.ToString()) return; // Done!
            }
            catch (IOException)
            {
                // No version file. OK.
            }

            NPath artifactPath;
            var localArtifactPath = Environment.GetEnvironmentVariable(LocalArtifactPathEnvironmentVariable);
            if (!string.IsNullOrEmpty(localArtifactPath))
            {
                // A local artifact path was provided (StevedoreArtifact.FromLocalPath); use that instead of the cache.
                artifactPath = localArtifactPath;
                using (var hasher = SHA256.Create())
                using (var stream = new FileStream(localArtifactPath, FileMode.Open, FileAccess.Read, FileShare.Read))
                {
                    var actualHash = hasher.ComputeHash(stream).ToHexString();
                    if (actualHash != artifactId.Version.Hash)
                        throw new HumaneException($"Cannot use local file for artifact {artifactId.Name} (integrity check failed): {localArtifactPath}\nExpected hash: {artifactId.Version.Hash}\nActual hash: {actualHash}\nFix the artifact file or the StevedoreArtifact.FromLocalFile invocation.");
                }
            }
            else
            {
                artifactPath = cache.EnsureArtifactIsCached(repoName, artifactId);
            }

            // 7za can create long paths, requiring special handling to delete on Windows
            if (unpackPath.Exists())
                RecursivelyDeleteDirectoryWithLongPaths(unpackPath);

            var sevenZipPath = Environment.GetEnvironmentVariable(SevenZipPathEnvironmentVariable);
            if (!string.IsNullOrEmpty(sevenZipPath))
            {
                if (!HostPlatform.IsWindows)
                    Shell.Execute($"/bin/chmod +x \"{sevenZipPath}\"");
                Shell.Execute($"{sevenZipPath} x {artifactPath.InQuotes(SlashMode.Native)} -o{unpackPathString}").ThrowOnFailure();
            }
            else
            {
                try
                {
                    ZipFile.ExtractToDirectory(artifactPath.ToString(SlashMode.Native), unpackPathString);
                }
                catch (PathTooLongException)
                {
                    throw new HumaneException($"Artifact '{artifactId.Name}' contains paths longer than {unpackPathBudget} characters, which is too long (for Windows) given the unpack path: {unpackPathString}");
                }
            }

            versionFilePath.WriteAllText(artifactId.ToString());
        }

        internal static void ShowConfig(StevedoreConfig config, TextWriter output)
        {
            output.Write("Config files:\n");
            foreach (var path in config.ConfigFiles)
                output.Write($"  {path.ToString(SlashMode.Native)}{(path.FileExists() ? "" : " (not present)")}\n");
            output.Write("\nConfiguration:\n");

            foreach (var kv in config.GetConfigValues())
                output.Write($"  {kv.Key} = {kv.Value}\n");
        }

        internal static void NewArtifact(StevedoreConfig config, TextWriter output, NPath source, ArtifactName name, string versionString)
        {
            ArtifactVersion.ValidateVersionString(versionString);

            var ext = Path.GetExtension(source.ToString(SlashMode.Native));
            if (ext != ".7z" && ext != ".zip")
                throw new HumaneException($"'{ext}' is not a supported artifact extension; use .7z or .zip");

            string hash;
            using (var hasher = SHA256.Create())
            using (var stream = File.OpenRead(source.ToString(SlashMode.Native)))
                hash = hasher.ComputeHash(stream).ToHexString();

            output.Write($"{name}/{versionString}_{hash}{ext}\n");
        }

        /// <summary>
        /// Jump through ridiculous hoops to delete paths longer than MAX_PATH
        /// on Windows. The trick used here is to move subdirectories up and
        /// rename them to shorter names, to get below the limit. That still
        /// can't delete ALL long paths, only those where each path SEGMENT on
        /// its own, plus the rootPath, is short enough to fit in 259 characters.
        ///
        /// (PInvoking the Win32 APIs would allow using the "\\?\" namespace
        /// prefix to handle long paths. The Win32 FindFileFirst API still
        /// limits individual path segments to 259 characters; but Windows file
        /// systems commonly have a "lpMaximumComponentLength" limit of 255
        /// anyway. For now, we'll stick to this pure .NET solution.)
        ///
        /// If rootPath is a symlink, it will be dereferenced; nested symlinks
        /// will be deleted but not dereferenced. (Unix only; how the function
        /// handles symlinks and directory junctions under Windows is undefined.)
        /// </summary>
        public static void RecursivelyDeleteDirectoryWithLongPaths(NPath rootPath)
        {
            if (!HostPlatform.IsWindows)
            {
                Directory.Delete(rootPath.ToString(SlashMode.Native), true);
                return;
            }

            // Create temp dir inside rootPath to greatly increase likelyhood
            // that we'll stay on the same disk/partition.
            NPath tempDir = null;
            for (int i = 0; i < int.MaxValue; ++i)
            {
                tempDir = rootPath.Combine(i.ToString());
                if (!tempDir.DirectoryExists()) break; // files will be deleted, so only dirs matter
            }
            if (tempDir == null) throw new InvalidOperationException(); // I don't even...
            tempDir.CreateDirectory();
            var tempDirStr = tempDir.ToString(SlashMode.Backward);

            int n = 0;
            var rmdir = new Action<string>(path =>
            {
                foreach (var entry in Directory.EnumerateFiles(path))
                    File.Delete(entry);

                foreach (var entry in Directory.EnumerateDirectories(path))
                {
                    if (entry == tempDirStr) continue;
                    Directory.Move(entry, tempDir.Combine(n.ToString()).ToString(SlashMode.Backward));
                    ++n;
                }
            });

            rmdir(rootPath.ToString(SlashMode.Native));

            for (int i = 0; i < n; ++i) // note: n may increase as we loop
                rmdir(tempDir.Combine(i.ToString()).ToString(SlashMode.Backward));

            Directory.Delete(rootPath.ToString(SlashMode.Native), recursive: true);
        }

        public static void EmitWarning(string warning)
        {
            Console.Error.Write($"stevedore: {warning}\n");
        }
    }
}
