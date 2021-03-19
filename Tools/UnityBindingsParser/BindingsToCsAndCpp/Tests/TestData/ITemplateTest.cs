using System;

namespace BindingsToCsAndCpp.Tests
{
    public interface ITemplateTest
    {
        string DefaultTemplatePath { get; }
        string DefaultZipFile { get; }
        string TemplatePath { get; set; }
        string ZipFile { get; set; }
        bool GenerateTemplates { get; set; }
    }
}
