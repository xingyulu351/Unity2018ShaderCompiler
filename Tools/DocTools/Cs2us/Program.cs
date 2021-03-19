using System;
using System.IO;

namespace Cs2us
{
    public class Program
    {
        static int Main(string[] args)
        {
            if (args.Length <= 1)
            {
                Console.WriteLine("argument needed for source and target file");
                return -1;
            }
            string content = "";
            try
            {
                content = File.ReadAllText(args[0]);
            }
            catch (FileNotFoundException e)
            {
                Console.WriteLine("reading source example failed: " + e.Message);
                return -2;
            }
            string usResult = "";
            try
            {
                usResult = Cs2UsUtils.ConvertClass(content, explicitlyTypedVariables: true, usePragmaStrict: true);
            }
            catch (Exception e)
            {
                Console.WriteLine("conversion failed: " + e.Message);
                return -3;
            }
            try
            {
                File.WriteAllText(args[1], usResult);
            }
            catch (Exception e)
            {
                Console.WriteLine("writing converted file failed: " +  e.Message);
                return -4;
            }

            return 0;
        }
    }
}
