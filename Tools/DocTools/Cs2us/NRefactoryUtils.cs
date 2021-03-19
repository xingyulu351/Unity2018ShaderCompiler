using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Threading.Tasks;
using ICSharpCode.NRefactory.CSharp;
using ICSharpCode.NRefactory.CSharp.Resolver;
using ICSharpCode.NRefactory.Semantics;
using ICSharpCode.NRefactory.TypeSystem;

namespace Cs2us
{
    internal class NRefactoryUtils
    {
        static Lazy<IList<IUnresolvedAssembly>> builtInLibs = new Lazy<IList<IUnresolvedAssembly>>(
            delegate
            {
                Assembly[] assemblies = new Assembly[4];
                assemblies[0] = typeof(object).Assembly;     // mscorlib
                assemblies[1] = typeof(Uri).Assembly;     // System.dll
                assemblies[2] = typeof(Enumerable).Assembly;     // System.Core.dll
                assemblies[3] = typeof(IProjectContent).Assembly;

                var projectContents = new IUnresolvedAssembly[assemblies.Length];
                Stopwatch total = Stopwatch.StartNew();
                Parallel.For(
                    0, assemblies.Length,
                    delegate(int i)
                    {
                        Stopwatch w = Stopwatch.StartNew();

                        AssemblyLoader loader = AssemblyLoader.Create();
                        projectContents[i] = loader.LoadAssemblyFile(assemblies[i].Location);
                        Debug.WriteLine(Path.GetFileName(assemblies[i].Location) + ": " + w.Elapsed);
                    });
                Debug.WriteLine("Total: " + total.Elapsed);
                return projectContents;
            });

        internal ResolveResult Resolve(SyntaxTree syntaxTree, AstNode node)
        {
            IProjectContent project = new CSharpProjectContent();
            var unresolvedFile = syntaxTree.ToTypeSystem();
            project = project.AddOrUpdateFiles(unresolvedFile);
            project = project.AddAssemblyReferences(builtInLibs.Value);

            ICompilation compilation = project.CreateCompilation();

            CSharpAstResolver resolver = new CSharpAstResolver(compilation, syntaxTree, unresolvedFile);
            return resolver.Resolve(node);
        }
    }
}
