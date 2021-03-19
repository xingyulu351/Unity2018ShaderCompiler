using System;
using System.Globalization;
using System.Threading;
using Unity.TinyProfiling;

namespace JamSharp.Runtime
{
    public static class RuntimeManager
    {
        public static bool Finished { get; private set; }
        public static bool Initialized { get; private set; }

        public static void Initialize()
        {
            Initialized = true;
        }

        public static void InvokeOnFinishedBindingPhase()
        {
            OnFinishedBindingPhase?.Invoke();
            TinyProfiler.Finish();
        }

        public static IJamStateBuilder ParallelJamStateBuilder { get; set; }

        public static event Action OnFinishedBindingPhase;
    }
}
