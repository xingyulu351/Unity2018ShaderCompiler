#pragma warning disable 414

using System;
using System.Collections.Generic;
using System.Threading;
using Unity.DataContract;
using Unity.PackageManager.Ivy;
using System.IO;

namespace Unity.PackageManager
{
    internal abstract class Service : Task
    {
        protected Queue<Task> tasks = new Queue<Task>();

        bool paused;
        bool running;
        object runLock = new object();

        public override bool IsRunning
        {
            get
            {
                lock (runLock)
                {
                    return running;
                }
            }
        }

        protected Service()
        {
            transient = false;
            Name = "Service Task";
            ProgressMessage = "Installing";
        }

        protected Service(Guid id) : base(id)
        {
            transient = false;
            Name = "Service Task";
            ProgressMessage = "Installing";
        }

        protected void HookupChildTask(Task task)
        {
            HookupChildTask(task, null);
        }

        protected void HookupChildTask(Task task, string name)
        {
            if (name != null)
                task.Name = name;
            task.Order = tasks.Count;
            task.Restarted = Restarted;
            tasks.Enqueue(task);
        }

        public override void Run()
        {
            lock (runLock)
            {
                running = true;
            }
            base.Run();
        }

        protected override bool TaskRunning()
        {
            if (!base.TaskRunning())
                return false;

            if (tasks.Count == 0)
                return true;

            bool taskCancelled = CancelRequested;
            bool taskSuccessful = true;
            Task task = null;
            int maxTimePerTask = 10000;
            int baseTimeout = 300;
            while (tasks.Count > 0 && !taskCancelled && taskSuccessful)
            {
                long ellapsed = 0;
                long remaining = 0;
                taskSuccessful = false;

                if (CancelRequested)
                    break;

                task = tasks.Dequeue();
                PackageManager.Instance.FireListeners(task);
                task.OnProgress += TaskUpdateProgress;
                task.Run();

                while (!CancelRequested && !task.Wait(baseTimeout))
                {
                    if (CancelRequested)
                    {
                        task.Cancel();
                        break;
                    }
                    ellapsed += baseTimeout;
                    remaining = task.EstimatedDuration;
                    if (ellapsed > maxTimePerTask)
                    {
                        if (remaining <= 0)
                        {
                            task.Cancel();
                            Cancel();
                        }
                    }
                }
                task.OnProgress -= TaskUpdateProgress;
                taskCancelled |= task.CancelRequested | CancelRequested;
                taskSuccessful = task.IsSuccessful;
                if (!taskSuccessful)
                    Console.WriteLine("Task failed: {0}", task.Name);
            }

            if (taskCancelled || !taskSuccessful)
                return false;

            if (task != null)
                Result = task.Result;

            return true;
        }

        protected virtual void TaskUpdateProgress(Task task, float progress)
        {
            UpdateProgress(progress);
        }

        protected override void TaskFinishing(bool success)
        {
            lock (runLock) {
                running = false;
            }
            if (!success)
            {
                ProgressMessage = "Installation or download error, please retry.";
            }
            base.TaskFinishing(success);
        }

        public override void Cancel()
        {
            base.Cancel();
            var temp = tasks;
            if (temp == null)
                return;
            foreach (var task in temp.ToArray())
            {
                if (task != null)
                    task.Cancel();
            }
            paused = true;
        }

        public void Cancel(bool enableResume)
        {
            Cancel();
            paused = enableResume;
        }

        public override void CleanupArtifacts()
        {
            base.CleanupArtifacts();
            if (!paused)
            {
                string file = Path.Combine(Settings.installLocation, "task-installer-" + JobId);
                if (File.Exists(file))
                    File.Delete(file);
            }
        }
    }
}
