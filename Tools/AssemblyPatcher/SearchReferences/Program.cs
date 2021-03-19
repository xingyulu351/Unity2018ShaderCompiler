using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using Mono.Cecil;
using Mono.Cecil.Cil;
using Mono.Collections.Generic;

namespace SearchReferences
{
    static class Program
    {
        private static ISet<MethodDefinition> visitedTypes = new HashSet<MethodDefinition>();
        private static Stack<MethodDefinition> callGraph = new Stack<MethodDefinition>();

        static void Main(string[] args)
        {
            var parser = new CommandLine.Parser(p => p.HelpWriter = Console.Out);

            var options = new CommandLineSpec();
            if (!parser.ParseArguments(args, options))
            {
                return;
            }

            DumpCommandLine(options);

            if (options.CheckReferences)
            {
                CheckReferences(options, options.AditionalAssembliesFolder);
                return;
            }

            Func<Instruction, bool> instuctionPredicate;
            Func<TypeReference, bool> typePredicate = type =>
            {
                if (type == null) return false;
                var typeDefinition = type.Resolve();
                if (typeDefinition == null)
                {
                    Console.WriteLine("Cannot resolve type {0}", type.FullName);
                    return false;
                }
                return typeDefinition.Module.Assembly.FullName.Contains(options.Search);
            };

            if (options.AssemblyDependency)
            {
                instuctionPredicate = inst => inst.Operand.ToString().Contains(options.Search);
            }
            else
            {
                instuctionPredicate = inst => inst.Operand.ToString().Contains(options.Search);
            }

            var assemblyResolver = new DefaultAssemblyResolver();
            assemblyResolver.AddSearchDirectory(Path.GetDirectoryName(options.Assembly));

            var parameters = new ReaderParameters
            {
                AssemblyResolver = assemblyResolver
            };

            var assembly = AssemblyDefinition.ReadAssembly(options.Assembly, parameters);
            foreach (var type in assembly.MainModule.Types)
            {
                SearchInTypeHierarchy(type, typePredicate);
                SearchInMembers(type, options.Search, instuctionPredicate);
            }
        }

        private static void SearchInTypeHierarchy(TypeDefinition type, Func<TypeReference, bool> predicate)
        {
            if (type.BaseType != null)
            {
                if (predicate(type.BaseType))
                {
                    Console.WriteLine("--------> {0} : {1}", type.FullName, type.BaseType.FullName);
                }
            }

            foreach (var itf in type.Interfaces)
            {
                if (predicate(itf.InterfaceType))
                {
                    Console.WriteLine("interface --------> {0} : {1}", type.FullName, itf.InterfaceType.FullName);
                }
            }

            foreach (var field in type.Fields)
            {
                CheckCustomAttribute(field.CustomAttributes, field.FullName, predicate);
                if (predicate(field.FieldType))
                {
                    Console.WriteLine("--------> field {0} : {1}", field.FullName, field.FieldType.FullName);
                }
            }

            foreach (var methodDefinition in type.Methods)
            {
                CheckCustomAttribute(methodDefinition.CustomAttributes, methodDefinition.FullName, predicate);
                if (predicate(methodDefinition.ReturnType))
                {
                    Console.WriteLine("--------> return type {0} : {1}", methodDefinition.FullName, methodDefinition.ReturnType.FullName);
                }

                foreach (var parameter in methodDefinition.Parameters)
                {
                    CheckCustomAttribute(parameter.CustomAttributes, methodDefinition.FullName + " -> " + parameter.Name, predicate);
                    if (predicate(parameter.ParameterType))
                    {
                        Console.WriteLine("--------> parameter type {0} {1} : {2}", methodDefinition.FullName, parameter.Name, parameter.ParameterType.FullName);
                    }
                }

                foreach (var genericParameter in methodDefinition.GenericParameters)
                {
                    Console.WriteLine("----> GEN PARAN IGNORED: {0} {1}", genericParameter.FullName, methodDefinition.FullName);
                    foreach (var constraint in genericParameter.Constraints)
                    {
                        Console.WriteLine("----> GEN PARAN CONSTRAINT IGNORED: {0} {1} {2}", genericParameter.FullName, methodDefinition.FullName, constraint.FullName);
                    }
                }

                CheckCustomAttribute(type.CustomAttributes, type.FullName, predicate);
            }

            foreach (var @event in type.Events)
            {
                if (predicate(@event.EventType))
                {
                    Console.WriteLine("----> EVENT {0} : {1}", @event.FullName, @event.EventType.FullName);
                }
            }

            foreach (var genericParameter in type.GenericParameters)
            {
                Console.WriteLine("----> TYPE GEN PARAN IGNORED: {0} {1}", genericParameter.FullName, type.FullName);
                foreach (var constraint in genericParameter.Constraints)
                {
                    Console.WriteLine("----> TYPE GEN PARAN CONSTRAINT IGNORED: {0} {1} {2}", genericParameter.FullName, type.FullName, constraint.FullName);
                }
            }
        }

        private static void CheckCustomAttribute(Collection<CustomAttribute> customAttributes, string fullName, Func<TypeReference, bool> predicate)
        {
            foreach (var customAttribute in customAttributes)
            {
                if (predicate(customAttribute.AttributeType))
                {
                    Console.WriteLine("CUSTOM ATTR: {0} in {1}", customAttribute.AttributeType.FullName, fullName);
                }
            }
        }

        private static void DumpCommandLine(CommandLineSpec options)
        {
            if (options.Verbose == 0) return;

            Console.WriteLine("Source Assembly     : {0}", options.Assembly);
            Console.WriteLine();

            Console.WriteLine("Target Framework    : {0}", options.TargetFrameworkFolder);
            Console.WriteLine();

            if (options.AditionalAssembliesFolder != null)
            {
                Console.WriteLine("Aditional Assemblies: {0}", string.Join("\r\n                      ", Directory.GetFiles(options.AditionalAssembliesFolder, "*.dll")));
                Console.WriteLine();
            }
        }

        private static void CheckReferences(CommandLineSpec options, string aditionalAssembliesFolder)
        {
            var output = new List<string>();
            int count = 0;
            var offenderMap = new APIChecker(options.TargetFrameworkFolder, aditionalAssembliesFolder).Check(options.Assembly);

            var offendersByType = offenderMap.Keys.GroupBy(mr => mr is TypeReference, (is_type, enumerable) => new { Name = is_type ? "Types" : "Members", Items = enumerable });

            foreach (var offenderByType in offendersByType)
            {
                Console.WriteLine(offenderByType.Name);
                Console.WriteLine("----------------------------------");

                foreach (var offender in offenderByType.Items)
                {
                    Console.WriteLine(offender.FullName);

                    if (options.Verbose == 0)
                    {
                        continue;
                    }

                    var offendeeList = offenderMap[offender];

                    foreach (var offendee in offendeeList)
                    {
                        Console.WriteLine("\t{0}", offendee());
                    }
                    Console.WriteLine();
                }

                Console.WriteLine();
                output.Add(string.Format("{0} missing {1}", offenderByType.Items.Count(), offenderByType.Name));
                count += offenderByType.Items.Count();
            }
            Console.WriteLine();
            Console.WriteLine(string.Join(" and ", output.ToArray()));
        }

        private static void SearchInMembers(TypeDefinition type, string tbf, Func<Instruction, bool> predicate)
        {
            foreach (var method in type.Methods)
            {
                SearchInMethod(method, tbf, predicate);
            }
        }

        private static void SearchInMethod(MethodDefinition method, string tbf, Func<Instruction, bool> predicate)
        {
            if (!method.HasBody)
                return;

            if (visitedTypes.Contains(method)) return;

            visitedTypes.Add(method);
            callGraph.Push(method);

            foreach (var instruction in method.Body.Instructions)
            {
                if (instruction.Operand == null)
                    continue;

                if (predicate(instruction))
                {
                    Console.WriteLine("{0}", instruction.Operand);
                    int n = 0;

                    Action<MethodDefinition> a = m => Console.WriteLine("{0:X2}{1}{2}", instruction.Offset, new String(' ', n), m);

                    var methods = callGraph.Reverse();
                    foreach (var m in methods.Take(1))
                    {
                        a(m);
                        n += 2;
                    }

                    using (new WithConsoleColors(ConsoleColor.White, ConsoleColor.DarkMagenta))
                    {
                        a(methods.Last());
                    }

                    Console.WriteLine();
                }
                else if (instruction.Operand is MethodReference)
                {
                    var methodDecl = (instruction.Operand as MethodReference).Resolve();
                    SearchInMethod(methodDecl, tbf, inst => inst.Operand.ToString().Contains(tbf));
                }
            }

            callGraph.Pop();
        }
    }
}
