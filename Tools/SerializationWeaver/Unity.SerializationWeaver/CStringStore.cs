using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Mono.Cecil;
using Mono.Cecil.Cil;
using Mono.Cecil.Rocks;

namespace Unity.Serialization.Weaver
{
    class CStringStore
    {
        private static Dictionary<ModuleDefinition, CStringStore> instances = new Dictionary<ModuleDefinition, CStringStore>();

        private readonly ModuleDefinition module;
        private readonly TypeDefinition fieldNamesStorageType;
        private readonly TypeDefinition fieldNamesType;
        private readonly FieldDefinition fieldNamesField;
        private readonly List<byte> fieldNames = new List<byte>();
        private readonly Dictionary<string, int> stringOffsets = new Dictionary<string, int>();

        private static FieldDefinition GetStorageField(ModuleDefinition module)
        {
            CStringStore instance;

            if (!instances.TryGetValue(module, out instance))
            {
                instance = new CStringStore(module);
                instances.Add(module, instance);
            }

            return instance.fieldNamesField;
        }

        public static int GetStringOffset(ModuleDefinition module, string str)
        {
            CStringStore instance;

            if (!instances.TryGetValue(module, out instance))
            {
                instance = new CStringStore(module);
                instances.Add(module, instance);
            }

            return instance.GetOffsetForString(str);
        }

        public static void SaveCStringStore(ModuleDefinition module)
        {
            CStringStore instance;

            if (!instances.TryGetValue(module, out instance))
            {
                instance = new CStringStore(module);
                instances.Add(module, instance);
            }

            instance.SaveCStringStore();
        }

        private CStringStore(ModuleDefinition module)
        {
            this.module = module;

            var systemValueType = module.ImportReference(module.TypeSystem.Object.Resolve().Module.Types.First(t => t.FullName == "System.ValueType"));

            fieldNamesStorageType = new TypeDefinition("UnityEngine.Internal", "$FieldNamesStorage", TypeAttributes.Abstract | TypeAttributes.Sealed | TypeAttributes.NotPublic, module.TypeSystem.Object);

            fieldNamesType = new TypeDefinition("UnityEngine.Internal", "$FieldNames", TypeAttributes.ExplicitLayout | TypeAttributes.Sealed | TypeAttributes.NestedAssembly, systemValueType);
            fieldNamesType.ClassSize = 1;
            fieldNamesType.PackingSize = 1;

            module.Types.Add(fieldNamesStorageType);
            fieldNamesStorageType.NestedTypes.Add(fieldNamesType);

            fieldNamesField = new FieldDefinition("$RuntimeNames", FieldAttributes.Assembly | FieldAttributes.Static, new ArrayType(module.TypeSystem.Byte));
            fieldNamesStorageType.Fields.Add(fieldNamesField);
        }

        internal static void StoreStoragePointerIntoLocalVariable(ILProcessor processor, LocalVariable fieldNamesVariable, ModuleDefinition module)
        {
            var instructions = new[]
            {
                Instruction.Create(OpCodes.Ldsfld, GetStorageField(module)),
                Instruction.Create(OpCodes.Ldc_I4_0),
                Instruction.Create(OpCodes.Ldelema, module.TypeSystem.Byte),
                Instruction.Create(OpCodes.Stloc, processor.Body.Variables[fieldNamesVariable.Index])
            };

            for (int i = instructions.Length - 1; i > -1; i--)
                processor.Body.Instructions.Insert(0, instructions[i]);
        }

        private int GetOffsetForString(string str)
        {
            int offset;

            if (stringOffsets.TryGetValue(str, out offset))
                return offset;

            offset = fieldNames.Count;
            stringOffsets.Add(str, offset);

            fieldNames.AddRange(Encoding.UTF8.GetBytes(str));
            fieldNames.Add(0); // null terminator

            return offset;
        }

        private void SaveCStringStore()
        {
            if (fieldNames.Count == 0)
                fieldNames.Add(0);  // We don't ever want it to be empty, otherwise we can't take pointer to the first member

            fieldNamesType.ClassSize = fieldNames.Count;

            var rvaStaticNames = new FieldDefinition("$RVAStaticNames", FieldAttributes.Assembly | FieldAttributes.Static | FieldAttributes.InitOnly | FieldAttributes.HasFieldRVA, fieldNamesType);
            rvaStaticNames.InitialValue = fieldNames.ToArray();
            fieldNamesStorageType.Fields.Add(rvaStaticNames);

            var runtimeHelpersClass = module.TypeSystem.Object.Resolve().Module.Types.First(t => t.FullName == "System.Runtime.CompilerServices.RuntimeHelpers");
            var initializeArrayMethod = module.ImportReference(runtimeHelpersClass.Methods.Single(m => m.Name == "InitializeArray" && m.Parameters.Count == 2 && m.Parameters[0].ParameterType.FullName == "System.Array"
                && m.Parameters[1].ParameterType.FullName == "System.RuntimeFieldHandle"));

            var moduleType = module.Types.First(t => t.FullName == "<Module>");

            bool assemblyInitializerExisted = true;
            var assemblyInitializer = moduleType.Methods.FirstOrDefault(m => m.Name == ".cctor" && (m.Attributes & MethodAttributes.Static) != (MethodAttributes)0 && (m.Attributes & MethodAttributes.RTSpecialName) != (MethodAttributes)0);

            if (assemblyInitializer == null)
            {
                assemblyInitializerExisted = false;
                assemblyInitializer = new MethodDefinition(".cctor", MethodAttributes.Static | MethodAttributes.SpecialName | MethodAttributes.RTSpecialName | MethodAttributes.Private, module.TypeSystem.Void);
                moduleType.Methods.Add(assemblyInitializer);
            }

            var instructions = new List<Instruction>()
            {
                Instruction.Create(OpCodes.Ldc_I4, fieldNames.Count),
                Instruction.Create(OpCodes.Newarr, module.TypeSystem.Byte),
                Instruction.Create(OpCodes.Dup),
                Instruction.Create(OpCodes.Ldtoken, rvaStaticNames),
                Instruction.Create(OpCodes.Call, initializeArrayMethod),
                Instruction.Create(OpCodes.Stsfld, fieldNamesField)
            };

            if (!assemblyInitializerExisted)
                instructions.Add(Instruction.Create(OpCodes.Ret));

            for (int i = instructions.Count - 1; i > -1; i--)
                assemblyInitializer.Body.Instructions.Insert(0, instructions[i]);

            assemblyInitializer.Body.OptimizeMacros();
        }
    }
}
