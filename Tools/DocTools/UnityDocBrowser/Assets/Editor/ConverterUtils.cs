using System;
using System.Text;
using UnderlyingModel.MemDoc;
using UnderlyingModel;
using UnityScriptUtils;
using MemDoc;

using System.Diagnostics;
using System.IO;

internal class ConverterUtils
{
    internal struct ConverterResult
    {
        internal string convertedText;
        internal bool isError;
    }

    internal class LanguageTempFiles
    {
        internal string TempFolder { get; private set; }
        readonly CompLang m_SourceLang;

        internal LanguageTempFiles(CompLang sourceLang)
        {
            m_SourceLang = sourceLang;
            TempFolder = Path.GetTempPath();
        }

        internal string PathToConverter
        {
            get
            {
                return m_SourceLang == CompLang.CSharp
                    ? @"Tools/DocTools/Cs2us/bin/debug/Cs2us.exe"
                    : @"Tools/DocTools/UnityExampleConverter/cssrc/bin/debug/UnityExampleConverter.exe";
            }
        }

        internal string GetFullName(CompLang compLang)
        {
            switch (compLang)
            {
                case CompLang.JavaScript:
                    return Path.Combine(TempFolder, "example.js");
                case CompLang.CSharp:
                    return Path.Combine(TempFolder, "example.cs");
            }
            return "dummy.dumdum";
        }

        internal void WriteSourceFile(string sourceContent)
        {
            try
            {
                var fname = GetFullName(m_SourceLang);
                UnityEngine.Debug.Log("attempting to write to " + fname);
                File.WriteAllText(fname, sourceContent);
            }
            catch (Exception ex)
            {
                UnityEngine.Debug.LogError(ex);
            }
        }
    }

    public static ConverterResult ConvertExample(string sourceContent, CompLang sourceLang)
    {
        var langTemp = new LanguageTempFiles(sourceLang);
        langTemp.WriteSourceFile(sourceContent);

        UnityEngine.Debug.Log("cwd = " + Directory.GetCurrentDirectory());

        var sourceFileName = langTemp.GetFullName(sourceLang);

        var targetLang = sourceLang == CompLang.JavaScript ? CompLang.CSharp : CompLang.JavaScript;
        var targetFullPath = langTemp.GetFullName(targetLang);

        //start converter process with output file specified
        var psi = ProcessUtil.GetStartInfo(langTemp.PathToConverter, sourceFileName + " \"" + targetFullPath + "\"");
        psi.RedirectStandardOutput = true;
        psi.RedirectStandardError = true;

        UnityEngine.Debug.Log("Converter = " + psi.FileName + " Args = " + psi.Arguments);
        var p = new Process {StartInfo = psi};
        var sb = new StringBuilder();
        var sbError = new StringBuilder();
        p.OutputDataReceived += (sender, eventargs) => {sb.AppendLine(eventargs.Data); };
        p.ErrorDataReceived += (sender, eventargs) => { sbError.AppendLine(eventargs.Data); };
        p.Start();
        p.BeginErrorReadLine();
        p.BeginOutputReadLine();

        if (!p.WaitForExit(10000))
        {
            UnityEngine.Debug.LogError("the converter process didn't finish within 10s, likely a conversion error");
            return new ConverterResult {convertedText = "Process timeout, likely a conversion failure", isError = true};
        }
        if (p.ExitCode != 0)
        {
            return new ConverterResult
            {
                convertedText = "converter returned error " + p.ExitCode + "\noutput = " + sb + "\nError = " + sbError,
                isError = true
            };
        }

        //get output from file
        string convertedContent = "";
        try
        {
            /*string error = Compilation.CompileUnityScript(targetFullPath);
                if (error != null)
                return new ConverterResult
                {
                    convertedText = "converter error = " + error,
                    isError = true
                }; */
            convertedContent = File.ReadAllText(targetFullPath);
        }
        catch (Exception ex)
        {
            UnityEngine.Debug.LogError(ex);
        }

        UnityEngine.Debug.Log("convertedContent = " + convertedContent);
        return new ConverterResult {convertedText = convertedContent, isError = false};
    }
}
