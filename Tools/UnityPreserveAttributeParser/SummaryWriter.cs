using System;
using System.Collections.Generic;
using System.IO;
using Mono.Cecil;

namespace UnityPreserveAttributeParser
{
    public class SummaryWriter : IStrippingInformationOutputHandler
    {
        protected string Output;
        protected string AssemblyOutput;
        protected string XmlPath;
        protected string Module;
        protected bool TypeHasMethods;
        protected bool TypeHasFields;
        protected bool TypeHasHeaders;
        protected bool AssemblyHasTypes;
        protected readonly string Check;

        public SummaryWriter(string xmlPath, string check)
        {
            XmlPath = xmlPath;
            Check = check;
            Output = "<!--" + Program.GeneratedMessageSummary + "-->\n";
        }

        public virtual void ProcessAssembly(AssemblyDefinition assembly)
        {
            AssemblyHasTypes = false;
            AssemblyOutput = "<assembly fullname=\"" + assembly.Name.Name + "\">\n";
        }

        public virtual void ProcessModule(string _module)
        {
            Module = _module;
            AssemblyOutput += "\t<module name=\"" + Module + "\">\n";
        }

        public virtual void ProcessType(TypeDefinition type, PreserveState state)
        {
            TypeHasMethods = false;
            TypeHasFields = false;
            TypeHasHeaders = false;
            AssemblyHasTypes = true;
            AssemblyOutput += "\t\t<type fullname=\"" + type.FullName + "\" state=\"" + state + "\"";
        }

        public virtual void ProcessMethod(MethodDefinition method, PreserveState state)
        {
            if (state == PreserveState.Required)
            {
                if (!TypeHasMethods && !TypeHasFields && !TypeHasHeaders)
                    AssemblyOutput += ">\n";
                TypeHasMethods = true;
                AssemblyOutput += "\t\t\t<method signature=\"" + method.FullName + "\" state=\"" + state + "\"/>\n";
            }
        }

        public virtual void EndType()
        {
            if (TypeHasMethods || TypeHasFields || TypeHasHeaders)
                AssemblyOutput += "\t\t</type>\n";
            else
                AssemblyOutput += "/>\n";
        }

        public virtual void EndAssembly()
        {
            AssemblyOutput += "</assembly>\n";
            if (AssemblyHasTypes)
                Output += AssemblyOutput;
        }

        public virtual void EndModule()
        {
            AssemblyOutput += "\t</module>\n";
        }

        public virtual void End()
        {
            if (XmlPath != null)
            {
                if (Check != null)
                {
                    var expected = File.ReadAllText(XmlPath);
                    if (!expected.Contains(Output))
                    {
                        Console.WriteLine("Generated Output:");
                        Console.WriteLine(Output);
                        Console.WriteLine("Expected Output from file on disk:");
                        Console.WriteLine(expected);
                        Console.WriteLine(XmlPath + " was last written to on " + new FileInfo(XmlPath).LastWriteTime);

                        if (File.Exists(Check))
                            File.Delete(Check);
                        throw new Exception("Verification failed! You seem to have changed the [RequiredByNativeCode] or [UsedByNativeCode] attributes in your UnityEngine or UnityEditor dll without rebuilding the generated information. Run \"./jam GenerateNativeDependenciesForManagedCode\" to fix this, and read http://confluence.hq.unity3d.com/display/DEV/Using+scripting+classes+and+methods+from+native+code for more information.");
                    }
                    else
                        Program.WriteAllTextToFile(Check, "Success");
                }
                else
                    Program.WriteAllTextToFile(XmlPath, Output);
            }
        }
    }
}
