using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using Mono.Cecil;
using Mono.Cecil.Cil;
using NUnit.Framework;
using Unity.ReferenceRewriter;

namespace Test.Driver
{
    [TestFixture]
    public class ReferenceRewriting
    {
        [Test]
        public void AssemblyReferences()
        {
            AssertRewrite(
                ctx => Assert.AreEqual("2.0.0.0", ctx.TargetModule.AssemblyReferences.Single(r => r.Name == "mscorlib").Version.ToString(4)),
                ctx => Assert.AreEqual("4.0.0.0", ctx.TargetModule.AssemblyReferences.Single(r => r.Name == "mscorlib").Version.ToString(4)));
        }

        [Test]
        public void ArrayListReferences()
        {
            AssertRewrite(
                ctx => {},
                ctx =>
                {
                    var os = ctx.TargetModule.GetType("Test.Target.ObjectStore");
                    var add = os.Methods.Single(m => m.Name == "AddObject");

                    var call = (MethodReference)add.Body.Instructions.Single(i => i.OpCode.OperandType == OperandType.InlineMethod).Operand;

                    Assert.AreEqual("Test.Support", call.DeclaringType.Scope.Name);
                    Assert.AreEqual(1, ctx.TargetModule.AssemblyReferences.Count(r => r.Name == "Test.Support"));
                });
        }

        public static void AssertRewrite(Action<RewriteContext> preAssertions, Action<RewriteContext> postAssertions)
        {
            var supportPath = "Test.Support.dll";
            if (!File.Exists(supportPath))
            {
                supportPath = Path.Combine(TestContext.CurrentContext.TestDirectory, @"..\..\..\Test.Support\bin\Debug",
                    "Test.Support.dll");
            }
            var targetPath = "Test.Target.dll";
            if (!File.Exists(targetPath))
            {
                targetPath = Path.Combine(TestContext.CurrentContext.TestDirectory, @"..\..\..\Test.Target\bin\Debug",
                    "Test.Target.dll");
            }

            var frameworkPaths = new string[] { Path.GetDirectoryName(typeof(object).Assembly.Location) };
            var resolver = RewriteResolver.CreateAssemblyResolver(new[] { targetPath }, frameworkPaths, string.Empty, new string[0], string.Empty);
            var supportModule = RewriteResolver.ReadModule(supportPath, resolver);
            var target = RewriteResolver.ReadModule(targetPath, resolver);

            var context = RewriteContext.For(
                target,
                resolver,
                DebugSymbolFormat.None,
                supportModule,
                string.Empty,
                frameworkPaths,
                new string[0],
                new string[0],
                new Dictionary<string, ModuleDefinition[]>(),
                new Dictionary<string, IList<string>>());

            var operation = RewriteOperation.Create(ns => ns.StartsWith("System") ? "Test.Support" + ns.Substring("System".Length) : ns);

            preAssertions(context);
            operation.Execute(context);
            postAssertions(context);
        }
    }
}
