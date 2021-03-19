using System.IO;
using NiceIO;
using Unity.BuildTools;

namespace Bee.Stevedore.Program
{
    public class DownloadCache
    {
        readonly StevedoreConfig m_Config;

        public DownloadCache(StevedoreConfig config)
        {
            m_Config = config;
            ValidateCacheFolder(m_Config.CacheFolder);
        }

        public NPath EnsureArtifactIsCached(string repoName, ArtifactId artifactId)
        {
            var artifactPath = PathFor(artifactId);
            if (artifactPath.FileExists())
                return artifactPath;

            var stream = MakeTempFileFor(artifactId.Name, out var tempPath);
            try
            {
                new Downloader().Download(repoName, m_Config.RepoConfigFor(repoName), artifactId, stream);
                tempPath.Move(artifactPath);
            }
            catch (UnavailableRepositoryException e)
            {
                throw e.CannotDownload(artifactId.Name);
            }
            finally
            {
                stream.Close();
                tempPath.DeleteIfExists();
            }
            return artifactPath;
        }

        /// <summary>
        /// Create a temporary file that can later be renamed to a real artifact
        /// path as given by PathFor (which means ensuring that the temp file
        /// resides on the same partition as the real file).
        ///
        /// Caller must delete or rename file.
        /// </summary>
        FileStream MakeTempFileFor(ArtifactName artifactName, out NPath path)
        {
            const int WARN_WIN32_FILE_EXISTS = unchecked((int)0x80070050); // also works on Mac/Linux

            while (true)
            {
                path = m_Config.CacheFolder.Combine("artifacts", (string)artifactName, ".tmp-" + Path.GetRandomFileName());
                path.EnsureParentDirectoryExists();
                try
                {
                    return new FileStream(path.ToString(SlashMode.Native), FileMode.CreateNew);
                }
                catch (IOException e) when (e.HResult == WARN_WIN32_FILE_EXISTS)
                {
                    // Just try again with a new random name
                }
            }
        }

        public NPath PathFor(ArtifactId artifactId) => PathFor(m_Config.CacheFolder, artifactId);

        internal static NPath PathFor(NPath cacheFolder, ArtifactId artifactId)
        {
            return cacheFolder.Combine("artifacts", (string)artifactId.Name, (string)artifactId.Version);
        }

        static void ValidateCacheFolder(NPath cacheFolder)
        {
            if (cacheFolder == null)
                throw new HumaneException("'cache-folder' could not be inferred from environment, nor was it configured in Stevedore.conf");

            if (cacheFolder.IsRelative)
                throw new HumaneException("'cache-folder' is not an absolute path (see 'bee steve config')");

            if (HostPlatform.IsWindows)
            {
                // Verify that cache folder has path budget for the longest possible artifact ID.
                int cacheFolderMaxLength = 259 - "/artifacts/".Length - ArtifactId.MaxLength;
                var cacheFolderString = cacheFolder.ToString(SlashMode.Native);
                if (cacheFolderString.Length > cacheFolderMaxLength)
                    throw new HumaneException($"'cache-folder' exceeds {cacheFolderMaxLength} characters, which is too long for Windows: {cacheFolderString}");
            }
        }
    }
}
