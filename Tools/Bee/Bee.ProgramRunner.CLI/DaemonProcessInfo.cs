using System;
using System.Collections.Generic;
using Newtonsoft.Json;
using Newtonsoft.Json.Converters;

namespace Bee.ProgramRunner.CLI
{
    [JsonConverter(typeof(StringEnumConverter))]
    public enum DaemonStatus
    {
        NotRunning,
        StartingUp,
        Running,
    }

    public struct DaemonProcessInfo
    {
        public int PID;
        public string ProgramExecutable;
        public string Arguments;
        public int DaemonVersion;
        public Dictionary<string, int> RunnerEnvironmentProviderVersions;
        public DaemonStatus DaemonStatus;
    }
}
