using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.Versioning;
using System.Text;
using AssemblyPatcher.Events;
using AssemblyPatcher.Extensions;
using AssemblyPatcher.Configuration;
using Mono.Cecil;
using Mono.Cecil.Cil;
using Mono.Cecil.Rocks;
using MethodAttributes = Mono.Cecil.MethodAttributes;
using ParameterAttributes = Mono.Cecil.ParameterAttributes;

namespace AssemblyPatcher
{
    internal class AssemblyPatcher
    {
        public AssemblyPatcher(IEnumerable<PatchRule> rules, string searchPath, TargetPlatform platform)
        {
            this.searchPath = searchPath;
            patchRules = rules;
            this.platform = platform;
        }

        public string Patch(string assemblyPath, string output, string keyFilePath = null)
        {
            var assembly = AssemblyOperations.ReadAssemblyToBePatched(assemblyPath, searchPath);

            FixTargetFrameworkVersion(assembly);

            RemoveStrongName(assembly, keyFilePath);

            //PatchAttributes(assembly);
            PatchTypes(assembly.MainModule.Types);

            FixAssemblyReferences(assembly);

            return AssemblyOperations.SavePatchedAssembly(assemblyPath, output, assembly, keyFilePath, platform);
        }

        private void RemoveStrongName(AssemblyDefinition assembly, string keyFilePath)
        {
            if (assembly.Name.HasPublicKey && string.IsNullOrEmpty(keyFilePath))
            {
                assembly.Name.PublicKey = new byte[0];
                assembly.MainModule.Attributes = ModuleAttributes.ILOnly;
            }
        }

        private void PatchTypes(IEnumerable<TypeDefinition> toBePatched)
        {
            foreach (var typeDefinition in toBePatched)
            {
                PatchType(typeDefinition);
            }
        }

        private void PatchType(TypeDefinition typeDefinition)
        {
            if (typeDefinition.HasNestedTypes)
                PatchTypes(typeDefinition.NestedTypes);

            PatchBaseType(typeDefinition);
            PatchMembers(typeDefinition);
            //PatchAttributes(typeDefinition);
        }

        private void PatchBaseType(TypeDefinition toBePatched)
        {
            if (toBePatched.BaseType == null) return;

            var patchRule = PatchRuleFor(toBePatched.BaseType);
            if (patchRule == null) return;

            var targetAssemblyRef = ReferencedAssembly(patchRule.DeclaringAssembly, toBePatched.Module);

            var newBase = string.IsNullOrWhiteSpace(patchRule.TargetMemberFullyQualifiedName)
                ? RetargetTypeReference(toBePatched.BaseType, toBePatched.Module, targetAssemblyRef)
                : toBePatched.Module.ImportReference(ResolveTypeDefinition(patchRule.TargetMemberFullyQualifiedName, patchRule, toBePatched.Module));

            RaiseBaseTypePatching(toBePatched, newBase, patchRule);
            toBePatched.BaseType = newBase;
        }

        private void PatchMembers(TypeDefinition typeDefinition)
        {
            foreach (var field in typeDefinition.Fields)
            {
                PatchFieldDeclaration(field);
            }

            foreach (var method in typeDefinition.Methods)
            {
                PatchMethodDeclaration(method);
            }

            foreach (var @event in typeDefinition.Events)
            {
                PatchEventDeclaration(@event);
            }

            foreach (var property in typeDefinition.Properties)
            {
                PatchProperty(property);
            }
        }

        private void PatchFieldDeclaration(FieldDefinition field)
        {
            var patchRule = PatchRuleFor(field.FieldType);
            if (patchRule == null) return;

            var patchedFieldType = ResolveTypeReference(field.FieldType, patchRule, field.Module);
            RaiseFieldTypePatching(field, patchedFieldType, patchRule);

            field.FieldType = patchedFieldType;
        }

        private void PatchEventDeclaration(EventDefinition @event)
        {
            var patchRule = PatchRuleFor(@event.EventType);
            if (patchRule != null)
            {
                throw new Exception(string.Format("Missed event {0}. Rule applies: {1}", @event.FullName, patchRule));
            }
        }

        private void PatchProperty(PropertyDefinition property)
        {
            var patchRule = PatchRuleFor(property.PropertyType);
            if (patchRule == null) return;

            var patchedPropertyType = ResolveTypeReference(property.PropertyType, patchRule, property.Module);
            RaisePropertyTypePatching(property, patchedPropertyType, patchRule);
            property.PropertyType = patchedPropertyType;
        }

        private void PatchMethodDeclaration(MethodDefinition method)
        {
            PatchBodyRemoval(method);

            PatchSignature(method);

            PatchLocalVariables(method);

            PatchMethodInstructions(method);

            PatchExceptionHandlers(method);
        }

        private void PatchBodyRemoval(MethodDefinition method)
        {
            var patchRule = PatchRuleFor(method);
            if (patchRule == null || patchRule.Operation != OperationKind.RemoveBody) return;

            if (method.MethodReturnType.ReturnType != method.Module.TypeSystem.Void)
            {
                throw new PatchingException(string.Format("Patch rule '{0}' cannot be applied to non void return type method '{1}'",
                    patchRule,
                    method.FullName));
            }

            method.Body.Instructions.Clear();
            method.Body.Instructions.Add(method.Body.GetILProcessor().Create(OpCodes.Ret));
        }

        private void PatchExceptionHandlers(MethodDefinition method)
        {
            if (!method.HasBody || !method.Body.HasExceptionHandlers) return;

            foreach (var handler in method.Body.ExceptionHandlers.Where(candidate => candidate.HandlerType == ExceptionHandlerType.Catch))
            {
                var patchRule = PatchRuleFor(handler.CatchType);
                if (patchRule == null) continue;

                var patchedCatchType = ResolveTypeReference(handler.CatchType, patchRule, method.Module);
                RaiseCatchTypePatching(method, handler, patchedCatchType, patchRule);

                handler.CatchType = patchedCatchType;
            }
        }

        private void PatchMethodInstructions(MethodDefinition method)
        {
            if (!method.HasBody) return;

            method.Body.SimplifyMacros();

            var toBeFixed = new HashSet<Action<ILProcessor>>();
            foreach (var instruction in method.Body.Instructions)
            {
                if (TryPatchMethodReference(method, instruction, toBeFixed)) continue;

                if (TryPatchFieldReference(instruction, method, method.Module)) continue;

                if (TryPatchTypeReference(instruction, method, method.Module, toBeFixed)) continue;
            }

            ApplyMethodBodyFixups(method, toBeFixed);

            method.Body.OptimizeMacros();
        }

        private static void ApplyMethodBodyFixups(MethodDefinition method, IEnumerable<Action<ILProcessor>> toBeFixed)
        {
            var ilProcessor = method.Body.GetILProcessor();
            foreach (var action in toBeFixed)
            {
                action(ilProcessor);
            }
        }

        private bool TryPatchTypeReference(Instruction referencee, MethodDefinition method, ModuleDefinition module, ISet<Action<ILProcessor>> toBeFixed)
        {
            var typeReference = referencee.Operand as TypeReference;
            if (typeReference == null) return false;

            var retargeted = RetargetTypeReference(typeReference, module);
            if (retargeted != typeReference)
            {
                RaiseTypeReferencePatching(method, referencee, module.ImportReference(retargeted));
                toBeFixed.Add(il => referencee.Operand = module.ImportReference(retargeted));
            }

            return true;
        }

        private bool TryPatchMethodReference(MethodDefinition method, Instruction instruction, ISet<Action<ILProcessor>> toBeFixed)
        {
            var methodReference = instruction.Operand as MethodReference;
            if (methodReference == null)
                return false;

            var patchRule = PatchRuleFor(methodReference, method);
            if (patchRule == null || !patchRule.IsApplicableToOperands())
                return true;

            if (patchRule.Operation == OperationKind.Throw && !methodReference.Resolve().IsConstructor)
            {
                toBeFixed.Add(il =>
                {
                    var loadDetailsInst = il.Create(OpCodes.Ldstr, value: DetailsFor(method, instruction));
                    il.InsertBefore(instruction, loadDetailsInst);
                    FixExceptionHandlersBoundaries(method.Body.ExceptionHandlers, instruction, loadDetailsInst);
                });
            }

            var newTarget = ReplacementMethodFor(methodReference, method, patchRule);
            if (newTarget.Resolve().IsStatic != methodReference.Resolve().IsStatic)
            {
                toBeFixed.Add(il => il.Replace(instruction, il.Create(OpCodes.Call, method.Module.ImportReference(newTarget))));
            }
            else
            {
                toBeFixed.Add(il => instruction.Operand = method.Module.ImportReference(newTarget));
            }

            RaiseMethodReferencePatching(method, newTarget, instruction, patchRule);

            return true;
        }

        private void FixExceptionHandlersBoundaries(IEnumerable<ExceptionHandler> exceptionHandlers, Instruction instruction, Instruction newInst)
        {
            foreach (var eh in exceptionHandlers)
            {
                if (eh.HandlerEnd != instruction)
                    continue;

                eh.HandlerEnd = newInst;
                eh.TryEnd.Previous.Operand = newInst; // fix leave instruction.
            }
        }

        private MethodReference ReplacementMethodFor(MethodReference toBeReplaced, MethodDefinition caller, PatchRule patchRule)
        {
            MethodReference newTarget;
            if (patchRule.Operation == OperationKind.Throw)
            {
                newTarget = toBeReplaced.Resolve().IsConstructor
                    ? caller.Module.TypeSystem.Object.Resolve().GetConstructors().Single(candidate => candidate.Parameters.Count == 0)
                    : GlobalThrowingMethodReplacementFor(caller, toBeReplaced);
            }
            else
            {
                var targetTypeDef = ResolveTypeDefinition(toBeReplaced.DeclaringType, patchRule, caller.Module);
                newTarget = ReplacementMethodFromType(targetTypeDef, toBeReplaced, patchRule);
            }

            if (newTarget == null)
            {
                throw new MissingMethodException(string.Format("{0} not found (referenced in {1})", toBeReplaced.FullName,
                    caller.FullName));

                // TODO: Handle it correctly
                // methoddef not found. may be due to:
                // 1. Incorrect patch rule (mispelled type name, method name, etc) -> Throw error stop processing.
                // 2. Missing method in the target on specific platforms -> Either emit warning and inject throwning method or
                //                                                          throw error and stop processing.
            }
            return newTarget;
        }

        private static string DetailsFor(MethodDefinition method, Instruction instruction)
        {
            var methodReference = (MethodReference)instruction.Operand;

            var sequencePoint = method.DebugInformation.GetSequencePoint(instruction);
            while (sequencePoint == null)
            {
                instruction = instruction.Previous;
                if (instruction == null)
                    break;
                sequencePoint = method.DebugInformation.GetSequencePoint(instruction);
            }

            var sb = new StringBuilder(methodReference.FullName);
            if (instruction != null && sequencePoint != null)
            {
                sb.AppendFormat(" called in {0} ({1}, {2}).", sequencePoint.Document.Url, sequencePoint.StartLine, sequencePoint.StartColumn);
            }
            return sb.ToString();
        }

        private bool TryPatchFieldReference(Instruction instruction, MethodDefinition caller, ModuleDefinition module)
        {
            var fieldReference = instruction.Operand as FieldReference;
            if (fieldReference == null) return false;

            return PatchFieldReference(instruction, fieldReference, caller, module);
        }

        private bool PatchFieldReference(Instruction instruction, FieldReference fieldReference, MethodDefinition caller, ModuleDefinition module)
        {
            var patchRule = PatchRuleFor(fieldReference);
            if (patchRule != null)
            {
                if (patchRule.SourceMemberFullyQualifiedName == caller.DeclaringType.FullName) return true;

                var patchedFieldReference = module.ImportReference(ResolveFieldDefinition(fieldReference, patchRule, module));
                RaiseFieldReferencePatching(caller, instruction, patchedFieldReference, patchRule);

                instruction.Operand = patchedFieldReference;
            }
            else
            {
                patchRule = PatchRuleFor(fieldReference.FieldType);
                if (patchRule == null) return true;

                //Field type has changed, reference the new field
                var fld = new FieldReference(fieldReference.Name, ResolveTypeReference(fieldReference.FieldType, patchRule, module), fieldReference.DeclaringType);
                var patchedFieldReference = module.ImportReference(fld);
                RaiseFieldReferencePatching(caller, instruction, patchedFieldReference, patchRule);

                instruction.Operand = patchedFieldReference;
            }
            return true;
        }

        private FieldDefinition ResolveFieldDefinition(FieldReference field, PatchRule patchRule, ModuleDefinition module)
        {
            var typeDef = ResolveTypeDefinition(field.FieldType, patchRule, module);

            var fieldName = (patchRule.Kind == MemberKind.Other) ? SimpleMemberReference.Parse(patchRule.TargetMemberFullyQualifiedName).Name : field.Name;

            return typeDef.Fields.SingleOrDefault(f => f.Name == fieldName);
        }

        private TypeDefinition ResolveTypeDefinition(TypeReference typeReference, ModuleDefinition module)
        {
            var patchRule = PatchRuleFor(typeReference);
            if (patchRule == null) return typeReference.Resolve();

            return ResolveTypeDefinition(typeReference.FullName, patchRule, module);
        }

        private TypeDefinition ResolveTypeDefinition(TypeReference typeReference, PatchRule patchRule, ModuleDefinition module)
        {
            return ResolveTypeReference(typeReference, patchRule, module).Resolve();
        }

        private TypeReference ResolveTypeReference(TypeReference typeReference, PatchRule patchRule, ModuleDefinition module)
        {
            var ret = ResolveTypeDefinition(typeReference.FullName, patchRule, module);
            if (ret != null)
                return module.ImportReference(ret);

            if (patchRule.Kind != MemberKind.Type || patchRule.TargetMemberFullyQualifiedName == null)
                throw new ArgumentException("Cannot resolve type '" + typeReference.FullName + "' as defined in patch rule: " + patchRule, "typeReference");

            return module.ImportReference(Type.GetType(patchRule.TargetMemberFullyQualifiedName));
        }

        private TypeDefinition ResolveTypeDefinition(string typeFQN, PatchRule patchRule, ModuleDefinition module)
        {
            if (patchRule.IsNullTypeReplacement)
            {
                return module.TypeSystem.Object.Resolve();
            }

            var assembly = ReferencedAssembly(patchRule.DeclaringAssembly ?? module.Assembly.FullName, module);
            var assemblyDef = ResolveAssembly(module, assembly);

            var targetTypeFQN = TypeNameFrom(typeFQN, patchRule);
            return assemblyDef.MainModule.Types.SingleOrDefault(t => t.FullName == targetTypeFQN);
        }

        private static AssemblyDefinition ResolveAssembly(ModuleDefinition module, AssemblyNameReference assembly)
        {
            return (assembly == module.Assembly.Name)
                ? module.Assembly
                : module.AssemblyResolver.Resolve(assembly);
        }

        internal static string GetVariableName(VariableDefinition variable, MethodDefinition method)
        {
            if (method.DebugInformation == null)
            {
                return variable.ToString();
            }

            foreach (var scope in method.DebugInformation.GetScopes())
            {
                foreach (var symbol in scope.Variables)
                {
                    if (symbol.Index == variable.Index)
                        return symbol.Name;
                }
            }

            return variable.ToString();
        }

        private void PatchLocalVariables(MethodDefinition method)
        {
            if (!method.HasBody) return;

            foreach (var localVar in method.Body.Variables)
            {
                var patchRule = PatchRuleFor(localVar.VariableType);
                if (patchRule == null)
                    continue;

                if (patchRule.Kind != MemberKind.Type)
                {
                    throw new Exception("Invalid patch rule for variable " +
                        GetVariableName(localVar, method) + " : " + patchRule);
                }

                var patchedVariableType = method.Module.ImportReference(ResolveTypeDefinition(localVar.VariableType, patchRule, method.Module));
                RaiseLocalVariablePatching(method, localVar, patchedVariableType, patchRule);

                localVar.VariableType = patchedVariableType;
            }
        }

        private void PatchSignature(MethodDefinition method)
        {
            foreach (var parameter in method.Parameters)
            {
                var patchRule = PatchRuleFor(parameter.ParameterType);
                if (patchRule == null)
                    continue;

                if (patchRule.Kind != MemberKind.Type)
                {
                    throw new Exception("Invalid patch rule for parameter " + parameter.Name + " : " + patchRule);
                }

                var patchedParameterType = method.Module.ImportReference(ResolveTypeDefinition(parameter.ParameterType.FullName, patchRule, method.Module));
                RaiseParameterTypePatching(method, parameter, patchedParameterType, patchRule);

                parameter.ParameterType = patchedParameterType;
            }

            var patch = PatchRuleFor(method.ReturnType);
            if (patch == null)
                return;

            var patchedReturnType = ResolveTypeDefinition(method.ReturnType.FullName, patch, method.Module);

            RaiseMethodReturnTypePatching(method, patchedReturnType, patch);

            method.ReturnType = method.Module.ImportReference(patchedReturnType);
        }

        private static MethodReference ReplacementMethodFromType(TypeDefinition type, MethodReference toBeReplaced, PatchRule patchRule)
        {
            if (patchRule.Kind == MemberKind.Type)
            {
                //TODO: Handle generics.
                //TODO: Add test for base type method search.
                //TODO: Ensure we can always rely in TargetMemberFQN
                return patchRule.TargetMemberFullyQualifiedName == null
                    ? type.GetAllMethods().SingleOrDefault(m => m.FullName == toBeReplaced.FullName)
                    : type.GetAllMethods().SingleOrDefault(m => m.Name == toBeReplaced.Name && patchRule.TargetMemberFullyQualifiedName ==  m.DeclaringType.FullName && toBeReplaced.HasCompatibleParametersWith(m));
            }

            return type.Methods.SingleOrDefault(m => m.FullName == patchRule.TargetMemberFullyQualifiedName);
        }

        private string TypeNameFrom(string originalDeclaringTypeName, PatchRule patchRule)
        {
            if (patchRule.Kind == MemberKind.Type)
                return patchRule.TargetMemberFullyQualifiedName ?? originalDeclaringTypeName;

            return SimpleMemberReference.Parse(patchRule.TargetMemberFullyQualifiedName).FullName;
        }

        /// <summary>
        /// Remove any TargetFrameworkAttribute so VS will not complain when referencing the assembly from a
        /// WSA app.
        /// </summary>
        private void FixTargetFrameworkVersion(AssemblyDefinition assembly)
        {
            var tfa = assembly.CustomAttributes.SingleOrDefault(ca => ca.AttributeType.FullName == typeof(TargetFrameworkAttribute).FullName);
            if (tfa != null)
            {
                assembly.CustomAttributes.Remove(tfa);
            }
        }

        /// <summary>
        /// Make sure we depend on old enough versions
        /// </summary>
        private void FixAssemblyReferences(AssemblyDefinition assembly)
        {
            switch (platform)
            {
                case TargetPlatform.WSA:
                    break;
                default:
                    return;
            }

            // Workaround to make resulting file compatible with 8.0 sdk
            foreach (var reference in assembly.MainModule.AssemblyReferences)
            {
                if (reference.Name == "System.Runtime")
                    reference.Version = new Version(4, 0, 0, 0);
            }
        }

        private TypeReference RetargetTypeReference(TypeReference typeReference, ModuleDefinition inModule)
        {
            var rule = PatchRuleFor(typeReference);
            if (rule == null)
                return inModule.ImportReference(typeReference);

            var targetAssemblyRef = ReferencedAssembly(rule.DeclaringAssembly, inModule);

            return RetargetTypeReference(typeReference, inModule, targetAssemblyRef);
        }

        private TypeReference RetargetTypeReference(TypeReference typeReference, ModuleDefinition inModule, AssemblyNameReference targetAssemblyRef)
        {
            var retargetTypeReference = typeReference.Clone();
            retargetTypeReference.Scope = targetAssemblyRef;
            return retargetTypeReference;
        }

        private AssemblyNameReference ReferencedAssembly(string targetAssembly, ModuleDefinition toBeFixed)
        {
            var alreadyReferenced = (targetAssembly == toBeFixed.Assembly.FullName)
                ? toBeFixed.Assembly.Name
                : toBeFixed.AssemblyReferences.SingleOrDefault(ar => ar.FullName == targetAssembly);

            return alreadyReferenced ?? ResolveAssemblyNameReference(targetAssembly, toBeFixed);
        }

        private AssemblyNameReference ResolveAssemblyNameReference(string assembly, ModuleDefinition toBeFixed)
        {
            AssemblyNameReference assemblyReference;
            if (!targetAssemblies.TryGetValue(assembly, out assemblyReference))
            {
                var module = ModuleDefinition.ReadModule(FullPathFor(assembly));
                assemblyReference = AssemblyNameReferenceFor(module.Assembly);

                toBeFixed.AssemblyReferences.Add(assemblyReference);
                targetAssemblies[assembly] = assemblyReference;
            }

            return targetAssemblies[assembly];
        }

        private AssemblyNameReference AssemblyNameReferenceFor(AssemblyDefinition assembly)
        {
            return assembly.Name.HasPublicKey
                ? new AssemblyNameReference(assembly.Name.Name, assembly.Name.Version) { PublicKeyToken = assembly.Name.PublicKeyToken }
                : assembly.Name;
        }

        private string FullPathFor(string targetAssembly)
        {
            return string.IsNullOrWhiteSpace(searchPath)
                ? targetAssembly
                : Path.Combine(searchPath, targetAssembly);
        }

        private PatchRule PatchRuleFor(MemberReference memberReference, MethodDefinition referencingMethod = null)
        {
            var declaringTypeName = memberReference.DeclaringType != null
                ? memberReference.DeclaringType.FullName
                : null;

            var patchRule = patchRules.FirstOrDefault(
                rule => (rule.SourceMemberFullyQualifiedName == memberReference.FullName || rule.SourceMemberFullyQualifiedName == declaringTypeName) && IsApplicableTo(rule, referencingMethod));

            FixPatchRuleDeclaringAssembly(memberReference, patchRule);

            return patchRule;
        }

        private bool IsApplicableTo(PatchRule patchRule, MethodDefinition referencingMethod)
        {
            return referencingMethod == null
                || patchRule.ApplicableWith == null
                || patchRule.ApplicableWith == referencingMethod.FullName;
        }

        private static void FixPatchRuleDeclaringAssembly(MemberReference memberReference, PatchRule patchRule)
        {
            if (patchRule == null
                || patchRule.DeclaringAssembly != null
                || (patchRule.Kind == MemberKind.Other && patchRule.Operation != OperationKind.Replace)
                || patchRule.IsNullTypeReplacement) return;

            var assemblyName = AssemblyNameReferenceFrom(memberReference);
            patchRule.DeclaringAssembly = assemblyName.FullName;
        }

        private static AssemblyNameReference AssemblyNameReferenceFrom(MemberReference memberReference)
        {
            var typeReference = memberReference as TypeReference;
            if (typeReference == null)
                typeReference = memberReference.DeclaringType;

            var assemblyNameReference = typeReference.Scope as AssemblyNameReference;
            if (assemblyNameReference != null) return assemblyNameReference;

            var moduleDefinition = typeReference.Scope as ModuleDefinition;
            if (moduleDefinition == null)
            {
                throw new ArgumentException();
            }

            return moduleDefinition.Assembly.Name;
        }

        private MethodDefinition GlobalThrowingMethodReplacementFor(MethodDefinition method, MethodReference methodReference)
        {
            var moduleMethods = method.Module.Types[0].Methods;
            var rt = MappedTypeReferenceFor(methodReference.ReturnType, method.Module);

            var replecement = moduleMethods.SingleOrDefault(m => m.IsStatic && m.HasUnsupportedCompatibleMethodName(rt) && m.HasCompatibleSignatureWithUnsupportedMethod(methodReference, tr => MappedTypeReferenceFor(tr, method.Module)));
            if (replecement == null)
            {
                replecement = EmitUnsupportedMethodHelper(method.Module, methodReference);
                moduleMethods.Add(replecement);
            }

            return replecement;
        }

        private MethodDefinition EmitUnsupportedMethodHelper(ModuleDefinition module, MethodReference callee)
        {
            var typeSystem = module.TypeSystem;

            var retargetedReturnType = RetargetTypeReference(callee.ReturnType, module);

            var throwingMethod = new MethodDefinition(
                Naming.ForUnsupportedMethod(callee),
                MethodAttributes.Public | MethodAttributes.Static,
                retargetedReturnType);

            var calleeMethodDef = callee.Resolve();
            if (calleeMethodDef == null)
            {
                throw new Exception("Cannot resolve " + callee.FullName);
            }

            callee = calleeMethodDef;

            if (!calleeMethodDef.IsStatic)
            {
                var typeReference = MappedTypeReferenceFor(callee.DeclaringType, PatchRuleFor(callee), module);
                throwingMethod.Parameters.Add(new ParameterDefinition("self", ParameterAttributes.None, module.ImportReference(typeReference)));
            }

            foreach (var parameter in callee.Parameters)
            {
                var parameterType = module.ImportReference(ResolveTypeDefinition(parameter.ParameterType, module));

                if (parameter.ParameterType.IsByReference)
                    parameterType = parameterType.MakeByReferenceType();

                if (parameter.ParameterType.IsArray)
                    parameterType = parameterType.MakeArrayType();

                throwingMethod.Parameters.Add(new ParameterDefinition(parameter.Name, parameter.Attributes, parameterType));
            }

            throwingMethod.Parameters.Add(new ParameterDefinition("signature", ParameterAttributes.None, typeSystem.String));

            var ilProcessor = throwingMethod.Body.GetILProcessor();

            ilProcessor.Emit(OpCodes.Ldarg, throwingMethod.Parameters[throwingMethod.Parameters.Count - 1]);
            ilProcessor.Emit(OpCodes.Newobj, module.ImportReference(typeof(MissingMemberException).GetConstructor(new[] { typeof(string) })));
            ilProcessor.Emit(OpCodes.Throw);
            if (calleeMethodDef.ReturnType.FullName != typeSystem.Void.FullName)
            {
                if (calleeMethodDef.ReturnType.Resolve().IsValueType)
                {
                    var returnVar = new VariableDefinition(retargetedReturnType);
                    throwingMethod.Body.Variables.Add(returnVar);
                    throwingMethod.Body.InitLocals = true;

                    ilProcessor.Emit(OpCodes.Ldloca_S, returnVar);
                    ilProcessor.Emit(OpCodes.Initobj, retargetedReturnType);
                    ilProcessor.Emit(OpCodes.Ldloc_S, returnVar);
                }
                else
                {
                    ilProcessor.Emit(OpCodes.Ldnull);
                }
            }
            ilProcessor.Emit(OpCodes.Ret);

            return throwingMethod;
        }

        private TypeReference MappedTypeReferenceFor(TypeReference originalTypeRef, ModuleDefinition module)
        {
            var patchRule = PatchRuleFor(originalTypeRef);
            if (patchRule == null) return originalTypeRef;

            return MappedTypeReferenceFor(originalTypeRef, patchRule, module);
        }

        private static TypeReference MappedTypeReferenceFor(TypeReference originalTypeRef, PatchRule patchRule, ModuleDefinition module)
        {
            return patchRule.IsNullTypeReplacement ? module.TypeSystem.Object : originalTypeRef;
        }

        #region Event Handling

        public event EventHandler<TypeReferencePatchingEventEventArgs> BaseTypePatchingEvent;
        public event EventHandler<TypeReferencePatchingEventEventArgs> FieldTypePatchingEvent;
        public event EventHandler<TypeReferencePatchingEventEventArgs> PropertyTypePatchingEvent;
        public event EventHandler<TypeReferencePatchingEventEventArgs> TypeReferencePatchingEvent;
        public event EventHandler<MethodReferencePatchingEventEventArgs> MethodReferencePatching;
        public event EventHandler<MemberReferencePatchingEventArgs> FieldReferencePatching;
        public event EventHandler<ParameterTypePatchingEventArgs> ParameterTypePatching;
        public event EventHandler<LocalVariableTypePatchingEventArgs> LocalVariableTypePatching;
        public event EventHandler<TypeReferencePatchingEventEventArgs> MethodReturnTypePatching;
        public event EventHandler<CatchTypePatchingEventArgs> CatchTypePatchingEvent;

        private void RaiseBaseTypePatching(TypeDefinition patchedType, TypeReference newBase, PatchRule patchRule)
        {
            Raise(BaseTypePatchingEvent, new TypeReferencePatchingEventEventArgs(patchedType, newBase, patchRule));
        }

        private void RaiseFieldTypePatching(FieldDefinition field, TypeReference newType, PatchRule patchRule)
        {
            Raise(FieldTypePatchingEvent, new TypeReferencePatchingEventEventArgs(field, newType, patchRule));
        }

        private void RaisePropertyTypePatching(PropertyDefinition property, TypeReference newType, PatchRule patchRule)
        {
            Raise(PropertyTypePatchingEvent, new TypeReferencePatchingEventEventArgs(property, newType, patchRule));
        }

        private void RaiseCatchTypePatching(MethodDefinition method, ExceptionHandler handler, TypeReference newType, PatchRule patchRule)
        {
            Raise(CatchTypePatchingEvent, new CatchTypePatchingEventArgs(method, handler, newType, patchRule));
        }

        private void RaiseTypeReferencePatching(MethodDefinition method, Instruction referencee, TypeReference newType)
        {
            Raise(TypeReferencePatchingEvent, new TypeReferencePatchingEventEventArgs(method, newType, referencee));
        }

        private void RaiseMethodReferencePatching(MethodDefinition method, MethodReference targetMethodDef, Instruction instruction, PatchRule patchRule)
        {
            Raise(MethodReferencePatching, new MethodReferencePatchingEventEventArgs(method, targetMethodDef, instruction, patchRule));
        }

        private void RaiseFieldReferencePatching(MethodDefinition method, Instruction instruction, FieldReference patchedFieldReference, PatchRule patchRule)
        {
            Raise(FieldReferencePatching, new MemberReferencePatchingEventArgs(method, patchedFieldReference, instruction, patchRule));
        }

        private void RaiseParameterTypePatching(MethodDefinition method, ParameterDefinition parameter, TypeReference newType, PatchRule patchRule)
        {
            Raise(ParameterTypePatching, new ParameterTypePatchingEventArgs(method, parameter, newType, patchRule));
        }

        private void RaiseLocalVariablePatching(MethodDefinition method, VariableDefinition localVar, TypeReference newType, PatchRule patchRule)
        {
            Raise(LocalVariableTypePatching, new LocalVariableTypePatchingEventArgs(method, localVar, newType, patchRule));
        }

        private void RaiseMethodReturnTypePatching(MethodDefinition method, TypeDefinition newReturnType, PatchRule patchRule)
        {
            Raise(MethodReturnTypePatching, new TypeReferencePatchingEventEventArgs(method, newReturnType, patchRule));
        }

        private void Raise<T>(EventHandler<T> handler, T args) where T : EventArgs
        {
            if (handler != null)
            {
                handler(this, args);
            }
        }

        #endregion

        private readonly string searchPath;

        private readonly IEnumerable<PatchRule> patchRules;
        private readonly TargetPlatform platform;
        private readonly IDictionary<string, AssemblyNameReference> targetAssemblies = new Dictionary<string, AssemblyNameReference>();
    }
}
