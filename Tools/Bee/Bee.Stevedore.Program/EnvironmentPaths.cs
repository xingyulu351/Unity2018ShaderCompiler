using System;
using System.Collections;
using NiceIO;

namespace Bee.Stevedore.Program
{
    public class EnvironmentPaths
    {
        public NPath Home { get; }
        public NPath Config { get; }
        public NPath Cache { get; }

        public EnvironmentPaths()
            : this(Environment.GetEnvironmentVariables())
        {
        }

        public EnvironmentPaths(IDictionary environment)
        {
            // Always prefer explicitly set environment variables.
            if (environment.TryGetValue("XDG_CONFIG_HOME", out string XDG_CONFIG_HOME)) Config = XDG_CONFIG_HOME;
            if (environment.TryGetValue("XDG_CACHE_HOME", out string XDG_CACHE_HOME)) Cache = XDG_CACHE_HOME;
            else if (environment.TryGetValue("LOCALAPPDATA", out string LOCALAPPDATA)) Cache = LOCALAPPDATA;

            if (environment.TryGetValue("USERPROFILE", out string USERPROFILE))
            {
                // Use Windows layout (to the extent that's a thing)
                Home = USERPROFILE;
                if (Config == null) Config = Home;
            }
            else if (environment.TryGetValue("HOME", out string HOME))
            {
                Home = HOME;
                var library = Home.Combine("Library");
                if (library.DirectoryExists())
                {
                    // Use macOS layout
                    if (Config == null) Config = library;
                    if (Cache == null) Cache = library.Combine("Caches");
                }
                else
                {
                    // Use Linux/XDG layout
                    if (Config == null) Config = Home.Combine(".config");
                    if (Cache == null) Cache = Home.Combine(".cache");
                }
            }
        }
    }
}
