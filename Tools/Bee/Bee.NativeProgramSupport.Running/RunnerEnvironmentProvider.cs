using System;
using System.Collections.Generic;

namespace Bee.NativeProgramSupport.Running
{
    // New versions should be backwards compatible for reading.
    // Change the version every time the format has changed or the RunnerEnvironmentProvider has been improved to the extent that we no longer want allow older versions to run.
    // RunnerEnvironments itself should stay backwards compatible!
    public class RunnerEnvironmentProviderVersion : Attribute
    {
        public int Version { get; }

        public RunnerEnvironmentProviderVersion(int version)
        {
            Version = version;
        }
    }

    // Baseclass that is easily created and used via reflection.
    public abstract class RunnerEnvironmentProvider : IDisposable
    {
        // Filter used for DynamicLoader when looking up RunnerEnvironmentProvider
        public const string AssemblyFilter = "Bee.*.dll";

        public abstract IEnumerable<RunnerEnvironment> ProvideInternal();
        public abstract IEnumerable<RunnerEnvironment> UpdateStreamInternal();

        public virtual void Dispose()
        {
        }
    }

    // Class to be implemented by concrete RunnerEnvironmentProvider, enforces RunnerEnvironment type.
    public abstract class RunnerEnvironmentProviderImplementation<T> : RunnerEnvironmentProvider
        where T : RunnerEnvironment
    {
        public override IEnumerable<RunnerEnvironment> ProvideInternal() => Provide();
        public override IEnumerable<RunnerEnvironment> UpdateStreamInternal() => UpdateStream();

        // Lists all currently available RunnerEnvironments
        public abstract IEnumerable<T> Provide();

        // Provides a continous stream of updated RunnerEnvironments.
        // Blocks until disposed or live updates are impossible/meaningless (e.g. HostPlatform)
        public virtual IEnumerable<T> UpdateStream()
        {
            yield break;
        }
    }
}
