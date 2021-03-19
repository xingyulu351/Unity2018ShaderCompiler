using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading;
using NiceIO;
using Bee.NativeProgramSupport;
using Bee.Stevedore;
using Unity.BuildSystem.NativeProgramSupport;
using Unity.BuildTools;

namespace Bee.Toolchain.Emscripten
{
    public class EmscriptenSdkLocator : SdkLocator<EmscriptenSdk>
    {
        public override EmscriptenSdk Dummy => new DummyEmscriptenSdk();
        public override EmscriptenSdk UserDefault => FromPath(Environment.GetEnvironmentVariable("EMSCRIPTEN"));
        public EmscriptenSdk LatestSdk => (EmSdkPath != null ? FromEmSdkPath(EmSdkPath) : Dummy);
        public NPath EmSdkPath { get; }

        public EmscriptenSdkLocator(NPath emsdkPath = null)
        {
            var path = Environment.GetEnvironmentVariable("EMSDK");
            if (emsdkPath != null)
                EmSdkPath = emsdkPath;
            else if (path != null)
                EmSdkPath = new NPath(path);
        }

        private EmscriptenSdk FromEmSdkPath(NPath path, string version = null)
        {
            if (path == null)
                return null;

            var emscriptenDir = path.Combine("emscripten");
            var clangDir = path.Combine("clang");
            var pythonDir = path.Combine("python");
            var nodeDir = path.Combine("node");
            NPath pythonExe = null;
            NPath nodeExe = null;

            var pythonEnv = Environment.GetEnvironmentVariable("PYTHON");
            if (pythonEnv != null)
            {
                pythonExe = new NPath(pythonEnv).Combine("python");
            }

            // these two are required in the SDK
            if (!emscriptenDir.Exists() || !clangDir.Exists())
                return null;

            Version versionAsVersion = null;

            if (version == null)
            {
                string highestVersionStr = null;
                Version highestVersion = new Version(0, 0);
                foreach (var d in emscriptenDir.Contents())
                {
                    // if you want an explicit version like "master" or similar,
                    // then you need to specify it explicitly.  We find the latest "real" version.
                    try
                    {
                        var foundVersion = Version.Parse(d.FileName);
                        if (foundVersion < highestVersion)
                            continue;
                        highestVersion = foundVersion;
                        highestVersionStr = d.FileName;
                    }
                    catch (Exception)
                    {
                    }
                }

                if (highestVersionStr == null)
                    return null;

                version = highestVersionStr;
                versionAsVersion = highestVersion;
            }
            else
            {
                Version.TryParse(version, out versionAsVersion);
            }

            emscriptenDir = emscriptenDir.Combine(version);
            foreach (var d in clangDir.Contents())
            {
                if (d.FileName.Contains(version))
                {
                    clangDir = d;
                    break;
                }
            }

            // if nothing was found
            if (clangDir.FileName == "clang")
                return null;

            if (pythonExe == null)
            {
                var pythonExeName = "python" + HostPlatform.Exe;
                foreach (var d in pythonDir.Directories().Where(d => d.FileName.Contains("64bit")))
                {
                    // TODO compare versions to find highest
                    pythonDir = d;
                    if (!pythonDir.Exists(pythonExeName))
                    {
                        // this is a stupid one; emscripten changed where python was unpacked
                        pythonDir = pythonDir.Directories().FirstOrDefault(s => s.FileName.EndsWith("amd64"));
                        if (pythonDir == null || !pythonDir.Exists(pythonExeName))
                            continue;
                    }

                    pythonExe = pythonDir.Combine("python");
                    break;
                }

                if (pythonExe == null)
                {
                    // nothing was found; try to get lucky
                    pythonDir = null;
                    pythonExe = new NPath("python");
                }
            }

            foreach (var d in nodeDir.Directories())
            {
                try
                {
                    var major = Int32.Parse(d.FileName.Split('.')[0]);
                    if (major >= 8)
                    {
                        nodeDir = d;
                        break;
                    }
                }
                catch (Exception)
                {
                }
            }

            if (nodeDir.FileName == "node")
            {
                // nothing was found; try to get lucky
                nodeDir = null;
                nodeExe = new NPath("node");
            }
            else nodeExe = nodeDir.Combine("bin/node");

            return new EmscriptenSdk(emscriptenDir, clangDir, pythonExe, nodeExe, versionAsVersion);
        }

        protected override Platform Platform { get; }

        protected override IEnumerable<EmscriptenSdk> ScanForSystemInstalledSdks() => Array.Empty<EmscriptenSdk>();

        protected override EmscriptenSdk NewSdkInstance(NPath path, Version version, bool isDownloadable)
        {
            throw new NotImplementedException("An EmscriptenSdk cannot be made out of a single path");
        }

        public override EmscriptenSdk FromPath(NPath path)
        {
            throw new NotImplementedException("An EmscriptenSdk cannot be made out of a single path");
        }
    }

    public class DummyEmscriptenSdk : EmscriptenSdk
    {
        public override NPath Path => "/__unable_to_locate_emscripten_sdk__";
        public override string CppCompilerCommand => Path.ToString();
        public override string StaticLinkerCommand => Path.ToString();
        public override string DynamicLinkerCommand => Path.ToString();
        public override IEnumerable<NPath> LibraryPaths { get; } = Array.Empty<NPath>();
    }
}
