using System;
using Mono.Cecil;

namespace Unity.SerializationWeaver.Common
{
    public class LocalVariable
    {
        private readonly int _index;
        private readonly TypeReference _type;
        private readonly Action _loadGenerator;
        private readonly Action _storeGenerator;

        public LocalVariable(int index, TypeReference type, Action loadGenerator, Action storeGenerator)
        {
            _index = index;
            _type = type;
            _loadGenerator = loadGenerator;
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

        public void EmitStore()
        {
            IsUsed = true;
            _storeGenerator();
        }
    }
}
