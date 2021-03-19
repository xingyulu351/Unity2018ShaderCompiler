using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Bee.Core;
using NiceIO;
using Unity.BuildTools;

namespace Unity.BuildSystem.NativeProgramSupport
{
    static class Lumping
    {
        public static IEnumerable<CLikeCompilationUnitInstruction> SetupLumps(NPath artifactPath, List<CLikeCompilationUnitInstruction> inputInstructions, int idealFilesPerLump = 8)
        {
            foreach (var cu in inputInstructions.Where(c => !CanLump(c)))
                yield return cu;

            var lumpable = inputInstructions.Where(CanLump);

            foreach (var groupedByParentDir in lumpable.GroupBy(cu => cu.InputFile.Parent))
            {
                int parentDirBucketCounter = 0;
                //First, let's group all files by their defines, includedirectories, parent folder, language and codegen settings.
                //we can only lump files that have identical settings for all of these.
                foreach (var group in groupedByParentDir.GroupBy(cu => cu.HashSettingsRelevantForCompilation()))
                {
                    //great now we have a bunch of files that we can lump. We want to apply some moderation in how
                    //big we'll allow lumps to get. so if we go over a certain limit, we make multiple lumps.
                    //we use a non conventional way to distribute all the individual files over the multiple lumpfiles.
                    //we hash the filename of the individual file, and that determines in which "bucket" lumpfile it goes.
                    //the benefit of distributing like this is that when someone adds or removes a file, the lumps stay
                    //"stable".  so you're less likely to have an add/remove of a file cause completely unrelated compile
                    //errors because all lumpfiles "now move one over", and when files move to a different lump they might
                    //now conflict with another file in the lump.
                    //
                    //When you add so many files that we feel we need to go say from 3 to 4 buckets, then you will
                    //experience a rather large shuffle, but even because the hash is stable, and we use modulo to distribute
                    //over buckets, files that were together before are still going to be mostly together with a new bucket added.
                    //
                    //credit for this trick goes to richard fine

                    var bucketCount = (int)Math.Ceiling(@group.Count() / (float)idealFilesPerLump);

                    var fileToBucket = @group.ToDictionary(f => f, f => BucketFor(f, bucketCount));
                    var copySettingsFrom = @group.First();

                    for (int bucket = 0; bucket != bucketCount; bucket++)
                    {
                        var compilationUnitsInBucket = @group.Where(f => fileToBucket[f] == bucket).ToArray();
                        if (compilationUnitsInBucket.Length == 0)
                            continue;
                        if (compilationUnitsInBucket.Length == 1)
                        {
                            yield return compilationUnitsInBucket.Single();
                            continue;
                        }

                        // For input file like Foo/Bar/*.cpp, the lumped location will be Foo_Bar_0.cpp under artifacts.
                        //
                        // Also: some tools (StaticBundledLibraryFormat) don't support + characters
                        // in object file paths; sanitize lump name for that
                        var lumpName = copySettingsFrom.InputFile.Parent;
                        if (lumpName.IsRoot || lumpName.IsCurrentDir)
                            lumpName = new NPath((bucket + parentDirBucketCounter).ToString());
                        else
                            lumpName = lumpName + "/" + (bucket + parentDirBucketCounter);
                        lumpName += "." + copySettingsFrom.InputFile.Extension;
                        lumpName = new NPath(lumpName.ToString().Replace("+", "_").Replace("/", "_"));

                        var lumpCpp = artifactPath.Combine(lumpName);

                        Backend.Current.AddWriteTextAction(lumpCpp, LumpContentFor(compilationUnitsInBucket), "MakeLump");


                        (Backend.Current as IJamBackend)?.NotifyOfLump(lumpCpp, compilationUnitsInBucket.Select(c => c.InputFile).ToArray());

                        yield return new CLikeCompilationUnitInstruction(copySettingsFrom.CompilerSettings, lumpCpp);
                    }
                    parentDirBucketCounter += bucketCount;
                }
            }
        }

        static int BucketFor(CLikeCompilationUnitInstruction cui, int bucketCount)
        {
            var knutHash = KnuthHash.Create();
            knutHash.Add(cui.InputFile.FileName);
            return (int)(knutHash.Value % (ulong)bucketCount);
        }

        static bool CanLump(CLikeCompilationUnitInstruction arg)
        {
            return arg.InputFile.HasExtension("c", "cpp");
        }

        static string LumpContentFor(IEnumerable<CLikeCompilationUnitInstruction> chunk)
        {
            var sb = new StringBuilder();
            sb.AppendLine($"//Generated lump file. generated by {typeof(Lumping).FullName}");
            foreach (var u in chunk.OrderBy(u => u.InputFile.FileName))
                sb.AppendLine($"#include {u.InputFile.InQuotes()}");
            return sb.ToString();
        }
    }
}
