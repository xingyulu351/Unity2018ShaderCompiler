namespace UnityBindingsParser
{
    internal class CancellationToken
    {
        public bool IsCancelled { get; private set; }

        public void Cancel()
        {
            IsCancelled = true;
        }
    }
}
