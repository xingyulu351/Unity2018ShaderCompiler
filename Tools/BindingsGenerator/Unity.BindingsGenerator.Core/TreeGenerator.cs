using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Runtime.CompilerServices;
using Mono.Cecil;
using Mono.CSharp;
using UnityEngine.Bindings;
using Unity.BindingsGenerator.Core.Nodes;
using UnityEngine;
using TypeDefinition = Mono.Cecil.TypeDefinition;

namespace Unity.BindingsGenerator.Core
{
    public class TreeGenerator : Visitor
    {
        private readonly FileNode _sourceFile = new FileNode();
        private readonly FileNode _headerFile = new FileNode();
        private readonly HashSet<string> _processedTypes = new HashSet<string>();
        private NamespaceNode _marshallingNamespace;
        private NamespaceNode _registrationCheckNamespace;
        private ScriptingBackend _scriptingBackend;

        public FileNode SourceFile { get { return _sourceFile; }}
        public FileNode HeaderFile { get { return _headerFile; } }

        public NamespaceNode MarshallingNamespace
        {
            get { return _marshallingNamespace; }
        }

        public TreeGenerator() : this(ScriptingBackend.Mono)
        {}

        public TreeGenerator(ScriptingBackend scriptingBackend)
        {
            _scriptingBackend = scriptingBackend;
        }

        public override void Visit(MethodDefinition methodDefinition)
        {
            if (methodDefinition.RequiresBindingsGeneration())
                GenerateFunction(methodDefinition);
        }

        public override void Visit(PropertyDefinition propertyDefinition)
        {
            GenerateProperty(propertyDefinition);
        }

        public TreeGenerator Generate(NonFilteringVisitor visitor)
        {
            Initialize();
            visitor.Accept(this);
            return this;
        }

        public void Initialize()
        {
            _registrationCheckNamespace = new NamespaceNode
            {
                Name = Naming.RegistrationChecksNamespace,
                Comment = "If any of the following fail to compile, ensure there is a call to " + Naming.BindManagedTypeMacroName + "(<native type>, <managed type>) in the .h declaring <native type>."
            };

            _sourceFile.Namespaces.Insert(0, _registrationCheckNamespace);
            _marshallingNamespace = new NamespaceNode { Name = Naming.MarshallingNamespace };
            _sourceFile.Namespaces.Insert(1, _marshallingNamespace);
        }

        public void GenerateClass(TypeDefinition typeDefinition, GenerateReason reason)
        {
            _headerFile.AddIncludes(IncludesProvider.IncludesFor(typeDefinition));

            if (reason == GenerateReason.HasExtern)
                return;

            EnsureMarshallingGeneratedFor(typeDefinition);
        }

        void EnsureMarshallingGeneratedFor(TypeDefinition typeDefinition)
        {
            typeDefinition = TypeUtils.MarshallingType(typeDefinition).Resolve();
            //generate marshalling info for structs, IntPtrObjects, ObjectAsStruct, and UnityEngineObjects
            var scriptingObjectMarshallingType = TypeUtils.GetScriptingObjectMarshallingType(typeDefinition);

            if (!TypeUtils.IsStruct(typeDefinition) &&
                scriptingObjectMarshallingType != ScriptingObjectMarshallingType.IntPtrObject &&
                scriptingObjectMarshallingType != ScriptingObjectMarshallingType.UnityEngineObject &&
                scriptingObjectMarshallingType != ScriptingObjectMarshallingType.ObjectAsStruct &&
                !typeDefinition.IsEnum)
                return;
            //do not generate MarshalInfo for MonoBehaviour or ScriptableObject, as they are unrelated classes which both map to the same native type
            if (typeDefinition.FullName == "UnityEngine.MonoBehaviour" ||
                typeDefinition.FullName == "UnityEngine.ScriptableObject")
                return;

            var requiresMarshaling = TypeUtils.RequireMarshalling(typeDefinition, _scriptingBackend);
            var generateGenerateOption = CodegenOptions.Auto;

            IBindingsGenerateMarshallingTypeAttribute attr;
            if (typeDefinition.TryGetAttributeInstance(out attr))
                generateGenerateOption = attr.CodegenOptions;

            var intermediateStructName = TypeUtils.IntermediateTypeName(typeDefinition);

            if (!_processedTypes.Add(intermediateStructName))
                return;

            if (typeDefinition.NativeProxy() == null)
                GenerateMarshalInfo(typeDefinition, requiresMarshaling);

            if (TypeUtils.IsStruct(typeDefinition) || scriptingObjectMarshallingType == ScriptingObjectMarshallingType.ObjectAsStruct)
            {
                foreach (var fieldDefinition in typeDefinition.Fields)
                {
                    if (fieldDefinition.GetAttributeInstance<FixedBufferAttribute>() != null)
                        continue;

                    EnsureGenerated(fieldDefinition.FieldType, GenerateReason.IsFieldType);
                }

                if (generateGenerateOption != CodegenOptions.Custom)
                {
                    if (_sourceFile.DefaultNamespace.Structs.Any(n => n.Name == intermediateStructName))
                        throw new InvalidOperationException("Intermediate type " + intermediateStructName + " already exists. There may not be two structs with the same intermediate type name.");

                    var className = TypeUtils.RegisteredNameFor(typeDefinition);

                    var proxyName = typeDefinition.NativeProxy();
                    if (proxyName != null)
                        intermediateStructName = proxyName;
                    else
                        GenerateIntermediateStruct(typeDefinition, intermediateStructName);

                    if (requiresMarshaling)
                        GenerateMarshalFunctions(typeDefinition, intermediateStructName, className);
                }
            }
        }

        private void GenerateMarshalFunctions(TypeDefinition typeDefinition, string intermediateStructName,
            string className)
        {
            var marshalFunction = new FunctionNode
            {
                UseDefineSentinel = true,
                Name = Naming.MarshalFunctionName,
                TemplateSpecialization = { className, intermediateStructName },
                Parameters =
                {
                    new FunctionParameter
                    {
                        Type = className.Pointer(),
                        Name = "marshalled"
                    },
                    new FunctionParameter
                    {
                        Type = intermediateStructName.ConstPointer(),
                        Name = "unmarshalled"
                    }
                },
                IsInline = true
            };
            _marshallingNamespace.Functions.Add(marshalFunction);

            var unmarshalFunction = new FunctionNode
            {
                UseDefineSentinel = true,
                Name = Naming.UnmarshalFunctionName,
                TemplateSpecialization = { intermediateStructName, className },
                Parameters =
                {
                    new FunctionParameter
                    {
                        Type = intermediateStructName.Pointer(),
                        Name = "unmarshalled"
                    },
                    new FunctionParameter
                    {
                        Type = className.ConstPointer(),
                        Name = "marshalled"
                    }
                },
                IsInline = true
            };
            _marshallingNamespace.Functions.Add(unmarshalFunction);

            if (typeDefinition.NativeProxy() != null)
            {
                marshalFunction.Statements.Add(new ExpressionStatementNode
                {
                    Expression = new AssignNode
                    {
                        Rhs = new StringExpressionNode("*unmarshalled"),
                        Lhs = new StringExpressionNode("*marshalled")
                    }
                });
                unmarshalFunction.Statements.Add(new ExpressionStatementNode
                {
                    Expression = new AssignNode
                    {
                        Lhs = new StringExpressionNode("*unmarshalled"),
                        Rhs = new StringExpressionNode("*marshalled")
                    }
                });
            }
            else
                foreach (var field in typeDefinition.Fields)
                {
                    if (field.IsStatic || field.HasAttribute<IgnoreAttribute>() || ExcludeField(field))
                        continue;
                    if (field.HasAttribute<FixedBufferAttribute>())
                        throw new NotImplementedException("Fixed buffers are not supported in non-blittable structs");

                    var targetFieldName = TypeUtils.NativeNameFor(field, true);
                    var managedFieldName = TypeUtils.NativeNameFor(field, false);

                    marshalFunction.Statements.Add(new ExpressionStatementNode
                    {
                        // TODO (ulfj): It's not a static function call but we cannot specify namespace on free function calls yet
                        Expression = new FunctionCallNode
                        {
                            Qualifier = new StaticMemberQualifier { ClassName = "Marshalling" },
                            FunctionName = "Marshal",
                            Arguments =
                            {
                                new FunctionArgument { Name = Naming.AddressOf(string.Format("(marshalled->{0})", targetFieldName)) },
                                new FunctionArgument { Name = Naming.AddressOf(string.Format("(unmarshalled->{0})", managedFieldName)) }
                            }
                        }
                    });

                    unmarshalFunction.Statements.Add(new ExpressionStatementNode
                    {
                        // TODO (ulfj): It's not a static function call but we cannot specify namespace on free function calls yet
                        Expression = new FunctionCallNode
                        {
                            Qualifier = new StaticMemberQualifier { ClassName = "Marshalling" },
                            FunctionName = "Unmarshal",
                            Arguments =
                            {
                                new FunctionArgument { Name = Naming.AddressOf(string.Format("(unmarshalled->{0})", managedFieldName)) },
                                new FunctionArgument { Name = Naming.AddressOf(string.Format("(marshalled->{0})", targetFieldName)) }
                            }
                        }
                    });
                }
        }

        private void GenerateIntermediateStruct(TypeDefinition typeDefinition, string intermediateStructName)
        {
            var structNode = new StructDefinitionNode
            {
                ForceUniqueViaPreprocessor = true,
                Name = intermediateStructName
            };

            GenerateTypeVerification(typeDefinition);

            foreach (var fieldDefinition in typeDefinition.Fields)
            {
                if (fieldDefinition.IsStatic)
                    continue;

                if (ExcludeField(fieldDefinition))
                    continue;

                FixedBufferAttribute fixedBufferAttribute = fieldDefinition.GetAttributeInstance<FixedBufferAttribute>();
                if (fixedBufferAttribute != null)
                {
                    var fixedTypeFieldDefinition = fieldDefinition.FieldType.Resolve().Fields.First();
                    structNode.Members.Add(new FieldDeclarationStatementNode
                    {
                        Name = TypeUtils.NativeNameFor(fieldDefinition, false),
                        Type = ResolveUnmarshalledTypeNameAndGenerateTypeVerification(fixedTypeFieldDefinition.FieldType, TypeUsage.StructField),
                        ElementCount = fixedBufferAttribute.Length
                    });
                }
                else
                {
                    structNode.Members.Add(new FieldDeclarationStatementNode
                    {
                        Name = TypeUtils.NativeNameFor(fieldDefinition, false),
                        Type = ResolveUnmarshalledTypeNameAndGenerateTypeVerification(fieldDefinition.FieldType, TypeUsage.StructField),
                    });
                }
            }
            _headerFile.DefaultNamespace.Structs.Add(structNode);
        }

        private bool ExcludeField(FieldDefinition fieldDefinition)
        {
            IgnoreAttribute ignoreAttribute;
            if (fieldDefinition.TryGetAttributeInstance(out ignoreAttribute, false))
            {
                return ignoreAttribute.DoesNotContributeToSize;
            }
            return false;
        }

        private void GenerateMarshalInfo(TypeDefinition typeDefinition, bool requiresMarshaling)
        {
            bool hasIntermediateType = TypeUtils.IsStruct(typeDefinition) ||
                typeDefinition.IsEnum ||
                TypeUtils.GetScriptingObjectMarshallingType(typeDefinition) == ScriptingObjectMarshallingType.ObjectAsStruct;
            var targetStructName = hasIntermediateType ?
                TypeUtils.IntermediateTypeName(typeDefinition) :
                TypeUtils.RegisteredNameFor(typeDefinition);

            var nativeProxy = typeDefinition.NativeProxy();

            var structDeclarationNode = new StructDefinitionNode
            {
                ForceUniqueViaPreprocessor = true,
                Name = Naming.MarshalInfo,
                IsTemplate = true,
                TemplateSpecialization = { targetStructName },
                Members =
                {
                    new FieldDeclarationStatementNode
                    {
                        Name = Naming.RequiresMarshalingField,
                        Type = "bool",
                        IsStatic = true,
                        IsConst = true,
                        Initializer = new StringExpressionNode {Str = requiresMarshaling ? "true" : "false" }
                    }
                },
                FunctionDefinitions =
                {
                    new FunctionNode
                    {
                        Name = Naming.ScriptingClassFunction,
                        IsStatic = true,
                        IsInline = true,
                        ReturnType = "ScriptingClassPtr",
                        Statements = new List<IStatementNode>
                        {
                            new StringStatementNode
                            {
                                Str = nativeProxy ??
                                    // fast path
                                    string.Format("return RequireType (\"{0}\", \"{1}\", \"{2}\")",
                                    System.IO.Path.GetFileName(typeDefinition.Module.FileName),
                                    typeDefinition.SafeNamespace(), typeDefinition.NestedName("/"))
                                    // slow path
                            }
                        }
                    }
                }
            };
            if (!requiresMarshaling && hasIntermediateType)
            {
                var sizeCheckStatement = new StringStatementNode
                {
                    Str = $"ASSERT_SAME_SIZE({targetStructName}, {TypeUtils.RegisteredNameFor(typeDefinition)}, \"Blittable type {typeDefinition.Name} size does not match registered type size\")"
                };
                structDeclarationNode.FunctionDefinitions[0].Statements.Insert(0, sizeCheckStatement);

                if (typeDefinition.IsEnum)
                {
                    var podCheckStatement = new StringStatementNode
                    {
                        Str = $"ASSERT_IS_POD({TypeUtils.RegisteredNameFor(typeDefinition)}, \"Type bound to an enum {typeDefinition.Name} must be a POD type\")"
                    };
                    structDeclarationNode.FunctionDefinitions[0].Statements.Insert(0, podCheckStatement);
                }
            }
            _marshallingNamespace.Structs.Add(structDeclarationNode);
        }

        void GenerateTypeVerification(TypeReference typeReference)
        {
            typeReference = typeReference.GetElementType(); // GetElementType to remove array '[]', reference '&'

            var sourceType = TypeUtils.RegisteredNameFor(typeReference);
            var flattenedManagedTypeName = sourceType.Replace(Naming.BindingsTypePrefix, "");

            var checkTypeDef = new TypeDefNode
            {
                SourceType = sourceType,
                DestType = Naming.VerifyBindingNameFor(flattenedManagedTypeName)
            };
            if (_registrationCheckNamespace.TypeDefs.All(t => t.SourceType != checkTypeDef.SourceType))
                _registrationCheckNamespace.TypeDefs.Add(checkTypeDef);
        }

        class Parameter
        {
            public TypeReference ManagedType { get; set; }
            public ParameterDefinition ManagedParameter { get; set; }
            public FunctionParameter NativeParameter { get; set; }
            public bool MustMarshal { get; set; }
            public bool RequiresBoxing { get; set; }
            public bool CheckNull { get; set; }
        }

        public void GenerateFunction(MethodDefinition methodDefinition)
        {
            var isGetter = methodDefinition.IsGetter;
            var isSetter = methodDefinition.IsSetter;
            var propertyDefinition = methodDefinition.GetMatchingPropertyDefinition();

            if (isSetter || isGetter)
                EnsureGenerated(propertyDefinition, GenerateReason.HasExtern);

            EnsureGenerated(methodDefinition.DeclaringType, GenerateReason.HasExtern);
            EnsureGenerated(methodDefinition.ReturnType, GenerateReason.IsReturnType);

            _headerFile.AddIncludes(IncludesProvider.IncludesFor(methodDefinition));

            var isStatic = methodDefinition.IsStatic;

            IBindingsIsThreadSafeProviderAttribute isThreadSafeAttr;

            var isThreadSafe = methodDefinition.TryGetAttributeInstance(out isThreadSafeAttr)
                ? isThreadSafeAttr.IsThreadSafe
                : methodDefinition.HasAttribute<ThreadAndSerializationSafeAttribute>();

            if (propertyDefinition != null)
            {
                if (propertyDefinition.TryGetAttributeInstance(out isThreadSafeAttr))
                    isThreadSafe = isThreadSafeAttr.IsThreadSafe;
                else if (methodDefinition.HasAttribute<ThreadAndSerializationSafeAttribute>())
                    isThreadSafe = true;
            }

            var isMemberFunction = !isStatic;
            var mustReturnStructAsOutParameter = TypeUtils.IsStruct(methodDefinition.ReturnType);
            var hasThrowsAttribute = ThrowsManagedException(methodDefinition, propertyDefinition);
            var isExternInstanceMethodInStruct = TypeUtils.IsExternInstanceMethodInStruct(methodDefinition, _scriptingBackend);

            var parameters = new List<Parameter>();
            var arguments = new List<FunctionArgument>();

            if (isExternInstanceMethodInStruct)
            {
                parameters.Add(
                    new Parameter
                    {
                        ManagedType = methodDefinition.DeclaringType,
                        MustMarshal = !TypeUtils.IsNativePodType(methodDefinition.DeclaringType, _scriptingBackend),
                        CheckNull = false,
                        NativeParameter = new FunctionParameter
                        {
                            Name = Naming.Self,
                            Type = ResolveUnmarshalledTypeNameAndGenerateTypeVerification(methodDefinition.DeclaringType, TypeUsage.StructThisParameter),
                            Usage = TypeUsage.StructThisParameter,
                            IsSelf = true,
                        }
                    });
            }
            else if (isMemberFunction)
            {
                var isWritableSelf = false;
                IBindingsWritableSelfProviderAttribute isWritableSelfAttribute;
                if (methodDefinition.TryGetAttributeInstance(out isWritableSelfAttribute))
                    isWritableSelf = isWritableSelfAttribute.WritableSelf;

                var selfParameterUsage = isWritableSelf ? TypeUsage.WritableParameter : TypeUsage.Parameter;
                var argumentType = ResolveUnmarshalledTypeNameAndGenerateTypeVerification(methodDefinition.DeclaringType, selfParameterUsage);
                parameters.Add(
                    new Parameter
                    {
                        ManagedType = methodDefinition.DeclaringType,
                        MustMarshal = true,
                        CheckNull = true,
                        RequiresBoxing = methodDefinition.DeclaringType.NativeProxy() != null,
                        NativeParameter = new FunctionParameter
                        {
                            Name = Naming.Self,
                            Type = argumentType,
                            Usage = selfParameterUsage,
                            IsSelf = true,
                        }
                    });
            }

            foreach (var p in methodDefinition.Parameters)
            {
                var usage = TypeUsageFor(p);
                var name = isSetter ? Naming.PropertyValue : p.Name;
                var requiresMarshalling = TypeUtils.RequiresMarshalling(p.ParameterType, usage, _scriptingBackend);
                var checkNull = TypeUtils.CouldBeNull(p.ParameterType) && p.HasAttribute<NotNullAttribute>();

                parameters.Add(
                    new Parameter
                    {
                        ManagedType = p.ParameterType,
                        ManagedParameter = p,
                        MustMarshal = requiresMarshalling,
                        CheckNull = checkNull,
                        RequiresBoxing = p.ParameterType.Resolve()?.NativeProxy() != null,
                        NativeParameter = new FunctionParameter
                        {
                            Name = name,
                            Type = ResolveUnmarshalledTypeNameAndGenerateTypeVerification(p.ParameterType, usage),
                            Usage = usage,
                        }
                    });

                if (usage != TypeUsage.UnmarshalledParameter)
                    EnsureGenerated(p.ParameterType, GenerateReason.IsArgumentType);

                var argumentName = requiresMarshalling ? name.Marshalled() : name;
                arguments.Add(new FunctionArgument {Name = argumentName});
            }

            if (hasThrowsAttribute)
                arguments.Add(new FunctionArgument { Name = Naming.AddressOf(Naming.ExceptionVariableName) });

            if (mustReturnStructAsOutParameter)
            {
                parameters.Add(new Parameter
                {
                    ManagedType = methodDefinition.ReturnType,
                    MustMarshal = TypeUtils.RequiresMarshalling(methodDefinition.ReturnType, TypeUsage.ReturnParameter, _scriptingBackend),
                    NativeParameter = new FunctionParameter { Name = Naming.ReturnVar, Type = ResolveUnmarshalledTypeNameAndGenerateTypeVerification(methodDefinition.ReturnType, TypeUsage.ReturnParameter), IsOutReturn = true}
                });
            }

            var returnType = mustReturnStructAsOutParameter ? Naming.VoidType : ResolveUnmarshalledTypeNameAndGenerateTypeVerification(methodDefinition.ReturnType, TypeUsage.ReturnType, false);

            var functionNode = new FunctionNode
            {
                Name = TypeUtils.ResolveFunctionName(methodDefinition),
                ManagedName = TypeUtils.ResolveManagedFunctionName(methodDefinition),
                DeclSpec = Naming.DeclSpec,
                CallingConvention = Naming.CallingConvention,
                ReturnType = returnType,
                Parameters = parameters.Select(p =>
                    p.RequiresBoxing ?
                    new FunctionParameter
                    {
                        Name = "_" + p.NativeParameter.Name,
                        Type = ResolveUnmarshalledTypeNameAndGenerateTypeVerification(p.ManagedType, p.NativeParameter.Usage, false),
                        Usage = p.NativeParameter.Usage,
                    }
                    : p.NativeParameter
                    ).ToList()
            };

            if (mustReturnStructAsOutParameter || TypeUtils.MethodHasByValueStructParameter(methodDefinition) || isExternInstanceMethodInStruct)
            {
                functionNode.Name = Naming.ByRefInjectedMethod(TypeUtils.ResolveFunctionName(methodDefinition, true));
                functionNode.ManagedName = Naming.ByRefInjectedMethod(functionNode.ManagedName);
            }

            foreach (var p in parameters)
            {
                if (p.RequiresBoxing)
                {
                    var type = ResolveUnmarshalledTypeNameAndGenerateTypeVerification(p.ManagedType, p.NativeParameter.Usage);
                    functionNode.Statements.Add(
                        new VariableDeclarationStatementNode(
                            $"{p.NativeParameter.Name}(_{p.NativeParameter.Name})",
                            type
                        )
                    );
                }
            }

            GenerateFunctionCheckStatements(functionNode, methodDefinition.Name, functionNode.Name, isThreadSafe);

            functionNode.Statements.AddRange(GenerateParameterMarshallingStatements(parameters));
            functionNode.Statements.AddRange(GenerateParameterNullCheckStatements(parameters));

            functionNode.Statements.AddRange(GenerateStaticAccessorNullCheckStatements(methodDefinition));

            var isExternInstanceMethodInBlittableStruct = isExternInstanceMethodInStruct && TypeUtils.IsNativePodType(methodDefinition.DeclaringType, _scriptingBackend);

            var callNode = InvocationFor(methodDefinition, arguments, propertyDefinition, isExternInstanceMethodInBlittableStruct);

            if (mustReturnStructAsOutParameter)
                functionNode.Statements.AddRange(GenerateOutParameterMarshallingStatements(methodDefinition.ReturnType, callNode, hasThrowsAttribute));
            else
                functionNode.Statements.AddRange(GenerateReturnMarshallingStatements(methodDefinition.ReturnType, callNode, hasThrowsAttribute));


            bool generateExceptionSupport = hasThrowsAttribute || parameters.Any(p => p.CheckNull);
            if (generateExceptionSupport)
            {
                GenerateExceptionSupport(functionNode);
            }

            StubUtils.AddStubIfApplicable(methodDefinition, functionNode, mustReturnStructAsOutParameter);

            _sourceFile.DefaultNamespace.Functions.Add(functionNode);
        }

        private static TypeUsage TypeUsageFor(ParameterDefinition p)
        {
            if (p.IsOut)
                return TypeUsage.OutParameter;

            if (p.HasAttribute<WritableAttribute>())
                return TypeUsage.WritableParameter;

            if (p.HasAttribute<UnmarshalledAttribute>())
                return TypeUsage.UnmarshalledParameter;

            return TypeUsage.Parameter;
        }

        static IExpressionNode InvocationFor(MethodDefinition methodDefinition, List<FunctionArgument> arguments, PropertyDefinition propertyDefinition, bool isExternInstanceMethodInBlittableStruct)
        {
            NativePropertyAttribute nativePropertyAttribute;
            if (propertyDefinition != null &&
                (methodDefinition.TryGetAttributeInstance(out nativePropertyAttribute) ||
                 propertyDefinition.TryGetAttributeInstance(out nativePropertyAttribute)) &&
                nativePropertyAttribute.TargetType == TargetType.Field)
            {
                var fieldReferenceNode = FieldReferenceFor(methodDefinition, propertyDefinition, nativePropertyAttribute, nativePropertyAttribute.IsFreeFunction, isExternInstanceMethodInBlittableStruct);
                if (methodDefinition.IsGetter)
                    return fieldReferenceNode;
                else
                {
                    Debug.Assert(methodDefinition.IsSetter);
                    Debug.Assert(arguments.Count == 1);
                    return new AssignNode
                    {
                        Lhs = fieldReferenceNode,
                        Rhs = new StringExpressionNode {Str = arguments[0].Name}
                    };
                }
            }
            return FunctionCallNodeFor(methodDefinition, arguments, isExternInstanceMethodInBlittableStruct);
        }

        private static FieldReferenceNode FieldReferenceFor(MethodDefinition methodDefinition, PropertyDefinition propertyDefinition, NativePropertyAttribute nativePropertyAttribute, bool isFree, bool isExternInstanceMethodInBlittableStruct)
        {
            IMemberQualifier qualifier;
            StaticAccessorAttribute staticAccessorAttribute;
            if (!methodDefinition.IsStatic)
            {
                var thisExpression = isExternInstanceMethodInBlittableStruct ? Naming.Self : Naming.Self.Marshalled();
                qualifier = new InstanceMemberQualifier { Expression = thisExpression };
            }
            else if (methodDefinition.TryGetAttributeInstance(out staticAccessorAttribute) ||
                     propertyDefinition.TryGetAttributeInstance(out staticAccessorAttribute) ||
                     methodDefinition.DeclaringType.TryGetAttributeInstance(out staticAccessorAttribute))
            {
                qualifier = QualifierForStaticAccessorAttribute(staticAccessorAttribute);
            }
            else if (isFree)
            {
                qualifier = new FreeQualifier();
            }
            else
            {
                qualifier = new StaticMemberQualifier { ClassName = TypeUtils.RegisteredNameFor(methodDefinition.DeclaringType) };
            }
            string name;

            NativeNameAttribute nameAttribute;
            if (methodDefinition.TryGetAttributeInstance(out nameAttribute))
                name = nameAttribute.Name;
            else
                name = nativePropertyAttribute.Name ?? propertyDefinition.Name;

            return new FieldReferenceNode
            {
                Qualifier = qualifier,
                Name = name
            };
        }

        static FunctionCallNode FunctionCallNodeFor(MethodDefinition methodDefinition, List<FunctionArgument> arguments, bool isExternInstanceMethodInBlittableStruct)
        {
            var isFreeFunction = false;
            var explicitThis = false;
            IBindingsIsFreeFunctionProviderAttribute isFreeFunctionProvider;
            if (methodDefinition.TryGetAttributeInstance(out isFreeFunctionProvider))
            {
                isFreeFunction = isFreeFunctionProvider.IsFreeFunction;
                explicitThis = isFreeFunctionProvider.HasExplicitThis;
            }

            IMemberQualifier qualifier;

            var isStatic = methodDefinition.IsStatic;
            StaticAccessorAttribute staticAccessorAttr;

            if (!isStatic)
            {
                var thisArgument = isExternInstanceMethodInBlittableStruct ? Naming.Self : Naming.Self.Marshalled();
                if (explicitThis)
                {
                    arguments.Insert(0, new FunctionArgument
                    {
                        Name = thisArgument
                    });
                    qualifier = FreeQualifier.Instance;
                }
                else
                    qualifier = new InstanceMemberQualifier { Expression = thisArgument };
            }
            else if (isFreeFunction)
                qualifier = FreeQualifier.Instance;
            else if (!methodDefinition.TryGetAttributeInstance(out staticAccessorAttr) &&
                     !(methodDefinition.GetMatchingPropertyDefinition() != null && methodDefinition.GetMatchingPropertyDefinition().TryGetAttributeInstance(out staticAccessorAttr)) &&
                     !methodDefinition.DeclaringType.TryGetAttributeInstance(out staticAccessorAttr))
            {
                qualifier = new StaticMemberQualifier { ClassName = TypeUtils.RegisteredNameFor(methodDefinition.DeclaringType) };
            }
            else
            {
                qualifier = QualifierForStaticAccessorAttribute(staticAccessorAttr);
            }

            return new FunctionCallNode
            {
                Qualifier = qualifier,
                FunctionName = TypeUtils.NativeNameFor(methodDefinition),
                Arguments = arguments
            };
        }

        private static IMemberQualifier QualifierForStaticAccessorAttribute(StaticAccessorAttribute staticAccessorAttr)
        {
            if (staticAccessorAttr.Type == StaticAccessorType.DoubleColon)
            {
                return new StaticMemberQualifier
                {
                    ClassName = staticAccessorAttr.Name
                };
            }
            else
            {
                return new InstanceMemberQualifier
                {
                    Expression = staticAccessorAttr.Name,
                    IsReference = staticAccessorAttr.Type == StaticAccessorType.Dot
                };
            }
        }

        static bool ThrowsManagedException(MethodDefinition methodDefinition, PropertyDefinition propertyDefinitionn)
        {
            IBindingsThrowsProviderAttribute provider;

            if (methodDefinition.TryGetAttributeInstance(out provider))
                return provider.ThrowsException;

            return propertyDefinitionn != null && propertyDefinitionn.TryGetAttributeInstance(out provider) && provider.ThrowsException;
        }

        static void GenerateExceptionSupport(FunctionNode functionNode)
        {
            var blockNode = new BlockNode
            {
                Statements = new List<IStatementNode>(functionNode.Statements)
            };

            InjectReturnStatementIfNecessary(blockNode);

            var exceptionDeclaration = new VariableDeclarationStatementNode
            {
                Name = Naming.ExceptionVariableName,
                Type = Naming.ScriptingExceptionType,
                Initializer = new StringExpressionNode { Str = "SCRIPTING_NULL"}
            };

            var labelNode = new LabelNode {Name = Naming.HandleExceptionLabel };

            var raiseExceptionCall = new ExpressionStatementNode
            {
                Expression = new FunctionCallNode
                {
                    Qualifier = FreeQualifier.Instance,
                    FunctionName = Naming.ScriptingRaiseExceptionFunction,
                    Arguments = { new FunctionArgument { Name = Naming.ExceptionVariableName } }
                }
            };

            functionNode.Statements = new List<IStatementNode>
            {
                exceptionDeclaration,
                blockNode,
                labelNode,
                raiseExceptionCall
            };
        }

        private static void InjectReturnStatementIfNecessary(BlockNode blockNode)
        {
            if (!(blockNode.Statements.Last() is ReturnStatementNode))
                blockNode.Statements.Add(new ReturnStatementNode());
        }

        public void EnsureGenerated(PropertyReference propertyReference, GenerateReason reason)
        {
            var propertyDefinition = propertyReference.Resolve();

            GenerateProperty(propertyDefinition);
        }

        public enum GenerateReason
        {
            HasExtern,
            IsReturnType,
            IsArgumentType,
            IsFieldType,
        }

        public void EnsureGenerated(TypeReference typeReference, GenerateReason reason)
        {
            var typeDefinition = typeReference.Resolve();

            if (typeDefinition.IsEnum)
                GenerateEnum(typeDefinition, _sourceFile, reason);
            else if (TypeUtils.IsStruct(typeDefinition))
                GenerateStruct(typeDefinition, reason);
            else if (typeDefinition.IsClass)
                GenerateClass(typeDefinition, reason);
        }

        public void GenerateProperty(PropertyDefinition propertyDefinition)
        {
            _headerFile.AddIncludes(IncludesProvider.IncludesFor(propertyDefinition));

            EnsureGenerated(propertyDefinition.DeclaringType, GenerateReason.HasExtern);
        }

        public void GenerateStruct(TypeDefinition structDefinition, GenerateReason reason)
        {
            _headerFile.AddIncludes(IncludesProvider.IncludesFor(structDefinition));

            if (reason == GenerateReason.HasExtern)
                return;

            EnsureMarshallingGeneratedFor(structDefinition);
        }

        public IEnumerable<IStatementNode> GenerateOutParameterMarshallingStatements(TypeReference returnType, IExpressionNode callNode, bool hasThrowAttribute)
        {
            var statements = new List<IStatementNode>();

            if (TypeUtils.RequiresMarshalling(returnType, TypeUsage.ReturnType, _scriptingBackend))
            {
                statements.Add(new VariableDeclarationStatementNode
                {
                    Type = ResolveMarshalledTypeNameAndGenerateTypeVerification(returnType, TypeUsage.ReturnParameter),
                    Name = Naming.ReturnVar.Marshalled(),
                    Initializer = callNode
                });

                if (hasThrowAttribute)
                    AddExceptionCheckStatement(statements);

                var marshallingArguments = new List<FunctionArgument>
                {
                    new FunctionArgument {Name = Naming.AddressOf(Naming.ReturnVar)},
                    new FunctionArgument {Name = Naming.AddressOf(Naming.ReturnVar.Marshalled())}
                };
                statements.Add(new ExpressionStatementNode
                {
                    Expression = new FunctionCallNode
                    {
                        Qualifier = new StaticMemberQualifier { ClassName = "Marshalling" },
                        FunctionName = "Unmarshal",
                        Arguments = marshallingArguments
                    }
                });
            }
            else
            {
                statements.Add(new ExpressionStatementNode
                {
                    Expression = new AssignNode
                    {
                        Lhs = new StringExpressionNode { Str = Naming.ReturnVar },
                        Rhs = callNode
                    }
                });

                if (hasThrowAttribute)
                    AddExceptionCheckStatement(statements);
            }

            return statements;
        }

        string ResolveUnmarshalledTypeNameAndGenerateTypeVerification(TypeReference type, TypeUsage usage, bool allowProxyStructs = true)
        {
            var unmarshalledTypeName = TypeUtils.ResolveUnmarshalledTypeName(type, usage, _scriptingBackend, allowProxyStructs);
            if (unmarshalledTypeName.ExposesManagedTypeNameFor(type))
                GenerateTypeVerification(type);

            return unmarshalledTypeName;
        }

        string ResolveMarshalledTypeNameAndGenerateTypeVerification(TypeReference returnType, TypeUsage typeUsage)
        {
            var marshalledTypeName = TypeUtils.ResolveMarshalledTypeName(returnType, typeUsage, _scriptingBackend);
            if (marshalledTypeName.ExposesManagedTypeNameFor(returnType))
                GenerateTypeVerification(returnType);

            return marshalledTypeName;
        }

        public IEnumerable<IStatementNode> GenerateReturnMarshallingStatements(TypeReference returnType, IExpressionNode callNode, bool hasThrowAttribute)
        {
            var statements = new List<IStatementNode>();

            if (returnType.MetadataType == MetadataType.Void)
            {
                statements.Add(new ExpressionStatementNode { Expression = callNode });

                if (hasThrowAttribute)
                    AddExceptionCheckStatement(statements);
            }
            else if (TypeUtils.RequiresMarshalling(returnType, TypeUsage.ReturnType, _scriptingBackend))
            {
                statements.Add(new VariableDeclarationStatementNode
                {
                    Type = ResolveMarshalledTypeNameAndGenerateTypeVerification(returnType, TypeUsage.ReturnType),
                    Name = Naming.ReturnVar,
                    Initializer = callNode
                });

                if (hasThrowAttribute)
                    AddExceptionCheckStatement(statements);

                statements.Add(new VariableDeclarationStatementNode
                {
                    Type = ResolveUnmarshalledTypeNameAndGenerateTypeVerification(returnType, TypeUsage.ReturnType),
                    Name = Naming.ReturnVar.Unmarshalled()
                });

                var marshallingArguments = new List<FunctionArgument>
                {
                    new FunctionArgument {Name = Naming.AddressOf(Naming.ReturnVar.Unmarshalled())},
                    new FunctionArgument {Name = Naming.AddressOf(Naming.ReturnVar) }
                };
                statements.Add(new ExpressionStatementNode
                {
                    Expression = new FunctionCallNode
                    {
                        Qualifier = new StaticMemberQualifier { ClassName = "Marshalling" },
                        FunctionName = "Unmarshal",
                        Arguments = marshallingArguments
                    }
                });

                statements.Add(new ReturnStatementNode { Expression = new StringExpressionNode { Str = Naming.ReturnVar.Unmarshalled() } });
            }
            else
            {
                if (!hasThrowAttribute)
                    statements.Add(new ReturnStatementNode {Expression = callNode});
                else
                {
                    statements.Add(new VariableDeclarationStatementNode
                    {
                        Type = ResolveUnmarshalledTypeNameAndGenerateTypeVerification(returnType, TypeUsage.ReturnType),
                        Name = Naming.ReturnVar,
                        Initializer = callNode
                    });

                    AddExceptionCheckStatement(statements);

                    statements.Add(new ReturnStatementNode {Expression = new StringExpressionNode {Str = Naming.ReturnVar}});
                }
            }

            return statements;
        }

        private void AddExceptionCheckStatement(List<IStatementNode> statements)
        {
            statements.Add(new IfStatementNode
            {
                Condition = new StringExpressionNode {Str = Naming.ExceptionVariableName + " != " + Naming.ScriptingNull},
                Statements = {new StringStatementNode {Str = "goto " + Naming.HandleExceptionLabel}}
            });
        }

        private void GenerateFunctionCheckStatements(FunctionNode functionNode, string managedMethodName, string nativeFunctionName, bool isThreadSafe)
        {
            functionNode.Statements.Add(new StringStatementNode { Str = string.Format("{0}({1})", Naming.EtwEntryMacro, nativeFunctionName) });

            functionNode.Statements.Add(new StringStatementNode { Str = string.Format("{0}({1})", Naming.StackCheckMacro, managedMethodName) });

            if (!isThreadSafe)
                functionNode.Statements.Add(new StringStatementNode { Str = string.Format("{0}({1})", Naming.ThreadSafeMacro, managedMethodName) });
        }

        private IEnumerable<IStatementNode> GenerateParameterNullCheckStatements(List<Parameter> parameters)
        {
            var statements = new List<IStatementNode>();

            for (int i = 0; i < parameters.Count; i++)
            {
                var p = parameters[i];
                var nativeParameter = p.NativeParameter;

                if (!p.CheckNull)
                    continue;

                var scriptingObjectMarshallingType = TypeUtils.GetScriptingObjectMarshallingType(p.ManagedType);

                var condition = TypeUtils.NullCheckFor(nativeParameter.Name, nativeParameter.Usage, scriptingObjectMarshallingType);

                var nullCheckStatement = new IfStatementNode();
                nullCheckStatement.Condition = new StringExpressionNode
                {
                    Str = condition
                };

                FunctionCallNode createExceptionNode;
                if (scriptingObjectMarshallingType == ScriptingObjectMarshallingType.UnityEngineObject && p.NativeParameter.IsSelf)
                {
                    createExceptionNode = new FunctionCallNode
                    {
                        Qualifier = new FreeQualifier(),
                        FunctionName = "Marshalling::CreateNullExceptionForUnityEngineObject",
                        Arguments = { new FunctionArgument { Name = nativeParameter.Name } }
                    };
                }
                else
                {
                    createExceptionNode = new FunctionCallNode
                    {
                        Qualifier = new FreeQualifier(),
                        FunctionName = "Scripting::CreateArgumentNullException",
                        Arguments = { new FunctionArgument { Name = String.Format("\"{0}\"", nativeParameter.Name) } }
                    };
                }
                nullCheckStatement.Statements = new List<IStatementNode>
                {
                    new ExpressionStatementNode
                    {
                        Expression = new AssignNode
                        {
                            Lhs = new StringExpressionNode { Str = Naming.ExceptionVariableName },
                            Rhs = createExceptionNode
                        }
                    },
                    new StringStatementNode { Str = "goto " + Naming.HandleExceptionLabel }
                };

                statements.Add(nullCheckStatement);
            }

            return statements;
        }

        public static IStatementNode MarshallingStatementFor(string variable)
        {
            var arguments = new List<FunctionArgument>
            {
                new FunctionArgument {Name = Naming.AddressOf(variable.Marshalled())},
                new FunctionArgument {Name = Naming.AddressOf(variable)}
            };

            return new ExpressionStatementNode
            {
                Expression = new FunctionCallNode
                {
                    Qualifier = new StaticMemberQualifier { ClassName = "Marshalling" },
                    FunctionName = Naming.MarshalFunctionName,
                    Arguments = arguments
                }
            };
        }

        private IEnumerable<IStatementNode> GenerateParameterMarshallingStatements(List<Parameter> parameters)
        {
            var statements = new List<IStatementNode>();

            for (var i = 0; i < parameters.Count; ++i)
            {
                var p = parameters[i];
                if (!MustGenerateMarshallingStatementFor(p))
                    continue;

                var type = ResolveMarshalledTypeNameAndGenerateTypeVerification(p.ManagedType, p.NativeParameter.Usage);
                statements.Add(new VariableDeclarationStatementNode { Type = type, Name = p.NativeParameter.Name.Marshalled() });
            }

            for (var i = 0; i < parameters.Count; ++i)
            {
                var p = parameters[i];
                if (!MustGenerateMarshallingStatementFor(p))
                    continue;

                statements.Add(MarshallingStatementFor(p.NativeParameter.Name));
            }

            return statements;
        }

        private IEnumerable<IStatementNode> GenerateStaticAccessorNullCheckStatements(MethodDefinition methodDefinition)
        {
            var statements = new List<IStatementNode>();

            StaticAccessorAttribute staticAccessorAttr;
            if (methodDefinition.TryGetAttributeInstance(out staticAccessorAttr) ||
                (methodDefinition.GetMatchingPropertyDefinition() != null && methodDefinition.GetMatchingPropertyDefinition().TryGetAttributeInstance(out staticAccessorAttr)) ||
                methodDefinition.DeclaringType.TryGetAttributeInstance(out staticAccessorAttr))
            {
                if (staticAccessorAttr.Type == StaticAccessorType.ArrowWithDefaultReturnIfNull)
                {
                    statements.Add(new IfStatementNode()
                    {
                        Condition = new StringExpressionNode { Str = staticAccessorAttr.Name + " == NULL" },
                        Statements = { new ReturnStatementNode { Expression = new StringExpressionNode { Str = StubUtils.DefaultNativeReturnValue(methodDefinition.ReturnType) } } }
                    });
                }
            }

            return statements;
        }

        private static bool MustGenerateMarshallingStatementFor(Parameter p)
        {
            if (p.NativeParameter.IsOutReturn)
                return false;

            return p.MustMarshal;
        }

        public void GenerateEnum(TypeDefinition typeDefinition, FileNode fileNode, GenerateReason reason)
        {
            _headerFile.AddIncludes(IncludesProvider.IncludesFor(typeDefinition));
            EnsureMarshallingGeneratedFor(typeDefinition);

            var nativeEnumName = TypeUtils.IntermediateTypeName(typeDefinition);
            if (_headerFile.DefaultNamespace.Structs.Any(e => e.Name == nativeEnumName))
                return;

            var valueField = typeDefinition.Fields.First(f => f.Name == "value__");

            var enumNode = new StructDefinitionNode
            {
                Name = nativeEnumName,
                ForceUniqueViaPreprocessor = true,
                Members = { new FieldDeclarationStatementNode("value", TypeUtils.ResolvePrimitiveNativeTypeName(valueField.FieldType)) }
            };

            _headerFile.DefaultNamespace.Structs.Add(enumNode);
        }

        private static TypeSystem GetTypeSystem(TypeReference p)
        {
            return p.Module.TypeSystem;
        }
    }
}
