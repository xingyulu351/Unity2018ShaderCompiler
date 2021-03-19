using System;
using System.IO;
using System.Runtime.InteropServices;
using NiceIO;
using Unity.BuildSystem.NativeProgramSupport;

namespace Bee.Toolchain.UWP
{
    public static class PEHeaderParser
    {
// "is never assigned to"
#pragma warning disable 0649
// "is never used"
#pragma warning disable 0169
        private struct IMAGE_DOS_HEADER
        {
            public UInt16 e_magic;       // Magic number
            public UInt16 e_cblp;        // Bytes on last page of file
            public UInt16 e_cp;         // Pages in file
            public UInt16 e_crlc;        // Relocations
            public UInt16 e_cparhdr;      // Size of header in paragraphs
            public UInt16 e_minalloc;      // Minimum extra paragraphs needed
            public UInt16 e_maxalloc;      // Maximum extra paragraphs needed
            public UInt16 e_ss;         // Initial (relative) SS value
            public UInt16 e_sp;         // Initial SP value
            public UInt16 e_csum;        // Checksum
            public UInt16 e_ip;         // Initial IP value
            public UInt16 e_cs;         // Initial (relative) CS value
            public UInt16 e_lfarlc;       // File address of relocation table
            public UInt16 e_ovno;        // Overlay number
            public UInt16 e_res_0;       // Reserved words
            public UInt16 e_res_1;       // Reserved words
            public UInt16 e_res_2;       // Reserved words
            public UInt16 e_res_3;       // Reserved words
            public UInt16 e_oemid;       // OEM identifier (for e_oeminfo)
            public UInt16 e_oeminfo;      // OEM information; e_oemid specific
            public UInt16 e_res2_0;       // Reserved words
            public UInt16 e_res2_1;       // Reserved words
            public UInt16 e_res2_2;       // Reserved words
            public UInt16 e_res2_3;       // Reserved words
            public UInt16 e_res2_4;       // Reserved words
            public UInt16 e_res2_5;       // Reserved words
            public UInt16 e_res2_6;       // Reserved words
            public UInt16 e_res2_7;       // Reserved words
            public UInt16 e_res2_8;       // Reserved words
            public UInt16 e_res2_9;       // Reserved words
            public UInt32 e_lfanew;       // File address of new exe header
        }

        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        private struct IMAGE_FILE_HEADER
        {
            public UInt16 Machine;
            public UInt16 NumberOfSections;
            public UInt32 TimeDateStamp;
            public UInt32 PointerToSymbolTable;
            public UInt32 NumberOfSymbols;
            public UInt16 SizeOfOptionalHeader;
            public UInt16 Characteristics;
        }
#pragma warning restore 0169
#pragma warning restore 0649

        private static T FromBinaryReader<T>(BinaryReader reader) where T : struct
        {
            byte[] bytes = reader.ReadBytes(Marshal.SizeOf(typeof(T)));

            GCHandle handle = GCHandle.Alloc(bytes, GCHandleType.Pinned);
            var theStructure = (T)Marshal.PtrToStructure(handle.AddrOfPinnedObject(), typeof(T));
            handle.Free();

            return theStructure;
        }

        public static Architecture DetectArchitectureFromExecutable(NPath appExecutable)
        {
            using (var fileStream = new FileStream(appExecutable.ToString(), FileMode.Open))
            {
                using (var reader = new BinaryReader(fileStream))
                {
                    var dosHeader = FromBinaryReader<IMAGE_DOS_HEADER>(reader);
                    reader.BaseStream.Position = fileStream.Seek(dosHeader.e_lfanew, SeekOrigin.Begin) + 4;
                    var fileHeader = FromBinaryReader<IMAGE_FILE_HEADER>(reader);

                    switch (fileHeader.Machine)
                    {
                        case 0x14c:  // IMAGE_FILE_MACHINE_I386
                            return Architecture.x86;

                        case 0x8664: // IMAGE_FILE_MACHINE_AMD64
                            return Architecture.x64;

                        case 0x1c0: // IMAGE_FILE_MACHINE_ARM
                            return Architecture.Armv7;

                        case 0xaa64: // IMAGE_FILE_MACHINE_ARM64
                            return Architecture.Arm64;

                        default:
                            return null;
                    }
                }
            }
        }
    }
}
