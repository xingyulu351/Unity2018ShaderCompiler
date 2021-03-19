using System;
using System.Collections.Generic;
using System.IO;
using System.Security.Cryptography;
using Bee.Core;
using Bee.DotNet;
using Bee.Stevedore.Program;
using Bee.Tools;
using NiceIO;
using Unity.BuildTools;

namespace Bee.Stevedore
{
    public class StevedoreArtifact : IBackendRegistrable
    {
        // Short-hands for commonly used repo names.
        public static StevedoreArtifact Public(string artifactId) => new StevedoreArtifact("public", new ArtifactId(artifactId));
        public static StevedoreArtifact Testing(string artifactId) => new StevedoreArtifact("testing", new ArtifactId(artifactId));
        public static StevedoreArtifact UnityInternal(string artifactId) => new StevedoreArtifact("unity-internal", new ArtifactId(artifactId));

        /// <summary>
        /// Creates a StevedoreArtifact that's already present locally in the
        /// repository, and thus should not be downloaded. Intended SOLELY as
        /// a transitional measure, allowing the Stevedore APIs to be used for
        /// existing largefiles until they're moved to a proper Stevedore server.
        /// </summary>
        public static StevedoreArtifact FromLocalFile(string artifactName, NPath source)
        {
            // The current working directory (CWD) may change between now and the
            // time we actually register with the backend (in Register, below).
            // But the source path (if relative) must be relative to the CWD that
            // we have *now* (as we need to hash the file *now*). We make it
            // absolute so it'll *also* be valid in Register, even if the CWD
            // has changed by then.
            source = source.MakeAbsolute();
            using (var hasher = SHA256.Create())
            using (var stream = new FileStream(source.ToString(SlashMode.Native), FileMode.Open, FileAccess.Read, FileShare.Read))
            {
                var artifactHash = hasher.ComputeHash(stream).ToHexString();
                return new StevedoreArtifact(source, new ArtifactId
                {
                    Name = new ArtifactName(artifactName),
                    Version = new ArtifactVersion("local_" + artifactHash + "." + source.Extension),
                });
            }
        }

        static RunnableProgram SteveExe => new DotNetRunnableProgram(new DotNetAssembly(typeof(StevedoreProgram).Assembly.Location, Framework.Framework46));

        public string RepoName { get; }
        public ArtifactId Id { get; }
        public NPath Path { get; }
        public NPath ArtifactVersionFile => Path.Combine(StevedoreProgram.StevedoreVersionFileName);
        public bool ExtractUsingSevenZip { get; set; } = true;

        readonly NPath m_LocalPath;

        public StevedoreArtifact(string repoName, ArtifactId id)
        {
            RepoName = repoName;
            Id = id;
            Path = new NPath("artifacts/Stevedore").Combine(Id.Name);  // TODO: somewhere else?
        }

        StevedoreArtifact(NPath localPath, ArtifactId id)
            : this("local", id)
        {
            m_LocalPath = localPath;
        }

        void IBackendRegistrable.Register(IBackend backend)
        {
            var backendSettings = StevedoreBackendSettings.Get(backend);
            var environmentVariables = new Dictionary<string, string>();
            if (m_LocalPath == null)
            {
                try
                {
                    // Validate that RepoName specifies an actual, configured repository.
                    backendSettings.Config.RepoConfigFor(RepoName);
                }
                catch (UnavailableRepositoryException e)
                {
                    throw e.CannotDownload(Id.Name);
                }
            }
            else
            {
                environmentVariables[StevedoreProgram.LocalArtifactPathEnvironmentVariable] = m_LocalPath.ToString(SlashMode.Native);

                // If the local file changes, StevedoreArtifact.FromLocalPath must be
                // run again (to prevent Tundra from using the frozen, incorrect hash).
                backend.RegisterFileInfluencingGraph(m_LocalPath.RelativeTo("."));
                environmentVariables[StevedoreProgram.LocalArtifactPathEnvironmentVariable] = m_LocalPath.ToString(SlashMode.Native);
            }

            NPath sevenZipPath = null;
            if (ExtractUsingSevenZip)
            {
                sevenZipPath = backendSettings.SevenZipPath;
                if (sevenZipPath == null)
                {
                    sevenZipPath = StevedoreBackendSettings.DefaultSevenZipForPlatform.Path.Combine("7za" + HostPlatform.Exe);
                    backend.Register(StevedoreBackendSettings.DefaultSevenZipForPlatform);
                }

                environmentVariables[StevedoreProgram.SevenZipPathEnvironmentVariable] = sevenZipPath.MakeAbsolute().ToString(SlashMode.Native);
            }

            backend.AddAction(
                actionName: m_LocalPath == null ? "Download and unpack" : "Unpack from local path",
                targetFiles: new[] { ArtifactVersionFile },
                inputs: sevenZipPath == null ? Array.Empty<NPath>() : new[] { sevenZipPath },
                executableStringFor: SteveExe.InvocationString,
                commandLineArguments: new[] { "steve", "internal-unpack", RepoName, Id.ToString(), Path.InQuotes() },
                environmentVariables: environmentVariables,
                allowUnwrittenOutputFiles: true
            );

            backend.AddImplicitDependenciesToAllActionsWithInputsThatLiveIn(Path, ArtifactVersionFile);
        }
    }
}
