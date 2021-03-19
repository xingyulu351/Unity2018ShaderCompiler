using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using Mono.Cecil;
using Mono.Cecil.Cil;

namespace SearchReferences
{
    public class APIChecker
    {
        private readonly string aditionalAssembliesFolder;
        private readonly string referencedAssembliesSourceFolder;

        public APIChecker(string referencedAssembliesSourceFolder, string aditionalAssembliesFolder)
        {
            this.aditionalAssembliesFolder = aditionalAssembliesFolder;
            this.referencedAssembliesSourceFolder = referencedAssembliesSourceFolder.Replace("\"", "");
        }

        public IDictionary<MemberReference, IList<Func<string>>> Check(string sourceAssemblyPath)
        {
            try
            {
                var assemblyResolver = new DefaultAssemblyResolver();
                assemblyResolver.AddSearchDirectory(Path.GetDirectoryName(sourceAssemblyPath));
                assemblyResolver.AddSearchDirectory(referencedAssembliesSourceFolder);
                if (aditionalAssembliesFolder != null)
                    assemblyResolver.AddSearchDirectory(aditionalAssembliesFolder);

                var parameters = new ReaderParameters
                {
                    AssemblyResolver = assemblyResolver,
                };

                var assemblyToCheck = AssemblyDefinition.ReadAssembly(sourceAssemblyPath, parameters);

                ReadSymbolsFor(assemblyToCheck);

                CheckTypes(assemblyToCheck, assemblyToCheck.MainModule.Types);
            }
            catch (Exception e)
            {
                using (new WithConsoleColors(ConsoleColor.White, ConsoleColor.Red))
                {
                    Console.WriteLine(e.ToString());
                }
            }

            return offendersMap;
        }

        private void CheckTypes(AssemblyDefinition assemblyToCheck, IEnumerable<TypeDefinition> types)
        {
            foreach (var type in types)
            {
                CheckInheritedTypes(type, assemblyToCheck);
                CheckMethods(type, assemblyToCheck);
                CheckFields(assemblyToCheck, type);
                //CheckEvents(assemblyToCheck, type);

                if (type.HasNestedTypes)
                    CheckTypes(assemblyToCheck, type.NestedTypes);
            }
        }

        private static void ReadSymbolsFor(AssemblyDefinition assemblyToCheck)
        {
            try
            {
                assemblyToCheck.MainModule.ReadSymbols();
            }
            catch (FileNotFoundException)
            {
            }
        }

        private void CheckFields(AssemblyDefinition assemblyToCheck, TypeDefinition type)
        {
            foreach (var field in type.Fields)
            {
                var localField = field;
                CheckType(field.FieldType, assemblyToCheck, definition => ReportOffender(localField.FieldType, () => string.Format("[field] {0} {1}", type.FullName, localField.Name)));
            }
        }

        private void CheckMethods(TypeDefinition type, AssemblyDefinition assemblyToCheck)
        {
            foreach (var method in type.Methods)
            {
                CheckMethod(method, assemblyToCheck);
            }
        }

        private void CheckMethod(MethodDefinition method, AssemblyDefinition assemblyToCheck)
        {
            CheckType(
                method.ReturnType,
                assemblyToCheck,
                targetAssembly => ReportOffender(method.ReturnType, () => string.Format("[return] ({0}) {1}", method.DeclaringType.FullName, method.FullName)));

            foreach (var parameter in method.Parameters)
            {
                var localParameter = parameter;
                CheckType(
                    parameter.ParameterType,
                    assemblyToCheck,
                    targetAssembly => ReportOffender(localParameter.ParameterType, () => string.Format("[param] ({0}) {1} : {2}", method.DeclaringType.FullName, method.FullName, localParameter.Name)));
            }

            if (method.HasGenericParameters)
            {
                foreach (var genericParameter in method.GenericParameters)
                {
                    //CheckType(genericParameter.Type, targetAssembly => Console.WriteLine("[param] {0} ({1}) : {2}", parameter.Name, method.FullName, parameter.ParameterType.FullName));
                    //TODO: what to do?
                }
            }

            if (!method.HasBody) return;

            foreach (var instruction in method.Body.Instructions)
            {
                CheckInstruction(method, instruction, assemblyToCheck);
            }

            foreach (var handler in method.Body.ExceptionHandlers)
            {
                var localHandler = handler;
                CheckType(handler.CatchType, assemblyToCheck, assembly => ReportOffender(localHandler.CatchType, () => string.Format("[catch] {0} : {1}", method.FullName, localHandler.CatchType.FullName)));
            }
        }

        private void CheckInstruction(MethodDefinition method, Instruction instruction, AssemblyDefinition assemblyToCheck)
        {
            var typeReference = instruction.Operand as TypeReference;
            if (typeReference != null)
            {
                CheckType(typeReference.Resolve(), assemblyToCheck, ta => ReportOffender(method, () => string.Format("[reference] {0} (@ {1})", instruction, DebugInfo(instruction, method))));
                return;
            }

            var memberReference = instruction.Operand as MemberReference;
            if (memberReference == null) return;

            CheckMemberReference(method, instruction, memberReference, assemblyToCheck);
        }

        private void CheckMemberReference(MethodDefinition method, Instruction referencee, MemberReference memberReference, AssemblyDefinition assemblyToCheck)
        {
            if (memberReference == null) return;

            var typeDefinition = memberReference.DeclaringType.Resolve();
            if (typeDefinition == null)
            {
                CheckType(memberReference.DeclaringType, assemblyToCheck, a => ReportOffender(memberReference.DeclaringType, () => method.DeclaringType.Name + "." + method.FullName + " -> " + memberReference.FullName));
                return;
            }

            var declaringAssembly = typeDefinition.Module.Assembly;

            var comparisonAssembly = GetAssemblyDefinition(declaringAssembly.FullName, declaringAssembly.Name.Name);
            if (comparisonAssembly == null) return;

            if (CheckMethodReference(method, referencee, memberReference, comparisonAssembly)) return;

            CheckFieldReference(method, referencee, memberReference, comparisonAssembly);
        }

        private void CheckFieldReference(MethodDefinition method, Instruction referencee, MemberReference memberReference, AssemblyDefinition comparisonAssembly)
        {
            var fieldReference = referencee.Operand as FieldReference;
            if (fieldReference == null) return;

            var found = comparisonAssembly.Modules
                .SelectMany(m => m.Types, (moduleDef, typeDef) => typeDef.Resolve())
                .SingleOrDefault(t => t.Fields.SingleOrDefault(f => f.Name == fieldReference.Name && f.FieldType.FullName == fieldReference.FieldType.FullName) != null);

            if (found != null) return;

            found = comparisonAssembly.Modules
                .SelectMany(m => m.ExportedTypes, (moduleDef, typeDef) => typeDef.Resolve())
                .SingleOrDefault(t => t.Fields.SingleOrDefault(f => f.Name == fieldReference.Name && f.FieldType.FullName == fieldReference.FieldType.FullName) != null);

            if (found != null) return;

            ReportOffender(memberReference,
                () => string.Format("{0} [{1}] {2}", method.FullName, referencee.OpCode, DebugInfo(referencee, method)));
        }

        private bool CheckMethodReference(MethodDefinition method, Instruction referencee, MemberReference memberReference, AssemblyDefinition comparisonAssembly)
        {
            var methodRef = memberReference as MethodReference;
            if (methodRef == null) return false;

            var found = FindMethodDefinition(
                comparisonAssembly.Modules.SelectMany(m => m.ExportedTypes, (moduleDef, typeDef) => typeDef.Resolve()),
                memberReference,
                comparisonAssembly);

            if (found != null)
                return true;

            found = FindMethodDefinition(
                comparisonAssembly.Modules.SelectMany(m => m.Types, (moduleDef, typeDef) => typeDef.Resolve()),
                memberReference,
                comparisonAssembly);

            if (found != null)
                return true;

            ReportOffender(memberReference,
                () => string.Format("{0} [{1}] {2}", method.FullName, referencee.OpCode, DebugInfo(referencee, method)));

            return true;
        }

        private static MethodDefinition FindMethodDefinition(IEnumerable<TypeDefinition> types, MemberReference memberReference, AssemblyDefinition comparisonAssembly)
        {
            if (!types.Any()) return null;

            var toBeFound = memberReference as MethodReference;
            if (toBeFound == null)
            {
                throw new Exception("oppss.. : " + memberReference.FullName);
            }

            var pending = new List<TypeDefinition>();

            foreach (var type in types)
            {
                var found = type.Methods.Where(mm => Predicate(mm, toBeFound));
                if (found.Count() == 1) return found.Single();

                if (found.Count() > 1)
                {
                    Console.WriteLine("Found multiple candidates for: {0}", memberReference.FullName);
                    foreach (var definition in found)
                    {
                        Console.WriteLine("\t{0}", definition.FullName);
                    }
                }

                if (type.HasNestedTypes)
                {
                    pending.AddRange(type.NestedTypes);
                }
            }

            return FindMethodDefinition(pending, memberReference, comparisonAssembly);
        }

        private void ReportOffender(MemberReference memberReference, Func<string> offenderInfoExtractor)
        {
            IList<Func<string>> offenders;
            if (!offendersMap.TryGetValue(memberReference, out offenders))
            {
                offenders = new List<Func<string>>();
                offendersMap[memberReference] = offenders;
            }

            offenders.Add(offenderInfoExtractor);
        }

        private static bool Predicate(MethodDefinition candidate, MethodReference memberReference)
        {
            var nameAndTDeclaringTypeMatches = candidate.Name == memberReference.Name && candidate.DeclaringType.Name == memberReference.DeclaringType.Name;
            return nameAndTDeclaringTypeMatches && memberReference.HasCompatibleParametersWith(candidate);
        }

        private void CheckInheritedTypes(TypeDefinition type, AssemblyDefinition assemblyToCheck)
        {
            CheckType(
                type.BaseType,
                assemblyToCheck,
                targetAssembly => ReportOffender(type.BaseType, () => string.Format("[base] {0}", type.FullName)));

            CheckInterfaces(type, assemblyToCheck);
        }

        private void CheckInterfaces(TypeDefinition type, AssemblyDefinition assemblyToCheck)
        {
            foreach (var itf in type.Interfaces)
            {
                CheckType(
                    itf.InterfaceType,
                    assemblyToCheck,
                    targetAssembly => ReportOffender(itf.InterfaceType, () => string.Format("[interface] {0}", type.FullName)));
            }
        }

        private void CheckType(TypeReference typeToCheck, AssemblyDefinition assemblyToCheck, Action<AssemblyDefinition> action)
        {
            if (typeToCheck == null)
                return;

            CheckGenericTypeParameters(typeToCheck, assemblyToCheck, action);

            typeToCheck = typeToCheck.UnWrap();

            if (IsDefinedInAssembly(typeToCheck, assemblyToCheck))
            {
                return;
            }

            AssemblyNameReference assemblyRef = null;

            var moduleDefinition = typeToCheck.Scope as ModuleDefinition;
            assemblyRef = moduleDefinition != null
                ? moduleDefinition.Assembly.Name
                : typeToCheck.Scope as AssemblyNameReference;

            var targetAssembly = GetAssemblyDefinition(assemblyRef.FullName, assemblyRef.Name);

            if (targetAssembly == null) return;

            if (targetAssembly.MainModule.ExportedTypes.Any(et => et.FullName == typeToCheck.FullName))
                return;

            if (targetAssembly.MainModule.Types.Any(et => et.FullName == typeToCheck.FullName))
                return;


            if (typeToCheck.Resolve() != null)
                return;

            action(targetAssembly);
        }

        private static Type SystemTypeFor(TypeReference typeToCheck)
        {
            var moduleDefinition = typeToCheck.Scope as ModuleDefinition;
            if (moduleDefinition != null)
            {
                var systemTypeFor = Type.GetType(typeToCheck.FullName + "," + moduleDefinition.Assembly.Name, false, true);
                if (systemTypeFor == null)
                {
                    Console.WriteLine();
                }
                return systemTypeFor;
            }

            return Type.GetType(typeToCheck.FullName + ", " + typeToCheck.Scope);
        }

        private void CheckGenericTypeParameters(TypeReference typeToCheck, AssemblyDefinition assemblyToCheck, Action<AssemblyDefinition> action)
        {
            var genericType = typeToCheck as GenericInstanceType;
            if (genericType == null) return;

            foreach (var genericArgument in genericType.GenericArguments)
            {
                if (genericArgument.IsGenericParameter) continue;

                CheckType(genericArgument, assemblyToCheck, action);
                //Console.WriteLine("---> GEN ARG: {0} @ {1}", genericArgument, typeToCheck);
            }
        }

        private static bool IsDefinedInAssembly(TypeReference typeToCheck, AssemblyDefinition assemblyToCheck)
        {
            return typeToCheck.Scope.Name == assemblyToCheck.Name.Name;
        }

        private AssemblyDefinition GetAssemblyDefinition(Assembly assembly)
        {
            var fullName = assembly.FullName;
            var simpleName = assembly.GetName().Name;

            return GetAssemblyDefinition(fullName, simpleName);
        }

        private AssemblyDefinition GetAssemblyDefinition(string fullName, string simpleName)
        {
            if (loadedAssemblies.ContainsKey(fullName))
            {
                return loadedAssemblies[fullName];
            }

            var targetAssemblyPath = Path.Combine(referencedAssembliesSourceFolder, simpleName + ".dll");
            if (!File.Exists(targetAssemblyPath))
            {
                //Console.WriteLine("\tAssembly not found: {0}", targetAssemblyPath);
                return null;
            }

            var assemblyDefinition = AssemblyDefinition.ReadAssembly(targetAssemblyPath);
            loadedAssemblies[assemblyDefinition.FullName] = assemblyDefinition;

            return assemblyDefinition;
        }

        private string DebugInfo(Instruction insn, MethodDefinition method)
        {
            var seqPoint = method.DebugInformation.GetSequencePoint(insn);
            while (seqPoint == null && insn.Previous != null)
            {
                insn = insn.Previous;
                seqPoint = method.DebugInformation.GetSequencePoint(insn);
            }

            if (seqPoint == null) return "na";

            return string.Format("({0}:{1},{2})", seqPoint.Document.Url, seqPoint.StartLine, seqPoint.StartColumn);
        }

        private IDictionary<string, AssemblyDefinition> loadedAssemblies = new Dictionary<string, AssemblyDefinition>();
        private IDictionary<MemberReference, IList<Func<string>>> offendersMap = new Dictionary<MemberReference, IList<Func<string>>>();
    }

    static class CecilExtensions
    {
        public static bool HasCompatibleParametersWith(this MethodReference self, MethodReference other)
        {
            //TODO: Take into account type patching / mapping.
            var sourceParams = self.Parameters;
            var otherParams = other.Parameters;

            if (sourceParams.Count != otherParams.Count)
                return false;

            for (int i = 0; i < sourceParams.Count; i++)
            {
                var sourceParam = sourceParams[i].ParameterType.UnWrap() as GenericParameter;
                var otherParam = otherParams[i].ParameterType.UnWrap() as GenericParameter;
                if (sourceParam != null && otherParam != null)
                {
                    //Console.WriteLine("Assuming generic methods are a match: \r\n\t{0}\r\n\t{1}", self.FullName, other.FullName);
                    return sourceParam.Position == otherParam.Position;
                }

                if (sourceParams[i].ParameterType.FullName != otherParams[i].ParameterType.FullName)
                    return false;
            }

            return true;
        }

        public static bool HasCompatibleSignatureWithUnsupportedMethod(this MethodDefinition candidate, MethodReference original)
        {
            if (candidate.ReturnType.FullName != original.ReturnType.FullName)
                return false;

            var originalResolved = original.Resolve();
            Func<int, int> fixStaticMethodOffset = index => index + (originalResolved.IsStatic ? 0 : 1);

            var originalParamCount = ActualParameterCount(original);
            if (candidate.Parameters.Count != originalParamCount + 1)
                return false;

            for (int i = 0; i < original.Parameters.Count; i++)
            {
                if (candidate.Parameters[fixStaticMethodOffset(i)].ParameterType.FullName != original.Parameters[i].ParameterType.FullName)
                {
                    return false;
                }
            }

            if (!originalResolved.IsStatic && candidate.Parameters[0].ParameterType.FullName != original.DeclaringType.FullName)
            {
                return false;
            }

            // Check the final string param.
            for (int i = originalParamCount; i < candidate.Parameters.Count; i++)
            {
                if (candidate.Parameters[i].ParameterType.FullName != candidate.Module.TypeSystem.String.FullName) return false;
            }

            return true;
        }

        public static IEnumerable<MethodDefinition> GetAllMethods(this TypeDefinition self)
        {
            var ret = self.Methods.AsEnumerable();

            return self.BaseType == null
                ? ret
                : ret.Concat(GetAllMethods(self.BaseType.Resolve()));
        }

        public static TypeReference MakeGenericType(this TypeReference self, params TypeReference[] arguments)
        {
            if (self.GenericParameters.Count != arguments.Length)
                throw new ArgumentException();

            var instance = new GenericInstanceType(self);
            foreach (var argument in arguments)
                instance.GenericArguments.Add(argument);

            return instance;
        }

        public static MethodReference MakeGeneric(this MethodReference self, params TypeReference[] arguments)
        {
            var reference = new MethodReference(self.Name, self.ReturnType)
            {
                DeclaringType = self.DeclaringType.MakeGenericType(arguments),
                HasThis = self.HasThis,
                ExplicitThis = self.ExplicitThis,
                CallingConvention = self.CallingConvention,
            };

            foreach (var parameter in self.Parameters)
                reference.Parameters.Add(new ParameterDefinition(parameter.ParameterType));

            foreach (var generic_parameter in self.GenericParameters)
                reference.GenericParameters.Add(new GenericParameter(generic_parameter.Name, reference));

            return reference;
        }

        public static TypeReference UnWrap(this TypeReference type)
        {
            var typeWithElement = type as TypeSpecification;
            if (typeWithElement != null) return typeWithElement.ElementType;

            return type;
        }

        private static int ActualParameterCount(MethodReference original)
        {
            return original.Parameters.Count + (original.Resolve().IsStatic ? 0 : 1);
        }
    }
}
