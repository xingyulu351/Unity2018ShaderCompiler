using System;
using System.IO;
using System.Net;
using System.Threading;
using System.Collections.Generic;
using Unity.PackageManager.Ivy;

namespace Unity.PackageManager
{
    public delegate void TaskStartedHandler(Task task);
    public delegate void TaskProgressHandler(Task task, float progress);
    public delegate void TaskFinishedHandler(Task task, bool success);

    public class Task : IDisposable, IAsyncResult
    {
        string name = "Generic Task";
        Uri localPath;
        Guid jobId = Guid.Empty;
        bool cancelRequested = false;

        Action<Task> taskStarting;
        Func<Task, bool> taskRunning;
        Action<Task, bool> taskFinishing;

        protected bool transient = true;

        public event TaskStartedHandler OnStart;
        public event TaskProgressHandler OnProgress;
        public event TaskFinishedHandler OnFinish;

        protected Uri LocalPath
        {
            get
            {
                if (localPath == null)
                {
                    localPath = new Uri(Path.Combine(Settings.downloadLocation, JobId.ToString()));
                    if (!Directory.Exists(localPath.LocalPath))
                        Directory.CreateDirectory(localPath.LocalPath);
                }
                return localPath;
            }
        }

        public Guid JobId
        {
            get
            {
                if (jobId == Guid.Empty)
                    jobId = Guid.NewGuid();
                return jobId;
            }
            set
            {
                jobId = value;
            }
        }

        public string Name { get { return name; } set { name = value; } }

        /// <summary>
        /// Returns true if the task has been cancelled (might still be running, will
        /// stop at the first available opportunity)
        /// </summary>
        public bool CancelRequested { get { return cancelRequested; } }

        /// <summary>
        /// If the task was completed successfully, whatever it returns will be available
        /// through this property
        /// </summary>
        public object Result { get; set; }

        /// <summary>
        /// Returns true if the task is currently running. This property is not reentrant
        /// (i.e., it locks).
        /// </summary>
        public virtual bool IsRunning { get; private set; }

        /// <summary>
        /// Returns true if the task is done (might have been cancelled, might have
        /// failed, might have been successful, check other properties for status)
        /// </summary>
        public bool IsCompleted { get; private set; }

        /// <summary>
        /// Returns true if the task completed successfully and wasn't cancelled
        /// </summary>
        public bool IsSuccessful { get; private set; }

        /// <summary>
        /// How far along the task is towards completion. Value is normalized.
        /// </summary>
        public float Progress { get; protected set; }

        /// <summary>
        /// A descriptive name for the progress (Downloading, Installing, etc)
        /// </summary>
        public string ProgressMessage { get; protected set; }

        public bool Restarted { get; set; }
        public bool Shared { get; set; }

        public WaitHandle AsyncWaitHandle { get; private set; }

        /// <summary>
        /// This property is set to > 0 if the task can estimate how long it'll
        /// take to complete (in milliseconds). The task controller can then
        /// adjust the timeouts to give it more time.
        /// </summary>
        public long EstimatedDuration { get; protected set;  }

        internal int Order { get; set; }
        object IAsyncResult.AsyncState { get { throw new NotImplementedException(); } }
        bool IAsyncResult.CompletedSynchronously { get { throw new NotImplementedException(); } }

        protected Task()
        {
            AsyncWaitHandle = new EventWaitHandle(false, EventResetMode.ManualReset);
            ProgressMessage = "";
        }

        public Task(Guid id) : this()
        {
            jobId = id;
            Shared = true;
        }

        /// <summary>
        /// This constructor can be used to define what happens at each stage of
        /// running a task instead of implementing a subclass for it (for quick oneliner tasks)
        ///
        /// All callbacks are invoked on the task thread.
        /// </summary>
        /// <param name="taskStarting"></param>
        /// <param name="taskRunning"></param>
        /// <param name="taskFinishing"></param>
        public Task(Action<Task> taskStarting,
                    Func<Task, bool> taskRunning,
                    Action<Task, bool> taskFinishing) : this()
        {
            this.taskStarting = taskStarting;
            this.taskRunning = taskRunning;
            this.taskFinishing = taskFinishing;
        }

        public virtual void Run()
        {
            IsRunning = true;
            ThreadPool.QueueUserWorkItem(RunTask);
        }

        public virtual void Stop()
        {
            Stop(false);
        }

        /// <summary>
        /// Stop a task and optionally wait for it to actually stop.
        /// </summary>
        /// <param name="wait"></param>
        public virtual void Stop(bool wait)
        {
            if (!IsRunning)
            {
                CleanupHandlers();
                return;
            }
            transient = true;
            Console.WriteLine("Shutting down " + name);
            Cancel();

            ManualResetEvent signal = null;
            if (wait)
            {
                signal = new ManualResetEvent(false);
                if (IsCompleted)
                    signal.Set();
                else
                    OnFinish += (t, s) => signal.Set();

                if (!signal.WaitOne(500))
                {
                    CleanupHandlers();
                    Console.WriteLine("{0} stopped.", Name);
                    return;
                }
            }
        }

        public bool Wait(int milliseconds)
        {
            return AsyncWaitHandle.WaitOne(milliseconds);
        }

        public virtual void Cancel()
        {
            cancelRequested = true;
            if (IsCompleted)
            {
                InvokeOnFinish(this, IsSuccessful);
            }
        }

        void RunTask(object state)
        {
            bool ret = false;

            try
            {
                if (!cancelRequested)
                    ret = TaskStarting();

                if (!cancelRequested && ret)
                    ret = TaskRunning();

                if (cancelRequested)
                    ret = false;

                TaskFinishing(ret);
            }
            catch (ThreadAbortException)
            {
                Console.WriteLine("Cancelling tasks, domain is going down");
                ret = false;
            }
            catch (Exception ex)
            {
                Exception ub = ex.InnerException;
                while (ub != null && !(ub is ThreadAbortException))
                    ub = ub.InnerException;
                if (ub != null && ub is ThreadAbortException)
                    Console.WriteLine("Cancelling tasks, domain is going down");
                else
                    Console.WriteLine("Aborting task {0}.{1}{2}", Name, Environment.NewLine, ex);
            }

            IsSuccessful = ret;
            IsCompleted = true;

            ((EventWaitHandle)AsyncWaitHandle).Set();

            if (transient)
                CleanupHandlers();

            CleanupArtifacts();
        }

        /// <summary>
        /// Updates the progress property and OnProgress event
        /// </summary>
        /// <param name="progress">Progress, from 0 to 1.</param>
        protected virtual void UpdateProgress(float progress)
        {
            Progress = progress;

            InvokeOnProgress(this, progress);
        }

        protected virtual bool TaskStarting()
        {
            if (cancelRequested)
                return false;

            if (taskStarting != null)
                taskStarting(this);

            if (cancelRequested)
                return false;

            InvokeOnStart(this);

            if (cancelRequested)
                return false;

            return true;
        }

        protected virtual bool TaskRunning()
        {
            if (taskRunning != null)
                return taskRunning(this);
            return true;
        }

        protected virtual void TaskFinishing(bool success)
        {
            if (taskFinishing != null)
                taskFinishing(this, success);

            InvokeOnFinish(this, success);
        }

        protected void InvokeOnStart(Task task)
        {
            if (OnStart != null)
                OnStart(task);
        }

        /// <summary>
        /// Invokes the OnProgress even.
        /// </summary>
        /// <param name="task">Task.</param>
        /// <param name="progress">Progress, from 0 to 1</param>
        protected void InvokeOnProgress(Task task, float progress)
        {
            if (OnProgress != null)
                OnProgress(task, progress);
        }

        protected void InvokeOnFinish(Task task, bool success)
        {
            if (OnFinish != null)
                OnFinish(task, success);
        }

        public void Dispose()
        {
            CleanupHandlers();
        }

        void CleanupHandlers()
        {
            OnStart = null;
            OnProgress = null;
            OnFinish = null;
            taskStarting = null;
            taskRunning = null;
            taskFinishing = null;
        }

        public virtual void CleanupArtifacts()
        {
            // don't cleanup artifacts on child tasks when they're done, the controller task will do it after everything is done
            if (Shared)
                return;

            try
            {
                if (localPath != null && Directory.Exists(localPath.LocalPath))
                    Directory.Delete(localPath.LocalPath, true);
                localPath = null;
            }
            catch
            {
                Console.WriteLine("Unable to clean up artifacts at {0}", localPath);
            }
        }
    }

    internal class DownloaderTask : Task
    {
        private readonly Uri uri;
        WebRequest request;
        string streamName;
        long bytes;

        public DownloaderTask(Uri uri)
        {
            Name = "Downloader Task";
            ProgressMessage = "Downloading";
            string name = uri.Segments[uri.Segments.Length - 1];
            if (name == "/")
                name = "temp";
            streamName = Path.Combine(LocalPath.LocalPath, name);
            this.uri = uri;
        }

        public DownloaderTask(IvyArtifact artifact)
        {
            Name = "Downloader Task";
            ProgressMessage = "Downloading";
            streamName = Path.Combine(LocalPath.LocalPath, artifact.Filename);
            uri = artifact.Url;
        }

        public DownloaderTask(IvyArtifact artifact, Guid id) : base(id)
        {
            Name = "Downloader Task";
            ProgressMessage = "Downloading";
            streamName = Path.Combine(LocalPath.LocalPath, artifact.Filename);
            uri = artifact.Url;


            //          Console.WriteLine ("{0} {1} {2} {3}", Name, uri, id, streamName);
        }

        public DownloaderTask(Uri uri, Guid id) : base(id)
        {
            Name = "Downloader Task";
            ProgressMessage = "Downloading";
            string name = uri.Segments[uri.Segments.Length - 1];
            if (name == "/")
                name = "temp";
            streamName = Path.Combine(LocalPath.LocalPath, name);
            this.uri = uri;

            //          Console.WriteLine ("{0} {1} {2} {3}", Name, uri, id, streamName);
        }

        protected override bool TaskStarting()
        {
            if (!base.TaskStarting())
                return false;

            if (Restarted)
            {
                var fi = new FileInfo(Path.Combine(LocalPath.LocalPath, streamName));
                if (fi.Exists && fi.Length > 0)
                    bytes = fi.Length;
                else if (fi.Length == 0)
                    fi.Delete();
            }

            request = WebRequest.Create(uri);
            if (request is HttpWebRequest)
            {
                ((HttpWebRequest)request).UserAgent = "Unity PackageManager v" + PackageManager.Instance.Version;
                if (bytes > 0)
                    ((HttpWebRequest)request).AddRange((int)bytes);  // TODO: fix classlibs to take long overloads
            }
            request.Method = "GET";
            request.Timeout = 3000;

            return true;
        }

        protected override bool TaskRunning()
        {
            if (Restarted && bytes > 0)
                Console.WriteLine("Resuming download of {0} to {1}", uri, streamName);
            else
                Console.WriteLine("Downloading {0} to {1}", uri, streamName);

            Result = streamName;

            using (WebResponse response = request.GetResponseWithoutException())
            {
                if (response == null)
                    return false;
                else if (Restarted && bytes > 0 && response is HttpWebResponse)
                {
                    if ((int)(((HttpWebResponse)response).StatusCode) == 416)
                    {
                        UpdateProgress(1);
                        return true;
                    }
                    else if ((int)(((HttpWebResponse)response).StatusCode) != 200)
                    {
                        return false;
                    }
                }

                long respSize = response.ContentLength;
                if (Restarted && bytes > 0)
                {
                    UpdateProgress(bytes / respSize);
                    if (bytes == respSize)
                        return true;
                }

                using (Stream rStream = response.GetResponseStream())
                {
                    using (Stream localStream = new FileStream(streamName, FileMode.Append))
                    {
                        if (CancelRequested)
                            return false;

                        return Utils.Copy(rStream, localStream, 8192, respSize,
                            (totalRead, timeToFinish) =>
                            {
                                EstimatedDuration = timeToFinish;
                                UpdateProgress((float)totalRead / respSize);
                                //Console.WriteLine ("Is {0} {1} cancelled? {2}", Name, JobId, CancelRequested);
                                return !CancelRequested;
                            },
                            100);
                    }
                }
            }
        }

        protected override void TaskFinishing(bool success)
        {
            base.TaskFinishing(success);
        }
    }

    internal static class TaskFactory
    {
        /// <summary>
        /// Scans the installation directory for task files and recreates them so they can be resumed
        /// </summary>
        /// <returns></returns>
        public static Task[] FromExisting(string path)
        {
            List<Task> tasks = new List<Task>();
            if (!Directory.Exists(path))
                return tasks.ToArray();

            var files = Directory.GetFiles(path);
            foreach (var f in files)
            {
                string file = Path.GetFileName(f);
                //              Console.WriteLine ("file {0}", file);
                if (file.StartsWith("task-installer-"))
                {
                    var installer = Installer.FromExisting(f);
                    if (installer != null)
                        tasks.Add(installer);
                }
            }
            return tasks.ToArray();
        }
    }
}
