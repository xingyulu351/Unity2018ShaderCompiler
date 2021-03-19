using System;
using System.Collections.Generic;
using NDesk.Options;

namespace Unity.InternalCallRegistrationWriter.Runner
{
    public class InternalCallOptions
    {
        public string InputAssembly;
        public string[] InputAssemblies;
        public string OutputFile;
        public string OutputSummary;

        public static InternalCallOptions ParseOptionsFrom(IEnumerable<string> arguments)
        {
            var internalCallOptions = new InternalCallOptions();
            var optionSet = new OptionSet()
            {
                {"assembly=", v => internalCallOptions.InputAssemblies = v.Split(';')},
                {"output=", v => internalCallOptions.OutputFile = v},
                {"summary=", v => internalCallOptions.OutputSummary = v},
            };
            optionSet.Parse(arguments);

            if (internalCallOptions.InputAssemblies == null)
                throw new ArgumentException("assembly");

            if (internalCallOptions.OutputFile == null)
                internalCallOptions.OutputFile = "RegisterIcalls.cpp";


            return internalCallOptions;
        }
    }
}
