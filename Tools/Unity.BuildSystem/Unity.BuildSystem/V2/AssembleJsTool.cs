using System.Collections.Generic;
using JamSharp.Runtime;
using NiceIO;
using Unity.BuildSystem.Common;

namespace Unity.BuildSystem.V2
{
    class AssembleJsTool : BuildSystemTool
    {
        private static readonly string[] s_InputArguments;
        private static readonly List<NPath> s_InputFiles = new List<NPath>();

        static AssembleJsTool()
        {
            var args = new List<string>();
            NPath assembleJs = "PlatformDependent/WebGL/UnityLoader/assemble.js";
            args.Add(assembleJs.InQuotes());
            s_InputFiles.Add(assembleJs);
            foreach (var inputfile in new NPath("PlatformDependent/WebGL/UnityLoader/Source").Files("*.js"))
            {
                args.Add("--unityloader-source");
                args.Add(inputfile.InQuotes());
                s_InputFiles.Add(inputfile);
            }
            var brotlifiles = new NPath[]
            {
                "decompress.js",
                "dec/bit_reader.js",
                "dec/context.js",
                "dec/decode.js",
                "dec/dictionary.js",
                "dec/dictionary.bin.js",
                "dec/dictionary-browser.js",
                "dec/huffman.js",
                "dec/prefix.js",
                "dec/streams.js",
                "dec/transform.js",
                "node_modules/base64-js/index.js"
            };
            args.AddRange(ArgsForModule("brotli", "External/Compression/Brotli/brotli.js-1.3.1", brotlifiles));

            var gzipfiles = new NPath[]
            {
                "inflate.js",
                "utils/common.js",
                "utils/strings.js",
                "zlib/inflate.js",
                "zlib/constants.js",
                "zlib/messages.js",
                "zlib/zstream.js",
                "zlib/gzheader.js",
                "zlib/adler32.js",
                "zlib/crc32.js",
                "zlib/inffast.js",
                "zlib/inftrees.js"
            };
            args.AddRange(ArgsForModule("gzip", "External/Compression/gzip/pako-1.0.4/lib", gzipfiles));
            args.Add("--uglify");
            args.Add(new NPath("External/uglify-js").MakeAbsolute().InQuotes());
            args.Add("--template");
            args.Add("PlatformDependent/WebGL/WebGLTemplates/Default/TemplateData");

            s_InputArguments = args.ToArray();
        }

        private static IEnumerable<string> ArgsForModule(string moduleName, NPath moduleRoot, IEnumerable<NPath> files)
        {
            yield return $"--{moduleName}-root";
            yield return moduleRoot.InQuotes();
            foreach (var moduleFile in files)
            {
                yield return $"--{moduleName}-module";
                yield return moduleFile.InQuotes();
                s_InputFiles.Add(moduleRoot.Combine(moduleFile));
            }
        }

        public NPath[] SetupInvocation(NPath outputFile, NPath outputMinifiedFile)
        {
            var args = new List<string>(s_InputArguments)
            {
                "--output",
                outputFile.InQuotes(),
                "--output-minified",
                outputMinifiedFile.InQuotes()
            };

            var jamTargets = new JamTarget[] {outputFile, outputMinifiedFile};
            base.SetupInvocation(jamTargets, s_InputFiles.ToArray(), args, NodeJs.NodeBinary.ToString(SlashMode.Native));
            return new[] {outputFile, outputMinifiedFile};
        }
    }
}
