using System;
using System.Diagnostics;
using System.IO;
using System.Text.RegularExpressions;
using NUnit.Framework;
using System.Linq;
using Unity.CommonTools;

namespace BindingsToCsAndCpp.Tests
{
    [TestFixture]
    public class BindingsToCsAndCppTests : ITemplateTest
    {
        const string defaultTemplatePath = "BindingsToCsAndCpp_Generated";
        const string defaultZipFile = "BindingsToCsAndCpp_Templates.zip";

        public string DefaultTemplatePath { get { return defaultTemplatePath; } }
        public string DefaultZipFile { get { return defaultZipFile; } }

        string templatePath = Path.GetFullPath(Path.Combine(Workspace.BasePath, "artifacts", "Tests", "UnityBindingsParser", "BindingsToCsAndCpp", defaultTemplatePath));
        string resultPath = Path.GetFullPath(Path.Combine(Workspace.BasePath, "artifacts", "Tests", "UnityBindingsParser", "BindingsToCsAndCpp", "TestResults"));
        public string TemplatePath
        {
            get { return templatePath; }
            set { templatePath = value; }
        }

        public string ResultPath
        {
            get { return resultPath; }
        }

        string zipFile =  Path.GetFullPath(Path.Combine(Workspace.BasePath, "Tools", "UnityBindingsParser", "BindingsToCsAndCpp", "Tests", defaultZipFile));
        public string ZipFile
        {
            get { return zipFile; }
            set { zipFile = value; }
        }

        public bool GenerateTemplates { get; set; }

        PreProcessRunner runner = new NewCsPreProcessRunner();

        [TestFixtureSetUp]
        public void Setup()
        {
            if (GenerateTemplates)
                return;

            if (TemplatePath == defaultTemplatePath)
                TemplatePath = Path.Combine(Workspace.BasePath, "Tools", "UnityBindingsParser", "BindingsToCsAndCpp", "Tests", defaultTemplatePath);

            if (ZipFile == defaultZipFile)
                ZipFile = Path.Combine(Workspace.BasePath, "Tools", "UnityBindingsParser", "BindingsToCsAndCpp", "Tests", defaultZipFile);

            Console.WriteLine("Using templates at {0}", TemplatePath);
            if (File.Exists(zipFile) && !Directory.Exists(templatePath))
            {
                Console.WriteLine("Populating templates from {0}", ZipFile);
                BindingsToCsAndCpp.TestGenerator.ZipUtils.ExtractZipFile(zipFile, templatePath);
            }
        }

        [Test]
        public void SingleCSRAW()
        {
            AssertProducesEqualOutputCS("CSRAW hell222o");
        }

        [Test]
        public void SingleCppRAW()
        {
            AssertProducesEqualOutputCPP(@"C++RAW yeah

");
        }

        [Test]
        public void RawAndEnum()
        {
            var txt = @"
            CSRAW
using bla;
ENUM ScaleMode
    StretchToFill = 0,
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void ClassTest()
        {
            var txt = @"
CSRAW
using System;

CLASS Camera : Behaviour
    CSRAW public float fov { get { return fieldOfView; } set { fieldOfView = value; } }
END
";

            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void NamespaceTest()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEditor {
CLASS Test
    CUSTOM public void Test1 ()
    {
    }
END
}
namespace UnityEditorInternal {
CLASS Camera : Behaviour
    CUSTOM public void MyMethod ()
    {
    }
END
}
";

            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void NamespaceTest2()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEditor {
CLASS Test
    CUSTOM public void Test1 ()
    {
    }
END

namespace UnityEditorInternal {
CLASS Camera : Behaviour
    CUSTOM public void MyMethod ()
    {
    }
END
}
}
";

            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void NamespaceTest_Compare()
        {
            var txt1 = @"
CSRAW
using System;
namespace UnityEngine.WSA
{
CLASS File
    CUSTOM public static byte[] ReadAllBytes(string path)
    {
    }
END
}
";

            var txt2 = @"
CSRAW
using System;
namespace UnityEngine
{
namespace WSA
{
CLASS File
    CUSTOM public static byte[] ReadAllBytes(string path)
    {
    }
END
}
}";

            AssertProducesEqualOutputCPP(txt1);
            AssertProducesEqualOutputCPP(txt2);
        }

        [Test]
        public void SimpleCustomMethod()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEditor.One.Two.Three {
{
CLASS Camera : Behaviour
    CUSTOM public void MyMethod ()
    {
        //@TODO: This can add assets to anything, do asset database checks if the operaiton is sensible
        MakeAssetPersistent((EditorExtension&)*objectToAdd, ConvertPathSeperators(assetPath));
    }
END
";

            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void SimpleCustomMethodWithWhitespace()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS Camera : Behaviour
    CUSTOM public void               MyMethod ()
    {
    }
END
";

            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void SimpleCustomMethodWithWhitespace2()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS Camera : Behaviour
    CUSTOM public void               MyMethod()
    {
    }
END
";

            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void NewLinesRemovedBetweenUsingAndNamespace()
        {
            var txt = @"
CSRAW
using System;

using System.IO;

namespace UnityEngine
{
";

            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void NewLinesAreKeptBeforeNamespace()
        {
            var txt = @"
CSRAW




namespace UnityEngine
{
";

            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void CustomMethodWithBody()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS Camera : Behaviour
    CUSTOM public void MyMethod ()
    {
        a++;
    }
END
";

            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void CustomMethodWithArgument()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS Camera : Behaviour
    CUSTOM public void MyMethod (int a)
    {
        a++;
    }
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void StaticCustomMethodWithArgument()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS Camera : Behaviour
    CUSTOM public static void MyMethod (int a)
    {
        a++;
    }
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void CustomMethodWithIntArrayArgument()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS Camera : Behaviour
    CUSTOM public static void MyMethod (int[] a)
    {
        a++;
    }
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void CustomMethodWithTexture2DArgument()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS Camera : Behaviour
    CUSTOM public static void MyMethod (Texture2D tex)
    {
        a++;
    }
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void CustomMethodWithWritableTexture2DArgument()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS Camera : Behaviour
    CUSTOM public static void MyMethod ([Writable]Texture2D tex)
    {
        a++;
    }
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void CustomMethodWithVector3Argument()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS Camera : Behaviour
    CUSTOM public static void MyMethod (Vector3 v)
    {
        a++;
    }
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void CustomMethodWithStructAndClassArgument()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS Camera : Behaviour
    CUSTOM public static void MyMethod (Vector3 v, Texture2D tex)
    {
        a++;
    }
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void CustomMethodWithStructAndClassArgumentNoSpaces()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS Camera : Behaviour
    CUSTOM public static void MyMethod (Vector3 v,Texture2D tex)
    {
        a++;
    }
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void CustomMethodWithStructAndWritableClassArgument()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS Camera : Behaviour
    CUSTOM public static void MyMethod (Vector3 v, [Writable]Texture2D tex)
    {
        a++;
    }
END
";

            AssertProducesEqualOutputCS(txt);
        }

        [Test]
        public void ConditionalEmpty()
        {
            var txt = @"
CSRAW
CONDITIONAL UNITY_EDITOR";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test(Description = @"This demonstrates how to use the FLUSHCONDITIONS tag to force the perl parser to create
correct code. This tag is ignored in the c# parser.")]
        public void Conditional_SucceedsWithFlushConditions()
        {
            var txt = @"
CSRAW
CONDITIONAL UNITY_EDITOR
FLUSHCONDITIONS
FLUSHCONDITIONS
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void ConditionalWithClasses()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CONDITIONAL UNITY_EDITOR
CLASS Class1 : Behaviour
END
CLASS Class2 : Behaviour
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void ConditionalWithClassAndCustom()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS Camera : Behaviour
CONDITIONAL ACONDITION
    CUSTOM public static void MyMethod (Vector3 v, [Writable]Texture2D tex)
    {
        a++;
    }
END
";
            AssertProducesEqualOutputCS(txt);
        }

        [Test]
        public void ConditionalWithClassAndCustom2()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CONDITIONAL ACONDITION
CLASS Camera : Behaviour
    CUSTOM public static void MyMethod (Vector3 v, [Writable]Texture2D tex)
    {
        a++;
    }
END
";
            AssertProducesEqualOutputCS(txt);
        }

        [Test]
        public void ConditionalWithClassAndEnum()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS Class1 : Parent1
    CUSTOM public static void MyMethod ()
    {
        a++;
    }
END
CONDITIONAL ACONDITION
ENUM ScaleMode
    StretchToFill = 0,
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void ConditionalWithCustomProp()
        {
            var txt = @"
CSRAW
namespace UnityEngine
{
CLASS Camera : Behaviour
CONDITIONAL ACONDITION
    CUSTOM_PROP static int myProp
    {
        return 0;
    }
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void ConditionalWithCustomProp2()
        {
            var txt = @"
CSRAW
namespace UnityEngine
{
CLASS Camera : Behaviour
CONDITIONAL ACONDITION
    CUSTOM_PROP static int myProp
    {
        return 0;
    }
    CUSTOM_PROP static int myProp2
    {
        return 0;
    }
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void ConditionalWithCustomProp3()
        {
            var txt = @"
CSRAW
namespace UnityEngine
{
CONDITIONAL ACONDITION
CLASS Camera
    CUSTOM_PROP static int myProp
    {
        return 0;
    }
    CUSTOM_PROP static int myProp2
    {
        return 0;
    }
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void ConditionalWithCustomProp4()
        {
            var txt = @"
CSRAW
namespace UnityEngine
{
CLASS Camera
CONDITIONAL ACONDITION
    CUSTOM_PROP static int myProp
    {
        return 0;
    }
    CUSTOM_PROP static int myProp2
    {
        return 0;
    }
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void ConditionalInClassAndCustomProp()
        {
            var txt = @"
CSRAW
namespace UnityEngine
{
CONDITIONAL ACONDITION
CLASS Camera
CONDITIONAL ANOTHERCONDITION
    CUSTOM_PROP static int myProp
    {
        return 0;
    }
    CUSTOM_PROP static int myProp2
    {
        return 0;
    }
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void ConditionalInClassAndCustomProp2()
        {
            var txt = @"
CSRAW
namespace UnityEngine
{
CONDITIONAL ACONDITION
CLASS Camera
CONDITIONAL ANOTHERCONDITION
    CUSTOM_PROP static int myProp
    {
        return 0;
    }
    CUSTOM_PROP static int myProp2
    {
        return 0;
    }
    CUSTOM_PROP static int myProp3
    {
        return 0;
    }
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void ConditionalInClassAndCustomProp3()
        {
            var txt = @"
CSRAW
namespace UnityEngine
{
CONDITIONAL ACONDITION
CLASS Camera
    CUSTOM_PROP static int myProp
    {
        return 0;
    }
    CUSTOM_PROP static int myProp2
    {
        return 0;
    }
CONDITIONAL ANOTHERCONDITION
    CUSTOM_PROP static int myProp3
    {
        return 0;
    }
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void SameConditionalInTwoClasses()
        {
            var txt = @"
CSRAW
namespace UnityEngine
{
CONDITIONAL ACONDITION
CLASS Class1
    CUSTOM_PROP static int myProp1
    {
        return 0;
    }
END
CONDITIONAL ACONDITION
CLASS Class2
    CUSTOM_PROP static int myProp2
    {
        return 0;
    }
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void CustomProp()
        {
            var txt = @"
CSRAW
namespace UnityEngine
{
CLASS Camera : Behaviour
    CUSTOM_PROP static int myProp
    {
        return 0;
    }
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void CustomPropNew()
        {
            var txt = @"
CSRAW
namespace UnityEngine
{
CLASS Camera
    CUSTOM_PROP new static int myProp
    {
        return 0;
    }
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void CustomPropOverride()
        {
            var txt = @"
CSRAW
namespace UnityEngine
{
CLASS Camera
    CUSTOM_PROP override static int myProp
    {
        return 0;
    }
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void CustomPropGetterSetter()
        {
            var txt = @"
CSRAW
namespace UnityEngine
{
CLASS Camera : Behaviour
    CUSTOM_PROP static int myProp
    {
        return 0;
    }
    {
    }
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void CustomPropGetterSetterReturnsStruct()
        {
            var txt = @"
CSRAW
namespace UnityEngine
{
CLASS Camera : Behaviour
    CUSTOM_PROP Vector3 Vector3PropGetterSetter { return null; }  { a = value; }
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void CustomMethodNew()
        {
            var txt = @"
CSRAW
namespace UnityEngine
{
CLASS Camera
    CUSTOM new static int myProp()
    {
        return 0;
    }
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void CustomMethodOverride()
        {
            var txt = @"
CSRAW
namespace UnityEngine
{
CLASS Camera
    CUSTOM override static int myProp()
    {
        return 0;
    }
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void CustomMethodWithDefaultArgumentValue()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS Camera : Behaviour
    CUSTOM static IntPtr      GetConstructorID(IntPtr javaClass, string signature = """")
    {
        a++;
    }
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void CustomMethodWithDefaultArgumentValue2()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS Camera : Behaviour
    CUSTOM static IntPtr      GetConstructorID(IntPtr javaClass, string signature = """", int another = 2)
    {
        a++;
    }
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void CustomInternalMethod()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS Camera : Behaviour
    CUSTOM internal void SetParameter()
    {
        a++;
    }
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        [Ignore]
        [ExpectedException(typeof(UnbalancedBracesException))]
        public void UnbalancedBraces()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS Camera : Behaviour
    CUSTOM internal void SetParameter() {
    {
        a++;
    }
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void ClassAndConditionalClass()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS Camera : Behaviour
    CUSTOM internal void SetParameter()
    {
        a++;
    }
END

CONDITIONAL UNITY_EDITOR
CLASS internal HumanTrait : Object

    CUSTOM_PROP static int MuscleCount
    {
        return HumanTrait::MuscleCount;
    }
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void AutoProp()
        {
            var txt = @"
CSRAW
namespace UnityEngine
{
CLASS Camera : Behaviour
    AUTO_PROP float frameRate GetSampleRate
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void AutoPropWithSetter()
        {
            var txt = @"
CSRAW
namespace UnityEngine
{
CLASS Camera : Behaviour
    AUTO_PROP float frameRate GetSampleRate SetSampleRate
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void AutoPropString()
        {
            var txt = @"
CSRAW
namespace UnityEngine
{
CLASS Camera : Behaviour
    AUTO_PROP string frameRate GetSampleRate
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void AutoPropStringWithSetter()
        {
            var txt = @"
CSRAW
namespace UnityEngine
{
CLASS Camera : Behaviour
    AUTO_PROP string frameRate GetSampleRate SetSampleRate
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void AutoPropSingleName()
        {
            var txt = @"
CSRAW
namespace UnityEngine
{
CLASS Camera : Behaviour
    AUTO_PROP string frameRate SampleRate
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void AutoPropSingleNameWithSimilarTypename()
        {
            var txt = @"
CSRAW
namespace UnityEngine
{
CLASS Camera : Behaviour
    AUTO_PROP bool isReady Ready
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void AutoPropVector3()
        {
            var txt = @"
CSRAW
namespace UnityEngine
{
CLASS Camera : Behaviour
AUTO_PROP Vector3 vector3Auto Vector3Auto
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void CustomWithArrayReturn()
        {
            var txt = @"
CSRAW
namespace UnityEngine
{
CLASS Camera : Behaviour
    CUSTOM Boolean[] GetArray()
    {
        return null;
    }
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void ConditionalSameInDifferentMethods()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS MyClass
CONDITIONAL ACONDITION
    CUSTOM public static void MyMethod ()
    {
        a++;
    }
CONDITIONAL ACONDITION
    CUSTOM public static void MyMethod1 ()
    {
        a++;
    }
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void CustomPropVector3()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS MyClass
    CUSTOM_PROP Vector3 averageSpeed { return self->GetAverageSpeed(); }
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void TestAllPossibleTypeCombos()
        {
            /* INVALID

             AUTO_PROP static bool staticboolAutoGetterSetter GetBoolAuto2 SetBoolAuto2
             AUTO_PROP string stringAutoGetterSetter aStringAuto anotherStringAuto
             AUTO_PROP private bool privateboolAuto BoolAutoPrivate
             AUTO_PROP private bool private_boolAutoPrivate GetBoolAutoPrivate SetBoolAutoPrivate
             AUTO_PROP private static bool private_static_boolAuto private_static_BoolAuto
             AUTO_PROP private static bool private_static_boolAutoGetterSetter_private GetBoolAuto_private_static SetBoolAuto_private_static
             AUTO_PROP static private bool static_private_boolAuto private_static_BoolAuto
             AUTO_PROP static private bool static_private_boolAutoGetterSetter GetBoolAuto_static_private SetBoolAuto_static_private
             *

             GENERATES BAD OUTPUT
             * AUTO_PROP static bool staticboolAuto static_BoolAuto
             */


            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS MyClass
    CUSTOM_PROP int intPropGetter { return null; }
    CUSTOM_PROP int intPropGetterSetter { return null; }  { a = value; }

    CUSTOM_PROP string stringPropGetter { return null; }
    CUSTOM_PROP string stringPropGetterSetter { return null; }  { a = value; }

    CUSTOM_PROP Vector3 Vector3PropGetter { return null; }
    CUSTOM_PROP Vector3 Vector3PropGetterSetter { return null; }  { a = value; }

    CUSTOM_PROP static int static_intPropGetter { return null; }
    CUSTOM_PROP private int private_intPropGetter { return null; }
    CUSTOM_PROP private static int private_static_intPropGetter { return null; }
    CUSTOM_PROP static private int static_private_intPropGetter { return null; }

    AUTO_PROP bool boolAuto BoolAuto
    AUTO_PROP bool boolAutoGetterSetter BoolAutoGetter BoolAutoSetter

    AUTO_PROP string stringAuto StringAuto
    AUTO_PROP string stringAutoGetterSetter GetStringAuto SetStringAuto

    AUTO_PROP Vector3 vector3Auto Vector3Auto
    AUTO_PROP Vector3 vector3AutoGetterSetter GetVector3Auto SetVector3Auto

    AUTO_PTR_PROP Flare flarePtr GetFlarePtr SetFlarePtr
    AUTO_PROP Flare flare GetFlare SetFlare

    AUTO_PTR_PROP bool boolAutoPtr BoolAutoPtr
    AUTO_PTR_PROP bool boolAutoGetterSetterPtr BoolAutoGetterPtr BoolAutoSetterPtr

    AUTO_PTR_PROP string stringAutoPtr StringAutoPtr
    AUTO_PTR_PROP string stringAutoGetterSetterPtr GetStringAutoPtr SetStringAutoPtr

    AUTO_PTR_PROP Vector3 vector3AutoPtr Vector3AutoPtr
    AUTO_PTR_PROP Vector3 vector3AutoGetterSetterPtr GetVector3AutoPtr SetVector3AutoPtr


    CUSTOM int intMethod () { a++; }
    CUSTOM string stringMethod () { a++; }
    CUSTOM IntPtr IntPtrMethod () { a++; }
    CUSTOM Vector3 Vector3Method () { a++; }


    CUSTOM static int static_intMethod () { a++; }
    CUSTOM private int private_intMethod () { a++; }
    CUSTOM private static int private_static_intMethod () { a++; }
    CUSTOM static private int static_private_intMethod () { a++; }

    CUSTOM void voidMethodVector3 (Vector3 a1) { a++; }
    CUSTOM void voidMethodVector3Out (out Vector3 a1) { a++; }
    CUSTOM void voidMethodVector3Ref (ref Vector3 a1) { a++; }
    CUSTOM void voidMethodVector3OutIntOut (out Vector3 a1, out int b) { a++; }
    CUSTOM void voidMethodVector3OutVector3Out (out Vector3 a1, out Vector3 b) { a++; }
    CUSTOM void voidMethodVector3RefVector3Ref (ref Vector3 a1, ref Vector3 b) { a++; }
    CUSTOM void voidMethodWritableObject ([Writable]Object rhs) { a++; }

    CUSTOM static void staticvoidMethodVector3 (Vector3 a1) { a++; }

    CUSTOM static void      StaticVoidMethodStringDef(string argDef1 = """") { a++; }
    CUSTOM static void      StaticVoidMethodStringStringDef(string arg1, string argDef1 = """") { a++; }
    CUSTOM static void      StaticVoidMethodStringStringDefStringDef(string arg1, string argDef1 = """", string argDef2 = """") { a++; }
    CUSTOM static void      StaticVoidMethodStringStringStringDef(string arg1, string argDef1 = """") { a++; }

    THREAD_SAFE CUSTOM static IntPtr      SafeIntPtrMethodIntPtrStringDef(IntPtr javaClass, string signature = """") { a++; }
    CUSTOM static IntPtr      IntPtrMethodIntPtrStringDef(IntPtr javaClass, string signature = """") { a++; }

    THREAD_SAFE CUSTOM void SafevoidMethodVector3 (Vector3 a1) { a++; }
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void Bug_498674()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS MyClass
    CUSTOM public void PublicMethodVector3WritableTexture2D (Vector3 v, [Writable]Texture2D tex) { a++; }
    CUSTOM public static void PublicStaticMethodVector3WritableTexture2D (Vector3 v, [Writable]Texture2D tex) { a++; }
END
";

            AssertProducesEqualOutputCS(txt);
        }

        [Test]
        public void Auto()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEditor
{
CLASS MyClass
    AUTO void EnsureQuaternionContinuity();
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void Auto2()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS Camera
    AUTO void EnsureQuaternionContinuity();
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void Auto_ReturnsBool()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS MyClass
    AUTO bool EnsureQuaternionContinuity();
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void Auto_ReturnsString()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS MyClass
    AUTO string EnsureQuaternionContinuity();
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void Auto_ReturnsStruct()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS MyClass
    AUTO Vector3 EnsureQuaternionContinuity();
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void Auto_Bool()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS MyClass
    AUTO void EnsureQuaternionContinuity(bool arg1);
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void Auto_Bool2()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS MyClass
    AUTO void EnsureQuaternionContinuity(bool arg1, bool arg2);
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void Auto_String()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS MyClass
    AUTO void EnsureQuaternionContinuity(string arg1);
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void Auto_StringStruct()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS MyClass
    AUTO void EnsureQuaternionContinuity(string arg1, Vector3 arg2);
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void Auto_StringUnknownType()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS MyClass
    AUTO void EnsureQuaternionContinuity(string arg1, SomethingElse arg2);
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void OverLoads()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS MyClass
    CUSTOM void MatchTarget(Vector3 matchPosition,  Quaternion matchRotation, AvatarTarget targetBodyPart,  MatchTargetWeightMask weightMask, float startNormalizedTime, float targetNormalizedTime = 1)
    {
        self->MatchTarget(matchPosition, matchRotation, (int)targetBodyPart, weightMask, startNormalizedTime, targetNormalizedTime);
    }
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void OverloadsInCSRAW()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS MyClass
    CSRAW public bool Play (PlayMode mode = PlayMode.StopSameLayer) { return PlayDefaultAnimation (mode); }

    CSRAW public bool Play (PlayMode mode ) { return PlayDefaultAnimation (mode); }
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void ConditionalRawContent()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS MyClass
    CONDITIONAL ENABLE_AUDIO
    CSRAW public static void PlayClipAtPoint (AudioClip clip, Vector3 position, float volume = 1.0F)
    {
        GameObject go = new GameObject (""One shot audio"");
        go.transform.position = position;
        AudioSource source = (AudioSource)go.AddComponent (typeof(AudioSource));
        source.clip = clip;
        source.volume = volume;
        source.Play ();
        // Note: timeScale > 1 means that game time is accelerated. However, the sounds play at their normal speed, so we need to postpone the point in time, when the sound is stopped.
        Destroy (go, clip.length * Time.timeScale);
    }
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void ObsoleteRawContent()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS MyClass
    OBSOLETE planned Returns the animation clip named /name/.
    CSRAW public AnimationClip GetClip (string name) {
        return null;
    }
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void ObsoleteRawContent2()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS MyClass
    CSRAW
    OBSOLETE planned Returns the animation clip named /name/.
    public AnimationClip GetClip (string name) {
        return null;
    }
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void ObsoleteConditionalRawContent()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS MyClass
    OBSOLETE planned Returns the animation clip named /name/.
    CONDITIONAL ENABLE_AUDIO
    CSRAW public AnimationClip GetClip (string name) {
        return null;
    }
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void ObsoleteConditionalRawContent2()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS MyClass
    CONDITIONAL ENABLE_AUDIO
    OBSOLETE planned Returns the animation clip named /name/.
    CSRAW public AnimationClip GetClip (string name) {
        return null;
    }
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void ObsoleteConditionalRawContent3()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS MyClass
    OBSOLETE planned Returns the animation clip named /name/.
    CONDITIONAL ENABLE_AUDIO
    CSRAW public AnimationClip GetClip (string name) {
        return null;
    }
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void ObsoleteConditionalRawContent4()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS MyClass
    CSRAW
    CONDITIONAL ENABLE_AUDIO
    OBSOLETE planned Returns the animation clip named /name/.
    public AnimationClip GetClip (string name) {
        return null;
    }
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void ObsoleteConditionalRawContent5()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS MyClass
    CSRAW
    OBSOLETE planned Returns the animation clip named /name/.
    CONDITIONAL ENABLE_AUDIO
    public AnimationClip GetClip (string name) {
        return null;
    }
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void ObsoleteWarningRawContent()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS MyClass
    OBSOLETE warning A Warning
    CSRAW public AnimationClip GetClip (string name) {
        return null;
    }
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void ObsoleteWarningRawContent2()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS MyClass
    CSRAW
    OBSOLETE warning A Warning
    public AnimationClip GetClip (string name) {
        return null;
    }
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void ObsoleteWarningConditionalRawContent()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS MyClass
    OBSOLETE warning A Warning
    CONDITIONAL ENABLE_AUDIO
    CSRAW public AnimationClip GetClip (string name) {
        return null;
    }
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void ObsoleteWarningConditionalRawContent2()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS MyClass
    CONDITIONAL ENABLE_AUDIO
    OBSOLETE warning A Warning
    CSRAW public AnimationClip GetClip (string name) {
        return null;
    }
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void ObsoleteWarningConditionalRawContent3()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS MyClass
    OBSOLETE warning A Warning
    CONDITIONAL ENABLE_AUDIO
    CSRAW public AnimationClip GetClip (string name) {
        return null;
    }
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void ObsoleteWarningConditionalRawContent4()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS MyClass
    CSRAW
    CONDITIONAL ENABLE_AUDIO
    OBSOLETE warning A Warning
    public AnimationClip GetClip (string name) {
        return null;
    }
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void ObsoleteWarningConditionalRawContent5()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS MyClass
    CSRAW
    OBSOLETE warning A Warning
    CONDITIONAL ENABLE_AUDIO
    public AnimationClip GetClip (string name) {
        return null;
    }
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void ObsoleteErrorRawContent()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS MyClass
    OBSOLETE error An Error
    CSRAW public AnimationClip GetClip (string name) {
        return null;
    }
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void ObsoleteErrorRawContent2()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS MyClass
    CSRAW
    OBSOLETE error An Error
    public AnimationClip GetClip (string name) {
        return null;
    }
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void ObsoleteErrorConditionalRawContent()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS MyClass
    OBSOLETE error An Error
    CONDITIONAL ENABLE_AUDIO
    CSRAW public AnimationClip GetClip (string name) {
        return null;
    }
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void ObsoleteErrorConditionalRawContent2()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS MyClass
    CONDITIONAL ENABLE_AUDIO
    OBSOLETE error An Error
    CSRAW public AnimationClip GetClip (string name) {
        return null;
    }
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void ObsoleteErrorConditionalRawContent3()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS MyClass
    OBSOLETE error An Error
    CONDITIONAL ENABLE_AUDIO
    CSRAW public AnimationClip GetClip (string name) {
        return null;
    }
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void ObsoleteErrorConditionalRawContent4()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS MyClass
    CSRAW
    CONDITIONAL ENABLE_AUDIO
    OBSOLETE error An Error
    public AnimationClip GetClip (string name) {
        return null;
    }
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void ObsoleteErrorConditionalRawContent5()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS MyClass
    CSRAW
    OBSOLETE warning A Warning
    CONDITIONAL ENABLE_AUDIO
    public AnimationClip GetClip (string name) {
        return null;
    }
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void CustomVsCSRaw()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS MyClass
    CUSTOM public static void Custom (Vector3 start, Vector3 end, Color color = Color.white, float duration = 0.0f, bool depthTest = true) { DebugDrawLine (start, end, color, duration, depthTest); }
    CSRAW public static void Raw (Vector3 start, Vector3 end, Color color = Color.white, float duration = 0.0f, bool depthTest = true) { DebugDrawLine (start, end, color, duration, depthTest); }
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void EnumWithIfBlocks()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
ENUM TextureFormat
    DXT5 = 12,
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void DefaultParameters()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS MyClass
    CUSTOM void Apply (bool updateMipmaps=true, bool makeNoLongerReadable=false) {}
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void AutoPropGet()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS MyClass
    AUTO_PROP bool isReadable GetIsReadable
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void Comments()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
    CLASS internal ScrollViewState
        CSRAW
        ///*undocumented*
        public Rect scrollTo;

        ///*undocumented*
        /*public void ScrollTo (Rect position) {
            Vector2 pos = GUIClip.Unclip (new Vector2 (position.xMin, position.yMin));
            if (!hasScrollTo) {
            }
        }*/

        internal void ScrollTo (Rect position) {
            Vector2 pos = new Vector2 (position.xMin, position.yMin);
            if (!hasScrollTo) {
            }
        }
    END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void RawContent()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
    CLASS internal ScrollViewState
    CSRAW static internal void CheckOnGUI() {
    /// @TODO: Implement
//      if (GUIState.s_GUIState.m_OnGUIDepth < 1) {
//          throw new ArgumentException(""You can only call GUI functions from inside OnGUI."");
//      }
    }

    CUSTOM_PROP static internal bool mouseUsed
        { return GetGUIState().m_CanvasGUIState.m_IsMouseUsed != 0; }
        { GetGUIState().m_CanvasGUIState.m_IsMouseUsed = value ? 1 : 0; }

    static internal Vector2 s_EditorScreenPointOffset = Vector2.zero;
    static internal bool s_HasKeyboardFocus = false;
    END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void PropertiesInStructs()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
    STRUCT aStruct
        CUSTOM_PROP bool isIdentity { return self.IsIdentity(); }
    END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void PropertiesInStructsWithSetter()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
    STRUCT aStruct
        CUSTOM_PROP bool isIdentity
        { return self.IsIdentity(); }
        { self.SetIdentity(value); }
    END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void Constructor()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS NavMeshPath
    CUSTOM NavMeshPath ()
    {
        MonoNavMeshPath managedPath;
        managedPath.native = new NavMeshPath ();
        MarshallNativeStructIntoManaged (managedPath,self);
    }
    END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void RawAttribute()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CSRAW [AttributeUsage(AttributeTargets.Method, AllowMultiple=true)]
CLASS RPC : Attribute
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void RawConditionalAttribute()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CONDITIONAL ENABLE_NETWORK
CSRAW [AttributeUsage(AttributeTargets.Method, AllowMultiple=true)]
CLASS RPC : Attribute
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void SyncJobsAutoProp()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS TestClass
SYNC_JOBS AUTO_PROP float startDelay GetStartDelay SetStartDelay
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void SyncJobsCustomProp()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS TestClass
CUSTOM_PROP bool myCustomProp { return true; }
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void SyncJobsCustom()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS TestClass
    CUSTOM public void MyMethod ()
    {
        a++;
    }
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test(Description = "This illustrates a bug in the old parser, where a property will have the wrong output with the syncjobs tag and a struct before it")]
        public void CSPREPROCESS_BUG_SyncJobsAutoPropWithStructBefore()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS TestClass
STRUCT TestStruct
END
SYNC_JOBS AUTO_PROP float startDelay GetStartDelay SetStartDelay
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void CustomOverride()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS TestClass
CUSTOM public override string ToString() {
        return CreateScriptingString(GetSafeString(BaseObject, UnityObjectToString (self)));
    }
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void CustomOverrideStatic()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS TestClass
CUSTOM public static override string ToString() {
        return CreateScriptingString(GetSafeString(BaseObject, UnityObjectToString (self)));
    }
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void AbstractClass()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEditor
{
NONSEALED_CLASS abstract PropertyAttribute : Attribute
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void AbstractSealedClass()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS abstract PropertyAttribute : Attribute
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void AbstractPublicClass()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
NONSEALED_CLASS public abstract PropertyAttribute : Attribute
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void AbstractPublicSealedClass()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS public abstract PropertyAttribute : Attribute
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void ClassWithConditionalAndConstructor()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS AnimationCurve
    CONDITIONAL UNITY_FLASH || PLATFORM_METRO
    /// *undocumented*
    CSRAW public AnimationCurve(IntPtr nativeptr) { m_Ptr = nativeptr; }

    /// Creates an empty animation curve
    CSRAW public AnimationCurve ()  { Init(null); }
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void ClassWithConstructorWithStructArg()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS TerrainRenderer
        CUSTOM TerrainRenderer(int instanceID, TerrainData terrainData, Vector3 position, int lightmapIndex)
        {
            TerrainRenderer* t = new TerrainRenderer(instanceID, terrainData, position, lightmapIndex);
            MarshallNativeStructIntoManaged(t, self);
        }
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void CustomMultipleDimensionalArrayParameter()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS TerrainRenderer
    CUSTOM private void Internal_SetHeights (float[,] heights)
    {
        GETHEIGHT->SetHeights(xBase, yBase, width, height, &GetMonoArrayElement<float>(heights, 0), false);
        GETTREEDATABASE->RecalculateTreePositions();
    }
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void CustomMultipleDimensionalArrayParameter2()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS TerrainRenderer
    CUSTOM private void Internal_SetHeights (int xBase, int yBase, int width, int height, float[,] heights)
    {
        GETHEIGHT->SetHeights(xBase, yBase, width, height, &GetMonoArrayElement<float>(heights, 0), false);
        GETTREEDATABASE->RecalculateTreePositions();
    }
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void CustomMultipleDimensionalArrayParameter2NoSpaces()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS TerrainRenderer
    CUSTOM private void Internal_SetHeights (int xBase,int yBase,int width,int height,float[,] heights)
    {
        GETHEIGHT->SetHeights(xBase, yBase, width, height, &GetMonoArrayElement<float>(heights, 0), false);
        GETTREEDATABASE->RecalculateTreePositions();
    }
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void RawMultipleDimensionalArrayParameter()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS TerrainRenderer
        CSRAW public void SetHeights (int xBase, int yBase, float[,] heights) {
            a++
        }
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void RawMultipleDimensionalArray2Parameter()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS TerrainRenderer
        CSRAW public void SetHeights (int xBase, int yBase, float[,,] heights) {
            a++
        }
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void RawCustomWithDefaultParamAndConditional()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS TerrainRenderer
    CSRAW
#if !PLATFORM_WEBGL
    public static string EscapeURL(string s, Encoding e=System.Text.Encoding.UTF8) {
        a++;
    }
#endif
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void ReallyComplexBlockShouldHaveEverythingButComments()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS TerrainRenderer
    CSRAW
    internal static void DoTextField (Rect position, int id, GUIContent content, bool multiline, int maxLength, GUIStyle style
#if PLATFORM_IPHONE || PLATFORM_ANDROID
        , string secureText = null
#endif
    ) {

        //Pre-cull input string to maxLength.
        if (maxLength >= 0 && content.text.Length > maxLength)
            content.text = content.text.Substring (0, maxLength);

        GUIUtility.CheckOnGUI ();
        TextEditor editor = (TextEditor)GUIUtility.GetStateObject (typeof (TextEditor), id);
        editor.content.text = content.text;
        editor.SaveBackup ();
        editor.position = position;
        editor.style = style;
        editor.multiline = multiline;
        editor.controlID = id;
        editor.ClampPos ();
        Event evt = Event.current;

#if PLATFORM_IPHONE || PLATFORM_ANDROID
        switch (evt.type) {
        case EventType.MouseDown:
            if (position.Contains (evt.mousePosition)) {
                GUIUtility.hotControl = id;

                // Disable keyboard for previously active text field, if any
                if (hotTextField != -1 && hotTextField != id) {
                    TextEditor currentEditor = (TextEditor)GUIUtility.GetStateObject (typeof (TextEditor), hotTextField);
                    currentEditor.keyboardOnScreen = null;
                }

                hotTextField = id;

                // in player setting keyboard control calls OnFocus every time, don't want that. In editor it does not do that for some reason
                if (GUIUtility.keyboardControl != id)
                    GUIUtility.keyboardControl = id;

                editor.keyboardOnScreen = TouchScreenKeyboard.Open(
                    (secureText != null) ? secureText : content.text,
                    TouchScreenKeyboardType.Default,
                    true, // autocorrection
                    multiline,
                    (secureText != null));

                evt.Use ();
            }
            break;
            case EventType.Repaint:
                if (editor.keyboardOnScreen != null) {
                    content.text = editor.keyboardOnScreen.text;
                    if (maxLength >= 0 && content.text.Length > maxLength)
                        content.text = content.text.Substring (0, maxLength);

                    if (editor.keyboardOnScreen.done) {
                        editor.keyboardOnScreen = null;
                        changed = true;
                    }
                }
                style.Draw (position, content, id, false);
                break;
        }
#else // #if PLATFORM_IPHONE || PLATFORM_ANDROID
        bool change = false;
        switch (evt.type) {
        case EventType.MouseDown:
            if (position.Contains (evt.mousePosition)) {
                GUIUtility.hotControl = id;
                GUIUtility.keyboardControl = id;
                editor.m_HasFocus = true;
                editor.MoveCursorToPosition (Event.current.mousePosition);
                if (Event.current.clickCount == 2 && GUI.skin.settings.doubleClickSelectsWord) {
                    editor.SelectCurrentWord ();
                    editor.DblClickSnap(TextEditor.DblClickSnapping.WORDS);
                    editor.MouseDragSelectsWholeWords (true);
                } if (Event.current.clickCount == 3 && GUI.skin.settings.tripleClickSelectsLine) {
                    editor.SelectCurrentParagraph ();
                    editor.MouseDragSelectsWholeWords (true);
                    editor.DblClickSnap(TextEditor.DblClickSnapping.PARAGRAPHS);
                }
                evt.Use ();
            }
            break;
        case EventType.MouseDrag:
            if (GUIUtility.hotControl == id)
            {
                if (evt.shift)
                    editor.MoveCursorToPosition (Event.current.mousePosition);
                else
                    editor.SelectToPosition (Event.current.mousePosition);
                evt.Use ();
            }
            break;
        case EventType.MouseUp:
            if (GUIUtility.hotControl == id) {
                editor.MouseDragSelectsWholeWords (false);
                GUIUtility.hotControl = 0;
                evt.Use ();
            }
            break;
        case EventType.KeyDown:
            if (GUIUtility.keyboardControl != id)
                return;

            if (editor.HandleKeyEvent (evt)) {
                evt.Use ();
                change = true;
                content.text = editor.content.text;
                break;
            }

            // Ignore tab & shift-tab in textfields
            if (evt.keyCode == KeyCode.Tab || evt.character == '\t')
                return;

            char c = evt.character;

            if (c == '\n' && !multiline && !evt.alt)
                return;


            // Simplest test: only allow the character if the display font supports it.
            Font font = style.font;
            if (!font)
                font = GUI.skin.font;

            if (font.HasCharacter (c) || c == '\n') {
                editor.Insert (c);
                change = true;
                break;
            }

            // On windows, keypresses also send events with keycode but no character. Eat them up here.
            if (c == 0) {

                // if we have a composition string, make sure we clear the previous selection.
                if (Input.compositionString.Length > 0)
                {
                    editor.ReplaceSelection ("""");
                    change = true;
                }

                evt.Use ();
            }
//              else {
// REALLY USEFUL:
//              Debug.Log (""""unhandled """" +evt);
//              evt.Use ();
//          }
            break;
        case EventType.Repaint:
            // If we have keyboard focus, draw the cursor
            // TODO:    check if this OpenGL view has keyboard focus
            if (GUIUtility.keyboardControl != id) {
                style.Draw (position, content, id, false);
            } else {
                editor.DrawCursor (content.text);
            }
            break;
        }

        if (GUIUtility.keyboardControl == id)
            GUIUtility.textFieldInput = true;

        if (change) {
            changed = true;
            content.text = editor.content.text;
            if (maxLength >= 0 && content.text.Length > maxLength)
                content.text = content.text.Substring (0, maxLength);
            evt.Use ();
        }
    #endif  // #if PLATFORM_IPHONE || PLATFORM_ANDROID
    }
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void AttributeSecuritySafeCritical()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS TerrainRenderer
    CSRAW
    [System.Security.SecuritySafeCritical] // due to Marshal.SizeOf
    public void SetData (System.Array data) {
        InternalSetData (data, Marshal.SizeOf(data.GetType().GetElementType()));
    }
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void CustomBlockFollowedByConditionalCsRaw()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS TerrainRenderer
    CUSTOM void SetData(float[] data, int offsetSamples)
    {
        self->SetData(&GetScriptingArrayElement<float>(data, 0), GetScriptingArraySize (data) / self->GetChannelCount(), offsetSamples);
    }

    CONDITIONAL ENABLE_AUDIO_FMOD
    /// *listonly*
    CSRAW
    public static AudioClip Create(string name, int lengthSamples, int channels, int frequency, bool _3D, bool stream)
    {
        AudioClip clip = Create (name, lengthSamples, channels, frequency, _3D, stream, null, null);
        return clip;
    }

    public static AudioClip Create()
    {
        return null;
    }
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void Constructors()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS AnimationCurve
    CSRAW public AnimationCurve (params Keyframe[] keys) { Init(keys); }

    CONDITIONAL UNITY_FLASH || PLATFORM_METRO
    /// *undocumented*
    CSRAW public AnimationCurve(IntPtr nativeptr) { m_Ptr = nativeptr; }

    /// Creates an empty animation curve
    CSRAW public AnimationCurve ()  { Init(null); }

    THREAD_SAFE
    CUSTOM private void Init (Keyframe[] keys)
    {
        self.SetPtr(new AnimationCurve());
        if (keys != SCRIPTING_NULL) AnimationCurve_CUSTOM_SetKeys(self, keys);
    }
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void CsRawMethod()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS AnimationCurve
    ///
    CSRAW public bool isMouse {
        get { EventType t = type; return t == EventType.MouseMove || t == EventType.MouseDown  || t == EventType.MouseUp || t == EventType.MouseDrag; }
    }

    /// Create a keyboard event.
    /// This is useful when you need to check if a certain key has been pressed - possibly with modifiers. The syntax for the key string is a key name
    /// (same as in the Input Manager), optionally prefixed by any number of modifiers: \\
    /// & = Alternate, ^ = Control, % = Command/Windows key, # = Shift \\
    CONVERTEXAMPLE
    BEGIN EX
    function OnGUI () {
    }
    END EX
    CSRAW public static Event KeyboardEvent (string key) {
        Event evt = new Event ();
        evt.type = EventType.KeyDown;
        // Can't use string.IsNullOrEmpty because it's not supported in NET 1.1
        if (key == null || key == String.Empty)
            return evt;
#if !PLATFORM_WEBGL
        int startIdx = 0;
        bool found = false;
        do {
            switch (key[startIdx]) {
            default:
                found = false;
                break;
            }
        } while (found);
        string subStr = key.Substring (startIdx, key.Length - startIdx).ToLower();
#endif
        return evt;
    }
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void ConditionalAndCsRawProperty()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS AnimationCurve
    CONDITIONAL !UNITY_FLASH && !PLATFORM_WEBGL
    OBSOLETE warning randomValue property is deprecated. Use randomSeed instead to control random behavior of particles.
    CSRAW public float randomValue { get { return BitConverter.ToSingle(BitConverter.GetBytes(m_RandomSeed), 0); } set { m_RandomSeed = BitConverter.ToUInt32(BitConverter.GetBytes(value), 0); } }
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void StructWithConditional()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
/// The limits defined by the [[CharacterJoint]]
CONDITIONAL ENABLE_PHYSICS
STRUCT SoftJointLimit
    CSRAW float m_Limit;
    CSRAW float m_Bounciness;
    CSRAW float m_Spring;
    CSRAW float m_Damper;

    CSRAW public float bounciness { get { return m_Bounciness; } set { m_Bounciness = value; } }

    OBSOLETE error Use SoftJointLimit.bounciness instead
    CSRAW public float bouncyness { get { return m_Bounciness; } set { m_Bounciness = value; } }
END


/// The [[ConfigurableJoint]] attempts to attain position / velocity targets based on this flag
CSRAW [Flags]
ENUM JointDriveMode
    /// Don't apply any forces to reach the target
    None = 0
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void FlushConditionsTagShouldBeIgnored()
        {
            var txt = @"
CSRAW
using System;

CLASS Camera : Behaviour
    CONDITIONAL MAYBE
    CSRAW public float fov { get { return fieldOfView; } set { fieldOfView = value; } }
    FLUSHCONDITIONS
    CSRAW public float fov2 { get { return fieldOfView; } set { fieldOfView = value; } }
END
";

            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void DontRemoveUrlsAsComments()
        {
            var txt = @"
CSRAW
using System;

CLASS Camera : Behaviour
CSRAW private const string kAssetStoreUrl = ""https://shawarma.unity3d.com"";
END
";

            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void DontRemoveCommentsInsideStrings()
        {
            var txt = @"
CSRAW
using System;
CLASS Camera : Behaviour
    CSRAW
    static void LoadScriptInfos () {
        // Load tooltip strings and actual text items...
        string tipStrings = System.IO.File.ReadAllText (EditorApplication.applicationContentsPath + ""/Resources/UI_Strings_EN.txt"");
        foreach (string tip in tipStrings.Split ('\n')) {
            if (tip.StartsWith(""//""))
                continue;
        }
    }
END
";

            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void DontRemoveCommentsInsideStrings2()
        {
            var txt = @"
CSRAW
using System;
CLASS Camera : Behaviour
    CSRAW
    static void LoadScriptInfos () {
        // Load tooltip strings and actual text items...
        string tipStrings = System.IO.File.ReadAllText (EditorApplication.applicationContentsPath + ""/Resources/UI_Strings_EN.txt"");
        foreach (string tip in tipStrings.Split ('\n')) {
            if (tip.StartsWith(""//""))//woot
                continue;
        }
    }
END
";

            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void DontRemoveCommentsInsideStrings3()
        {
            var txt = @"
CSRAW
using System;
CLASS Camera : Behaviour
    CSRAW
    static void LoadScriptInfos () {
        // Load tooltip strings and actual text items...
        string tipStrings = System.IO.File.ReadAllText (EditorApplication.applicationContentsPath + ""/Resources/UI_Strings_EN.txt"");
        foreach (string tip in tipStrings.Split ('\n')) {
            if (tip.StartsWith(""//"" + ""//"")) // yay a comment about ""//""
                continue;
        }
    }
END
";

            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void DontRemoveCommentsInsideStrings4()
        {
            var txt = @"
CSRAW
using System;
CLASS Camera : Behaviour
    CSRAW
    static void LoadScriptInfos () {
if( (Char)o == '""' )
                return ""\""\\\""\""""; // escape the '""' character
    }
END
";

            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void DontRemoveCommentsInsideStringsOneline()
        {
            var txt = @"
CSRAW
using System;
CLASS Camera : Behaviour
    CSRAW
    static void LoadScriptInfos () {
if( (Char)o == '""' )               return ""\""\\\""\""""; // escape the '""' character
    }
END
";

            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void Return_StringAuto()
        {
            var txt = @"
CSRAW
namespace UnityEngine
{
CLASS Camera : Behaviour
    AUTO string GetStringUTFChars(IntPtr str);
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void Return_StringAutoProp()
        {
            var txt = @"
CSRAW
namespace UnityEngine
{
CLASS Camera : Behaviour
    AUTO_PROP string getStringUTFChars GetStringUTFChars
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void Return_StringCustom()
        {
            var txt = @"
CSRAW
namespace UnityEngine
{
CLASS Camera : Behaviour
    CUSTOM string GetStringUTFChars(IntPtr str)
    {
        return SCRIPTING_NULL;
    }
    CUSTOM static string GetStringUTFCharsStatic(IntPtr str)
    {
        return SCRIPTING_NULL;
    }
END
}
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void Return_StringCustomProp()
        {
            var txt = @"
CSRAW
namespace UnityEngine
{
CLASS Camera : Behaviour
    CUSTOM_PROP string GetStringUTFChars
    {
        return SCRIPTING_NULL;
    }
    CUSTOM_PROP static string GetStringUTFCharsStatic
    {
        return SCRIPTING_NULL;
    }
END
}
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void Return_IntAuto()
        {
            var txt = @"
CSRAW
namespace UnityEngine
{
CLASS Camera : Behaviour
    AUTO int GetStringUTFChars(IntPtr str);
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void Return_IntAutoProp()
        {
            var txt = @"
CSRAW
namespace UnityEngine
{
CLASS Camera : Behaviour
    AUTO_PROP int getStringUTFChars GetStringUTFChars
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void Return_IntCustom()
        {
            var txt = @"
CSRAW
namespace UnityEngine
{
CLASS Camera : Behaviour
    CUSTOM int GetStringUTFChars(IntPtr str)
    {
        return 0;
    }
    CUSTOM static int GetStringUTFCharsStatic(IntPtr str)
    {
        return 0;
    }
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void Return_IntCustomProp()
        {
            var txt = @"
CSRAW
namespace UnityEngine
{
CLASS Camera : Behaviour
    CUSTOM_PROP int GetStringUTFChars
    {
        return 0;
    }
    CUSTOM_PROP static int GetStringUTFCharsStatic
    {
        return 0;
    }
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void Return_StructAuto()
        {
            var txt = @"
CSRAW
namespace UnityEngine
{
CLASS Camera : Behaviour
    AUTO Vector3 GetVector3();
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void Return_StructAutoProp()
        {
            var txt = @"
CSRAW
namespace UnityEngine
{
CLASS Camera : Behaviour
    AUTO_PROP Vector3 getVector3 GetVector3 SetVector3
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void Return_StructCustom()
        {
            var txt = @"
CSRAW
namespace UnityEngine
{
CLASS Camera : Behaviour
    CUSTOM Vector3 GetVector3()
    {
        return 0;
    }
    CUSTOM static int GetInt()
    {
        return 0;
    }
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void Return_StructCustomProp()
        {
            var txt = @"
CSRAW
namespace UnityEngine
{
CLASS Camera : Behaviour
    CUSTOM_PROP Vector3 GetSetVector3
    {
        return 0;
    }
    CUSTOM_PROP static int GetInt
    {
        return 0;
    }
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void Return_StructCustomProp2()
        {
            var txt = @"
CSRAW
namespace UnityEngine
{
CLASS Camera : Behaviour
    CUSTOM_PROP Vector3 GetSetVector3
    {
        return 0;
    }
    {
        bla = value;
    }
    CUSTOM_PROP static int GetInt
    {
        return 0;
    }
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void Return_StructCustomObject()
        {
            var txt = @"
CSRAW
namespace UnityEditor
{
CLASS AnimationUtility
    CUSTOM static AnimationCurve GetEditorCurve (AnimationClip clip, string relativePath, Type type, string propertyName)
    {
        return null;
    }
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void Conditionals2()
        {
            var txt = @"
C++RAW
#include ""UnityPrefix.h""

CSRAW
namespace UnityEngine
{
CONDITIONAL PLATFORM_ANDROID || UNITY_EDITOR
CLASS AndroidInput

    /// Returns object representing status of a specific touch on a secondary touchpad (Does not allocate temporary variables).
    CUSTOM static Touch GetSecondaryTouch (int index)
    {
        Touch touch;
        return touch;
    }

    /// Number of secondary touches. Guaranteed not to change throughout the frame. (RO).
    CUSTOM_PROP static int touchCountSecondary
    {
        return 0;
    }

END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void ConditionalClassWithCSRAWAndEnum()
        {
            var txt = @"
C++RAW
#include ""UnityPrefix.h""

CSRAW
namespace UnityEditor.VersionControl
{
CONDITIONAL ENABLE_VERSION_CONTROL_INTEGRATION
CLASS VCAsset

CSRAW [Flags]
ENUM States
    None = 0,
END
END
CSRAW
}
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void MemoryTypeShouldMapToIntWhenInsideAlignedBufferClass()
        {
            var txt = @"
C++RAW
#include ""UnityPrefix.h""

CSRAW
namespace UnityEngine
{
CLASS AlignedBuffer
    CUSTOM private IntPtr AllocBuffer (int size, Memory mem, int alignmem) {
        return new char[size];
    }
END
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void TypeMapping_Matrix4x4AsStruct()
        {
            var txt = @"
C++RAW
#include ""UnityPrefix.h""

CSRAW
using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace UnityEngine
{
THREAD_SAFE
STRUCT Matrix4x4
    CSRAW
    ///*undocumented*
    public float m00, m10, m20, m30;

    CUSTOM_PROP bool isIdentity { return self.IsIdentity(); }
END
}
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void MultipleConditionsOnCppAndCsraw()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{

CONDITIONAL UNITY_EDITOR
C++RAW
struct QualityCurvesTime
{
    float   fixedTime;
    float   variableEndStart;
    float   variableEndEnd;
    int     q;
};

CONDITIONAL UNITY_EDITOR
CLASS internal MuscleClipEditorUtilities
    CUSTOM static MuscleClipQualityInfo GetMuscleClipQualityInfo(AnimationClip clip, float startTime, float stopTime)
    {
        MonoObject *obj = mono_object_new (mono_domain_get(), MONO_COMMON.muscleClipQualityInfo);
        MuscleClipQualityInfoToMono(clip->GetMuscleClipQualityInfo(startTime,stopTime),ExtractMonoObjectData<MonoMuscleClipQualityInfo>(obj));
        return obj;
    }
END
CSRAW }
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void SignatureParsing_UnsignedInt()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS Camera
    CUSTOM static unsigned int GetLocalIdentifierInFile(int instanceID)
    {
        return GetPersistentManager().GetLocalFileID(instanceID);
    }
END
CSRAW }
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void OneLinerCppRaw()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
/// A single keyframe that can be injected into an animation curve.
STRUCT Keyframe
    CSRAW float m_Time;
    CSRAW float m_Value;
    CSRAW float m_InTangent;
    CSRAW float m_OutTangent;
END

CSRAW #pragma warning disable 414

/// An animation curve. Lets you add keyframes and evaluate the curve at a given time.
C++RAW static void CleanupAnimationCurve(void* animationCurve){ delete ((AnimationCurve*)animationCurve); };
/// A collection of curves form an [[AnimationClip]].
CSRAW  [StructLayout (LayoutKind.Sequential)]
THREAD_SAFE
CLASS AnimationCurve
    CSRAW
    internal IntPtr m_Ptr;

    THREAD_SAFE
    CUSTOM private void Cleanup () { CleanupAnimationCurve(self.GetPtr()); }
END
CSRAW }
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void OneLinerCppRaw2()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
///*undocumented*
CLASS Unsupported

    // TODO: hack, and still does not solve all problems, remove later
    C++RAW Vector3f MakeNice(const Vector3f& eulerAngles);
    CUSTOM internal static Vector3 MakeNiceVector3(Vector3 vector)
    {
        return MakeNice(vector);
    }
END
CSRAW }
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void AutoPtrPropWithCommentHandling()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS Unsupported
    AUTO_PTR_PROP AudioClip clip GetAudioClip SetAudioClip

    // Plays the ::ref::clip with a certain delay

    // __Note:__ To obtain sample accuracy with an [[AudioClip]] with a different samplerate (than 44.1 khz) you have
    CUSTOM void Play (UInt64 delay=0)
    {
#if ENABLE_AUDIO_FMOD
        if (delay!=0)
        {
            // reference output rate is 44100 khz
            int actualOutputRate;
            GetAudioManager().GetFMODSystem()->getSoftwareFormat(&actualOutputRate,0,0,0,0,0);
            delay =  (UInt64)((double)delay * ((double)actualOutputRate/44100));
        }
#endif
        self->Play(delay);
    }
END
CSRAW }
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void CSRawWithDefineAndComment()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS Unsupported
    CSRAW
#if !PLATFORM_WEBGL
    public static string EscapeURL(string s, Encoding e=System.Text.Encoding.UTF8)
    {
        if (s == null)
            return null;

        if (s == """")
            return """";

        if (e == null)
            return null;

        return WWWTranscoder.URLEncode(s,e);
    }
#endif

    // Decodes string from an URL-friendly format.

END
CSRAW }
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void CustomPropWithGetterAndCommentWithBracketAfter()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS Unsupported
    // Indicates if the user has an IME keyboard input source selected.
    CUSTOM_PROP static bool imeIsSelected { return (GetInputManager().GetIMEIsSelected()); }

    // Information about the current mouse or touch event, available during OnInput* series of callbacks.
    //CONDITIONAL ENABLE_NEW_EVENT_SYSTEM
    //CUSTOM_PROP    static Touch currentTouch { return GetInputManager().GetCurrentTouch(); }
END
CSRAW }
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void CustomPropWithGetterAndEmptySetter()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CLASS Unsupported
CUSTOM_PROP float recorderStartTime { return self->GetRecorderStartTime();} {}
END
CSRAW }
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void ConditionalClassWithCSRAWBlocks()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEngine
{
CSRAW [StructLayout (LayoutKind.Sequential)]
NONSEALED_CLASS Collision2D
    CSRAW internal Rigidbody2D    m_Rigidbody;
    CSRAW internal Collider2D     m_Collider;

    CSRAW public Rigidbody2D rigidbody { get { return m_Rigidbody; } }
    CSRAW public Collider2D collider { get { return m_Collider; } }
    CSRAW public Transform transform { get { return rigidbody != null ? rigidbody.transform : collider.transform; } }
    CSRAW public GameObject gameObject { get { return m_Rigidbody != null ? m_Rigidbody.gameObject : m_Collider.gameObject; } }
END
CSRAW }
";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void ConditionalRightBeforeEndOfClassWithCSRAWBlock()
        {
            var txt = @"
CSRAW
using System;

namespace UnityEngine
{

/// Simple class that containing the path to the scene asset
CLASS UnityScene
    CONDITIONAL UNITY_EDITOR
    CUSTOM_PROP bool IsDirty { return self->IsDirty(); }
END


CLASS SceneManager
    C++RAW
    ScriptingObjectPtr CreateMonoUnityScene(const UnityScene& unityScene)
    {
        ScriptingObjectPtr unitySceneMono = scripting_object_new (MONO_COMMON.unityScene);
        ScriptingObjectWithIntPtrField<UnityScene>(unitySceneMono).SetPtr(&const_cast<UnityScene&>(unityScene));
        return unitySceneMono;
    }

END
CSRAW
}  //UnityEngine namespace";
            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        [Test]
        public void ClassStatic()
        {
            var txt = @"
CSRAW
using System;
namespace UnityEditor {
{
STATIC_CLASS Camera
END
";

            AssertProducesEqualOutputCS(txt);
            AssertProducesEqualOutputCPP(txt);
        }

        void AssertProducesEqualOutputCS(string content)
        {
            StackTrace stack = new StackTrace(0, false);
            string name = stack.GetFrame(1).GetMethod().Name;
            AssertProducesEqualOutput(name, content, ".cs");
        }

        void AssertProducesEqualOutputCPP(string content)
        {
            StackTrace stack = new StackTrace(0, false);
            string name = stack.GetFrame(1).GetMethod().Name;
            AssertProducesEqualOutput(name, content, ".cpp");
        }

        void AssertProducesEqualOutput(string name, string content, string extension)
        {
            string template = Path.Combine(templatePath, name + extension);
            string result;
            if (extension == ".cpp")
                result = runner.TransformToCpp(name, content, "Editor");
            else
                result = runner.TransformToCSharp(name, content, "Editor");

            // Always write a result for now, so there's something to compare
            if (!Directory.Exists(ResultPath))
                Directory.CreateDirectory(ResultPath);
            File.WriteAllText(Path.Combine(ResultPath, name + extension), result);

            if (GenerateTemplates)
            {
                if (!Directory.Exists(templatePath))
                    Directory.CreateDirectory(templatePath);
                File.WriteAllText(template, result);
            }
            else
            {
                if (!File.Exists(template))
                    Assert.Inconclusive();
                string expected = File.ReadAllText(template);
                Assert.AreEqual(expected, result, "Mismatch from template file: " + template);
            }
        }

        // this is handy for running a compare tool to look at differences side-by-side
        private static void RunCompareTool(string oldcpp, string newcpp)
        {
            if (!File.Exists(@"C:\Program Files (x86)\Beyond Compare 3\BComp.exe"))
                return;

            if (!Directory.Exists(Path.Combine(Path.GetTempPath(), "parserErrors")))
                Directory.CreateDirectory(Path.Combine(Path.GetTempPath(), "parserErrors"));
            string fil = Path.GetRandomFileName();
            string oldOutputFile = Path.Combine(Path.GetTempPath(), "parserErrors", "old_" + fil);
            File.WriteAllText(oldOutputFile, oldcpp);

            string newOutputFile = Path.Combine(Path.GetTempPath(), "parserErrors", "new_" + fil);
            File.WriteAllText(newOutputFile, newcpp);


            var p = new Process()
            {
                StartInfo = new ProcessStartInfo()
                {
                    FileName = @"""C:\Program Files (x86)\Beyond Compare 3\BComp.exe""",
                    Arguments = oldOutputFile + " " + newOutputFile
                }
            };

            p.Start();
        }
    }
}
