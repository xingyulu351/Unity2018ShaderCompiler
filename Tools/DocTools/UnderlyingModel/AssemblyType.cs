namespace UnderlyingModel
{
    public enum AssemblyType
    {
        Unknown,
        Class,
        Struct,
        Delegate,
        Event,
        Interface,
        Property,
        Field,
        Method,
        Operator,
        ImplOperator,
        Constructor,
        Enum,
        EnumValue,
        Primitive,
        Assembly
    }

    public static class AssemblyTypeUtils
    {
        public static bool MultipleSignaturesPossibleForType(AssemblyType asmType)
        {
            return
                asmType == AssemblyType.Method ||
                asmType == AssemblyType.Operator ||
                asmType == AssemblyType.Constructor ||
                asmType == AssemblyType.Property ||
                asmType == AssemblyType.Unknown;
        }

        public static string AssemblyTypeNameForSignature(AssemblyType t)
        {
            switch (t)
            {
                case AssemblyType.Class: return "class";
                case AssemblyType.Constructor: return "constructor";
                case AssemblyType.Enum: return "enumeration";
                case AssemblyType.Event: return "event";
                case AssemblyType.EnumValue: return "enumeration value";
                case AssemblyType.Field: return "var";
                case AssemblyType.Interface: return "interface";
                case AssemblyType.Method: return "function";
                case AssemblyType.Primitive: return "primitive type";
                case AssemblyType.Property: return "var";
                case AssemblyType.Struct: return "struct";
                case AssemblyType.Operator: return "operator";
                case AssemblyType.Delegate: return "delegate";
                case AssemblyType.Unknown: return "not in assembly (unknown)";
                default: return "";
            }
        }

        public static string PluralForm(AssemblyType t)
        {
            switch (t)
            {
                case AssemblyType.Class: return "Classes";
                case AssemblyType.Constructor: return "Constructors";
                case AssemblyType.Enum: return "Enumerations";
                case AssemblyType.EnumValue: return "Enumeration values";
                case AssemblyType.Field: return "Fields";
                case AssemblyType.Interface: return "Interfaces";
                case AssemblyType.Method: return "Methods";
                case AssemblyType.Delegate: return "Delegates";
                case AssemblyType.Primitive: return "Primitive types";
                case AssemblyType.Property: return "Properties";
                case AssemblyType.Struct: return "Structs";
                case AssemblyType.Operator: return "Operators";
                case AssemblyType.ImplOperator: return "Implicit Operators";
                case AssemblyType.Unknown: return "Unknown";
                default: return "";
            }
        }

        public static AssemblyType AssemblyTypeFromString(string asmString)
        {
            AssemblyType asmType;

            switch (asmString.ToLower())
            {
                case "class": asmType = AssemblyType.Class; break;
                case "constructor": asmType = AssemblyType.Constructor; break;
                case "struct": asmType = AssemblyType.Struct; break;
                case "enum": asmType = AssemblyType.Enum; break;
                case "enumval":
                case "enumvalue": asmType = AssemblyType.EnumValue; break;
                case "method": asmType = AssemblyType.Method; break;
                case "delegate": asmType = AssemblyType.Delegate; break;
                case "prop":
                case "property": asmType = AssemblyType.Property; break;
                case "field": asmType = AssemblyType.Field; break;
                case "interface": asmType = AssemblyType.Interface; break;
                case "operator": asmType = AssemblyType.Operator; break;
                case "implop": asmType = AssemblyType.ImplOperator; break;
                default: asmType = AssemblyType.Unknown; break;
            }
            return asmType;
        }

        public static bool CanHaveChildren(AssemblyType asmType)
        {
            return asmType == AssemblyType.Class || asmType == AssemblyType.Struct || asmType == AssemblyType.Enum;
        }

        public static bool IsClassOrStruct(AssemblyType asmType)
        {
            return asmType == AssemblyType.Class || asmType == AssemblyType.Struct;
        }

        public static bool IsFieldOrProperty(AssemblyType asmType)
        {
            return asmType == AssemblyType.Field || asmType == AssemblyType.Property;
        }

        public static bool IsOperatorOrImpl(AssemblyType asmType)
        {
            return asmType == AssemblyType.Operator || asmType == AssemblyType.ImplOperator;
        }

        //these are the types that may belong to a class - or be by themselves
        public static bool CanBeStandalone(AssemblyType asmType)
        {
            return asmType == AssemblyType.Delegate;
        }
    }
}
