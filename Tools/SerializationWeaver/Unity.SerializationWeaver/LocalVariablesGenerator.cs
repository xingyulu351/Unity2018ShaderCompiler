using System;
using System.Collections.Generic;
using Mono.Cecil;
using Mono.Cecil.Cil;

namespace Unity.Serialization.Weaver
{
    public class LocalVariable
    {
        private readonly int _index;
        private readonly TypeReference _type;
        private readonly Action _loadGenerator;
        private readonly Action _loadAddressGenerator;
        private readonly Action _storeGenerator;

        public LocalVariable(int index, TypeReference type, Action loadGenerator, Action loadAddressGenerator, Action storeGenerator)
        {
            _index = index;
            _type = type;
            _loadGenerator = loadGenerator;
            _loadAddressGenerator = loadAddressGenerator;
            _storeGenerator = storeGenerator;
        }

        public TypeReference Type
        {
            get { return _type; }
        }

        public int Index
        {
            get { return _index; }
        }

        public bool IsUsed { get; private set; }

        public void EmitLoad()
        {
            IsUsed = true;
            _loadGenerator();
        }

        public void EmitLoadAddress()
        {
            IsUsed = true;
            _loadAddressGenerator();
        }

        public void EmitStore()
        {
            IsUsed = true;
            _storeGenerator();
        }
    }

    public class LocalVariablesGenerator
    {
        private readonly ILProcessor _processor;
        private readonly MethodDefinition _methodDefinition;
        private readonly List<LocalVariable> _variables = new List<LocalVariable>();

        public LocalVariablesGenerator(MethodDefinition methodDefinition, ILProcessor processor)
        {
            _processor = processor;
            _methodDefinition = methodDefinition;
        }

        public LocalVariable Create(TypeReference type)
        {
            _methodDefinition.Body.InitLocals = true;

            var local = new LocalVariable(
                _variables.Count,
                type,
                LoadGeneratorForNextVariable(),
                LoadAddressGeneratorForNextVariable(),
                StoreGeneratorForNextVariable());
            _variables.Add(local);
            _methodDefinition.Body.Variables.Add(new VariableDefinition(type));

            return local;
        }

        private Action LoadGeneratorForNextVariable()
        {
            var index = _variables.Count;
            if (index < 4)
                return () => _processor.Emit(LoadOpCodeForIndex(index));

            return () => _processor.Emit(LoadOpCodeForIndex(index), index);
        }

        private static OpCode LoadOpCodeForIndex(int index)
        {
            switch (index)
            {
                case 0:
                    return OpCodes.Ldloc_0;
                case 1:
                    return OpCodes.Ldloc_1;
                case 2:
                    return OpCodes.Ldloc_2;
                case 3:
                    return OpCodes.Ldloc_3;

                default:
                    return OpCodes.Ldloc;
            }
        }

        private Action LoadAddressGeneratorForNextVariable()
        {
            var index = _variables.Count;
            return () => _processor.Emit(OpCodes.Ldloca, index);
        }

        private Action StoreGeneratorForNextVariable()
        {
            var index = _variables.Count;
            if (index < 4)
                return () => _processor.Emit(StoreOpCodeForIndex(index));

            return () => _processor.Emit(StoreOpCodeForIndex(index), index);
        }

        private static OpCode StoreOpCodeForIndex(int index)
        {
            switch (index)
            {
                case 0:
                    return OpCodes.Stloc_0;
                case 1:
                    return OpCodes.Stloc_1;
                case 2:
                    return OpCodes.Stloc_2;
                case 3:
                    return OpCodes.Stloc_3;

                default:
                    return OpCodes.Stloc;
            }
        }
    }
}
