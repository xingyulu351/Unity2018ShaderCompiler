using System;
using System.Threading;
using Unity.BuildTools;

namespace Unity.BuildSystem.IntegrationTests.Framework
{
    public class TakeAtLeast : IDisposable
    {
        private readonly DateTime _endTime;

        public TakeAtLeast(TimeSpan span)
        {
            _endTime = DateTime.Now + span;
        }

        public void Dispose()
        {
            var timeSpan = _endTime - DateTime.Now;
            var milliseconds = (int)timeSpan.TotalMilliseconds;

            //max to 2000 milliseconds, to avoid a very very rare instabliity that happened on katana once.
            var maxed = Math.Max(0, Math.Min(milliseconds, 2000));
            Thread.Sleep(maxed);
        }
    }

    public class TakeAtLeastEnoughTimeForFileSystemResolutionToIncrement : TakeAtLeast
    {
        public TakeAtLeastEnoughTimeForFileSystemResolutionToIncrement() : base(TimeSpan.FromSeconds(3))
        {
        }
    }
}
