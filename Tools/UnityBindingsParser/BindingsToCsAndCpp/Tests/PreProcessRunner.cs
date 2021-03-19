using System;
using System.Diagnostics;
using System.IO;
using System.Text;
using Unity.CommonTools;

namespace BindingsToCsAndCpp.Tests
{
    public abstract class PreProcessRunner
    {
        public string TransformToCSharp(string filename, string txt, string target)
        {
            return RunCSPreProcessAndGrab(filename, txt, ".cs", target);
        }

        public string TransformToCpp(string filename, string txt, string target)
        {
            return RunCSPreProcessAndGrab(filename, txt, ".cpp", target);
        }

        public virtual string RunCSPreProcessAndGrab(string filename, string txt, string extension, string target)
        {
            throw new NotImplementedException();
        }
    }

    public class OldCsPreProcessRunner : PreProcessRunner
    {
        public override string RunCSPreProcessAndGrab(string filename, string txt, string extension, string target)
        {
            var temp = Path.Combine(Path.GetTempPath(), "cspreprocess", "Old", "Export");
            if (!Directory.Exists(temp))
                Directory.CreateDirectory(temp);
            temp = Path.Combine(temp, filename + ".bindings");
            File.WriteAllText(temp, txt);

            var destFolder = Path.GetTempPath();

            var arguments = Path.Combine(Workspace.BasePath, "Tools", "cspreprocess.pl") + " -jamplus \"" +
                temp.Replace('\\', '/') + "\" " + target + " -customOutputFolder \"" + destFolder.Replace('\\', '/') +
                "\" -nodocs -nodoclog";

            var p = new Process
            {
                StartInfo = new ProcessStartInfo
                {
                    FileName = "perl",
                    Arguments =
                        arguments,
                    RedirectStandardOutput = true,
                    RedirectStandardError = true,
                    UseShellExecute = false,
                    CreateNoWindow = true
                }
            };

            p.Start();

            if (!p.WaitForExit(5000))
                Console.WriteLine("The old parser timed out.");

            var outputfile = Path.Combine(destFolder, filename + extension);
            if (!File.Exists((outputfile)))
            {
                throw new Exception("output file wasn't generated. old cspreprocess parser output: " + p.StandardOutput.ReadToEnd() +
                    p.StandardError.ReadToEnd() + " outputfile expected at:" + outputfile + "  args: " +
                    p.StartInfo.Arguments);
            }

            var csContents = File.ReadAllText(outputfile);

            File.Delete(temp);
            File.Delete(outputfile);
            return csContents;
        }
    }


    public class NewCsPreProcessRunner : PreProcessRunner
    {
        public override string RunCSPreProcessAndGrab(string filename, string txt, string extension, string target)
        {
            var temp = Path.Combine(Path.GetTempPath(), "cspreprocess", "New", "Export");
            if (!Directory.Exists(temp))
                Directory.CreateDirectory(temp);
            temp = Path.Combine(temp, filename + ".bindings");
            File.WriteAllText(temp, txt);

            var destFolder = Path.GetTempPath();

            var arguments = extension == ".cpp" ? "-nocs" : "-nocpp";
            arguments += " -output " + destFolder + " -platform " + target + " " + temp;
            var app = Path.Combine(Workspace.BasePath, "build", "bin", "Debug", "BindingsToCsAndCpp.exe");
            var p = new Process
            {
                StartInfo = new ProcessStartInfo
                {
                    FileName = app,
                    Arguments = arguments,
                    RedirectStandardOutput = true,
                    RedirectStandardError = true,
                    UseShellExecute = false,
                    CreateNoWindow = true
                }
            };
            var output = new StringBuilder();
            var error = new StringBuilder();

            p.OutputDataReceived += (sender, e) => output.AppendLine(e.Data);
            p.ErrorDataReceived += (sender, e) => error.AppendLine(e.Data);

            p.Start();

            p.BeginOutputReadLine();
            p.BeginErrorReadLine();

            p.WaitForExit();

            if (p.ExitCode != (int)ReturnCodes.Success)
            {
                switch ((ReturnCodes)p.ExitCode)
                {
                    case ReturnCodes.InvalidMethodFound:
                        p.Close();
                        throw new InvalidMethodSignatureException(error.ToString(), "");
                    case ReturnCodes.UnbalancedBraces:
                        p.Close();
                        throw new UnbalancedBracesException(error.ToString());
                    default:
                        p.Close();
                        throw new Exception("The parser did not complete correctly" + Environment.NewLine +
                            Enum.GetName(typeof(ReturnCodes), p.ExitCode) + Environment.NewLine +
                            output + Environment.NewLine +
                            error);
                }
            }
            var outputfile = Path.Combine(destFolder, filename + "Bindings.gen" + extension);
            if (!File.Exists((outputfile)))
                throw new Exception("output file wasn't generated. new parser output: " + output + Environment.NewLine + error + " outputfile expected at:" + outputfile + "  args: " + p.StartInfo.Arguments);

            p.Close();
            var csContents = File.ReadAllText(outputfile);

            File.Delete(temp);
            File.Delete(outputfile);
            return csContents;
        }
    }
}
