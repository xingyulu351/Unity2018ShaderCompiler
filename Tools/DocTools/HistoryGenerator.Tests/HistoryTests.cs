using System.IO;
using NUnit.Framework;
using UnderlyingModel;

namespace HistoryGenerator.Tests
{
    [TestFixture]
    public class HistoryGeneratorTests
    {
        private NewDataItemProject m_MemberItemProject;
        private string m_OldCurDir;

        [OneTimeSetUp]
        public void Init()
        {
            Directory.SetCurrentDirectory(TestContext.CurrentContext.TestDirectory);
            m_OldCurDir = Directory.GetCurrentDirectory();
            Directory.SetCurrentDirectory("../../..");
            m_MemberItemProject = new NewDataItemProject();
            m_MemberItemProject.ReloadAllProjectData();
        }

        [OneTimeTearDown]
        public void Uninit()
        {
            Directory.SetCurrentDirectory(m_OldCurDir);
        }

        //mobile class UnityEngine
        [TestCase("class ADBannerView ||| 3.5.0", "ADBannerView | class | UnityEngine ||| 3.5.0")]
        //simple class, UnityEngine
        [TestCase("class AnimationEvent ||| 2.x.x", "AnimationEvent | class | UnityEngine ||| 2.x.x")]
        //simple class, UnityEditor
        [TestCase("class AnimationUtility ||| 2.x.x", "AnimationUtility | class | UnityEditor ||| 2.x.x")]
        //simple enum, UnityEngine
        [TestCase("enum AnimatorCullingMode ||| 4.0.0", "AnimatorCullingMode | enum | UnityEngine ||| 4.0.0")]
        //simple class that no longer exists
        [TestCase("class ADError ||| 3.5.0", "ADError | class |  ||| 3.5.0")]
        //enumval
        [TestCase("enumval AnimatorCullingMode AlwaysAnimate ||| 4.0.0", "AnimatorCullingMode.AlwaysAnimate | enumvalue | UnityEngine ||| 4.0.0")]
        //property, discard type
        [TestCase("prop Animation animatePhysics bool ||| 2.x.x", "Animation._animatePhysics | property | UnityEngine ||| 2.x.x")]
        //static prop, truncate subversions
        [TestCase("staticprop AndroidInput touchCountSecondary int ||| 3.3.0f4", "AndroidInput._touchCountSecondary | property (static) | UnityEngine ||| 3.3.0")]
        //regular method
        [TestCase("method Animation Rewind void Rewind () ||| 1.x.x", "Animation.Rewind | method | UnityEngine ||| 1.x.x")]
        //static method, UnityEditor
        [TestCase("staticmethod AssetDatabase Contains bool Contains (Object obj) ||| 2.x.x", "AssetDatabase.Contains | method (static) | UnityEditor ||| 2.x.x")]
        //constructor
        [TestCase("constructor AnimationClip AnimationClip AnimationClip () ||| 2.x.x", "AnimationClip.ctor | constructor | UnityEngine ||| 2.x.x")]
        //class with inheritance
        [TestCase("class Animation Behaviour, IEnumerable ||| 1.x.x", "Animation | class | UnityEngine ||| 1.x.x")]
        //class in a namespace (UnityEditor.Callbacks)
        [TestCase("class DidReloadScripts PostProcessAttribute ||| 4.1.0", "Callbacks.DidReloadScripts | class | UnityEditor.Callbacks ||| 4.1.0")]

        [TestCase("class GameCenterPlatform ISocialPlatform ||| 3.5.0", "SocialPlatforms.GameCenter.GameCenterPlatform | class | UnityEngine.SocialPlatforms.GameCenter ||| 3.5.0")]

        [TestCase("class IAchievement ||| 3.5.0", "SocialPlatforms.IAchievement | interface | UnityEngine.SocialPlatforms ||| 3.5.0")]

        // a class from an internal namespace, no proper namespace info
        [TestCase("class AssetStore ||| 3.2.0f1", "AssetStore | class |  ||| 3.2.0")]

        //a method inside a namespace
        [TestCase("staticmethod GameCenterPlatform ShowDefaultAchievementCompletionBanner void ShowDefaultAchievementCompletionBanner (bool value) ||| 3.5.0",
            "SocialPlatforms.GameCenter.GameCenterPlatform.ShowDefaultAchievementCompletionBanner | method (static) | UnityEngine.SocialPlatforms.GameCenter ||| 3.5.0")]

        //attribute class
        [TestCase("class NonSerialized Attribute ||| 2.x.x", "NonSerialized | class | System ||| 2.x.x")]

        //struct
        [TestCase("struct WheelHit ||| 1.x.x", "WheelHit | struct | UnityEngine ||| 1.x.x")]

        //tricky little operator
        [TestCase("staticmethod Color operator * Color operator * (Color a, Color b) ||| 1.x.x",
            "Color.op_Multiply | operator (static) | UnityEngine ||| 1.x.x")]

        //implicit operator => implop_returntype_params
        [TestCase("staticmethod Vector4 operator Vector2 implicit operator Vector2 (Vector4 v) ||| 3.4.0b3",
            "Vector4.implop_Vector2(Vector4) | implop (static) | UnityEngine ||| 3.4.0")]

        //indexer
        [TestCase("prop Vector2 this [int index] float ||| 1.x.x",
            "Vector2.this | property | UnityEngine ||| 1.x.x")]

        //delegate, CSNONE, need to get namespace from the parent
        [TestCase("overmethod AssetPostprocessor OnPostprocessAllAssets void OnPostprocessAllAssets (string[] importedAssets, string[] deletedAssets, string[] movedAssets, string[] movedFromPath) ||| 2.x.x",
            "AssetPostprocessor.OnPostprocessAllAssets | delegate | UnityEditor ||| 2.x.x")]

        [TestCase("enumval UserState Online ||| 3.5.0", "SocialPlatforms.UserState.Online | enumvalue | UnityEngine.SocialPlatforms ||| 3.5.0")]
        [TestCase("enumval UserState Offline ||| 3.5.0", "SocialPlatforms.UserState.Offline | enumvalue | UnityEngine.SocialPlatforms ||| 3.5.0")]
        [TestCase("class StaticOcclusionCullingVisualization ||| 3.0.0f1", "StaticOcclusionCullingVisualization | class | UnityEditor ||| 3.0.0")]
        public void HistoryTxtToMem(string input, string expectedOutput)
        {
            var hisPop = new HistoryPopulator(new MemberGetter(m_MemberItemProject));
            var versionEntry = hisPop.ConvertTxtToMemLine(input);
            var actualOutput = versionEntry.ToString();
            Assert.AreEqual(expectedOutput, actualOutput);
        }
    }
}
