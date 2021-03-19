using System.IO;
using NUnit.Framework;

namespace UnderlyingModel.Tests
{
    [TestFixture]
    public partial class NewModelTests
    {
        NewDataItemProject m_NewDataItemProject;
        private const string TestsDirectory = "../../..";
        private string m_OldCurDir;

        [OneTimeSetUp]
        public void Init()
        {
            Directory.SetCurrentDirectory(TestContext.CurrentContext.TestDirectory);
            m_OldCurDir = Directory.GetCurrentDirectory();
            Directory.SetCurrentDirectory(TestsDirectory);
            var dirs = DirectoryCalculator.GetMemberItemDirectoriesAsmXml(
                "Tools/DocTools/TestDlls",
                "Tools/DocTools/TestXml");
            m_NewDataItemProject = new NewDataItemProject(dirs, false);
            m_NewDataItemProject.ReloadAllProjectData();
            var dummyDir = Path.Combine(TestsDirectory, "CsNoneTests");
            DirectoryUtil.CreateDirectoryIfNeeded(dummyDir);
        }

        [OneTimeTearDown]
        public void Uninit()
        {
            var dummyDir = Path.Combine(TestsDirectory, "CsNoneTests");
            if (Directory.Exists(dummyDir))
                DirectoryUtil.DeleteAllFiles(dummyDir);
            Directory.SetCurrentDirectory(m_OldCurDir);
        }

        [Test]
        public void ProjectNotEmpty()
        {
            Assert.IsTrue(m_NewDataItemProject.ItemCount > 0);
        }

        [TestCase("PlayerSettings.Android")]
        [TestCase("PlayerSettings.Android._bundleVersionCode")]
        [TestCase("PlayerSettings.iOS")]
        [TestCase("Color.ctor")]
        [TestCase("Color._g")]
        [TestCase("AndroidJavaObject.ctor")]
        [TestCase("EditorGUI.DropShadowLabel")]
        [TestCase("EditorGUIUtility.SetIconSize")]
        [TestCase("AndroidJavaObject.Dispose")]
        [TestCase("Animation.AddClip")]
        [TestCase("AndroidInput.GetSecondaryTouch")]
        [TestCase("AccelerationEvent")]
        [TestCase("AccelerationEvent._deltaTime")]
        [TestCase("AndroidSdkVersions.AndroidApiLevel9")]
        [TestCase("iOSTargetOSVersion._iOS_4_0")]
        [TestCase("iOSTargetOSVersion")]
        [TestCase("AndroidSdkVersions")]
        [TestCase("LayerMask.implop_LayerMask(int)")]
        [TestCase("LayerMask.implop_int(LayerMask)")]
        [TestCase("Color32.implop_Color(Color32)")]
        [TestCase("ADBannerView.ctor")]
        [TestCase("UnityEngine")]

        //xbox docs are no longer present, because of modularization, find another way to test this
        //[TestCase("X360Achievements.OnAward")] //public field
        //[TestCase("X360Achievements.BasicDelegate")] //public delegate

        [TestCase("CharacterInfo._flipped")]
        [TestCase("AssetPostprocessor")] //Editor/Mono/AssetPipeline
        [TestCase("EditorStyles")] //Editor/Mono/GUI
        [TestCase("PivotMode")] //Editor/Mono/GUI
        [TestCase("ViewTool")] //Editor/Mono/GUI
        [TestCase("GUI.ModalWindow")]
        [TestCase("Material._passCount")]
        [TestCase("ScriptableObject.OnEnable")] //csnone message
        [TestCase("Array._length")] //csnone property of a csnone class
        [TestCase("Hashtable.Count")]
        [TestCase("Application")]
        [TestCase("WSA.Application")]
        [TestCase("ADBannerView._onBannerWasClicked")] //correct event name, no add_, underscore added for lowercase
        [TestCase("AndroidJavaException")] //class in Asm that doesn't have a doc
        [TestCase("RectTransform._drivenByObject")]
        public void VerifyMemberPresent(string st)
        {
            Assert.IsTrue(m_NewDataItemProject.ContainsMember(st));
            Assert.IsTrue(m_NewDataItemProject.m_MapNameToItem[st].ItemName == st);
        }

        /*obsolete members are in fact in the DLL, just marked as such, create other tests that reflect that assumption
         * Keeping the commented out tests for inspiration for new good ones
        */
        //[TestCase("Vector3.AngleBetween")] //incorrect inclusion of obsolete methods
        //[TestCase("AnimationEvent._data")] //incorrect inclusion of obsolete properties
        //[TestCase("RenderTexture._enabled")] //incorrect inclusion of obsolete properties
        //[TestCase("TextureFormat.PVRTC_4BPP_RGB")] //incorrect inclusion of obsolete enum values
        [TestCase("ADBannerView.onBannerWasClicked")] //correct event
        [TestCase("ADBannerView._add_onBannerWasClicked")] //incorrect event name
        [TestCase("ADBannerView.add_onBannerWasClicked")] //incorrect event name
        [TestCase("ADBannerView..ctor")] //incorrect constructor name
        [TestCase("Matrix4x4._op_Multiply")] //incorrect multiply
        [TestCase("iPhoneKeyboard.dtor")] //no mem files should exist for destructors
        [TestCase("GUIView._void")]
        [TestCase("View._void")]
        [TestCase("ContainerWindow.ctor")]
        [TestCase("EditorGUIUtility._string")]
        [TestCase("EditorStyles._m_BoldFont")] //incorrect inclusion of internal fields
        [TestCase("PropertyDrawer._s_BuiltinAttributes")]
        public void VerifyMemberAbsent(string st)
        {
            Assert.IsFalse(m_NewDataItemProject.ContainsMember(st), "Member {0} shouldn't be in the assembly", st);
        }

        [TestCase("Quaternion.this", "Quaternion.Item")] //indexer
        [TestCase("Quaternion.this", "Quaternion._this(int)")] //incorrect indexer naming
        //[TestCase("EditorLook.LikeInspector", "LikeInspector")] //internal enum
        //[TestCase("X360AchievementType.Tournament", "Tournament")] // an Xbox enum member
        [TestCase("Application._persistentDataPath", "Application._endif")] //CSRAW #endif
        [TestCase("SocialPlatforms.Local", "Local")]
        [TestCase("Rendering.CullMode", "CullMode")]
        [TestCase("SocialPlatforms.Impl.Achievement", "Achievement")]
        public void ThisButNotThat(string dis, string dat)
        {
            Assert.IsTrue(m_NewDataItemProject.ContainsMember(dis), "Member {0} SHOULD be in the assembly", dis);
            Assert.IsFalse(m_NewDataItemProject.ContainsMember(dat), "Member {0} shouldn't be in the assembly", dat);
        }

        [TestCase("AndroidJavaException")]
        [TestCase("AnimationClipSettings")]
        [TestCase("AnimationClipSettings._cycleOffset")]
        public void HasAsmButNotDoc(string name)
        {
            var mem = m_NewDataItemProject.GetMember(name);
            Assert.IsTrue(mem.AnyHaveAsm);
            Assert.IsFalse(mem.AnyHaveDoc);
        }

        [TestCase("AccelerationEvent", 1, 1)]
        [TestCase("AccelerationEvent._deltaTime", 1, 1)]
        [TestCase("AccelerationEvent._acceleration", 1, 1)]
        [TestCase("ADBannerView.ctor", 1, 1)]
        //[TestCase("AndroidJavaObject.ctor", 2, 1)] //one of the constructors not present in the mem file
        [TestCase("AndroidJavaObject.Dispose", 1, 1)]
        [TestCase("AndroidInput.GetSecondaryTouch", 1, 1)]
        [TestCase("AndroidSdkVersions", 1, 1)]
        [TestCase("AndroidSdkVersions.AndroidApiLevel9", 1, 1)]
        [TestCase("Color.ctor", 2, 2)] // R,G,B and R,G,B,A
        [TestCase("Color._g", 1, 1)]
        [TestCase("Color32.implop_Color(Color32)", 1, 1)]
        [TestCase("iOSTargetOSVersion", 1, 1)]
        [TestCase("iOSTargetOSVersion._iOS_4_0", 1, 1)]
        [TestCase("EditorGUI.DropShadowLabel", 4, 4)]
        [TestCase("EditorGUIUtility.SetIconSize", 1, 1)]
        [TestCase("PlayerSettings.Android", 1, 1)]
        [TestCase("PlayerSettings.Android._bundleVersionCode", 1, 1)]
        [TestCase("Quaternion.this", 1, 1)]

        //these tests are no longer valid because of modularization, use them for inspiration for correct tests
        //[TestCase("X360Achievements.OnAward", 1, 1)]
        //[TestCase("X360Achievements.BasicDelegate", 1, 1)]
        [TestCase("AssetPostprocessor", 1, 1)] //Editor/Mono/AssetPipeline
        [TestCase("EditorStyles", 1, 1)] //Editor/Mono/GUI
        [TestCase("PivotMode", 1, 1)] //Editor/Mono/GUI
        [TestCase("ViewTool", 1, 1)] //Editor/Mono/GUI
        [TestCase("CustomPropertyDrawer", 1, 1)]
        public void VerifySignatureCount(string st, int expectedAsm, int expectedDoc)
        {
            Assert.IsTrue(m_NewDataItemProject.m_MapNameToItem[st].Signatures.Count > 0);
            //make sure it contains an asm entry and a doc entry
            var actualAsm = m_NewDataItemProject.NumAsmSignaturesForMember(st);
            Assert.AreEqual(expectedAsm, actualAsm, "expectedAsm={0}, actualAsm={1}", expectedAsm, actualAsm);
            var actualDoc = m_NewDataItemProject.NumDocSignatures(st);
            Assert.AreEqual(expectedDoc, actualDoc, "expectedDoc={0}, actualDoc={1}", expectedDoc, actualDoc);
        }

        [TestCase("AccelerationEvent")]
        [TestCase("AccelerationEvent._acceleration")]
        public void EverythingPresent(string st)
        {
            Assert.IsTrue(m_NewDataItemProject.ContainsMember(st));

            var item = m_NewDataItemProject.m_MapNameToItem[st];
            Assert.IsTrue(item.AnyHaveDoc);
            Assert.IsTrue(item.AllThatHaveAsmHaveDoc);
            Assert.IsTrue(item.AnyHaveAsm);
            Assert.IsTrue(item.AllThatHaveDocHaveAsm);
        }

        [Test]
        public void OutParameter()
        {
            const string kSt = "Collider.Raycast"; // has an out parameter
            Assert.IsTrue(m_NewDataItemProject.m_MapNameToItem.ContainsKey(kSt));
            Assert.AreEqual(1, m_NewDataItemProject.m_MapNameToItem[kSt].Signatures.Count);
            Assert.AreEqual(true, m_NewDataItemProject.m_MapNameToItem[kSt].Signatures[0].InBothAsmAndDoc);
        }

        [Test]
        public void DocOnlyChildrenOfRegularClass()
        {
            var pass = false;
            var item = m_NewDataItemProject.GetMember("ScriptableObject");
            foreach (var child in item.ChildMembers)
            {
                if (child.ItemName == "ScriptableObject.OnEnable")
                    pass = true;
            }
            Assert.IsTrue(pass);
        }

        [Test]
        public void ChildrenOfDocOnlyClass()
        {
            var pass = false;
            var item = m_NewDataItemProject.GetMember("Array");
            foreach (var child in item.ChildMembers)
            {
                if (child.ItemName == "Array.Join")
                    pass = true;
            }
            Assert.IsTrue(pass);
        }
    }
}
