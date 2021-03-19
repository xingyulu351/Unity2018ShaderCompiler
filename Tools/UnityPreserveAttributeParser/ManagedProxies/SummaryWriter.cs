using System.Collections.Generic;
using System.Linq;
using Mono.Cecil;

namespace UnityPreserveAttributeParser.ManagedProxies
{
    public sealed class SummaryWriter : UnityPreserveAttributeParser.SummaryWriter
    {
        public SummaryWriter(string xmlPath, string check) : base(xmlPath, check)
        {
        }

        public override void ProcessType(TypeDefinition type, PreserveState state)
        {
            base.ProcessType(type, state);

            if (!type.RequiresManagedProxyGeneration())
                return;

            AssemblyOutput += " baseType=\"" + (type.BaseType == null ? "" : type.BaseType.FullName) + "\"";
            if (type.NativeProxyType() == null)
                AssemblyOutput += " managedProxy=\"\"";
            else
            {
                if (type.NativeProxyTypeDeclaration() != null)
                    AssemblyOutput += $" managedProxy=\"{type.NativeProxyType()}({type.NativeProxyTypeDeclaration()})\"";
                else
                    AssemblyOutput += $" managedProxy=\"{type.NativeProxyType()}\"";
            }

            foreach (var header in type.NativeProxyTypeHeaders())
            {
                if (!TypeHasHeaders)
                {
                    TypeHasHeaders = true;
                    AssemblyOutput += ">\n";
                }
                AssemblyOutput += "\t\t\t<header=\"" + header + "\">\n";
            }

            foreach (var fieldDefinition in type.Fields.Where(Extensions.CanHaveNativeFieldAccessor))
            {
                if (!TypeHasFields)
                {
                    TypeHasFields = true;
                    if (!TypeHasHeaders)
                        AssemblyOutput += ">\n";
                }
                AssemblyOutput += "\t\t\t<field type=\"" + fieldDefinition.FieldType.FullName + "\" name=\"" + fieldDefinition.Name + "\" nativeName=\"" + fieldDefinition.NativeFieldName() + "\"/>\n";
            }
        }
    }
}
