using System;

namespace AssemblyPatcher.Extensions
{
    public static class AppDomainExtensions
    {
        public static DisposableAppDomain AsDisposable(this AppDomain self)
        {
            if (self == null)
                throw new ArgumentNullException("self");

            return new DisposableAppDomain(self);
        }
    }

    public interface IDisposableAppDomain : IDisposable
    {
        AppDomain AppDomain { get; }
    }

    public class DisposableAppDomain : IDisposable
    {
        public DisposableAppDomain(AppDomain appDomain)
        {
            this.appDomain = appDomain;
        }

        public void Dispose()
        {
            if (appDomain != null)
            {
                var toBeUnloaded = appDomain;
                appDomain = null;

                AppDomain.Unload(toBeUnloaded);
            }
        }

        public static implicit operator AppDomain(DisposableAppDomain wrapper)
        {
            return wrapper.appDomain;
        }

        private AppDomain appDomain;
    }
}
