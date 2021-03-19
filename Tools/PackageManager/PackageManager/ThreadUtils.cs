using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;

namespace Unity.PackageManager
{
    static class ThreadUtils
    {
        public static int MainThread { get; set; }

        public static bool InMainThread { get { return MainThread == 0 || Thread.CurrentThread.ManagedThreadId == MainThread; } }

        public static void SetMainThread()
        {
            MainThread = Thread.CurrentThread.ManagedThreadId;
        }
    }
}
