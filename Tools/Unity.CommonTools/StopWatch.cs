using System.Diagnostics;

namespace Unity.CommonTools
{
    public interface IStopWatch
    {
        void Start();
        void Stop();

        long elapsedMilliseconds { get; }
    }

    public class StopWatch : IStopWatch
    {
        private readonly Stopwatch m_Stopwatch = new Stopwatch();

        public void Start()
        {
            m_Stopwatch.Start();
        }

        public void Stop()
        {
            m_Stopwatch.Stop();
        }

        public long elapsedMilliseconds { get { return m_Stopwatch.ElapsedMilliseconds; } }
    }
}
