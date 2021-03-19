using System;
using System.IO;
using System.Linq;
using System.Text;
using System.Collections.Generic;
using Mono.Cecil;


public class Program
{
    static bool IsObsoleteErrorType(TypeDefinition type)
    {
        foreach (var attribute in type.CustomAttributes)
        {
            if (attribute.AttributeType.Name == "ObsoleteAttribute")
            {
                if (attribute.ConstructorArguments.Count >= 2 && (bool)attribute.ConstructorArguments[1].Value == true)
                    return true;
            }
        }
        return false;
    }

    public static void GenerateForwarderCode(IEnumerable<AssemblyDefinition> assemblies, StringBuilder sb)
    {
        foreach (var assembly in assemblies)
        {
            sb.Append("// " + assembly.Name.Name + "\n");
            ModuleDefinition asmModule = assembly.MainModule;
            foreach (var type in asmModule.Types)
            {
                // Ignore <Module> and <PrivateImplementationDetails>
                if (type.ToString()[0] != '<')
                {
                    if (IsObsoleteErrorType(type))
                        continue;

                    sb.Append("[assembly:TypeForwardedToAttribute(typeof(");
                    if (type.HasGenericParameters)
                    {
                        var name = type.ToString();
                        name = name.Substring(0, name.IndexOf("`"));
                        sb.Append(name);
                        sb.Append("<");
                        for (int i = 1; i < type.GenericParameters.Count(); i++)
                            sb.Append(",");
                        sb.Append(">");
                    }
                    else
                        sb.Append(type.ToString());
                    sb.Append("))]\n");
                }
            }
        }
    }

    public static void GenerateObsoleteCode(IEnumerable<AssemblyDefinition> assemblies, StringBuilder sb)
    {
        foreach (var assembly in assemblies)
        {
            sb.Append("// " + assembly.Name.Name + "\n");
            ModuleDefinition asmModule = assembly.MainModule;
            foreach (var type in asmModule.Types)
            {
                // Ignore <Module> and <PrivateImplementationDetails>
                if (type.ToString()[0] != '<')
                {
                    if (IsObsoleteErrorType(type))
                    {
                        if (type.Namespace != "")
                            sb.AppendFormat("namespace {0} {{\n", type.Namespace);

                        // We only generate these so that dlls referencing UnityEngine.dll will load.
                        // So we generate all obsolete types as internal. That still allows loading them,
                        // but the will not be picked up by the C# compiler. That will pick up the one from
                        // the module dlls, containing the obsolete attribute with the desired error string.

                        sb.Append("internal ");
                        if (type.IsEnum)
                            sb.Append("enum ");
                        else if (type.IsClass)
                            sb.Append("class ");
                        else
                            sb.Append("struct ");
                        sb.Append(type.Name);
                        sb.Append(" {}\n");
                        if (type.Namespace != "")
                            sb.Append("}\n");
                    }
                }
            }
        }
    }

    public static int Main(string[] args)
    {
        string outputPath = null;
        var p = new Mono.Options.OptionSet() { new Mono.Options.ResponseFileSource() }
            .Add("output=", "Output cs path.", s => outputPath = s);
        var remaining = p.Parse(args);

        if (remaining.Count < 1 || outputPath == null)
        {
            Console.WriteLine("usage: TypeForwarderGenerator.exe [assembly] --output=[outputPath]");
            return 1;
        }

        var resolver = new DefaultAssemblyResolver();
        var readerParameters = new ReaderParameters
        {
            AssemblyResolver = resolver,
            ReadSymbols = true
        };
        var assemblies = remaining.Select(fileName => AssemblyDefinition.ReadAssembly(fileName, readerParameters));

        StringBuilder sb = new StringBuilder();
        sb.Append("// AUTO GENERATED - ANY MANUAL MODIFICATIONS WILL BE LOST\n\n");
        sb.Append("#pragma warning disable 618\n"); // Suppress obsolete warnings.
        sb.Append("using System.Runtime.CompilerServices;\n");

        GenerateForwarderCode(assemblies, sb);
        GenerateObsoleteCode(assemblies, sb);

        File.WriteAllText(outputPath, sb.ToString());

        return 0;
    }
}
