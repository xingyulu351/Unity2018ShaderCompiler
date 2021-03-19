using System.IO;
using Boo.Lang.Compiler.Ast;
using Boo.Lang.Compiler.IO;
using System;
using Boo.Lang.Compiler.MetaProgramming;

namespace UnityExampleConverter
{
    public class Program
    {
        public static CompileUnit[] convert(string fileName, string[] references)
        {
            UnityScriptConverter unityScriptConverter = new UnityScriptConverter();
            unityScriptConverter.Input.Add(new FileInput(fileName));
            foreach (string current in references)
                unityScriptConverter.AddReference(current);

            return unityScriptConverter.Run();
        }

        public static void printBooCodeFor(CompileUnit cu)
        {
            Console.WriteLine(cu.ToCodeString());
        }

        public static void printCSharpCodeFor(CompileUnit cu)
        {
            cu.Accept(new CSharpPrinter());
        }

        static void Main(string[] args)
        {
            if (args.Length <= 1)
            {
                Console.WriteLine("argument needed for source and target file");
                return;
            }
            string sourceContent = "";
            try
            {
                sourceContent = File.ReadAllText(args[0]);
            }
            catch (FileNotFoundException e)
            {
                Console.WriteLine(e.Message);
            }

            var snippetConverter = new SnippetConverter(true);

            string csResult;
            try
            {
                var conversionResult = snippetConverter.Convert(sourceContent);
                csResult = conversionResult.CSharpCode;
            }
            catch (CompilationErrorsException ex)
            {
                //look into BaseConversionTests in Cs2us.Tests to fix this
                csResult = "failed to convert" + ex.Errors;
                Console.WriteLine(ex.Errors);
            }

            try
            {
                File.WriteAllText(args[1], csResult);
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
            }
        }
    }
}
