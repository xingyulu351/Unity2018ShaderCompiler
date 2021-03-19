using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Text.RegularExpressions;
using NiceIO;

namespace Bee.Stevedore.Program
{
    public class StevedoreConfig
    {
        /// <summary>
        /// Repository config options. null (or -1) means "unset" (to be inherited from defaults, if any).
        /// </summary>
        public class RepoConfig
        {
            public string CustomHttpHeader { get; set; }
            public string HttpProxyUrl { get; set; }
            public string HttpsProxyUrl { get; set; }
            public int Timeout { get; set; } = -1;
            public string Url { get; set; }

            public static RepoConfig Combine(RepoConfig primary, RepoConfig fallback)
            {
                return new RepoConfig
                {
                    CustomHttpHeader = primary.CustomHttpHeader ?? fallback.CustomHttpHeader,
                    HttpProxyUrl = primary.HttpProxyUrl ?? fallback.HttpProxyUrl,
                    HttpsProxyUrl = primary.HttpsProxyUrl ?? fallback.HttpsProxyUrl,
                    Timeout = primary.Timeout >= 0 ? primary.Timeout : fallback.Timeout,
                    Url = primary.Url ?? fallback.Url,
                };
            }

            public IEnumerable<KeyValuePair<string, string>> GetConfigValues()
            {
                yield return new KeyValuePair<string, string>("http-header", CustomHttpHeader);

                if (HttpProxyUrl == HttpsProxyUrl)
                    yield return new KeyValuePair<string, string>("proxy", HttpProxyUrl);
                else
                {
                    yield return new KeyValuePair<string, string>("proxy.http", HttpProxyUrl);
                    yield return new KeyValuePair<string, string>("proxy.https", HttpsProxyUrl);
                }
                yield return new KeyValuePair<string, string>("timeout", Timeout < 0 ? null : Timeout.ToString());
                yield return new KeyValuePair<string, string>("url", Url);
            }

            public void SetConfigValue(string keyPrefix, string key, string value, Action<string> reportParseError)
            {
                var fullKey = keyPrefix + key;
                switch (key)
                {
                    case "http-header": TrySetConfigValue(value, reportParseError, fullKey, ParseHttpHeader, val => CustomHttpHeader = val); break;
                    case "oauth.client-id": /* ignore, for now */ break;
                    case "oauth.client-secret": /* ignore, for now */ break;
                    case "proxy":       TrySetConfigValue(value, reportParseError, fullKey, ParseProxyUrl, val => HttpProxyUrl = HttpsProxyUrl = val); break;
                    case "proxy.http":  TrySetConfigValue(value, reportParseError, fullKey, ParseProxyUrl, val => HttpProxyUrl = val); break;
                    case "proxy.https": TrySetConfigValue(value, reportParseError, fullKey, ParseProxyUrl, val => HttpsProxyUrl = val); break;
                    case "timeout":     TrySetConfigValue(value, reportParseError, fullKey, ParseNonNegativeInteger, val => Timeout = val); break;
                    case "url":         TrySetConfigValue(value, reportParseError, fullKey, ParseRepoUrl, val => Url = val); break;
                    default: reportParseError?.Invoke("unrecognized config setting: " + fullKey); break;
                }
            }
        }

        static readonly Regex reConfigLine = new Regex(@"^[ \t]*(?:|[;#].*|([-_.a-zA-Z0-9]+)[ \t]*=[ \t]*(.*?)[ \t]*)$");
        static readonly Regex reRepoKey = new Regex(@"^repo\.([-_a-zA-Z0-9]+)\.([-a-z.]+)$");

        public NPath CacheFolder { get; set; }
        public RepoConfig DefaultRepoConfig { get; } = new RepoConfig
        {
            Timeout = 10 * 1000,
        };

        private readonly List<NPath> m_ConfigFiles = new List<NPath>();
        public IReadOnlyList<NPath> ConfigFiles { get; }

        NPath m_HomeFolder;
        readonly Dictionary<string, RepoConfig> m_RepoConfigs = new Dictionary<string, RepoConfig>
        {
            { "public", new RepoConfig { Url = "https://stevedore.unity3d.com/r/public/" } },
            { "testing", new RepoConfig { Url = "http://stevedore.hq.unity3d.com/r/testing/" } },
            { "unity-internal", new RepoConfig { Url = "http://stevedore.hq.unity3d.com/r/unity-internal/" } },
        };

        public StevedoreConfig()
        {
            ConfigFiles = m_ConfigFiles.AsReadOnly();
        }

        /// <summary>
        /// Yields an approximation of the config values that lead to this StevedoreConfig.
        /// </summary>
        public IEnumerable<KeyValuePair<string, string>> GetConfigValues()
        {
            yield return new KeyValuePair<string, string>("cache-folder", CacheFolder?.ToString(SlashMode.Native) ?? "");

            foreach (var kv in DefaultRepoConfig.GetConfigValues())
            {
                if (kv.Value != null)
                    yield return kv;
            }

            foreach (var rc in m_RepoConfigs)
            {
                foreach (var kv in rc.Value.GetConfigValues())
                {
                    if (kv.Value != null)
                        yield return new KeyValuePair<string, string>($"repo.{rc.Key}.{kv.Key}", kv.Value);
                }
            }
        }

        public RepoConfig RepoConfigFor(string repoName)
        {
            if (m_RepoConfigs.TryGetValue(repoName, out var rc))
                return RepoConfig.Combine(rc, DefaultRepoConfig);
            throw new UnavailableRepositoryException($"No repository named '{repoName}' has been configured");
        }

        public void Read(NPath path, Action<string> reportParseError, bool required)
        {
            m_ConfigFiles.Add(path);
            try
            {
                using (var stream = File.OpenText(path.ToString(SlashMode.Native)))
                {
                    Read(stream, reportParseError);
                }
            }
            catch (DirectoryNotFoundException) when (!required) {}
            catch (FileNotFoundException) when (!required) {}
        }

        public void Read(TextReader reader, Action<string> reportParseError)
        {
            while (true)
            {
                string line = reader.ReadLine();
                if (line == null) break;

                var m = reConfigLine.Match(line);
                if (!m.Success) // garbage
                {
                    reportParseError?.Invoke("invalid config file syntax: " + line);
                    continue;
                }
                if (!m.Groups[1].Success) continue; // comment or blank line
                var key = m.Groups[1].Value;
                var value = m.Groups[2].Value;

                switch (key)
                {
                    case "cache-folder":
                        if (value.StartsWith("~/") || value.StartsWith(@"~\"))
                            value = m_HomeFolder + value.Substring(1);
                        CacheFolder = value == "" ? null : value;
                        break;

                    default:
                        var match = reRepoKey.Match(key);
                        if (match.Success)
                        {
                            string repoName = match.Groups[1].Value;
                            if (!m_RepoConfigs.TryGetValue(repoName, out var cfg))
                                cfg = m_RepoConfigs[repoName] = new RepoConfig();
                            cfg.SetConfigValue($"repo.{repoName}.", match.Groups[2].Value, value, reportParseError);
                        }
                        else
                        {
                            DefaultRepoConfig.SetConfigValue("", key, value, reportParseError);
                        }
                        break;
                }
            }
        }

        /// <summary>
        /// Read defaults from the runtim environment (environment variables
        /// and user config file).
        /// </summary>
        public void ReadEnvironmentDefaults(Action<string> reportParseError)
        {
            var environmentPaths = new EnvironmentPaths();
            m_HomeFolder = environmentPaths.Home;
            CacheFolder = environmentPaths.Cache?.Combine("Stevedore");

            TrySetConfigValueFromEnv(reportParseError, "HTTP_PROXY", ParseProxyUrl, val => DefaultRepoConfig.HttpProxyUrl = val);
            TrySetConfigValueFromEnv(reportParseError, "http_proxy", ParseProxyUrl, val => DefaultRepoConfig.HttpProxyUrl = val);
            TrySetConfigValueFromEnv(reportParseError, "HTTPS_PROXY", ParseProxyUrl, val => DefaultRepoConfig.HttpsProxyUrl = val);

            if (environmentPaths.Config != null)
                Read(environmentPaths.Config.Combine("Stevedore.conf"), reportParseError, required: false);
            if (m_HomeFolder != null && m_HomeFolder != environmentPaths.Config)
                Read(m_HomeFolder.Combine("Stevedore.conf"), reportParseError, required: false);
        }

        static bool TrySetConfigValue<T>(string text, Action<string> reportParseError, string key, Func<string, T> parser, Action<T> setter)
        {
            if (text == null)
                throw new ArgumentNullException(nameof(text));

            T value;
            try
            {
                value = parser(text);
            }
            catch (Exception e)
            {
                reportParseError?.Invoke($"Invalid '{key}' value ({e.Message}): {text}");
                return false;
            }
            setter(value);
            return true;
        }

        static bool TrySetConfigValueFromEnv<T>(Action<string> reportParseError, string key, Func<string, T> parser, Action<T> setter)
        {
            var text = Environment.GetEnvironmentVariable(key);
            return !string.IsNullOrEmpty(text) && TrySetConfigValue(text, reportParseError, key, parser, setter);
        }

        static string ParseHttpHeader(string text)
        {
            if (text == "") return text;
            if (text.IndexOf(':') == -1)
                throw new Exception("must contain a ':'");
            return text;
        }

        static int ParseNonNegativeInteger(string text)
        {
            try
            {
                int value = int.Parse(text, NumberStyles.None);
                if (value > 0) return value;
            }
            catch (ArgumentNullException) {}
            catch (ArgumentException) {}
            catch (FormatException) {}
            catch (OverflowException) {}

            throw new Exception("must be a non-negative integer");
        }

        static string ParseProxyUrl(string text)
        {
            if (text == "") return text;
            if (!text.StartsWith("http://", StringComparison.Ordinal) &&
                !text.StartsWith("https://", StringComparison.Ordinal))
                throw new Exception("unsupported URL; must start with 'http://' or 'https://'");
            return text;
        }

        static string ParseRepoUrl(string text)
        {
            if (text == "") return text;
            ParseProxyUrl(text);
            if (text.IndexOf('/', "https://".Length) == -1) // No further slashes beyond 'http(s)://'?
                throw new Exception("a slash must follow the hostname in a repository URL prefix");
            return text;
        }
    }
}
