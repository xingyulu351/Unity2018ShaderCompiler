//

using System;
using System.Linq;
using JamSharp.Runtime;
using NiceIO;
using Unity.BuildSystem;
using Unity.BuildTools;

class BeeRepoSync
{
    private static NPath TargetRepo;

    static void Main()
    {
        TargetRepo = NPath.HomeDirectory.Combine("bee");
        foreach (var d in TargetRepo.Contents().Where(d => !d.FileName.StartsWith(".")))
        {
            if (d.FileName == "bee_bootstrap.exe")
                continue;
            d.Delete();
        }

        Paths.ProjectRoot.Combine(Cecil.Current40Ref).Copy(TargetRepo.Combine("External/Mono.Cecil").EnsureDirectoryExists());
        Paths.ProjectRoot.Combine(NUnitPaths.Framework).Copy(TargetRepo.Combine("External/NUnit").EnsureDirectoryExists());
        Paths.ProjectRoot.Combine("External/Moq/moq.dll").Copy(TargetRepo.Combine("External/Moq").EnsureDirectoryExists());
        Paths.ProjectRoot.Combine("External/Moq/Castle.Core.dll").Copy(TargetRepo.Combine("External/Moq").EnsureDirectoryExists());

        CopyDirectory("External/TinyProfiler", "External");
        CopyDirectory("External/NiceIO", "External");
        CopyDirectory("External/Mono.Options", "External");

        Paths.ProjectRoot.Combine("External/Json.NET/Bin/Net40/Newtonsoft.Json.dll").Copy(TargetRepo.Combine("External/Newtonsoft").EnsureDirectoryExists());

        foreach (var d in Paths.ProjectRoot.Combine("External/tundra/builds").Directories().Where(d => d.FileName != "src"))
            d.Copy(TargetRepo.Combine("External/tundra").EnsureDirectoryExists(), BakFilter);

        foreach (var e in Paths.ProjectRoot.Combine("Tools/Bee").Contents())
            e.Copy(TargetRepo, BakFilter);
        Console.WriteLine("Success");
    }

    private static void CopyDirectory(string sourceDir, string targetParentDir)
    {
        Paths.ProjectRoot.Combine(sourceDir).Copy(TargetRepo.Combine(targetParentDir).EnsureDirectoryExists(), BakFilter);
    }

    private static readonly Func<NPath, bool> BakFilter = path => !path.HasExtension(".bak") && !path.HasExtension("orig");
}
