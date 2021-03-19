using System;
using System.IO;
using System.Reflection;
using AssemblyPatcher.Configuration;
using Mono.Cecil;
using log4net;
using log4net.Appender;
using log4net.Config;
using log4net.Layout;
using log4net.Repository.Hierarchy;

namespace AssemblyPatcher
{
    internal class Program
    {
        internal static int Main(string[] args)
        {
            var options = ParseCommandLine(args);
            if (options == null) return -1;

            InitializeLog(options);

            DumpCommandLine(options);

            if (!ValidateOptions(options))
            {
                return -2;
            }

            TargetPlatform platform;
            switch (options.Platform)
            {
                case "WSA":
                    platform = TargetPlatform.WSA;
                    break;
                case "WP8":
                    platform = TargetPlatform.WP8;
                    break;
                case "":
                case "General":
                    platform = TargetPlatform.General;
                    break;
                default:
                    throw new Exception("Unknown platform: " + options.Platform);
            }

            var patcher = new AssemblyPatcher(new Parser().Parse(options.ConfigFile), options.SearchPath, platform);

            try
            {
                SetupLogging(patcher, options);
                patcher.Patch(options.Assembly, options.Output, options.KeyFilePath);
            }
            catch (Exception ex)
            {
                log.Error(String.Format("Error patching {0}.\r\nPatched assembly was not saved.", options.Assembly), ex);
                return -2;
            }

            return 0;
        }

        private static bool ValidateOptions(CommandLineSpec options)
        {
            if (!File.Exists(options.ConfigFile))
            {
                log.ErrorFormat("Configuration file {0} not found.", options.ConfigFile);
                return false;
            }

            return true;
        }

        private static void SetupLogging(AssemblyPatcher patcher, CommandLineSpec options)
        {
            patcher.BaseTypePatchingEvent += (sender, args) => LogDebug(log, args.PatchRule, "[{0}] Base type changed to {1}", args.PatchedItem.FullName, args.Item.FullName);
            patcher.CatchTypePatchingEvent += (sender, args) => LogDebug(log, args.PatchRule, "[{0}] Type of catch statement changed from {1} to {2}", args.Method.FullName, args.Item.CatchType.FullName, args.NewType.FullName);
            patcher.FieldReferencePatching += (sender, args) => LogDebug(log, args.PatchRule, "[{0}] Field reference changed from {1} to {2}", args.PatchedItem.FullName, args.Instruction.Operand, args.Item.FullName);
            patcher.FieldTypePatchingEvent  += (sender, args) => LogDebug(log, args.PatchRule, "[{0}] Field type changed to {1}", args.PatchedItem.FullName, args.Item.FullName);
            patcher.PropertyTypePatchingEvent  += (sender, args) => LogDebug(log, args.PatchRule, "[{0}] Property type changed to {1}", args.PatchedItem.FullName, args.Item.FullName);
            patcher.LocalVariableTypePatching  += (sender, args) => LogDebug(log, args.PatchRule, "[{0}] Local variable type changed from {1} to {2}", args.DeclaringMethod.FullName, AssemblyPatcher.GetVariableName(args.Item, args.DeclaringMethod), args.NewType.FullName);
            patcher.MethodReturnTypePatching  += (sender, args) => LogDebug(log, args.PatchRule, "[{0}] Method return type changed to {1}", args.PatchedItem.FullName, MethodReference(args.PatchedItem).ReturnType.FullName, args.Item.FullName);
            patcher.ParameterTypePatching  += (sender, args) => LogDebug(log, args.PatchRule, "[{0}] Parameter {1} type changed from {2} to {3}", args.Method.FullName, args.Item.Name, args.Item.ParameterType.FullName, args.NewType.FullName);
            patcher.TypeReferencePatchingEvent += (sender, args) => LogDebug(log, args.PatchRule, "[{0}] Type changed to {1}", args.PatchedItem.FullName, args.Item.FullName);
            patcher.MethodReferencePatching += (sender, args) =>
            {
                if (args.PatchRule.Operation == OperationKind.Throw)
                {
                    LogWarning(log, args.PatchRule, "[{0}] Method reference changed from {1} to {2}", args.PatchedItem.FullName, args.Instruction.Operand, args.Item.FullName);
                }
                else
                {
                    LogDebug(log, args.PatchRule, "[{0}] Method reference changed from {1} to {2}", args.PatchedItem.FullName, args.Instruction.Operand, args.Item.FullName);
                }
            };
        }

        private static void InitializeLog(CommandLineSpec options)
        {
            var fileName = options.LogConfig ?? Path.Combine(Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location), "AssemblyPatcher.Log.Config.xml");

            if (!File.Exists(fileName))
            {
                Console.WriteLine("Could not find log configuration file: {0}", fileName);
                Environment.Exit(-1);
            }

            XmlConfigurator.Configure(new FileInfo(fileName));

            var logger = (Logger)log.Logger;
            if (options.LogLevel != null)
            {
                logger.Level = logger.Hierarchy.LevelMap[options.LogLevel];
            }

            if (options.LogFile != null)
            {
                var layout = new PatternLayout("%date [%thread] %-5level %logger - %message%newline");
                layout.ActivateOptions();

                var fileAppender = new FileAppender
                {
                    AppendToFile = true,
                    File = options.LogFile,
                    ImmediateFlush = true,
                    Layout = layout
                };

                fileAppender.ActivateOptions();
                logger.AddAppender(fileAppender);
            }
        }

        private static MethodReference MethodReference(MemberReference memberReference)
        {
            return ((MethodReference)memberReference);
        }

        private static void LogDebug(ILog log, PatchRule patchRule, string msg, params object[] args)
        {
            log.DebugFormat(msg, args);
            log.DebugFormat("\tpatch rule: {0}", patchRule);
        }

        private static void LogWarning(ILog log, PatchRule patchRule, string msg, params object[] args)
        {
            log.WarnFormat(msg, args);
            log.WarnFormat("\tpatch rule: {0}", patchRule);
        }

        private static CommandLineSpec ParseCommandLine(string[] args)
        {
            var parser = new CommandLine.Parser(p => p.HelpWriter = Console.Out);

            var options = new CommandLineSpec();
            if (parser.ParseArguments(args, options))
            {
                return options;
            }
            return null;
        }

        private static void DumpCommandLine(CommandLineSpec options)
        {
            Console.WriteLine("Assembly to patch: {0}", options.Assembly);
            Console.WriteLine("Configuration file: {0}", options.ConfigFile);
            Console.WriteLine("Assembly search path : {0}", options.SearchPath);
        }

        private static ILog log = LogManager.GetLogger(typeof(AssemblyPatcher));
    }
}
