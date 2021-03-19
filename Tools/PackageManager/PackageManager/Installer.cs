using System;
using System.IO;
using Unity.PackageManager.Ivy;

namespace Unity.PackageManager
{
    internal class Installer : Service
    {
        IvyModule package;
        public IvyModule Package { get { return package; } }

        public Installer(IvyModule package)
        {
            this.package = package;
            Name = "Installer Task";
            string file = Path.Combine(Settings.installLocation, "task-installer-" + JobId);
            if (File.Exists(file))
                File.Delete(file);
            File.WriteAllText(file, package.ToString());
        }

        public Installer(IvyModule package, Guid id) : base(id)
        {
            this.package = package;
            Name = "Installer Task";
            Restarted = true;
        }

        public static Installer FromExisting(string path)
        {
            var module = IvyParser.ParseFile<IvyModule>(path);
            if (module == null)
                return null;
            string jobid = Path.GetFileName(path).Substring("task-installer-".Length);
            return new Installer(module, new Guid(jobid));
        }

        protected override bool TaskStarting()
        {
            if (!base.TaskStarting())
                return false;

            IvyArtifact artifact = package.GetArtifact(ArtifactType.Package);
            if (artifact == null)
                return false;

            HookupChildTask(new DownloaderTask(artifact.MD5Uri, JobId), "MD5 Downloader Task");
            HookupChildTask(new DownloaderTask(artifact, JobId), "Package Downloader Task");
            HookupChildTask(new BinaryVerifier(LocalPath, artifact), "Checksum Verifier Task");
            HookupChildTask(new UnzipperTask(LocalPath, artifact));
            HookupChildTask(new ZipVerifier(Path.Combine(LocalPath.LocalPath, artifact.Filename), Settings.installLocation));

            return true;
        }

        protected override void TaskUpdateProgress(Task task, float progress)
        {
            float weight = 0f;
            float baseProgress = 0f;
            switch (task.Order)
            {
                case 0:
                    weight = 0.01f;
                    break;
                case 1:
                    weight = 0.49f;
                    baseProgress = 0.01f;
                    break;
                case 2:
                    weight = 0.10f;
                    baseProgress = 0.50f;
                    break;
                case 3:
                    weight = 0.25f;
                    baseProgress = 0.60f;
                    break;
                case 4:
                    weight = 0.15f;
                    baseProgress = 0.85f;
                    break;
            }
            //if (!String.IsNullOrEmpty (task.ProgressMessage))
            ProgressMessage = task.ProgressMessage;
            //else
            //  ProgressMessage = "Installing";
            base.TaskUpdateProgress(task, baseProgress + (progress * weight));
        }
    }
}
