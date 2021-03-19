using System;
using System.Linq;
using JamSharp.Runtime;
using JamSharp.Runtime.Parser;
using NUnit.Framework;
using Unity.BuildSystem.JamStateToTundraConverter;

namespace Unity.BuildSystem.JamStateToBeeConverter.Tests
{
    public class ActionBodyReplacerTests
    {
        ActionBodyReplacer _actionBodyReplacer;

        [SetUp]
        public void Setup()
        {
            _actionBodyReplacer = new ActionBodyReplacer();
        }

        [Test]
        public void Simple()
        {
            var result = _actionBodyReplacer.FindSpansToReplace("$(a)").Single();
            Assert.AreEqual(0, result.startIndex);
            Assert.AreEqual(4, result.endIndex);
            Assert.AreEqual("a", result.expression.To<VariableDereferenceExpression>().VariableExpression.To<LiteralExpression>().Value);
        }

        [Test]
        public void InContext()
        {
            var result = _actionBodyReplacer.FindSpansToReplace("hello $(c) there").Single();
            Assert.AreEqual(6, result.startIndex);
            Assert.AreEqual(10, result.endIndex);
            Assert.AreEqual("c", result.expression.To<VariableDereferenceExpression>().VariableExpression.To<LiteralExpression>().Value);
        }

        [Test]
        public void Two()
        {
            var result = _actionBodyReplacer.FindSpansToReplace("a $(b) $(c) d").ToArray();
            Assert.AreEqual(2, result.Length);

            var result0 = result[0];
            Assert.AreEqual(2, result0.startIndex);
            Assert.AreEqual(6, result0.endIndex);
            Assert.AreEqual("b", result0.expression.To<VariableDereferenceExpression>().VariableExpression.To<LiteralExpression>().Value);

            var result1 = result[1];
            Assert.AreEqual(7, result1.startIndex);
            Assert.AreEqual(11, result1.endIndex);
            Assert.AreEqual("c", result1.expression.To<VariableDereferenceExpression>().VariableExpression.To<LiteralExpression>().Value);
        }

        [Test]
        public void ActualReplace()
        {
            var globals = new GlobalVariables();
            globals["a"].Assign(new JamList("johny", "depp"));
            var shellCommand = _actionBodyReplacer.Replace("hello $(a) there", globals);
            Assert.AreEqual("hello johny depp there", shellCommand.CommandWithNonResponseCommandsFlattened);
            Assert.AreEqual("a", shellCommand.Replacements.Single().OriginalName);
        }

        [Test]
        public void TwoCombinedReplacements()
        {
            var globals = new GlobalVariables();
            globals["a"].Assign(new JamList("johny", "depp"));
            Assert.AreEqual("hello johnyjohny johnydepp deppjohny deppdepp there", _actionBodyReplacer.Replace("hello $(a)$(a) there", globals).CommandWithNonResponseCommandsFlattened);
        }

        [Test]
        public void ActualReplaceAtEnd()
        {
            var globals = new GlobalVariables();
            globals["2"].Assign(new JamList("bloink"));
            globals["XBuildCmd"].Assign(new JamList("xbuild"));
            Assert.AreEqual("xbuild /target:clean \"bloink\"", _actionBodyReplacer.Replace("$(XBuildCmd) /target:clean \"$(2)\"", globals).CommandWithNonResponseCommandsFlattened);
        }

        [Test]
        public void Replace()
        {
            var globals = new GlobalVariables();
            globals["2"].Assign(new JamList("bloink"));
            globals["XBuildCmd"].Assign(new JamList("xbuild"));
            Assert.AreEqual("xbuild /target:clean \"bloink\"", _actionBodyReplacer.Replace("$(XBuildCmd) /target:clean \"$(2)\"", globals).CommandWithNonResponseCommandsFlattened);
        }

        [Test]
        public void Indexer()
        {
            var globals = new GlobalVariables();
            globals["myvar"].Assign("one", "two", "three");
            var shellCommand = _actionBodyReplacer.Replace("a $(myvar[1]) b", globals);
            Assert.AreEqual("a one b", shellCommand.CommandWithNonResponseCommandsFlattened);
            Assert.AreEqual("myvar", shellCommand.Replacements.Single().OriginalName);
        }

        [Test]
        public void ResponseFile()
        {
            var globals = new GlobalVariables();
            globals["myvar"].Assign("one", "two", "three");

            var shellCommand = _actionBodyReplacer.Replace("a @@($(myvar[2])) b", globals);

            Assert.AreEqual("a @REPLACE_TOKEN_0_ b", shellCommand.CommandWithNonResponseCommandsFlattened);
            Assert.AreEqual("REPLACE_TOKEN_0_", shellCommand.Replacements.Single().StringToReplace);
            Assert.AreEqual("two", shellCommand.Replacements.Single().ReplacementContents);
            Assert.AreEqual(null, shellCommand.Replacements.Single().OriginalName);
        }

        [Test]
        public void ResponseFile2()
        {
            var globals = new GlobalVariables();
            globals["2"].Assign("hello");
            globals["newLine"].Assign("\n");

            Assert.Throws<ArgumentException>(() => _actionBodyReplacer.Replace("-Wl,-filelist,@($(2:CJ=$(newLine)))", globals));
        }

        [Test]
        public void CombineExpression()
        {
            var globals = new GlobalVariables();
            globals["references"].Assign("one", "two");

            var shellCommand = _actionBodyReplacer.Replace("-r:$(references)", globals);

            Assert.AreEqual("-r:one -r:two", shellCommand.CommandWithNonResponseCommandsFlattened);
        }

        [Test]
        public void CombineExpressionWithQuotesAndMinus()
        {
            var globals = new GlobalVariables();
            globals["binding_assembly_resolver"].Assign("RunBindingsGeneratorPatcher");
            var shellCommand = _actionBodyReplacer.Replace("--assembly-resolver=\"$(binding_assembly_resolver)\"", globals);
            Assert.AreEqual("--assembly-resolver=\"RunBindingsGeneratorPatcher\"", shellCommand.CommandWithNonResponseCommandsFlattened);
        }

        [Test]
        public void BackslashesGetPerserved()
        {
            var globals = new GlobalVariables();
            globals["TOP"].Assign(@"C:\unity");
            var shellCommand = _actionBodyReplacer.Replace(@"$(TOP)\External\Lua\Lua.exe", globals);
            Assert.AreEqual(@"C:\unity\External\Lua\Lua.exe", shellCommand.CommandWithNonResponseCommandsFlattened);
        }

        [Test]
        public void RuntimeEvaluationOfBSWithArgument()
        {
            var globals = new GlobalVariables();
            globals["a"].Assign(new JamList("johny.cpp", "mydir/depp"));
            Assert.AreEqual("johny.hpp depp.hpp", _actionBodyReplacer.Replace("$(a:BS=.hpp)", globals).CommandWithNonResponseCommandsFlattened);
        }

        [Test]
        public void IndexerWithBS()
        {
            var globals = new GlobalVariables();
            globals["1"].Assign(new JamList("artifacts/harry/is/de/man/johny.cpp", "mydir/depp"));
            Assert.AreEqual("johny.cpp", _actionBodyReplacer.Replace("$(1[1]:BS)", globals).CommandWithNonResponseCommandsFlattened);
        }

        [Test]
        public void Various_BS()
        {
            var globals = new GlobalVariables();
            globals["1"].Assign(new JamList("my.file.is.cpp", "mydir/myfile.harry.c", "mydir", "mydir/myfile"));
            Assert.AreEqual("my.file.is.cpp myfile.harry.c mydir myfile", _actionBodyReplacer.Replace("$(1:BS)", globals).CommandWithNonResponseCommandsFlattened);
        }
    }
}
