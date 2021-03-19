using System;
using System.Collections.Generic;
using System.Linq;
using Mono.Cecil;

namespace UnityPreserveAttributeParser
{
    public class XmlOutputWriter : IStrippingInformationOutputHandler
    {
        protected string Output;
        protected string AssemblyOutput;
        protected string TypeOutput;
        protected string XmlPath;
        protected bool TypeRequired;
        protected bool TypeHasMethods;
        protected bool TypeHasFields;
        protected bool AssemblyHasTypes;

        public XmlOutputWriter(string xmlPath)
        {
            XmlPath = xmlPath;
            Output += "<linker>\n";
        }

        public virtual void ProcessModule(string module)
        {
        }

        public virtual void ProcessAssembly(AssemblyDefinition assembly)
        {
            AssemblyHasTypes = false;
            AssemblyOutput = "\t<assembly fullname=\"" + assembly.Name.Name + "\" preserve=\"nothing\">\n";
        }

        public virtual void ProcessType(TypeDefinition type, PreserveState state)
        {
            TypeRequired = state == PreserveState.Required;
            TypeHasMethods = false;
            TypeHasFields = false;
            TypeOutput = "<!--" + Program.GeneratedMessageArtifacts + "-->\n";
            TypeOutput += "\t\t<type fullname=\"" + type.FullName + "\" preserve=\"nothing\"";
            AssemblyHasTypes = true;
        }

        public virtual void ProcessMethod(MethodDefinition method, PreserveState state)
        {
            if (state == PreserveState.Required)
            {
                if (!TypeHasMethods && !TypeHasFields)
                    TypeOutput += ">\n";
                TypeHasMethods = true;
                TypeRequired = true;
                TypeOutput += "\t\t\t<method name=\"" + method.Name + "\"/>\n";
            }
        }

        public virtual void EndType()
        {
            if (!TypeRequired)
                return;
            if (TypeHasMethods || TypeHasFields)
                TypeOutput += "\t\t</type>\n";
            else
                TypeOutput += "/>\n";
            AssemblyOutput += TypeOutput;
        }

        public virtual void EndModule()
        {
        }

        public virtual void EndAssembly()
        {
            AssemblyOutput += "\t</assembly>\n";
            if (AssemblyHasTypes)
                Output += AssemblyOutput;
        }

        public virtual void End()
        {
            Output += "</linker>\n";

            if (XmlPath != null)
                Program.WriteAllTextToFile(XmlPath, Output);
        }
    }
}
