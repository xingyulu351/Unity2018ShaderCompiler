using System;
using System.Collections.Generic;
using System.Linq;
using Mono.Cecil;
using Unity.Cecil.Visitor;
using UnityEngine.Bindings;
using UnityEngine;

namespace Unity.BindingsGenerator.Core
{
    class AssemblyVerifierVisitor : Visitor
    {
        public AssemblyVerifier.ErrorList Errors { get; set; }
    }

    internal class NativeAsStructVerifier : AssemblyVerifierVisitor
    {
        public override void Visit(TypeDefinition typeDefinition)
        {
            if (typeDefinition.IsValueType)
                return;

            NativeAsStructAttribute nativeAsStructAttribute;
            if (!typeDefinition.TryGetAttributeInstance(out nativeAsStructAttribute))
                return;

            if (!typeDefinition.IsSequentialLayout)
                Errors.Add(new AssemblyVerifier.Error(typeDefinition.FullName, AssemblyVerifier.Error.Type.KNativeAsStructRequiresExplicitLayout));
        }
    }

    class FreeFunctionIsStaticVerifier : AssemblyVerifierVisitor
    {
        public override void Visit(MethodDefinition methodDefinition)
        {
            if (!methodDefinition.RequiresBindingsGeneration())
                return;

            var isFreeFunction = false;

            IBindingsIsFreeFunctionProviderAttribute isFreeFunctionProvider;
            if (methodDefinition.TryGetAttributeInstance(out isFreeFunctionProvider))
                isFreeFunction = isFreeFunctionProvider.IsFreeFunction;

            if (!isFreeFunction)
                return;

            if (methodDefinition.IsStatic)
            {
                if (isFreeFunctionProvider.HasExplicitThis)
                    Errors.Add(new AssemblyVerifier.Error(methodDefinition.FullName, AssemblyVerifier.Error.Type.KFreeFunctionMustBeStaticOrExplicitThis));

                return;
            }

            if (!isFreeFunctionProvider.HasExplicitThis)
                Errors.Add(new AssemblyVerifier.Error(methodDefinition.FullName, AssemblyVerifier.Error.Type.KFreeFunctionMustBeStaticOrExplicitThis));
        }
    }

    internal class LegalParameterNameVerifier : AssemblyVerifierVisitor
    {
        private readonly ScriptingBackend scriptingBackend;

        public LegalParameterNameVerifier(ScriptingBackend scriptingBackend)
        {
            this.scriptingBackend = scriptingBackend;
        }

        public override void Visit(MethodDefinition methodDefinition)
        {
            if (!methodDefinition.RequiresBindingsGeneration())
                return;

            foreach (var p in methodDefinition.Parameters)
            {
                if (!methodDefinition.IsStatic)
                {
                    if (p.Name == Naming.Self)
                    {
                        Errors.Add(new AssemblyVerifier.Error(methodDefinition.FullName, AssemblyVerifier.Error.Type.KIllegalParameterName,
                            "'" + Naming.Self + "'"));
                        continue;
                    }

                    if (p.Name == Naming.Self.Marshalled())
                    {
                        Errors.Add(new AssemblyVerifier.Error(methodDefinition.FullName, AssemblyVerifier.Error.Type.KIllegalParameterName,
                            "'" + Naming.Self.Marshalled() + "'"));
                        continue;
                    }
                }

                if (TypeUtils.ResolveUnmarshalledTypeName(methodDefinition.ReturnType, TypeUsage.ReturnType, scriptingBackend) != Naming.VoidType)
                {
                    if (p.Name == Naming.ReturnVar)
                    {
                        Errors.Add(new AssemblyVerifier.Error(methodDefinition.FullName, AssemblyVerifier.Error.Type.KIllegalParameterName,
                            "'" + Naming.ReturnVar + "'"));
                        continue;
                    }

                    if (p.Name == Naming.ReturnVar.Marshalled())
                    {
                        Errors.Add(new AssemblyVerifier.Error(methodDefinition.FullName, AssemblyVerifier.Error.Type.KIllegalParameterName,
                            "'" + Naming.ReturnVar.Marshalled() + "'"));
                        continue;
                    }
                }

                var illegalName = p.Name.Marshalled();

                if (methodDefinition.Parameters.SingleOrDefault(t => t.Name == illegalName) != null)
                    Errors.Add(new AssemblyVerifier.Error(methodDefinition.FullName, AssemblyVerifier.Error.Type.KIllegalParameterName,
                        "Due to conflict with generated variable name, '" + illegalName + "'"));
            }
        }
    }

    class SensicalAttributeVerifier : AssemblyVerifierVisitor
    {
        // Native Enum, Include, Property, and Type attributes are all locked via AttributeTargets, so we do not
        // need to verify them at this stage

        private void AddErrorIfCustomAttributeFound<T>(MethodDefinition methodDefinition,
            AssemblyVerifier.Error.Type errorNo)
        {
            if (methodDefinition.HasAttribute<T>())
                Errors.Add(new AssemblyVerifier.Error(methodDefinition.FullName, errorNo));
        }

        public override void Visit(MethodDefinition methodDefinition)
        {
            if (!methodDefinition.RequiresBindingsGeneration())
                AddErrorIfCustomAttributeFound<NativeMethodAttribute>(methodDefinition, AssemblyVerifier.Error.Type.KNonSensicalAttribute);
        }
    }

    class AssemblyDependencyValidator : AssemblyVerifierVisitor
    {
        class MemberReferenceFinder : Unity.Cecil.Visitor.Visitor
        {
            private MemberReference referenceToFind;
            private string foundSource;

            private MemberReferenceFinder(MemberReference referenceToFind)
            {
                this.referenceToFind = referenceToFind;
            }

            public static string FindSource(MemberReference referenceToFind, AssemblyDefinition assemblyDefinition)
            {
                var finder = new MemberReferenceFinder(referenceToFind);
                finder.Visit(assemblyDefinition, Context.None);
                return finder.foundSource;
            }

            protected override void Visit(MethodReference methodReference, Context context)
            {
                VisitMemberReference(methodReference, context);
                base.Visit(methodReference, context);
            }

            protected override void Visit(FieldReference fieldReference, Context context)
            {
                VisitMemberReference(fieldReference, context);
                base.Visit(fieldReference, context);
            }

            protected override void Visit(TypeReference typeReference, Context context)
            {
                VisitMemberReference(typeReference, context);
                base.Visit(typeReference, context);
            }

            private void VisitMemberReference(MemberReference memberReference, Context context)
            {
                if (memberReference.FullName == referenceToFind.FullName)
                    foundSource = SourceStringForAppropriateParent(context) ?? context.Data?.ToString();
            }

            //find the closest human-readable source to the given context
            private string SourceStringForAppropriateParent(Context context)
            {
                switch (context.Role)
                {
                    case Role.Attribute:
                    case Role.GenericParameter:
                    case Role.Parameter:
                    case Role.MethodBody:
                        return context.Data.ToString();
                    case Role.None:
                        return null;
                    default:
                        return SourceStringForAppropriateParent(context.Parent);
                }
            }
        }

        bool CheckAttributes(Mono.Collections.Generic.Collection<CustomAttribute> attributes, MemberReference memberReference, string memberNameBeingChecked, AssemblyDefinition assemblyDefinition, string exportingAssemblyName, bool generateErrors = true)
        {
            var attribute = attributes.FirstOrDefault(x => x.AttributeType.Name == "VisibleToOtherModulesAttribute");
            if (attribute == null)
            {
                if (generateErrors)
                    Errors.Add(new AssemblyVerifier.Error(MemberReferenceFinder.FindSource(memberReference, assemblyDefinition), AssemblyVerifier.Error.Type.KCannotUseTypeInternalToOtherAssembly, memberNameBeingChecked, exportingAssemblyName, assemblyDefinition.Name.Name));
                return false;
            }
            var args = attribute.ConstructorArguments;
            if (args.Count() > 0)
            {
                foreach (var module in args[0].Value as CustomAttributeArgument[])
                {
                    var moduleName = module.Value as string;
                    if (moduleName == assemblyDefinition.Name.Name)
                        return true;
                }
                if (generateErrors)
                    Errors.Add(new AssemblyVerifier.Error(MemberReferenceFinder.FindSource(memberReference, assemblyDefinition), AssemblyVerifier.Error.Type.KCannotUseTypeInternalToOtherAssemblyNotAllowedByAttribute, memberNameBeingChecked, exportingAssemblyName, assemblyDefinition.Name.Name));
                return false;
            }
            return true;
        }

        public override void Visit(AssemblyDefinition assemblyDefinition)
        {
            // UnityEditor currently accesses UnityEngine internals all over the place.
            // While we want to restrict that, we need to do one step at a time.
            if (assemblyDefinition.Name.Name == "UnityEditor" || assemblyDefinition.Name.Name == "UnityEngine.EditorModule")
                return;

            var typesReferences = assemblyDefinition.MainModule.GetTypeReferences();
            foreach (var tr in typesReferences)
            {
                try
                {
                    var parentType = tr.Resolve();
                    var type = parentType;
                    while (type != null)
                    {
                        if (type.Module != assemblyDefinition.MainModule)
                        {
                            if (type.IsNotPublic || type.IsNestedAssembly)
                            {
                                if (!CheckAttributes(type.CustomAttributes, tr, parentType.FullName, assemblyDefinition, type.Module.Assembly.Name.Name))
                                {
                                    // stop here, don't need additional errors for this type.
                                    type = null;
                                    continue;
                                }
                            }
                        }
                        type = type.DeclaringType;
                    }
                }
                catch (AssemblyResolutionException)
                { // This can happen if we have references to assemblies not in the search path, such as WinRT bridge. Ignore these.
                }
            }
            var memberReferences = assemblyDefinition.MainModule.GetMemberReferences();
            foreach (var mr in memberReferences)
            {
                try
                {
                    var member = mr.Resolve();
                    if (member.DeclaringType.Module != assemblyDefinition.MainModule)
                    {
                        if ((member is MethodDefinition && ((MethodDefinition)member).IsAssembly)
                            || (member is FieldDefinition && ((FieldDefinition)member).IsAssembly))
                        {
                            if (member is MethodDefinition)
                            {
                                var method = member as MethodDefinition;
                                var prop = method.GetMatchingPropertyDefinition();
                                // Properties are a bit special. We want to allow the attribute to be on the property itself,
                                // but only if the property accessor method does not have stricter permissions.
                                // We cannot check permissions on the property itself, but we know that if the other accessor
                                // is not internal, then the internal attribute must be on the accessory method.
                                if (prop != null && prop.GetMethod != null && prop.SetMethod != null && prop.GetMethod.IsAssembly == prop.SetMethod.IsAssembly)
                                {
                                    if (CheckAttributes(prop.CustomAttributes, mr, prop.FullName, assemblyDefinition, member.DeclaringType.Module.Assembly.Name.Name, false))
                                        continue;
                                }
                            }

                            CheckAttributes(member.CustomAttributes, mr, member.FullName, assemblyDefinition, member.DeclaringType.Module.Assembly.Name.Name);
                        }
                    }
                }
                catch (AssemblyResolutionException)
                { // This can happen if we have references to assemblies not in the search path, such as WinRT bridge. Ignore these.
                }
            }
        }
    }

    class MarshallableObjectVerifier : AssemblyVerifierVisitor
    {
        private ScriptingBackend scriptingBackend;

        public MarshallableObjectVerifier(ScriptingBackend scriptingBackend)
        {
            this.scriptingBackend = scriptingBackend;
        }

        private bool IsTypeMarshallableObjectIfNeccesary(TypeDefinition type)
        {
            if (TypeMustBeMarshallable(type) == false)
                return true;

            if (TypeUtils.IsStruct(type))
                return true;

            if (TypeUtils.GetScriptingObjectMarshallingType(type) != ScriptingObjectMarshallingType.Invalid)
                return true;

            return false;
        }

        private bool IsTypeMarshallableUnityEngineObjectIfNeccesary(TypeDefinition type)
        {
            var scriptingObjectType = TypeUtils.GetScriptingObjectMarshallingType(type);
            return scriptingObjectType != ScriptingObjectMarshallingType.Invalid && scriptingObjectType != ScriptingObjectMarshallingType.ScriptingObjectPtr;
        }

        public override void Visit(MethodDefinition methodDefinition)
        {
            if (!methodDefinition.RequiresBindingsGeneration())
                return;

            foreach (var parameterDefinition in methodDefinition.Parameters)
            {
                var parameterType = parameterDefinition.ParameterType.Resolve();

                if (!IsTypeMarshallableObjectIfNeccesary(parameterType))
                    Errors.Add(new AssemblyVerifier.Error(methodDefinition.FullName,
                        AssemblyVerifier.Error.Type.KParameterMustBeMarshallable, parameterType.Name));
            }

            if (!methodDefinition.IsStatic)
            {
                // the method has an implicit self parameter, so we must check that the declaring type is marshallable
                if (IsTypeMarshallableUnityEngineObjectIfNeccesary(methodDefinition.DeclaringType))
                    return;

                // we also skip structs because they are verified in a different step
                if (methodDefinition.DeclaringType.IsValueType)
                    return;

                Errors.Add(new AssemblyVerifier.Error(methodDefinition.FullName,
                    AssemblyVerifier.Error.Type.KClassMustBeMarshallable, methodDefinition.DeclaringType.Name));
            }
        }

        public bool TypeMustBeMarshallable(TypeDefinition typeDefinition)
        {
            if (!typeDefinition.IsClass)
                return false;

            if (typeDefinition.IsEnum)
                return false;

            if (typeDefinition.FullName == "System.String")
                return false;

            if (typeDefinition.IsPrimitive)
                return false;

            if (TypeUtils.IsNativePodType(typeDefinition, scriptingBackend))
                return false;

            return true;
        }
    }

    class NotNullVerifier : AssemblyVerifierVisitor
    {
        public override void Visit(MethodDefinition methodDefinition)
        {
            foreach (var parameterDefinition in methodDefinition.Parameters)
            {
                if (!parameterDefinition.HasAttribute<NotNullAttribute>())
                    continue;

                if (!methodDefinition.RequiresBindingsGeneration())
                    Errors.Add(new AssemblyVerifier.Error(methodDefinition.FullName, AssemblyVerifier.Error.Type.KNonsensicalCanBeNullAttribute, parameterDefinition.Name));

                if (!TypeUtils.CouldBeNull(parameterDefinition.ParameterType))
                    Errors.Add(new AssemblyVerifier.Error(methodDefinition.FullName, AssemblyVerifier.Error.Type.KNonsensicalCanBeNullAttribute, parameterDefinition.Name));
            }
        }
    }

    class DefaultParameterVerifier : AssemblyVerifierVisitor
    {
        public override void Visit(MethodDefinition methodDefinition)
        {
            if (!methodDefinition.RequiresBindingsGeneration())
                return;

            foreach (var parameter in methodDefinition.Parameters)
            {
                if (parameter.HasDefault && TypeUtils.IsStruct(parameter.ParameterType))
                    Errors.Add(new AssemblyVerifier.Error(methodDefinition.FullName, AssemblyVerifier.Error.Type.KDefaultParameterNotSupported));
            }
        }
    }

    class ConditionalReturnTypeVerifier : AssemblyVerifierVisitor
    {
        public override void Visit(MethodDefinition methodDefinition)
        {
            if (!methodDefinition.RequiresBindingsGeneration())
                return;

            var nativeConditionalAttribute = StubUtils.NativeConditionalAttributeFor(methodDefinition);
            if (nativeConditionalAttribute != null && methodDefinition.ReturnType.MetadataType != MetadataType.Void)
            {
                bool canGenerateDefaultForReturnType = nativeConditionalAttribute.StubReturnStatement != null ||
                    StubUtils.DefaultNativeReturnValue(methodDefinition.ReturnType) != null;

                if (!canGenerateDefaultForReturnType)
                    Errors.Add(new AssemblyVerifier.Error(methodDefinition.FullName, AssemblyVerifier.Error.Type.KStubHasIncompatibleReturnType, methodDefinition.ReturnType.Name));
            }
        }
    }

    class ExternMethodNotOverloadedVerifier : AssemblyVerifierVisitor
    {
        public override void Visit(TypeDefinition typeDefinition)
        {
            foreach (var overloadedMethodName in typeDefinition.Methods.Where(m => m.RequiresBindingsGeneration()).Select(m => m.Name).GroupBy(x => x).Where(g => g.Count() > 1).Select(y => y.Key))
            {
                Errors.Add(new AssemblyVerifier.Error(typeDefinition.FullName, AssemblyVerifier.Error.Type.KMethodOverloadNotSupported, overloadedMethodName));
            }
        }
    }

    class IndexedPropertyNotSupportedVerifier : AssemblyVerifierVisitor
    {
        public override void Visit(PropertyDefinition propertyDefinition)
        {
            if (!propertyDefinition.RequiresBindingsGeneration())
                return;

            if (propertyDefinition.Parameters.Count > 0)
                Errors.Add(new AssemblyVerifier.Error(propertyDefinition.FullName, AssemblyVerifier.Error.Type.KIndexedPropertyNotSupported));
        }
    }

    class SensicalWritableAttributeVerifier : AssemblyVerifierVisitor
    {
        public override void Visit(MethodDefinition methodDefinition)
        {
            if (!methodDefinition.RequiresBindingsGeneration())
                return;

            foreach (var parameter in methodDefinition.Parameters)
            {
                if (parameter.HasAttribute<WritableAttribute>())
                {
                    if (!TypeUtils.TypeInheritsFromUnityEngineObject(parameter.ParameterType))
                    {
                        Errors.Add(new AssemblyVerifier.Error(methodDefinition.FullName, AssemblyVerifier.Error.Type.KNonSensicalWritableAttribute, parameter.Name));
                    }
                }
            }
        }
    }

    class NoListReturnVerifier : AssemblyVerifierVisitor
    {
        public override void Visit(MethodDefinition methodDefinition)
        {
            if (!methodDefinition.RequiresBindingsGeneration())
                return;

            if (methodDefinition.ReturnType.Name == "List`1")
                Errors.Add(new AssemblyVerifier.Error(methodDefinition.FullName, AssemblyVerifier.Error.Type.KUnsupportedListReturn));
        }
    }

    class StaticAccessorAttributeOnInstanceMethodVerifier : AssemblyVerifierVisitor
    {
        public override void Visit(MethodDefinition methodDefinition)
        {
            if (!methodDefinition.RequiresBindingsGeneration())
                return;

            if (methodDefinition.IsStatic)
                return;

            if (!methodDefinition.HasAttribute<StaticAccessorAttribute>())
                return;

            Errors.Add(new AssemblyVerifier.Error(methodDefinition.FullName, AssemblyVerifier.Error.Type.KStaticAccessorOnInstanceMethod));
        }
    }

    class StaticAccessorAttributeOnNonExternMethodVerifier : AssemblyVerifierVisitor
    {
        public override void Visit(MethodDefinition methodDefinition)
        {
            if (methodDefinition.RequiresBindingsGeneration())
                return;

            if (!methodDefinition.HasAttribute<StaticAccessorAttribute>())
                return;

            Errors.Add(new AssemblyVerifier.Error(methodDefinition.FullName, AssemblyVerifier.Error.Type.KStaticAccessorOnNonExternMethod));
        }
    }

    class ExternPropertyInStructIsErrorInPlayerVerifier : AssemblyVerifierVisitor
    {
        public override void Visit(PropertyDefinition propertyDefinition)
        {
            if (GlobalContext.HasDefine("UNITY_EDITOR"))
                return;

            if (!propertyDefinition.RequiresBindingsGeneration())
                return;

            if (!TypeUtils.IsStruct(propertyDefinition.DeclaringType))
                return;

            Errors.Add(new AssemblyVerifier.Error(propertyDefinition.FullName, AssemblyVerifier.Error.Type.KExternPropertyInStructInPlayer, propertyDefinition.PropertyType.SimpleTypeName(), propertyDefinition.Name, propertyDefinition.Name.Capitalize()));
        }
    }

    class NativeThrowsOnFieldTargetingPropertyIsErrorVerifier : AssemblyVerifierVisitor
    {
        public override void Visit(PropertyDefinition propertyDefinition)
        {
            NativePropertyAttribute nativePropertyAttribute;
            if (!propertyDefinition.TryGetAttributeInstance(out nativePropertyAttribute) ||
                nativePropertyAttribute.TargetType != TargetType.Field)
                return;

            MethodDefinition badMethodDefinition = null;
            NativeThrowsAttribute nativeThrowsAttribute;
            if (propertyDefinition.TryGetAttributeInstance(out nativeThrowsAttribute) ||
                (propertyDefinition.GetMethod?.TryGetAttributeInstance(out nativeThrowsAttribute) ?? false))
                badMethodDefinition = propertyDefinition.GetMethod;

            if (propertyDefinition.TryGetAttributeInstance(out nativeThrowsAttribute) ||
                (propertyDefinition.SetMethod?.TryGetAttributeInstance(out nativeThrowsAttribute) ?? false))
                badMethodDefinition = propertyDefinition.SetMethod;

            if (badMethodDefinition != null)
                Errors.Add(new AssemblyVerifier.Error(badMethodDefinition.FullName, AssemblyVerifier.Error.Type.KThrowsOnFieldTargetingProperty));
        }
    }

    public class OutEnumNotSupportedOnDotNetVerifier : Visitor
    {
        public AssemblyVerifier.ErrorList Errors { get; set; }

        public override void Visit(MethodDefinition methodDefinition)
        {
            if (!methodDefinition.RequiresBindingsGeneration())
                return;

            if (methodDefinition.Parameters.Any(p => p.ParameterType.IsArray && (TypeUtils.GetElementType(p.ParameterType)?.Resolve()?.IsEnum ?? false) && p.IsOut))
                Errors.Add(new AssemblyVerifier.Error(methodDefinition.FullName, AssemblyVerifier.Error.Type.KOutEnumNotSupportedOnDotNet));

            base.Visit(methodDefinition);
        }
    }

    public class AssemblyVerifier
    {
        private readonly NonFilteringVisitor _visitor;
        public ErrorList Errors { get; private set; }
        private ScriptingBackend _scriptingBackend;

        public AssemblyVerifier(NonFilteringVisitor cecilVisitor, ScriptingBackend scriptingBackend)
        {
            _visitor = cecilVisitor;
            _scriptingBackend = scriptingBackend;
            Errors = new ErrorList();
        }

        public static ErrorList Verify(TypeDefinition typeDefinition, ScriptingBackend scriptingBackend = ScriptingBackend.Mono)
        {
            var verifier = new AssemblyVerifier(new NonFilteringVisitor(typeDefinition), scriptingBackend);
            return verifier.Verify().Errors;
        }

        public static ErrorList Verify(AssemblyDefinition assemblyDefinition, ScriptingBackend scriptingBackend = ScriptingBackend.Mono)
        {
            var verifier = new AssemblyVerifier(new NonFilteringVisitor(assemblyDefinition), scriptingBackend);
            return verifier.Verify().Errors;
        }

        public static ErrorList Verify(MethodDefinition methodDefinition, ScriptingBackend scriptingBackend = ScriptingBackend.Mono)
        {
            var verifier = new AssemblyVerifier(new NonFilteringVisitor(methodDefinition), scriptingBackend);
            return verifier.Verify().Errors;
        }

        public AssemblyVerifier Verify()
        {
            return this
                .VerifyAttributesAreSensical()
                .VerifyParameterNamesAreLegal()
                .VerifyFreeFunctionsAreStatic()
                .VerifyNativeAsStruct()
                .VerifyMarshallableObjects()
                .VerifyCanBeNull()
                //.VerifyDefaultParameters()
                .VerifyConditionalReturnTypes()
                .VerifyMethodsAreNotOverloaded()
                .VerifyNoIndexedProperties()
                .VerifySensicalWritableAttribute()
                .VerifyNoListReturns()
                .VerifyNoStaticAccessorAttributeOnInstanceMethods()
                .VerifyNoStaticAccessorAttributeOnNonExternMethods()
                .VerifyNoExternPropertiesInStructsInPlayer()
                .VerifyNativeThrowsOnFieldTargetingPropertyIsErrorVerifier()
                .VerifyAssemblyDependencies()
                .VerifyOutEnumNotSupportedOnDotNet()
            ;
        }

        public AssemblyVerifier VerifyAttributesAreSensical()
        {
            _visitor.Accept(new SensicalAttributeVerifier {Errors = Errors});
            return this;
        }

        public AssemblyVerifier VerifyParameterNamesAreLegal()
        {
            _visitor.Accept(new LegalParameterNameVerifier(_scriptingBackend) {Errors = Errors});
            return this;
        }

        public AssemblyVerifier VerifyFreeFunctionsAreStatic()
        {
            _visitor.Accept(new FreeFunctionIsStaticVerifier {Errors = Errors});
            return this;
        }

        public AssemblyVerifier VerifyNativeAsStruct()
        {
            _visitor.Accept(new NativeAsStructVerifier {Errors = Errors});
            return this;
        }

        public AssemblyVerifier VerifyMarshallableObjects()
        {
            _visitor.Accept(new MarshallableObjectVerifier(_scriptingBackend) {Errors = Errors});
            return this;
        }

        public AssemblyVerifier VerifyAssemblyDependencies()
        {
            _visitor.Accept(new AssemblyDependencyValidator { Errors = Errors });
            return this;
        }

        public AssemblyVerifier VerifyCanBeNull()
        {
            _visitor.Accept(new NotNullVerifier {Errors = Errors});
            return this;
        }

        public AssemblyVerifier VerifyDefaultParameters()
        {
            _visitor.Accept(new DefaultParameterVerifier {Errors = Errors});
            return this;
        }

        public AssemblyVerifier VerifyConditionalReturnTypes()
        {
            _visitor.Accept(new ConditionalReturnTypeVerifier {Errors = Errors});
            return this;
        }

        public AssemblyVerifier VerifyMethodsAreNotOverloaded()
        {
            _visitor.Accept(new ExternMethodNotOverloadedVerifier {Errors = Errors});
            return this;
        }

        public AssemblyVerifier VerifyNoIndexedProperties()
        {
            _visitor.Accept(new IndexedPropertyNotSupportedVerifier {Errors = Errors});
            return this;
        }

        public AssemblyVerifier VerifySensicalWritableAttribute()
        {
            _visitor.Accept(new SensicalWritableAttributeVerifier {Errors = Errors});
            return this;
        }

        public AssemblyVerifier VerifyNoListReturns()
        {
            _visitor.Accept(new NoListReturnVerifier {Errors = Errors});
            return this;
        }

        public AssemblyVerifier VerifyNoStaticAccessorAttributeOnInstanceMethods()
        {
            _visitor.Accept(new StaticAccessorAttributeOnInstanceMethodVerifier {Errors = Errors});
            return this;
        }

        public AssemblyVerifier VerifyNoStaticAccessorAttributeOnNonExternMethods()
        {
            _visitor.Accept(new StaticAccessorAttributeOnNonExternMethodVerifier {Errors = Errors});
            return this;
        }

        public AssemblyVerifier VerifyNoExternPropertiesInStructsInPlayer()
        {
            _visitor.Accept(new ExternPropertyInStructIsErrorInPlayerVerifier {Errors = Errors});
            return this;
        }

        public AssemblyVerifier VerifyNativeThrowsOnFieldTargetingPropertyIsErrorVerifier()
        {
            _visitor.Accept(new NativeThrowsOnFieldTargetingPropertyIsErrorVerifier { Errors = Errors});
            return this;
        }

        public AssemblyVerifier VerifyOutEnumNotSupportedOnDotNet()
        {
            if (_scriptingBackend == ScriptingBackend.DotNet)
                _visitor.Accept(new OutEnumNotSupportedOnDotNetVerifier {Errors = Errors});
            return this;
        }

        public class Error
        {
            public enum Type
            {
                KFreeFunctionMustBeStaticOrExplicitThis,
                KNonsensicalNativeMethod,
                KIllegalParameterName,
                KNonSensicalAttribute,
                KClassMustBeMarshallable,
                KParameterMustBeMarshallable,
                KTypeNotSupported,
                KNonsensicalCanBeNullAttribute,
                KDefaultParameterNotSupported,
                KStubHasIncompatibleReturnType,
                KNativeTypeHasName,
                KMethodOverloadNotSupported,
                KIndexedPropertyNotSupported,
                KNonSensicalWritableAttribute,
                KUnsupportedListReturn,
                KStaticAccessorOnInstanceMethod,
                KStaticAccessorOnNonExternMethod,
                KExternPropertyInStructInPlayer,
                KThrowsOnFieldTargetingProperty,
                KNativeAsStructRequiresExplicitLayout,
                KCannotUseTypeInternalToOtherAssembly,
                KCannotUseTypeInternalToOtherAssemblyNotAllowedByAttribute,
                KOutEnumNotSupportedOnDotNet
            }

            private readonly Dictionary<Type, string> ErrorMessages = new Dictionary<Type, string>
            {
                { Type.KFreeFunctionMustBeStaticOrExplicitThis,  "Free function must be static, or HasExplicitThis must be set to true."},
                { Type.KNonsensicalNativeMethod,                 "Cannot apply NativeMethod attribute. Use NativeGetter/NativeSetter instead."},
                { Type.KIllegalParameterName,                    "{0} is not a legal parameter name."},
                { Type.KNonSensicalAttribute,                    "Cannot apply method attribute to non-extern method."},
                { Type.KClassMustBeMarshallable,                 "Class has extern member method and must inherit from UnityEngine.Object or have its first field typed as IntPtr."},
                { Type.KParameterMustBeMarshallable,             "Parameter '{0}' must inherit from UnityEngine.Object."},
                { Type.KNonsensicalCanBeNullAttribute,           "CanBeNull option does not make sense for parameter {0}."},
                { Type.KDefaultParameterNotSupported,            "Default parameters not yet supported for structs."},
                { Type.KStubHasIncompatibleReturnType,           "Cannot generate default stub value for return type {0}."},
                { Type.KNativeTypeHasName,                       "Type cannot both have NativeType and NativeName attributes."},
                { Type.KMethodOverloadNotSupported,              "Overloads found for method '{0}'. Overloads are not supported."},
                { Type.KIndexedPropertyNotSupported,             "Indexed properties are not supported."},
                { Type.KNonSensicalWritableAttribute,            "[Writable] only supported for UnityEngine.Objects, which '{0}' is not."},
                { Type.KUnsupportedListReturn,                   "Returning List<T> is not supported."},
                { Type.KStaticAccessorOnInstanceMethod,          "StaticAccessor attribute does not make sense on instance methods."},
                { Type.KStaticAccessorOnNonExternMethod,         "StaticAccessor attribute does not make sense on non extern methods."},
                { Type.KExternPropertyInStructInPlayer,          "Extern properties in structs are not allowed in player code. One workaround is to transform your extern property to a non-extern struct and two extern methods, like this: \n\npublic extern {0} {1} {{ get; set; }}\n\n ===>\n\npublic {0} {1} {{ get {{ return Get{2}(); }} set {{ Set{2}(value); }} }}\n\nprivate extern {0} Get{2}();\nprivate extern void Set{2}({0} value);\n\n"},
                { Type.KThrowsOnFieldTargetingProperty,          "[NativeThrows] is not compatible with " + nameof(TargetType) + "." + nameof(TargetType.Field)},
                { Type.KNativeAsStructRequiresExplicitLayout,    "[NativeAsStruct] can only be used on sequential layout structs."},
                { Type.KCannotUseTypeInternalToOtherAssembly,    "Cannot use internal type or member '{0}' from module '{1}' in module '{2}' without adding the [VisibleToOtherModules] attribute."},
                { Type.KCannotUseTypeInternalToOtherAssemblyNotAllowedByAttribute,    "Cannot use internal type or member '{0}' from module '{1}' in module '{2}' without adding the [VisibleToOtherModules] attribute (the attribute exists, but it has an explicit assembly list which does not contain '{2}')."},
                { Type.KOutEnumNotSupportedOnDotNet,             "'[Out]' is not supported on enum arrays on .NET backend because it must marshal to 'int[]' first."},
            };

            public string SourceName { get; private set; }
            public Type ErrorNo { get; private set; }
            public string[] MsgExtenders { get; private set; }

            public Error(string sourceName, Type errorNo, params string[] msgExtenders)
            {
                SourceName = sourceName;
                ErrorNo = errorNo;
                MsgExtenders = msgExtenders;
            }

            public override string ToString()
            {
                return string.Format("{0}: {1}", SourceName, string.Format(ErrorMessages[ErrorNo], MsgExtenders));
            }
        }

        public class ErrorList
        {
            private List<Error> _errors = new List<Error>();
            public List<Error> Errors { get { return _errors; } }
            public int Count { get { return _errors.Count;  }}

            public void Add(Error e)
            {
                _errors.Add(e);
            }

            public override string ToString()
            {
                return String.Join("\n", _errors.Select(e => e.ToString()));
            }
        }
    }
}
