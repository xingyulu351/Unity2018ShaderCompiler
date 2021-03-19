using System;
using Unity.DataContract;
using System.Linq;
using System.IO;

namespace Unity.PackageManager
{
    public static class PackageInfoExtensions
    {
        static void RefreshReleaseNotes(PackageInfo package)
        {
            if (!String.IsNullOrEmpty(package.releaseNotes))
                return;

            var notes = package.files.FirstOrDefault(x => x.Value.type == PackageFileType.ReleaseNotes);
            if (notes.Key == null)
            {
                package.releaseNotes = "None";
                return;
            }

            var path = package.basePath ?? Settings.installLocation;
            path = Path.Combine(path, notes.Key);
            if (!File.Exists(path))
                return;

            try
            {
                package.releaseNotes = File.ReadAllText(path);
            }
            catch (Exception)
            {
                // Conflict with downloader
                package.releaseNotes = "Refreshing...";
            }
            if (String.IsNullOrEmpty(package.releaseNotes))
                package.releaseNotes = "None";
        }

        public static PackageInfo Refresh(this Unity.DataContract.PackageInfo info)
        {
            if (info != null)
                RefreshReleaseNotes(info);
            return info;
        }
    }
}
