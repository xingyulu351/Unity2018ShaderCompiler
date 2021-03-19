using NUnit.Framework;

namespace Cs2us.Tests
{
    [TestFixture]
    public class IntegrationTests : ClassBasedTests
    {
        [TestCase("FullyFledgedConditional",
@"class ExampleClass {
    void Example() {
        int a = 5;
        int b = 6;
        int c;
        if (a < b)
        {
            c = 2;
        }
        else
        {
            c = a++;
        }
    }
}",
@"var a = 5;
var b = 6;
var c;
if (a < b) {
    c = 2;
}
else {
    c = a++;
}"
         )]
        [TestCase("CustomShaderGUI",
@"using System.Collections.Generic;
using UnityEngine;
using UnityEditor;
using System.Linq;

public class CustomShaderGUI : IShaderGUI
{
 public void OnGUI (MaterialEditor materialEditor)
 {
  // render the properties
  materialEditor.PropertiesDefaultGUI ( );

  // get the current keywords from the material
  Material targetMat = materialEditor.target as Material;
  string[] keyWords = targetMat.shaderKeywords;

  // see if redify is set, then show a checkbox
  bool redify = keyWords.Contains (""REDIFY_ON"");
  EditorGUI.BeginChangeCheck ( );
  redify = EditorGUILayout.Toggle (""Redify material"", redify);
  if (EditorGUI.EndChangeCheck ( ))
  {
   // if the checkbox is changed, reset the shader keywords
   var keywords = new List<string> { redify ? ""REDIFY_ON"" : ""REDIFY_OFF"" };
   targetMat.shaderKeywords = keywords.ToArray ( );
   EditorUtility.SetDirty (targetMat);
  }
 }
}", @"public class CustomShaderGUI implements IShaderGUI {
    public function OnGUI(materialEditor) {
        // render the properties
        materialEditor.PropertiesDefaultGUI();
        // get the current keywords from the material
        var targetMat = materialEditor.target as Material;
        var keyWords = targetMat.shaderKeywords;
        // see if redify is set, then show a checkbox
        var redify = keyWords.Contains(""REDIFY_ON"");
        EditorGUI.BeginChangeCheck();
        redify = EditorGUILayout.Toggle(""Redify material"", redify);
        if (EditorGUI.EndChangeCheck()) {
            // if the checkbox is changed, reset the shader keywords
            var keywords = new ListString();
            targetMat.shaderKeywords = keywords.ToArray();
            EditorUtility.SetDirty(targetMat);
        }
    }
}")]
        [TestCase("UniversalShaderEditor",
@"using UnityEngine;
using Object = UnityEngine.Object;
using System;

namespace UnityEditor
{
// We will be using 'UniversalShaderEditor' as class name until we change the shader
// name to 'Standard.shader' (then we will use StandardShaderGUI)
internal class UniversalShaderEditor : IShaderGUI
{
    public enum BlendMode
    {
        Opaque,
        Cutout,
        Transparent
    }

    public enum DetailMode
    {
        Overlay,
        Multiply,
        Add,
        Mix
    }

    class Styles
    {
        public GUIStyle optionsButton = ""PaneOptions"";
        public GUIContent uvSetLabel = new GUIContent(""UV Set"");
        public GUIContent[] uvSetOptions = new GUIContent[] { new GUIContent(""UV channel 0""), new GUIContent(""UV channel 1"") };
    }
    static Styles s_Styles;

    MaterialProperty albedoMap = null;

    MaterialEditor m_MaterialEditor;
    bool m_MaterialChanged;
    const int kSecondLevelIndentOffset = 2;
    float verticalSpacing
    {
        get { return EditorGUI.kControlVerticalSpacing; }
    }

    public void Init (Object[] materials)
    {
        MaterialProperty[] props = MaterialEditor.GetMaterialProperties (materials);
    }

    public void OnGUI(MaterialEditor materialEditor)
    {
    }
}

} // namespace UnityEditor
", "")]
        [TestCase("LoadAchievementDescriptions",
@"using UnityEngine;
using UnityEngine.SocialPlatforms;
using System.Collections;

public class Example : MonoBehaviour {
  void Start() {
    Social.LoadAchievementDescriptions (descriptions => {
        if (descriptions.Length > 0) {
            Debug.Log (""Got "" + descriptions.Length + "" achievement descriptions"");
            string achievementDescriptions = ""Achievement Descriptions:\n"";
            foreach (IAchievementDescription ad in descriptions) {
                achievementDescriptions += ""\t"" +
                    ad.id + "" "" +
                    ad.title + "" "" +
                    ad.unachievedDescription + ""\n"";
            }
            Debug.Log (achievementDescriptions);
        }
        else
            Debug.Log (""Failed to load achievement descriptions"");
    });
  }
}
",
@"public class Example {
    function Start() {
        Social.LoadAchievementDescriptions(function(descriptions) {
            if (descriptions.Length > 0) {
                Debug.Log(""Got "" + descriptions.Length + "" achievement descriptions"");
                var achievementDescriptions = ""Achievement Descriptions:\n"";
                for (var ad in descriptions) {
                    achievementDescriptions += ""\t"" + ad.id + "" "" + ad.title + "" "" + ad.unachievedDescription + ""\n"";
                }
                Debug.Log(achievementDescriptions);
            }
            else
                Debug.Log(""Failed to load achievement descriptions"");
        }
        );
    }
}")]
        public void IntegrationTest(string title, string csInput, string expectedUnityScript)
        {
            AssertConversion(csInput, expectedUnityScript, false, usePragmaStrict: false);
        }
    }
}
