using System;
using System.Linq;
using System.Runtime.Serialization.Formatters;
using Newtonsoft.Json;
using Newtonsoft.Json.Converters;
using Newtonsoft.Json.Serialization;
using NiceIO;
using Unity.BuildSystem.NativeProgramSupport;

namespace Bee.NativeProgramSupport.Running
{
    public abstract class RunnerEnvironment
    {
        [JsonConverter(typeof(TypeOnlyJsonConverter))]
        public Platform Platform { get; }
        public string EnvironmentId { get; }

        [JsonConverter(typeof(TypeOnlyJsonConverter))]
        [JsonProperty] // Enforce using the private setter.
        public Architecture[] SupportedArchitectures { get; private set; }

        [JsonIgnore]
        public string Identifier => CreateIdentifier(Platform, EnvironmentId);

        public static readonly NPath RunnerEnvironmentFilesDirectory = NPath.HomeDirectory.Combine(".Bee.RunnerEnvironments");
        public const string RunnerEnvironmentFilePrefix = "RunnerEnvironment_";

        [JsonIgnore]
        public NPath RunnerEnvironmentFilePath => RunnerEnvironmentFilesDirectory.Combine(RunnerEnvironmentFilePrefix + CreateFileIdentifier(Platform, EnvironmentId) + ".json");

        private static string CreateIdentifier(Platform platform, string environmentId)
        {
            return environmentId == null ? $"{platform.Name}" : $"{platform.Name}.{environmentId}";
        }

        public static string CreateFileIdentifier(Platform platform, string environmentId)
        {
            var identifier = CreateIdentifier(platform, environmentId);
            identifier = identifier.Replace('/', '_').Replace('\\', '_');
            return new NPath(identifier).WithoutInvalidCharacters('_').ToString();
        }

        [JsonConverter(typeof(StringEnumConverter))]
        public enum EnvironmentStatus
        {
            FullAccess,         // Environment is available and we expect that all operations will work
            NoLaunchPermission, // We are connected and can query data, but we are not able to launch.
            Offline,            // The environment is fully disconnected, we can not launch anything and are not able to query any data.
        }
        public EnvironmentStatus Status = EnvironmentStatus.FullAccess;

        protected RunnerEnvironment(Platform platform, Architecture[] supportedArchitectures, string environmentId)
        {
            this.Platform = platform;
            this.EnvironmentId = environmentId;
            this.SupportedArchitectures = supportedArchitectures;
        }

        // Instantiates the corresponding Runner for this RunnerEnvironment
        //
        // The arguments are completely optional and will be ignored by most runner implementations.
        // They are *not* arguments that are passed to any programs that might be run by the runner.
        // Instead, they allow Runner/RunnerEnvironment specific control on how the application is run.
        public abstract Runner InstantiateRunner(string runnerArguments = null);

        // Serialization binder that ignores fully qualified type if it fails.
        // We need to do this in order to support deserializing json files that were created with both bee.exe (all dlls merged) and the split-dll version.
        private class IgnoreAssemblyNameSerializationBinder : DefaultSerializationBinder
        {
            public override Type BindToType(string assemblyName, string typeName)
            {
                try
                {
                    return base.BindToType(assemblyName, typeName);
                }
                catch
                {
                    return AppDomain.CurrentDomain.GetAssemblies().Select(a => a.GetType(typeName)).FirstOrDefault(t => t != null);
                }
            }
        }

        public static JsonSerializerSettings JsonSerializerSettings =>
            new JsonSerializerSettings
        {
            NullValueHandling = NullValueHandling.Ignore,
            Formatting = Formatting.Indented,     // We want a human readable format!
            ConstructorHandling = ConstructorHandling.AllowNonPublicDefaultConstructor,
            TypeNameHandling = TypeNameHandling.Objects,
            TypeNameAssemblyFormat = FormatterAssemblyStyle.Simple,
            MissingMemberHandling = MissingMemberHandling.Ignore,
            Binder = new IgnoreAssemblyNameSerializationBinder()
        };

        public string SerializeToJson()
        {
            return JsonConvert.SerializeObject(this, JsonSerializerSettings);
        }

        public static RunnerEnvironment DeserializeFromJson(string jsonContent)
        {
            try
            {
                return JsonConvert.DeserializeObject(jsonContent, JsonSerializerSettings) as RunnerEnvironment;
            }
            catch (Exception e)
            {
                throw new Exception($"Failed to deserialize json:\n{jsonContent}", e);
            }
        }
    }
}
