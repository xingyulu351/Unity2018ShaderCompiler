using System;
using System.Collections.Generic;
using System.IO;
using System.Text;

namespace Unity.CommonTools
{
    public static class MicrosoftCppCompiler
    {
        private interface ICompilerOptions
        {
            string Cl { get; }
            IEnumerable<string> Options { get; }
            IEnumerable<string> Defines { get; }
            IEnumerable<string> LinkerOptions { get; }
        }

        private abstract class Wp80CompilerOptions : ICompilerOptions
        {
            private static string _sdkDirectory;
            private static string _kitDirectory;

            public abstract string Cl { get; }
            public virtual IEnumerable<string> Options { get; private set; }
            public virtual IEnumerable<string> Defines { get; private set; }
            public virtual IEnumerable<string> LinkerOptions { get; private set; }

            protected static string SdkDirectory
            {
                get
                {
                    if (_sdkDirectory == null)
                    {
                        var directory = Path.Combine(GetVs2012Directory(), @"VC\WPSDK\WP80");
                        if (!Directory.Exists(directory))
                            throw new Exception("Windows Phone 8.0 SDK directory not found.");
                        _sdkDirectory = directory;
                    }
                    return _sdkDirectory;
                }
            }

            protected static string KitDirectory
            {
                get
                {
                    if (_kitDirectory == null)
                    {
                        var directory = Path.Combine(GetProgramFilesDirectory(), @"Windows Phone Kits\8.0");
                        if (!Directory.Exists(directory))
                            throw new Exception("Windows Phone 8.0 kit directory not found.");
                        _kitDirectory = directory;
                    }
                    return _kitDirectory;
                }
            }

            public Wp80CompilerOptions()
            {
                var includeDirectory = Path.Combine(SdkDirectory, "include");
                Options = new[] { "/nologo", "/GS", "/GL", "/analyze-", "/W4", "/WX", "/Gy", "/Zc:forScope", "/Zc:wchar_t", "/Gm-", "/O2", "/sdl", "/fp:precise", "/errorReport:none", "/ZW", "/EHsc", "/Gd", "/Oy-", "/Oi", "/MD", string.Format("/I\"{0}\"", includeDirectory) };
                Defines = new[] { "_WINRT_DLL", "NDEBUG", "_WINDLL", "_UNICODE", "UNICODE", "WINAPI_FAMILY=WINAPI_FAMILY_PGONE_APP" };
                LinkerOptions = new[] { "/NOLOGO", "/MANIFEST:NO", "/LTCG", "/NXCOMPAT", "/DYNAMICBASE", "/DLL", "/NODEFAULTLIB:\"ole32.lib\"", "/WINMD", "/OPT:REF", "/SUBSYSTEM:CONSOLE", "/OPT:ICF", "/ERRORREPORT:NONE" };
            }
        }

        private sealed class Wp80ArmCompilerOptions : Wp80CompilerOptions
        {
            private static string _cl;
            private readonly IEnumerable<string> _options;
            private readonly IEnumerable<string> _linkerOptions;

            private static string StaticCl
            {
                get
                {
                    if (_cl == null)
                    {
                        var path = Path.Combine(SdkDirectory, @"bin\x86_arm\cl.exe");
                        if (!File.Exists(path))
                            throw new Exception("Windows Phone 8.0 ARM compiler not found.");
                        _cl = path;
                    }
                    return _cl;
                }
            }

            public override string Cl { get { return StaticCl; } }
            public override IEnumerable<string> Options { get { return _options; } }
            public override IEnumerable<string> LinkerOptions { get { return _linkerOptions; } }

            public Wp80ArmCompilerOptions()
            {
                var options = new List<string>(base.Options);
                options.Add(string.Format("/AI\"{0}\"", Path.Combine(SdkDirectory, @"lib\arm")));
                options.Add(string.Format("/AI\"{0}\"", Path.Combine(KitDirectory, "Windows MetaData")));
                _options = options;

                var linkerOptions = new List<string>(base.LinkerOptions);
                linkerOptions.Add("/MACHINE:ARM");
                linkerOptions.Add(string.Format("/LIBPATH:\"{0}\"", Path.Combine(SdkDirectory, @"lib\arm")));
                linkerOptions.Add(string.Format("/LIBPATH:\"{0}\"", Path.Combine(KitDirectory, @"lib\ARM")));
                _linkerOptions = linkerOptions;
            }
        }

        private static string _programFilesDirectory;
        private static string _vs2012Directory;

        private static string GetProgramFilesDirectory()
        {
            if (_programFilesDirectory == null)
            {
                var directory = Environment.GetEnvironmentVariable("ProgramFiles(x86)");
                if (string.IsNullOrEmpty(directory))
                    directory = @"C:\Program Files (x86)";
                if (!Directory.Exists(directory))
                    throw new Exception("Program Files directory not found.");
                _programFilesDirectory = directory;
            }
            return _programFilesDirectory;
        }

        private static string GetVs2012Directory()
        {
            if (_vs2012Directory == null)
            {
                var directory = Environment.GetEnvironmentVariable("VS110COMNTOOLS");
                if (string.IsNullOrEmpty(directory))
                    directory = Path.Combine(GetProgramFilesDirectory(), @"Microsoft Visual Studio 11.0\Common7\Tools");
                directory = Path.GetFullPath(Path.Combine(directory, @"..\.."));
                if (!Directory.Exists(directory))
                    throw new Exception("Visual Studio 2012 directory not found.");
                _vs2012Directory = directory;
            }
            return _vs2012Directory;
        }

        private static string BuildCompilerOptionsString(ICompilerOptions options, IEnumerable<string> sourceFiles, string output)
        {
            var sb = new StringBuilder();

            foreach (var option in options.Options)
            {
                if (sb.Length > 0)
                    sb.Append(' ');
                sb.Append(option);
            }

            foreach (var define in options.Defines)
            {
                if (sb.Length > 0)
                    sb.Append(' ');
                sb.Append("/D \"");
                sb.Append(define);
                sb.Append('"');
            }

            foreach (var sourceFile in sourceFiles)
            {
                if (sb.Length > 0)
                    sb.Append(' ');
                sb.Append('"');
                sb.Append(sourceFile);
                sb.Append('"');
            }

            if (sb.Length > 0)
                sb.Append(' ');
            sb.Append("/link /OUT:\"");
            sb.Append(output);
            sb.Append('"');

            foreach (var option in options.LinkerOptions)
            {
                if (sb.Length > 0)
                    sb.Append(' ');
                sb.Append(option);
            }

            return sb.ToString();
        }

        private static void Compile(ICompilerOptions options, string source, string output)
        {
            var tempDirectory = Files.GetTempFile();
            Directory.CreateDirectory(tempDirectory);

            try
            {
                var command = options.Cl;
                var sourceFile = Path.Combine(tempDirectory, "Source.cpp");
                File.WriteAllText(sourceFile, source);
                var dllFile = Path.Combine(tempDirectory, Path.GetFileName(output));
                var args = BuildCompilerOptionsString(options, new[] { sourceFile }, dllFile);

                string error;
                int exitCode;
                var stdout = Shell.ExecuteProgramAndGetStdout(command, args, out error, out exitCode);
                if (exitCode != 0)
                    throw new Exception(string.Format("Failed to compile \"{0}\":{1}{2}", output, Environment.NewLine, stdout));

                File.Move(dllFile, output);
                File.Move(Path.ChangeExtension(dllFile, ".winmd"), Path.ChangeExtension(output, ".winmd"));
            }
            finally
            {
                Directory.Delete(tempDirectory, true);
            }
        }

        public static void CompileWp80Arm(string source, string output)
        {
            Compile(new Wp80ArmCompilerOptions(), source, output);
        }
    }
}
