using NUnit.Framework;

namespace UnderlyingModel.Tests
{
    [TestFixture]
    public partial class NewModelTests
    {
        [TestCase("ADInterstitialAd._loaded", "ADInterstitialAd-loaded")] //property
        [TestCase("BitStream", "BitStream")] //class
        [TestCase("Matrix4x4.GetColumn", "Matrix4x4.GetColumn")] //method
        [TestCase("Social.Active", "Social.Active")] //method
        [TestCase("Color32._r", "Color32-r")] //field
        [TestCase("Vector3.op_Plus", "Vector3-operator_add")] //op +
        [TestCase("Vector3.op_Minus", "Vector3-operator_subtract")] //op -
        [TestCase("Vector3.op_Multiply", "Vector3-operator_multiply")] //op *
        [TestCase("Vector3.op_Divide", "Vector3-operator_divide")] //op /
        [TestCase("Vector3.op_Equal", "Vector3-operator_eq")] //op ==
        [TestCase("Vector3.op_NotEqual", "Vector3-operator_ne")] // op !=
        [TestCase("Matrix4x4.this", "Matrix4x4.Index_operator")] // accessor
        [TestCase("Color32.implop_Color32(Color)", "Color32-operator_Color")] // implop
        [TestCase("Color32.implop_Color(Color32)", "Color32-operator_Color32")] // implop
        [TestCase("Array._length", "Array-length")] // Csnone property
        [TestCase("Array", "Array")] //CsNone class
        [TestCase("Hashtable.Count", "Hashtable.Count")] //property which doesn't follow naming convention
        [TestCase("SocialPlatforms.GameCenter.GameCenterPlatform", "SocialPlatforms.GameCenter.GameCenterPlatform")]
        [TestCase("SocialPlatforms.Range._count", "SocialPlatforms.Range-count")]
        [TestCase("SocialPlatforms.Range", "SocialPlatforms.Range")]
        [TestCase("PlayerSettings.MetroApplicationShowName.AllLogos", "PlayerSettings.MetroApplicationShowName.AllLogos")]
        [TestCase("PlayerSettings.iOS", "PlayerSettings.iOS")]
        [TestCase("iOSTargetOSVersion", "iOSTargetOSVersion")]
        public void MemberNameProducesHtmlName(string memberName, string expectedHtmlName)
        {
            MemberItem member = m_NewDataItemProject.GetMember(memberName);

            Assert.AreEqual(expectedHtmlName, member.HtmlName);
        }

        [TestCase("iOSTargetOSVersion", "iOSTargetOSVersion")]
        [TestCase("iOSTargetOSVersion._iOS_7_0", "iOSTargetOSVersion-iOS_7_0")]
        public void MemberNameProducesHtmlNameMemberGetter(string memberName, string expectedHtmlName)
        {
            var getter = new MemberGetter(m_NewDataItemProject);
            MemberItem member = getter.GetMember(memberName);

            Assert.AreEqual(expectedHtmlName, member.HtmlName);
        }

        [TestCase("ADInterstitialAd._loaded", "ADInterstitialAd.loaded")] //property
        [TestCase("BitStream", "BitStream")] //class
        [TestCase("Matrix4x4.GetColumn", "Matrix4x4.GetColumn")] //method
        [TestCase("ScriptableObject.OnEnable", "ScriptableObject.OnEnable")]
        [TestCase("Color32._r", "Color32.r")] //field
        [TestCase("Vector3.op_Plus", "Vector3.operator +")] //op +
        [TestCase("Vector3.op_Minus", "Vector3.operator -")] //op -
        [TestCase("Vector3.op_Multiply", "Vector3.operator *")] //op *
        [TestCase("Vector3.op_Divide", "Vector3.operator /")] //op /
        [TestCase("Vector3.op_Equal", "Vector3.operator ==")] //op ==
        [TestCase("Vector3.op_NotEqual", "Vector3.operator !=")] // op !=
        [TestCase("SocialPlatforms.Range._count", "Range.count")]
        [TestCase("SocialPlatforms.IAchievement", "IAchievement")] //interface
        [TestCase("iOSTargetOSVersion._iOS_7_0", "iOSTargetOSVersion.iOS_7_0")]
        public void MemberNameProducesDisplayName(string memberName, string expectedDisplayName)
        {
            MemberItem member = m_NewDataItemProject.GetMember(memberName);

            Assert.AreEqual(expectedDisplayName, member.DisplayName);
        }

        [TestCase("iOSTargetOSVersion", AssemblyType.Enum, "iOSTargetOSVersion", "iOSTargetOSVersion")]
        [TestCase("iOSTargetOSVersion.iOS_4_0", AssemblyType.EnumValue, "iOSTargetOSVersion", "iOSTargetOSVersion.iOS_4_0")]
        [TestCase("AnimatedValues.BaseAnimValue_1", AssemblyType.Class, "AnimatedValues.BaseAnimValue_1", "AnimatedValues.BaseAnimValue<T0>")]
        public void SignatureDisplayNameTests(string entryName, AssemblyType entryType, string curClass, string expectedDisplayName)
        {
            var actualDisplayName = MemberItem.SignatureDisplayName(entryName, entryType, curClass);
            Assert.AreEqual(expectedDisplayName, actualDisplayName);
        }

        [Test]
        public void MetroApplicationShowName()
        {
            MemberItem member = m_NewDataItemProject.GetMember("PlayerSettings.MetroApplicationShowName.AllLogos");
            var test = member.GetNames().ClassAndMember;
            Assert.AreEqual("PlayerSettings.MetroApplicationShowName.AllLogos", test);
        }

        [Test]
        public void ADBannerViewEvent()
        {
            MemberItem member = m_NewDataItemProject.GetMember("ADBannerView._onBannerWasClicked");
            var test = member.GetNames().ClassAndMember;
            Assert.AreEqual("ADBannerView.onBannerWasClicked", test);
        }

        //androidjavarunnable
        [Test]
        public void AndroidJavaRunnable()
        {
            MemberItem member = m_NewDataItemProject.GetMember("AndroidJavaRunnable");
            Assert.IsNotNull(member);
            Assert.AreEqual("AndroidJavaRunnable", member.HtmlName);
            Assert.AreEqual("AndroidJavaRunnable", member.DisplayName);
            Assert.AreEqual("AndroidJavaRunnable", member.SignatureDisplayName());
            var names = member.GetNames();
            Assert.AreEqual("", names.NamespaceAndClass);
        }

        [Test]
        public void GenericUnityEvent1()
        {
            MemberItem member = m_NewDataItemProject.GetMember("Events.UnityEvent_1");
            Assert.IsNotNull(member);
            Assert.AreEqual("Events.UnityEvent_1", member.ItemName);
            Assert.AreEqual("Events.UnityEvent_1", member.HtmlName);
            Assert.AreEqual("UnityEvent<T0>", member.DisplayName);
        }

        [Test]
        public void GenericUnityEvent2()
        {
            MemberItem member = m_NewDataItemProject.GetMember("Events.UnityEvent_2");
            Assert.IsNotNull(member);
            Assert.AreEqual("Events.UnityEvent_2", member.ItemName);
            Assert.AreEqual("Events.UnityEvent_2", member.HtmlName);
            Assert.AreEqual("UnityEvent<T0,T1>", member.DisplayName);
        }

        [Test]
        public void GenericUnityAction()
        {
            MemberItem member = m_NewDataItemProject.GetMember("Events.UnityAction_1");
            Assert.IsNotNull(member);
            Assert.AreEqual("Events.UnityAction_1", member.ItemName);
            Assert.AreEqual("Events.UnityAction_1", member.HtmlName);
            Assert.AreEqual("UnityAction<T0>", member.DisplayName);
            var names = member.GetNames();
            Assert.AreEqual("Events", names.NamespaceAndClass);
        }

        [Test]
        public void TweenRunnerInit()
        {
            MemberItem member = m_NewDataItemProject.GetFirstMemberSimilarTo("TweenRunner_1.Init");
            Assert.IsNotNull(member);
            Assert.AreEqual("UI.CoroutineTween.TweenRunner_1.Init", member.ItemName);
            Assert.AreEqual("CoroutineTween.TweenRunner_1.Init", member.HtmlName);
            Assert.AreEqual("TweenRunner<T0>.Init", member.DisplayName);

            var names = member.GetNames();
            member.MakeNamesGeneric(ref names);
            Assert.AreEqual("CoroutineTween.TweenRunner<T0>", names.NamespaceAndClass);
        }

        [Test]
        public void TweenRunner()
        {
            MemberItem member = m_NewDataItemProject.GetFirstMemberSimilarTo("TweenRunner");
            Assert.IsNotNull(member);
            Assert.AreEqual("UI.CoroutineTween.TweenRunner_1", member.ItemName);
            Assert.AreEqual("CoroutineTween.TweenRunner_1", member.HtmlName);
            Assert.AreEqual("TweenRunner<T0>", member.DisplayName);
        }
    }
}
