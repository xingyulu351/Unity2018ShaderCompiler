using System;
using System.Collections.Generic;
using System.IO;
using NUnit.Framework;
using Unity.CommonTools;

namespace BindingsToCsAndCpp
{
    [TestFixture]
    internal class CSGenerationTests
    {
        [TestCase(
            @"void m2 (out Vector3 a1) { a++; }",
@"[UnityEngine.Scripting.GeneratedByOldBindingsGeneratorAttribute] // Temporarily necessary for bindings migration
[System.Runtime.CompilerServices.MethodImplAttribute((System.Runtime.CompilerServices.MethodImplOptions)0x1000)]
extern public void m2 (out Vector3 a1) ;"
         )]
        public void Method1(string content, string expected)
        {
            RunTestMethod(content, expected);
        }

        [TestCase(
            @"void m1 (Vector3 a1) { a++; }",

@"public void m1 (Vector3 a1) {
INTERNAL_CALL_m1 ( this, ref a1 );
}
[UnityEngine.Scripting.GeneratedByOldBindingsGeneratorAttribute] // Temporarily necessary for bindings migration
[System.Runtime.CompilerServices.MethodImplAttribute((System.Runtime.CompilerServices.MethodImplOptions)0x1000)]
private extern static void INTERNAL_CALL_m1 (MyClass self, ref Vector3 a1);"
         )]
        public void Method2(string content, string expected)
        {
            RunTestMethod(content, expected);
        }

        [TestCase(
            @"void m3 (ref Vector3 a1) { a++; }",

@"public void m3 (ref Vector3 a1) {
INTERNAL_CALL_m3 ( this, ref a1 );
}
[UnityEngine.Scripting.GeneratedByOldBindingsGeneratorAttribute] // Temporarily necessary for bindings migration
[System.Runtime.CompilerServices.MethodImplAttribute((System.Runtime.CompilerServices.MethodImplOptions)0x1000)]
private extern static void INTERNAL_CALL_m3 (MyClass self, ref Vector3 a1);"
         )]
        public void Method3(string content, string expected)
        {
            RunTestMethod(content, expected);
        }

        [TestCase(
            @"void m6 (ref Vector3 a1, ref Vector3 b) { a++; }",

@"public void m6 (ref Vector3 a1, ref Vector3 b) {
INTERNAL_CALL_m6 ( this, ref a1, ref b );
}
[UnityEngine.Scripting.GeneratedByOldBindingsGeneratorAttribute] // Temporarily necessary for bindings migration
[System.Runtime.CompilerServices.MethodImplAttribute((System.Runtime.CompilerServices.MethodImplOptions)0x1000)]
private extern static void INTERNAL_CALL_m6 (MyClass self, ref Vector3 a1, ref Vector3 b);"
         )]
        public void Method6(string content, string expected)
        {
            RunTestMethod(content, expected);
        }

        [TestCase(
            @"Vector3 s1 () { a++; }",

@"public Vector3 s1 () {
Vector3 result;
INTERNAL_CALL_s1 ( this, out result );
return result;
}
[UnityEngine.Scripting.GeneratedByOldBindingsGeneratorAttribute] // Temporarily necessary for bindings migration
[System.Runtime.CompilerServices.MethodImplAttribute((System.Runtime.CompilerServices.MethodImplOptions)0x1000)]
private extern static void INTERNAL_CALL_s1 (MyClass self, out Vector3 value);"
         )]
        public void SimpleMethod1(string content, string expected)
        {
            RunTestMethod(content, expected);
        }

        [TestCase(
            @"int prop1 { a++; } { a--; }",

@"public extern  int prop1
{
    [UnityEngine.Scripting.GeneratedByOldBindingsGeneratorAttribute] // Temporarily necessary for bindings migration
    [System.Runtime.CompilerServices.MethodImplAttribute((System.Runtime.CompilerServices.MethodImplOptions)0x1000)]
    get;
    [UnityEngine.Scripting.GeneratedByOldBindingsGeneratorAttribute] // Temporarily necessary for bindings migration
    [System.Runtime.CompilerServices.MethodImplAttribute((System.Runtime.CompilerServices.MethodImplOptions)0x1000)]
    set;
}
")]
        public void IntProperty(string content, string expected)
        {
            RunTestProperty(content, expected);
        }

        [TestCase(
            @"int prop1 { a++; }",

@"public extern  int prop1
{
    [UnityEngine.Scripting.GeneratedByOldBindingsGeneratorAttribute] // Temporarily necessary for bindings migration
    [System.Runtime.CompilerServices.MethodImplAttribute((System.Runtime.CompilerServices.MethodImplOptions)0x1000)]
    get;
}
")]
        public void IntNoSetterProperty(string content, string expected)
        {
            RunTestProperty(content, expected);
        }

        [TestCase(
            @"Vector3 prop1 { a++; } { }",

@"public  Vector3 prop1
{
    get { Vector3 tmp; INTERNAL_get_prop1(out tmp); return tmp;  }
    set { INTERNAL_set_prop1(ref value); }
}

[UnityEngine.Scripting.GeneratedByOldBindingsGeneratorAttribute] // Temporarily necessary for bindings migration
[System.Runtime.CompilerServices.MethodImplAttribute((System.Runtime.CompilerServices.MethodImplOptions)0x1000)]
extern private  void INTERNAL_get_prop1 (out Vector3 value) ;

[UnityEngine.Scripting.GeneratedByOldBindingsGeneratorAttribute] // Temporarily necessary for bindings migration
[System.Runtime.CompilerServices.MethodImplAttribute((System.Runtime.CompilerServices.MethodImplOptions)0x1000)]
extern private  void INTERNAL_set_prop1 (ref Vector3 value) ;
")]
        public void StructProperty(string content, string expected)
        {
            RunTestProperty(content, expected);
        }

        [TestCase(
            @"Vector3 prop1 { a++; }",
@"public  Vector3 prop1
{
    get { Vector3 tmp; INTERNAL_get_prop1(out tmp); return tmp;  }
}

[UnityEngine.Scripting.GeneratedByOldBindingsGeneratorAttribute] // Temporarily necessary for bindings migration
[System.Runtime.CompilerServices.MethodImplAttribute((System.Runtime.CompilerServices.MethodImplOptions)0x1000)]
extern private  void INTERNAL_get_prop1 (out Vector3 value) ;
")]
        public void StructPropertyNoSetter(string content, string expected)
        {
            RunTestProperty(content, expected);
        }

        [TestCase(
            @"void HasDefaultValue (bool val = true);",

@"[System.Obsolete ("""")]
[UnityEngine.Scripting.GeneratedByOldBindingsGeneratorAttribute] // Temporarily necessary for bindings migration
[System.Runtime.CompilerServices.MethodImplAttribute((System.Runtime.CompilerServices.MethodImplOptions)0x1000)]
extern public void HasDefaultValue ( [uei.DefaultValue(""true"")] bool val ) ;
[System.Obsolete ("""")]
[uei.ExcludeFromDocs]
public void HasDefaultValue () {
bool val = true;
HasDefaultValue ( val );
}"
         )]
        public void ObsoleteAttributeWithDefaultValue(string content, string expected)
        {
            RunTestMethod(content, expected, m => m.IsObsolete = true);
        }

        [TestCase(
            @"void HasDefaultValue (bool val = true, int val2 = 10);",

@"[System.Obsolete ("""")]
[UnityEngine.Scripting.GeneratedByOldBindingsGeneratorAttribute] // Temporarily necessary for bindings migration
[System.Runtime.CompilerServices.MethodImplAttribute((System.Runtime.CompilerServices.MethodImplOptions)0x1000)]
extern public void HasDefaultValue ( [uei.DefaultValue(""true"")] bool val , [uei.DefaultValue(""10"")]  int val2 ) ;
[System.Obsolete ("""")]
[uei.ExcludeFromDocs]
public void HasDefaultValue (bool val ) {
int val2 = 10;
HasDefaultValue ( val, val2 );
}
[System.Obsolete ("""")]
[uei.ExcludeFromDocs]
public void HasDefaultValue () {
int val2 = 10;
bool val = true;
HasDefaultValue ( val, val2 );
}"
         )]
        public void ObsoleteAttributeWith2DefaultValuea(string content, string expected)
        {
            RunTestMethod(content, expected, m => m.IsObsolete = true);
        }

        [Test]
        public void CSRawGenericMethodWithWhereClauseAndDefaultArgument()
        {
            const string inputCSRaw =
@"public T[] AMethod<T> (bool arg = false) where T : System.IDisposable
{
    return default(T[]);
}";

            const string expectedOutputCS =
@"using scm=System.ComponentModel;
using uei=UnityEngine.Internal;
using RequiredByNativeCodeAttribute=UnityEngine.Scripting.RequiredByNativeCodeAttribute;
using UsedByNativeCodeAttribute=UnityEngine.Scripting.UsedByNativeCodeAttribute;
[uei.ExcludeFromDocs]
public T[] AMethod<T> () where T : System.IDisposable {
    bool arg = false;
    return AMethod<T> ( arg );
}

public T[] AMethod<T>( [uei.DefaultValue(""false"")] bool arg ) where T : System.IDisposable
{
    return default(T[]);
}";
            var generator = new CSharpGenerator();
            generator.OnCsRaw(inputCSRaw.Split('\n'));
            Assert.AreEqual(expectedOutputCS, generator.GetOutput().Trim());
        }

        [Test]
        public void CSRawGenericMethodWithWhereClauseAndNoDefaultArguments()
        {
            const string inputCSRaw =
@"public T[] AMethod<T> (bool arg) where T : System.IDisposable
{
    return default(T[]);
}";

            const string expectedOutputCS =
@"using scm=System.ComponentModel;
using uei=UnityEngine.Internal;
using RequiredByNativeCodeAttribute=UnityEngine.Scripting.RequiredByNativeCodeAttribute;
using UsedByNativeCodeAttribute=UnityEngine.Scripting.UsedByNativeCodeAttribute;
public T[] AMethod<T> (bool arg) where T : System.IDisposable
{
    return default(T[]);
}";
            var generator = new CSharpGenerator();
            generator.OnCsRaw(inputCSRaw.Split('\n'));
            Assert.AreEqual(expectedOutputCS, generator.GetOutput().Trim());
        }

        readonly Dictionary<string, string> tests = new Dictionary<string, string>();
        static int count;

        [TestFixtureSetUp]
        public void Init()
        {
            var handlers = new Dictionary<string, Action<string>>();
            var start = false;
            var output = false;
            var ret = new IndentedWriter();
            var source = new IndentedWriter();

            handlers.Add("START", s => start = true);
            handlers.Add("STOP", s => {
                tests.Add(source.ToString(), ret.ToString());
                source.Clear();
                ret.Clear();
                start = output = false;
            });
            handlers.Add("RESULT", s => output = true);
            handlers.Add("", s => {
                if (start && output) ret.AppendLine(s);
                else if (!output) source.AppendLine(s);
            });

            string typeTestsPath = Paths.UnifyDirectorySeparator(Path.GetFullPath(Path.Combine(Workspace.BasePath, @"Tools\UnityBindingsParser\BindingsToCsAndCpp\Tests\typetests.bindings")));

            foreach (var line in File.ReadAllLines(typeTestsPath))
            {
                if (handlers.ContainsKey(line))
                    handlers[line](line);
                else
                    handlers[""](line);
            }
        }

        [Test]
        public void RunTypeTests()
        {
            var tags = new Dictionary<string, Action<string, string>>();
            tags.Add("CUSTOM", RunTestMethod);
            tags.Add("CUSTOM_PROP", RunTestProperty);
            tags.Add("AUTO_PROP", RunTestAutoProperty);
            tags.Add("THREAD_AND_SERIALIZATION_SAFE CUSTOM", RunTestMethodThreadSafe);

            foreach (var test in tests)
            {
                var start = test.Key.IndexOf(" ");
                var tag = test.Key.Substring(0, start).Trim();
                while (!tags.ContainsKey(tag))
                {
                    start = test.Key.IndexOf(" ", start + 1);
                    if (start < 0)
                        break;
                    tag = test.Key.Substring(0, start).Trim();
                }

                tags[tag.Trim()](test.Key.Substring(start + 1), test.Value);
            }
        }

        [Test]
        public void InvalidSignatureTest()
        {
            // This will fail because "event" is a C# keyword
            const string content =
@"CUSTOM void MyThing(string event, int meh)
{
    self->DoStuff();
}
";
            Assert.Throws(typeof(InvalidMethodSignatureException), () => new CSMethod("meh", content));
        }

        public void RunTestMethod(string content, string expected)
        {
            RunTestMethod(content, expected, null);
        }

        public void RunTestMethod(string content, string expected, Action<CSMethod> setupMethod)
        {
            const string className = "MyClass";
            var c = new CSMethod(className, content);
            if (setupMethod != null)
                setupMethod(c);

            var ret = c.Output.Clean();
            expected = expected.Clean();

            count++;
            Assert.AreEqual(expected, ret, "Test " + count + Environment.NewLine + expected + Environment.NewLine + ret);
        }

        public void RunTestMethodThreadSafe(string content, string expected)
        {
            RunTestMethod(content, expected, m => m.IsThreadSafe = true);
        }

        public void RunTestAutoProperty(string content, string expected)
        {
            const string className = "MyClass";
            var c = new CSAutoProperty(className, content);

            var ret = c.Output.Clean();
            expected = expected.Clean();

            count++;
            Assert.AreEqual(expected, ret, "Test " + count + Environment.NewLine + expected + Environment.NewLine + ret);
        }

        public void RunTestProperty(string content, string expected)
        {
            const string className = "MyClass";
            var c = new CSProperty(className, content);

            var ret = c.Output.Clean();
            expected = expected.Clean();

            count++;
            Assert.AreEqual(expected, ret, "Test " + count + Environment.NewLine + expected + Environment.NewLine + ret);
        }
    }
}
