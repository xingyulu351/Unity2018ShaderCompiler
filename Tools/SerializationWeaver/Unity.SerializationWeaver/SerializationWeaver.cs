using System.Diagnostics;
using System.Linq;
using Mono.Cecil;
using Mono.Cecil.Cil;

namespace Unity.Serialization.Weaver
{
    public class SerializationWeaver
    {
        public static void Weave(TypeDefinition typeDef, AssemblyDefinition serializationInferfaceProvider)
        {
            var weaver = new SerializationWeaver(typeDef, serializationInferfaceProvider);
            weaver.Weave();
        }

        public static void FinalizeModuleWeaving(ModuleDefinition module)
        {
            CStringStore.SaveCStringStore(module);
        }

        private readonly TypeDefinition _typeDef;
        private readonly SerializationBridgeProvider _serializationBridgeProvider;

        private SerializationWeaver(TypeDefinition typeDef, AssemblyDefinition unityEngineAssembly)
        {
            _typeDef = typeDef;
            _serializationBridgeProvider = new SerializationBridgeProvider(unityEngineAssembly);
        }

        private void Weave()
        {
            if (!MakeImplement(_serializationBridgeProvider.UnitySerializableInterface))
                return;

            MakeSureUsableCtorExists();
            PromoteToPublicIfNeeded();
            AddSerializeMethod();
            AddDeserializeMethod();
            AddRemapPPtrsMethod();
            AddNamedSerializeMethod();
            AddNamedDeserializeMethod();
        }

        private void PromoteToPublicIfNeeded()
        {
            if (_typeDef.IsNotPublic)
            {
                _typeDef.IsNotPublic = false;
                _typeDef.IsPublic = true;
            }
        }

        private void MakeSureUsableCtorExists()
        {
            bool useableDefault = false;
            var defaultCtor = EmptyConstructorFor(_typeDef);

            if (defaultCtor != null)
            {
                var ctor = defaultCtor.Resolve();
                useableDefault = true;

                if (!ctor.IsPublic)
                {
                    ctor.IsPublic = true;
                }
            }

            if (!useableDefault)
            {
                AddSerializationWeaverConstructor();
            }
        }

        private void AddSerializationWeaverConstructor()
        {
            var swConstructor = new MethodDefinition(
                ".ctor", DefaultConstructorAttributes(), Module.TypeSystem.Void);
            swConstructor.Parameters.Add(SelfUnitySerializableParam());
            var ilProcessor = swConstructor.Body.GetILProcessor();

            // Don't call base constructors for structures
            if (_typeDef.IsValueType == false)
            {
                if (_typeDef.BaseType != null)
                {
                    var injectedBaseCtor = SerializationWeaverInjectedConstructorFor(_typeDef.BaseType);
                    if (injectedBaseCtor != null)
                    {
                        ilProcessor.Emit(OpCodes.Ldarg_0);
                        ilProcessor.Emit(OpCodes.Ldarg_1);
                        ilProcessor.Emit(OpCodes.Call, injectedBaseCtor);
                    }
                    else
                    {
                        var emptyCtor = EmptyConstructorFor(_typeDef);
                        if (emptyCtor != null)
                        {
                            ilProcessor.Emit(OpCodes.Ldarg_0);
                            ilProcessor.Emit(OpCodes.Call, emptyCtor);
                        }
                        else
                        {
                            var baseEmptyCtor = EmptyConstructorFor(_typeDef.BaseType);
                            if (baseEmptyCtor != null)
                            {
                                ilProcessor.Emit(OpCodes.Ldarg_0);
                                ilProcessor.Emit(OpCodes.Call, baseEmptyCtor);
                            }
                        }
                    }
                }
                else
                {
                    var emptyCtor = EmptyConstructorFor(_typeDef);
                    if (emptyCtor != null)
                    {
                        var methodDefinition = emptyCtor.Resolve();
                        if (!methodDefinition.IsPrivate)
                        {
                            ilProcessor.Emit(OpCodes.Ldarg_0);
                            ilProcessor.Emit(OpCodes.Call, emptyCtor);
                        }
                    }
                }
            }

            ilProcessor.Emit(OpCodes.Ret);
            _typeDef.Methods.Add(swConstructor);
        }

        private MethodReference SerializationWeaverInjectedConstructorFor(TypeReference typeReference)
        {
            var typeDefinition = typeReference.Resolve();
            if (typeDefinition == null)
                return null;

            var ctorDefinition = typeDefinition.Methods.SingleOrDefault(IsSerializationWeaverInjectedConstructor);
            if (ctorDefinition == null)
                return null;

            var methodReference = new MethodReference(".ctor", Module.TypeSystem.Void, typeReference)
            {
                HasThis = true
            };
            methodReference.Parameters.Add(SelfUnitySerializableParam());
            return methodReference;
        }

        private MethodReference EmptyConstructorFor(TypeReference typeReference)
        {
            var typeDefinition = typeReference.Resolve();
            if (typeDefinition == null)
                return null;

            var ctorDefinition = typeDefinition.Methods.SingleOrDefault(IsEmptyConstructor);
            if (ctorDefinition == null)
                return null;

            return new MethodReference(".ctor", Module.TypeSystem.Void, typeReference)
            {
                HasThis = true
            };
        }

        private static bool IsEmptyConstructor(MethodDefinition methodDefinition)
        {
            return (methodDefinition.Name == ".ctor" && !methodDefinition.HasParameters);
        }

        private bool IsSerializationWeaverInjectedConstructor(MethodDefinition methodDefinition)
        {
            if (methodDefinition.Name != ".ctor" || methodDefinition.Parameters.Count != 1)
                return false;

            return methodDefinition.Parameters[0].ParameterType.FullName == _serializationBridgeProvider.UnitySerializableInterface.FullName;
        }

        private ParameterDefinition SelfUnitySerializableParam()
        {
            return new ParameterDefinition(
                "self",
                ParameterAttributes.None,
                Module.ImportReference(_serializationBridgeProvider.UnitySerializableInterface));
        }

        private static MethodAttributes DefaultConstructorAttributes()
        {
            return MethodAttributes.Public
                | MethodAttributes.HideBySig
                | MethodAttributes.SpecialName
                | MethodAttributes.RTSpecialName
                | MethodAttributes.CompilerControlled;
        }

        private void AddRemapPPtrsMethod()
        {
            _typeDef.Methods.Add(RemapPPtrsMethodEmitter.RemapPPtrsMethodDefinitionFor(_typeDef, _serializationBridgeProvider));
        }

        private void AddNamedSerializeMethod()
        {
            _typeDef.Methods.Add(NamedSerializeMethodEmitter.SerializeMethodDefinitionFor(_typeDef, _serializationBridgeProvider));
        }

        private void AddNamedDeserializeMethod()
        {
            _typeDef.Methods.Add(NamedDeserializeMethodEmitter.DeserializeMethodDefinitionFor(_typeDef, _serializationBridgeProvider));
        }

        private void AddSerializeMethod()
        {
            _typeDef.Methods.Add(SerializeMethodEmitter.SerializeMethodDefinitionFor(_typeDef, _serializationBridgeProvider));
        }

        private void AddDeserializeMethod()
        {
            _typeDef.Methods.Add(DeserializeMethodEmitter.DeserializeMethodDefinitionFor(_typeDef, _serializationBridgeProvider));
        }

        private bool MakeImplement(TypeDefinition typeDefinition)
        {
            if (_typeDef.Interfaces.Any(i => i.InterfaceType.FullName == typeDefinition.FullName))
                return false;

            _typeDef.Interfaces.Add(new InterfaceImplementation(Module.ImportReference(typeDefinition)));
            return true;
        }

        private ModuleDefinition Module
        {
            get { return _typeDef.Module; }
        }
    }
}
