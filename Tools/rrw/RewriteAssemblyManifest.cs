using System;
using System.IO;
using System.Linq;
using Mono.Cecil;

namespace Unity.ReferenceRewriter
{
    class RewriteAssemblyManifest : RewriteStep
    {
        protected override void Run()
        {
            if (Context.TargetModule.Architecture != TargetArchitecture.I386)
            {
                Context.TargetModule.Architecture = TargetArchitecture.I386;
                Context.RewriteTarget = true;
            }

            if ((Context.TargetModule.Attributes & ModuleAttributes.Required32Bit) != 0)
            {
                Context.TargetModule.Attributes &= ~ModuleAttributes.Required32Bit;
                Context.RewriteTarget = true;
            }

            if (Context.TargetModule.Runtime == TargetRuntime.Net_2_0)
            {
                Context.TargetModule.Runtime = TargetRuntime.Net_4_0;
                Context.RewriteTarget = true;
            }

            if (Context.TargetModule.Assembly.Name.HasPublicKey)
                RemoveStrongName();

            foreach (var reference in Context.TargetModule.AssemblyReferences)
                RewriteAssemblyReference(reference);
        }

        private void RemoveStrongName()
        {
            if (!ShouldRemoveStrongName(Context.TargetModule.Assembly.Name))
                return;

            Context.RewriteTarget = true;
            Context.TargetModule.Assembly.Name.PublicKey = new byte[0];
            Context.TargetModule.Attributes = ModuleAttributes.ILOnly;
        }

        private void RewriteAssemblyReference(AssemblyNameReference reference)
        {
            try
            {
                if (ShouldRewriteToWinmdReference(reference))
                {
                    Context.RewriteTarget = true;
                    reference.Version = new Version(255, 255, 255, 255);
                    reference.IsWindowsRuntime = true;
                }

                var assembly = Context.AssemblyResolver.Resolve(reference);

                if ((reference.IsWindowsRuntime != assembly.Name.IsWindowsRuntime) || (IsFrameworkAssembly(assembly) && ShouldRewriteFrameworkReference(reference, assembly)))
                {
                    Context.RewriteTarget = true;
                    reference.Version = assembly.Name.Version;
                    reference.PublicKeyToken = Copy(assembly.Name.PublicKeyToken);
                    reference.IsWindowsRuntime = assembly.Name.IsWindowsRuntime;
                }
            }
            catch (AssemblyResolutionException)
            {
            }
        }

        private static bool ShouldRewriteFrameworkReference(AssemblyNameReference reference, AssemblyDefinition assembly)
        {
            return reference.Version != assembly.Name.Version
                || !reference.PublicKeyToken.SequenceEqual(assembly.Name.PublicKeyToken);
        }

        private bool ShouldRemoveStrongName(AssemblyNameReference reference)
        {
            if (reference.PublicKeyToken == null || reference.PublicKeyToken.Length == 0)
                return false;

            return Context.StrongNameReferences.All(r => r != reference.Name);
        }

        private bool ShouldRewriteToWinmdReference(AssemblyNameReference reference)
        {
            return Context.WinmdReferences.Contains(reference.Name);
        }

        private static byte[] Copy(byte[] bytes)
        {
            var copy = new byte[bytes.Length];
            Buffer.BlockCopy(bytes, 0, copy, 0, bytes.Length);
            return copy;
        }

        private bool IsFrameworkAssembly(AssemblyDefinition assembly)
        {
            bool isFrameworkAssembly = false;

            foreach (var path in Context.FrameworkPaths)
            {
                if (FullPath(Path.GetDirectoryName(assembly.MainModule.FileName)) == FullPath(path))
                {
                    isFrameworkAssembly = true;
                }
            }

            return isFrameworkAssembly;
        }

        private static string FullPath(string path)
        {
            var fullPath = Path.GetFullPath(path);
            return fullPath.TrimEnd(Path.DirectorySeparatorChar, Path.AltDirectorySeparatorChar);
        }
    }
}
