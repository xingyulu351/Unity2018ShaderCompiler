using System;
using System.CodeDom.Compiler;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Runtime.Remoting.Messaging;
using System.Threading;
using Bee.TestHelpers;
using JamSharp.Runtime;
using NiceIO;
using NUnit.Framework;
using Unity.BuildTools;

namespace Bee.Tools.Tests
{
    // We're using on the fly compiled c# executables for our tests ehere.
    // This has less dependencies and is faster than building native executables, but has the disadvantage that we can not really test Shell.Execute without any arguments on non-windows plats.
    [TestFixture]
    public class ShellTests
    {
        private readonly NPath tempDir = new TempDirectory("ShellExecuteTests");

        private string CompileExecutable(string source)
        {
            var executableDir = tempDir.Combine(source.GetHashCode() + ".exe");

            var provider = CodeDomProvider.CreateProvider("CSharp");
            var cp = new CompilerParameters
            {
                GenerateExecutable = true,
                OutputAssembly = executableDir.ToString(),
                GenerateInMemory = false,
                TreatWarningsAsErrors = true
            };
            CompilerResults cr = provider.CompileAssemblyFromSource(cp, source);
            if (cr.Errors.Count <= 0)
            {
                if (HostPlatform.IsWindows)
                    return executableDir.InQuotes();
                else
                    return "mono " + executableDir.InQuotes();
            }

            foreach (CompilerError ce in cr.Errors)
            {
                Console.WriteLine("  {0}", ce.ToString());
                Console.WriteLine();
            }

            throw new Exception("Error building source:\n\n" + source);
        }

        [Test]
        public void SplitInvocationString_EmptyString()
        {
            Shell.SplitInvocationStringToExecutableAndArgs("", out string executable, out string arguments);
            Assert.That(executable, Is.EqualTo(""));
            Assert.That(arguments, Is.EqualTo(""));
        }

        [Test]
        public void SplitInvocationString_NoArguments()
        {
            Shell.SplitInvocationStringToExecutableAndArgs("command", out string executable, out string arguments);
            Assert.That(executable, Is.EqualTo("command"));
            Assert.That(arguments, Is.EqualTo(""));
        }

        [Test]
        public void SplitInvocationString_NoQuotes()
        {
            Shell.SplitInvocationStringToExecutableAndArgs("command arg1 arg2", out string executable, out string arguments);
            Assert.That(executable, Is.EqualTo("command"));
            Assert.That(arguments, Is.EqualTo("arg1 arg2"));
        }

        [Test]
        public void ExecuteArgs_FromCommandLine_WithQuotes()
        {
            var executeArgs = Shell.ExecuteArgs.FromCommandLine("\"some filepath with/spaces/command\" \"arg1 arg2 \" ");
            Assert.That(executeArgs.Executable, Is.EqualTo("\"some filepath with/spaces/command\""));
            Assert.That(executeArgs.Arguments, Is.EqualTo("\"arg1 arg2 \" "));
        }

        [Test]
        public void Execute_ReturnsEmptyExecuteArgs_ForProgramWithoutOutput()
        {
            var result = Shell.Execute(CompileExecutable(@"class Program { static void Main() {} }"));
            Assert.That(result.Duration.TotalMilliseconds, Is.GreaterThan(0));
            Assert.That(result.ExitCode, Is.Zero);
            Assert.That(result.StdErr, Is.Empty);
            Assert.That(result.StdOut, Is.Empty);
            Assert.That(result.ReachedTimeout, Is.False);
        }

        [Test]
        public void Execute_ReturnsStdOut()
        {
            Assert.That(Shell.Execute(CompileExecutable(@"class Program { static void Main() {
                    System.Console.Write(""stdout"");
            } }")).StdOut, Is.EqualTo("stdout"));
        }

        [Test]
        public void Execute_ReturnsStdErr()
        {
            Assert.That(Shell.Execute(CompileExecutable(@"class Program { static void Main() {
                    System.Console.Error.Write(""stderr"");
            } }")).StdErr, Is.EqualTo("stderr"));
        }

        [Test]
        public void Execute_ReturnsExitCode()
        {
            Assert.That(() => Shell.Execute(CompileExecutable(@"class Program { static int Main() {
                    return 21;
            } }")).ExitCode, Is.EqualTo(21));
        }

        [Test]
        public void Execute_CanSetEnvironmentVariable()
        {
            Assert.That(Shell.Execute(CompileExecutable(@"class Program { static int Main() {
                    return System.Environment.GetEnvironmentVariable(""SHELLEXECTESTVAR"") == ""1"" ? 21 : 0; } }"),
                new Dictionary<string, string>() {{"SHELLEXECTESTVAR", "1"}}).ExitCode, Is.EqualTo(21));
        }

        class TestExecuteController : Shell.IExecuteController
        {
            public string allStdOutReceived = "";
            public string allStdErrReceived = "";

            public void OnStdoutReceived(string line)
            {
                allStdOutReceived += line;
            }

            public void OnStderrReceived(string line)
            {
                allStdErrReceived += line;
            }
        }

        [Test]
        public void Execute_ExecuteController_ReceivesStdOut()
        {
            TestExecuteController controller = new TestExecuteController();
            Shell.Execute(Shell.ExecuteArgs.FromCommandLine(CompileExecutable(@"class Program { static void Main() {
                    System.Console.Write(""stdout"");
            } }")), controller);
            Assert.That(controller.allStdOutReceived, Is.EqualTo("stdout"));
            Assert.That(controller.allStdErrReceived, Is.Empty);
        }

        [Test]
        public void Execute_ExecuteController_ReceivesStdErr()
        {
            TestExecuteController controller = new TestExecuteController();
            Shell.Execute(Shell.ExecuteArgs.FromCommandLine(CompileExecutable(@"class Program { static void Main() {
                    System.Console.Error.Write(""stderr"");
            } }")), controller);
            Assert.That(controller.allStdOutReceived, Is.Empty);
            Assert.That(controller.allStdErrReceived, Is.EqualTo("stderr"));
        }

        [Test]
        public void Execute_WithTimeout()
        {
            const int timeoutMs = 1;
            const int maxActualTimeoutMs = 8000; // Generous margin since process killing can take a while on windows (test takes 6ms on mac and up to 5500ms on windows)

            var result = Shell.Execute(Shell.ExecuteArgs.FromCommandLine(CompileExecutable(@"class Program { static void Main() { while(true) {} } }")), null, timeoutMs);
            Assert.That(result.ReachedTimeout, Is.True);
            Assert.That(result.Duration.TotalMilliseconds, Is.LessThan(maxActualTimeoutMs));
        }

        [Test]
        public void Execute_PassArguments()
        {
            string invocationString = CompileExecutable(@"class Program { static void Main(string[] args) {
                System.Console.WriteLine(args[0]);
                System.Console.WriteLine(args[1]);
            } }") + " passed argument";
            Assert.That(Shell.Execute(invocationString).StdOut, Is.EqualTo($"passed{Environment.NewLine}argument"));
        }

        [Test]
        public void Execute_StressTest_WithParallelExecutionsAndOutput()
        {
            const int numParallelExecutions = 8;

            string baseInvocationString = CompileExecutable(@"class Program { static void Main(string[] args) {
                for(int i=0; i<int.Parse(args[0]); ++i) {
                System.Console.WriteLine(""stdout"");
                System.Console.Error.WriteLine(""stderr"");
            } } }");

            var threads = new Thread[numParallelExecutions];
            var results = new Shell.ExecuteResult[numParallelExecutions];
            for (int i = 0; i < numParallelExecutions; ++i)
            {
                var threadIdx = i;
                threads[i] = new Thread(() => results[threadIdx] = Shell.Execute($"{baseInvocationString} {threadIdx+1}"));
            }

            foreach (var thread in threads)
                thread.Start();
            foreach (var thread in threads)
                thread.Join();

            Assert.That(results.Select(x => x.ExitCode), Is.All.Zero);
            for (int i = 0; i < numParallelExecutions; ++i)
            {
                Assert.That(results[i].StdOut, Is.EqualTo(Enumerable.Repeat("stdout", i + 1).SeparateWith(Environment.NewLine)));
                Assert.That(results[i].StdErr, Is.EqualTo(Enumerable.Repeat("stderr", i + 1).SeparateWith(Environment.NewLine)));
            }
        }
    }
}
