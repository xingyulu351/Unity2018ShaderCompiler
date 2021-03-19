using MemDoc;
using UnderlyingModel.MemDoc;
using UnityEditor;
using UnityEngine;


internal class ConvertPreviewWindow : EditorWindow
{
    internal enum SourceLang
    {
        CS,
        JS
    }

    private ExampleBlock m_Example;
    private SourceLang m_SourceLang;
    private ConverterUtils.ConverterResult m_ConvertedResult;
    private DocBrowser m_Browser;

    internal static void Init(ExampleBlock example, SourceLang sourceLang, DocBrowser browser)
    {
        var window = (ConvertPreviewWindow)GetWindow(typeof(ConvertPreviewWindow));
        window.titleContent = new GUIContent("Example conversion preview");
        window.m_Example = example;
        window.m_SourceLang = sourceLang;

        var sourceText = sourceLang == SourceLang.JS ? example.JsExample : example.CsExample;
        window.m_ConvertedResult = GetConvertedContent(sourceText, sourceLang);
        window.m_Browser = browser;
    }

    void OnGUI()
    {
        GUIStyle style = m_ConvertedResult.isError ? DocBrowser.styles.exampleError : DocBrowser.styles.example;
        GUILayout.Label(m_ConvertedResult.convertedText, style);
        if (GUILayout.Button("Go back"))
        {
            Close();
        }
        if (!m_ConvertedResult.isError)
        {
            if (GUILayout.Button("Copy back"))
            {
                if (m_SourceLang == SourceLang.JS)
                    m_Example.CsExample = m_ConvertedResult.convertedText;
                else
                    m_Example.JsExample = m_ConvertedResult.convertedText;
                m_Browser.Repaint();
                Close();
            }
            if (GUILayout.Button("Make convertable"))
            {
                m_Example.IsConvertExample = true;
                if (m_SourceLang == SourceLang.JS)
                    m_Example.CsExample = "";
                else
                    m_Example.JsExample = "";
                m_Browser.Repaint();
                Close();
            }
        }
    }

    static ConverterUtils.ConverterResult GetConvertedContent(string sourceContent, SourceLang sourceLang)
    {
        var actualLanguage = sourceLang == SourceLang.CS ? CompLang.CSharp : CompLang.JavaScript;
        var result = ConverterUtils.ConvertExample(sourceContent, actualLanguage);
        Debug.Log("convertedText = " + result.convertedText + "isError = " + result.isError);
        return result;
    }
}
