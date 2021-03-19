using System;
using System.Linq;
using Mono.Cecil;
using UnityPreserveAttributeParser.ManagedProxies;

namespace UnityPreserveAttributeParser
{
    public class CommonScriptingClassesWriter : IStrippingInformationOutputHandler
    {
        public string Header;
        public string Source;
        protected string CppWrappers;
        protected string ModuleName = null;
        protected string HeaderPath;
        protected string CppPath;
        protected AssemblyDefinition Assembly;

        public CommonScriptingClassesWriter(string headerPath, string cppPath)
        {
            Header += "// " + Program.GeneratedMessageArtifacts + "\n";
            Header += "#pragma once\n";
            Header += "#include \"Runtime/Scripting/ScriptingTypes.h\"\n";
            Header += "#include \"Runtime/Math/Color.h\"\n";
            Header += "\n";
            Header += "#ifndef USE_NEW_SCRIPTINGCLASSES_WRAPPERS\n";
            Header += "#define USE_NEW_SCRIPTINGCLASSES_WRAPPERS 0\n";
            Header += "#endif\n";

            Source += "// " + Program.GeneratedMessageArtifacts + "\n";
            Source += "#include \"UnityPrefix.h\"\n";
            Source += "#undef USE_NEW_SCRIPTINGCLASSES_WRAPPERS\n";
            Source += "#define USE_NEW_SCRIPTINGCLASSES_WRAPPERS 0\n";
            Source += "#include \"Runtime/Scripting/ScriptingManager.h\"\n";
            Source += "#include \"Runtime/Scripting/ScriptingInvocation.h\"\n";
            Source += "#include \"Runtime/Scripting/Marshalling/StringMarshalling.h\"\n";

            HeaderPath = headerPath;
            CppPath = cppPath;
        }

        public virtual void ProcessAssembly(AssemblyDefinition assembly)
        {
            Assembly = assembly;
        }

        public virtual void ProcessModule(string module)
        {
            ModuleName = module;

            Header += "#if !USE_NEW_SCRIPTINGCLASSES_WRAPPERS\n";
            Header += "struct " + ModuleName + "ScriptingClasses\n";
            Header += "{\n";

            Source += "#include \"" + ModuleName + "ScriptingClasses.h\"\n";

            Source += "#include \"Runtime/Testing/Fakeable.h\"\n";
            Source += "\n";
            Source += "static " + ModuleName + "ScriptingClasses *s_" + ModuleName + "ScriptingClasses;\n";
            Source += "\n";
            Source += "static void Cleanup" + ModuleName + "ScriptingClasses()\n";
            Source += "{\n";
            Source += "\tUNITY_FREE(kMemScriptManager, s_" + ModuleName + "ScriptingClasses);\n";
            Source += "\ts_" + ModuleName + "ScriptingClasses = NULL;\n";
            Source += "}\n";
            Source += "\n";
            Source += "static void Initialize" + ModuleName + "ScriptingClasses()\n";
            Source += "{\n";
            Source += "\ts_" + ModuleName + "ScriptingClasses = UNITY_NEW(" + ModuleName + "ScriptingClasses, kMemScriptManager);\n";
        }

        public virtual void ProcessType(TypeDefinition type, PreserveState state)
        {
            if (state == PreserveState.Unused)
                return;

            var cppName = Naming.CppNamefor(type);
            Header += "\t// " + Program.GeneratedMessageArtifacts + "\n";
            Source += "\t// " + Program.GeneratedMessageArtifacts + "\n";
            Header += "\tScriptingClassPtr\t" + cppName + ";\n";

            var ns = type.Namespace;
            var name = type.Name;
            var op = (state == PreserveState.Required) ? "RequireType" : "OptionalType";

            if (type.IsNested)
            {
                var rootType = type.DeclaringType;
                while (rootType.IsNested)
                    rootType = rootType.DeclaringType;

                ns = rootType.Namespace;
                name = type.FullName.Split('.').Last();
            }

            Source += String.Format(
                "\ts_{0}ScriptingClasses->{1} = {2} (\"{3}.dll\", \"{4}\", \"{5}\");\n",
                ModuleName, cppName, op, Assembly.Name.Name, ns, name);
        }

        public virtual void ProcessMethod(MethodDefinition method, PreserveState state)
        {
            if (state == PreserveState.Unused)
                return;

            var name = method.Name;

            // We never call constructors explicitly from native code, and will have naming conflicts if we add them, so skip them here.
            if (name == ".ctor")
                return;

            if (method.NameIsAmbiguous())
            {
                var warning = "\t" + method.WarningMessageForAmbiguousName();
                Header += warning;
                Source += warning;
                return;
            }

            var type = method.DeclaringType;
            var cppName = Naming.CppNamefor(method);
            Header += "\t// " + Program.GeneratedMessageArtifacts + "\n";
            Source += "\t// " + Program.GeneratedMessageArtifacts + "\n";
            Header += "\tScriptingMethodPtr\t" + cppName + ";\n";

            var assemblyName = Assembly.Name.Name;
            var ns = type.Namespace;
            var typeName = type.Name;
            var op = (state == PreserveState.Required) ? "RequireMethod" : "OptionalMethod";

            if (type.IsNested)
            {
                var rootType = type.DeclaringType;
                while (rootType.IsNested)
                    rootType = rootType.DeclaringType;

                ns = rootType.Namespace;
                typeName = type.FullName.Split('.').Last();
            }

            Source += String.Format(
                "\ts_{0}ScriptingClasses->{1} = {2} (\"{3}.dll\", \"{4}\", \"{5}\", \"{6}\");\n",
                ModuleName, cppName, op, assemblyName, ns, typeName, name);

            if (!type.IsInterface)
                CppWrappers += MethodCallWrapperGenerator.MethodDefinitionToCppWrapper(method, ModuleName, state) + "\n";
        }

        public virtual void EndType()
        {
        }

        public virtual void EndAssembly()
        {
        }

        public virtual void EndModule()
        {
            Header += "};\n";

            Header += ModuleName + "ScriptingClasses& Get" + ModuleName + "ScriptingClasses();\n";
            Header += ModuleName + "ScriptingClasses* Get" + ModuleName + "ScriptingClassesPtr();\n";
            Header += "#endif\n";

            Source += "}\n";
            Source += "\n";
            Source += CppWrappers;
            Source += ModuleName + "ScriptingClasses* Get" + ModuleName + "ScriptingClassesPtr()\n";
            Source += "{\n";

            Source += "#if ENABLE_UNIT_TESTS_WITH_FAKES\n";
            Source += "   __FAKEABLE_FUNCTION__( Get" + ModuleName + "ScriptingClassesPtr, () );\n";
            Source += "#endif\n";

            Source += "\tif (s_" + ModuleName + "ScriptingClasses == NULL)\n";
            Source += "\t{\n";
            Source += "\t\tInitialize" + ModuleName + "ScriptingClasses();\n";
            Source += "\t\tSetupModuleScriptingClasses (Initialize" + ModuleName + "ScriptingClasses, Cleanup" + ModuleName + "ScriptingClasses);\n";
            Source += "\t}\n";
            Source += "\tAssert (s_" + ModuleName + "ScriptingClasses != NULL);\n";
            Source += "\treturn s_" + ModuleName + "ScriptingClasses;\n";
            Source += "}\n";
            Source += "\n";
            Source += ModuleName + "ScriptingClasses& Get" + ModuleName + "ScriptingClasses()\n";
            Source += "{\n";
            Source += "\treturn *Get" + ModuleName + "ScriptingClassesPtr();\n";
            Source += "}\n";
        }

        public void AddLengthCompileTimeAssert(ref string s)
        {
            var length = s.Split(new char[] {'\n'}).Length;
            s += "namespace ModificationCheck{ CompileTimeAssert (__LINE__ == " + length + ", \"File length does not match expectation. In case you missed the warnings, please do not edit the generated file!\"); }\n";
        }

        public virtual void End()
        {
            // we don't do these for now to prevent issues on automatic merges.
            // AddLengthCompileTimeAssert (ref header);
            // AddLengthCompileTimeAssert (ref source);

            if (HeaderPath != null)
                Program.WriteAllTextToFile(HeaderPath, Header);
            if (CppPath != null)
                Program.WriteAllTextToFile(CppPath, Source);
        }
    }
}
