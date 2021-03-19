#include "UnityPrefix.h"
#include "SurfaceAnalysis.h"
#include "External/ShaderCompilers/mojoshader/mojoshader.h"
#include "Utilities/ProcessIncludes.h"

#if ENABLE_UNIT_TESTS
#include "Runtime/Testing/Testing.h"

UNIT_TEST_SUITE(SurfaceAnalysis)
{
    struct SurfaceAnalysisFixture
    {
        SurfaceAnalysisFixture()
        {
            includeContext.includePaths.push_back(g_IncludesPath + "/CGIncludes");
            CreateDirectoryAtPath("redirected");
            includeContext.redirectedFolders.insert("re/dir", "redirected");
            analysisCache = CreateSurfaceAnalysisCache(includeContext);
        }

        bool RunAnalyze(const char* source)
        {
            ShaderArgs args;
            StringArray keywords;
            ShaderCompileRequirements requirements;
            const ShaderCompilerPlatform api = SurfaceData::AnalysisShaderCompilerPlatform;
            SetSurfaceShaderArgs(api, requirements, keywords, args);
            AnalyzeSurfaceShaderViaCompile(source, "frag", 0, includeContext.includePaths, api, args, out, errors, *analysisCache);
            return !errors.HasErrorsOrWarnings();
        }

        ~SurfaceAnalysisFixture()
        {
            DestroySurfaceAnalysisCache(analysisCache);
            DeleteDirectoryAtPath("redirected");
        }

        IncludeSearchContext includeContext;
        SurfaceAnalysisCache* analysisCache;
        ShaderImportErrors errors;
        ShaderDesc out;
    };
    // Cover actually failing part of case 549378: snippet used for surface shader
    // analysis was using more than 10 input+output counts, and Cg was running out
    // of automatic TEXCOORDn assignments for them. By now with HLSL compiler used
    // for the analysis this test probably does not make much sense, since the analysis
    // is not ran through automated-TEXCOORDn path.
    TEST_FIXTURE(SurfaceAnalysisFixture, SurfaceAnalysis_FragmentWithMoreThan8InputOutputs_CanBeAnalyzedSuccessfully)
    {
        const char* s =
            "#include \"HLSLSupport.cginc\"\n"
            // input struct will use 6 texcoord bindings
            // output struct will want to use 5 more: total of 11
            "cbuffer " kSurfaceAnalysisHLSLConstantBuffer " { float2 surfInputuv0; float2 surfInputuv1; float2 surfInputuv2; float2 surfInputuv3; float2 surfInputuv4; float2 surfInputuv5;\n"
            "half4 surfInputout0; half4 surfInputout1; half4 surfInputout2; half4 surfInputout3; half4 surfInputout4; };\n"
            "half4 frag () : SV_Target {\n"
            "\thalf4 res = 0;\n"
            "\tres.rg = surfInputuv0 + surfInputuv1 + surfInputuv2 + surfInputuv3 + surfInputuv4 + surfInputuv5;\n"
            "\tsurfInputout0 += 1.0; surfInputout1 += 1.0; surfInputout2 += 1.0; surfInputout3 += 1.0; surfInputout4 += 1.0;\n"
            "\tres += surfInputout0 + surfInputout1 + surfInputout2 + surfInputout3 + surfInputout4;\n"
            "\treturn res;\n"
            "}\n";
        CHECK_EQUAL(true, RunAnalyze(s));
        CHECK_EQUAL(11, out.inputs.size());
    }

    TEST_FIXTURE(SurfaceAnalysisFixture, SurfaceShader_HashErrorInSource_CompilationFails)
    {
        const char* s =
            "#error HASH_ERROR\n"
            "#include \"HLSLSupport.cginc\"\n"
            "half4 frag () : SV_Target {\n"
            "\treturn 0.0f;\n"
            "}\n";
        CHECK_EQUAL(false, RunAnalyze(s));
    }

    TEST_FIXTURE(SurfaceAnalysisFixture, SurfaceAnalysis_GetOpenedIncludeFiles_Works)
    {
        const char* include1 = "foo.cginc";
        const char* include2 = "bar.cginc";
        const char* redirInclude = "redirected/doh.cginc";

        const char* s =
            "#include \"foo.cginc\"\n"
            "half4 frag () : SV_Target {\n"
            "\treturn 0.0f;\n"
            "}\n";

        CHECK(WriteTextToFile(include1, "#include \"bar.cginc\"\n"));
        CHECK(WriteTextToFile(include2, "#include \"re/dir/doh.cginc\"\n"));
        CHECK(WriteTextToFile(redirInclude, ""));
        includeContext.includePaths.push_back(".");

        // Analysis phase will accumulate info about accessed include files
        CHECK_EQUAL(true, RunAnalyze(s));

        // Retrieve the include list and check for expected content
        std::vector<core::string> includes = GetSurfaceAnalysisOpenedIncludes(analysisCache);
        CHECK_EQUAL(3, includes.size());
        CHECK_CONTAINS(includes.begin(), includes.end(), core::string(include1));
        CHECK_CONTAINS(includes.begin(), includes.end(), core::string(include2));
        CHECK_CONTAINS(includes.begin(), includes.end(), core::string(redirInclude));

        includeContext.includePaths.pop_back();
        DeleteFileAtPath(include1);
        DeleteFileAtPath(include2);
        DeleteFileAtPath(redirInclude);
    }

    // 968997: Assertion failed: Shader Compiler Socket Exception when certain Shaders interact with each other
    // The D3D9 bytecode had struct data D3DXSHADER_TYPEINFO at the end of the bytecode stream
    // The struct memeber data (also D3DXSHADER_TYPEINFO) was earlier in the bytecode stream
    // Due to bugs in the mojoshader parsing of D3DXSHADER_TYPEINFO the parser incorrectly thought the stream was corrupt
    // In addition the parser was using the wrong offset to parse the member data (it was using a relative offset
    // to the current D3DXSHADER_TYPEINFO instead of relative to the start of the D3DXSHADER_CONSTANTTABLE data)
    TEST(Mojoshader_parse_ctab_typeinfo_StructureAtEndOfBytecode_DoesNotCrash)
    {
        std::vector<UInt32> bytecode;
        UInt32 majorVer = 3;
        UInt32 minorVer = 0;
        UInt32 version = (0xFFFF << 16) | majorVer << 8 | minorVer;
        bytecode.push_back(version);                                            // Pixel Shader 3.0

        UInt32 commentTokenCount = 0;
        int commentTokenCountIndex = bytecode.size();
        bytecode.push_back((commentTokenCount << 16) | 0xFFFE);                 // CommentTokenCount | CommentToken

        UInt32 commentTokenStart = bytecode.size();
        bytecode.push_back('BATC');                                             // 'CTAB'
        UInt32 _Size = 28;                                                      // Size = sizeof(D3DXSHADER_CONSTANTTABLE)
        UInt32 CreatorOffset = 0;
        UInt32 Version = version;
        UInt32 Constants = 1;
        UInt32 ConstantInfoOffset = 0;
        UInt32 Flags = 0;
        UInt32 TargetOffset = 0;

        /*
        D3DXSHADER_CONSTANTTABLE
        {
          DWORD Size;
          DWORD Creator;
          DWORD Version;
          DWORD Constants;
          DWORD ConstantInfo;
          DWORD Flags;
          DWORD Target;
        }
        */

        // All offsets are in bytes and relative to start of constantTable data
        UInt32 constantTableStart = bytecode.size();
        bytecode.push_back(_Size);                                              // Size = sizeof(D3DXSHADER_CONSTANTTABLE)
        int creatorOffsetIndex = bytecode.size();
        bytecode.push_back(CreatorOffset * 4);                                    // Creator = offset to creator string
        bytecode.push_back(Version);                                            // Version = shader version
        bytecode.push_back(Constants);                                          // Constants = number of constants
        int constantInfoOffsetIndex = bytecode.size();
        bytecode.push_back(ConstantInfoOffset * 4);                               // ConstantInfo = offset to D3DXSHADER_CONSTANTINFO[Constants]
        bytecode.push_back(Flags);                                              // Flags = D3DXSHADER Flag
        int targetOffsetIndex = bytecode.size();
        bytecode.push_back(TargetOffset * 4);                                     // Target = offset to target string

        // String table
        CreatorOffset = bytecode.size() - constantTableStart;
        bytecode.push_back('CCCC');
        bytecode.push_back(0x0);
        TargetOffset = bytecode.size() - constantTableStart;
        bytecode.push_back('TTTT');
        bytecode.push_back(0x0);

        UInt32 NameOffset = bytecode.size() - constantTableStart;
        bytecode.push_back('NNNN');
        bytecode.push_back(0x0);
        UInt32 DefaultValueOffset = bytecode.size() - constantTableStart;
        bytecode.push_back('DDDD');
        bytecode.push_back(0x0);

        UInt32 structMemberInfoNameOffset = bytecode.size() - constantTableStart;
        bytecode.push_back('SSSS');
        bytecode.push_back(0x0);

        UInt32 structMemberInfoTypeInfoOffset = bytecode.size() - constantTableStart;
        /*
        D3DXSHADER_TYPEINFO
        {
          WORD  Class;
          WORD  Type;
          WORD  Rows;
          WORD  Columns;
          WORD  Elements;
          WORD  StructMembers;
          DWORD StructMemberInfo;
        }
        */
        // Single D3DXSHADER_TYPEINFO entry reused for all struct members
        UInt16 MemberClass = 0; // D3DXPC_SCALAR
        UInt16 MemberType = 3; // D3DXPT_FLOAT
        UInt16 MemberRows = 1;
        UInt16 MemberColumns = 2;
        UInt16 MemberElements = 3;
        UInt16 MemberStructMembers = 0;
        UInt32 MemberStructMemberInfoOffset = 0;
        bytecode.push_back(MemberClass | (MemberType << 16));
        bytecode.push_back(MemberRows | (MemberColumns << 16));
        bytecode.push_back(MemberElements | (MemberStructMembers << 16));
        bytecode.push_back(MemberStructMemberInfoOffset * 4);

        UInt32 StructNumMembers = 10;
        UInt32 StructMemberInfoOffset = bytecode.size() - constantTableStart;

        //D3DXSHADER_STRUCTMEMBERINFO[StructNumMembers] : but all member data entries point to the same entry

        /*
        D3DXSHADER_STRUCTMEMBERINFO
        {
            DWORD Name;
            DWORD TypeInfo;
        }
        */

        // All offsets are in bytes and relative to start of constantTable data
        for (int i = 0; i < StructNumMembers; ++i)
        {
            bytecode.push_back(structMemberInfoNameOffset * 4);                   // Name = offset to string
            bytecode.push_back(structMemberInfoTypeInfoOffset * 4);               // TypeInfo = offset to D3DXSHADER_TYPEINFO data
        }

        ConstantInfoOffset = bytecode.size() - constantTableStart;

        /*
        D3DXSHADER_CONSTANTINFO
        {
            DWORD Name;
            WORD  RegisterSet;
            WORD  RegisterIndex;
            WORD  RegisterCount;
            WORD  Reserved;
            DWORD TypeInfo;
            DWORD DefaultValue;
        }
        */

        // All offsets are in bytes and relative to start of constantTable data
        UInt16 RegisterSet = 1;
        UInt16 RegisterIndex = 2;
        UInt16 RegisterCount = 3;
        UInt16 Reserved = 4;
        UInt32 TypeInfoOffset = 0;
        bytecode.push_back(NameOffset * 4);                                     // Name = offset to name string
        bytecode.push_back(RegisterSet | RegisterIndex << 16);                // RegisterSet | RegisterIndex
        bytecode.push_back(RegisterCount | Reserved << 16);                   // RegisterCount | Reserved
        UInt32 typeInfoOffsetIndex = bytecode.size();
        bytecode.push_back(TypeInfoOffset * 4);                                 // TypeInfo = offset to D3DXSHADER_TYPEINFO data
        bytecode.push_back(DefaultValueOffset * 4);                             // DefaultValue = offset to default value string

        /*
        D3DXSHADER_TYPEINFO
        {
          WORD  Class;
          WORD  Type;
          WORD  Rows;
          WORD  Columns;
          WORD  Elements;
          WORD  StructMembers;
          DWORD StructMemberInfo;
        }
        */
        TypeInfoOffset = bytecode.size() - constantTableStart;
        UInt16 StructClass = 5; // D3DXPC_STRUCT
        UInt16 StructType = 0; // D3DXPT_VOID
        UInt16 StructRows = 1;
        UInt16 StructColumns = 2;
        UInt16 StructElements = 3;
        UInt16 StructMembers = StructNumMembers;
        bytecode.push_back(StructClass | (StructType << 16));
        bytecode.push_back(StructRows | (StructColumns << 16));
        bytecode.push_back(StructElements | (StructMembers << 16));
        bytecode.push_back(StructMemberInfoOffset * 4);

        // Update all the D3DXSHADER_CONSTANTTABLE offsets with the correct relative values
        bytecode[creatorOffsetIndex] = CreatorOffset * 4;
        bytecode[constantInfoOffsetIndex] = ConstantInfoOffset * 4;
        bytecode[targetOffsetIndex] = TargetOffset * 4;

        // Update all the D3DXSHADER_CONSTANTINFO offsets with the correct relative values
        bytecode[typeInfoOffsetIndex] = TypeInfoOffset * 4;

        // Update the actual number of comment tokens
        UInt32 commentTokenEnd = bytecode.size();
        commentTokenCount = commentTokenEnd - commentTokenStart;
        bytecode[commentTokenCountIndex] = (commentTokenCount << 16) | 0xFFFE;  // CommentTokenCount | CommentToken

        const MOJOSHADER_parseData* disasm = MOJOSHADER_parse(MOJOSHADER_PROFILE_BYTECODE, (const unsigned char*)&bytecode[0], (const unsigned int)bytecode.size() * 4, NULL, 0, NULL, 0, NULL, NULL, NULL);
        CHECK_EQUAL(0, disasm->error_count);
        CHECK_EQUAL(MOJOSHADER_TYPE_PIXEL, disasm->shader_type);
        CHECK_EQUAL(majorVer, disasm->major_ver);
        CHECK_EQUAL(minorVer, disasm->minor_ver);
        CHECK_EQUAL(0, disasm->uniform_count);
        CHECK_EQUAL(0, disasm->constant_count);
        CHECK_EQUAL(0, disasm->sampler_count);
        CHECK_EQUAL(0, disasm->attribute_count);
        CHECK_EQUAL(0, disasm->output_count);
        CHECK_EQUAL(1, disasm->symbol_count);

        MOJOSHADER_symbol& symbol = disasm->symbols[0];
        CHECK_EQUAL("NNNN", symbol.name);
        CHECK_EQUAL(RegisterSet, symbol.register_set);
        CHECK_EQUAL(RegisterIndex, symbol.register_index);
        CHECK_EQUAL(RegisterCount, symbol.register_count);

        MOJOSHADER_symbolTypeInfo& info = disasm->symbols[0].info;
        CHECK_EQUAL(StructClass, info.parameter_class);
        CHECK_EQUAL(StructType, info.parameter_type);
        CHECK_EQUAL(StructRows, info.rows);
        CHECK_EQUAL(StructColumns, info.columns);
        CHECK_EQUAL(StructElements, info.elements);
        CHECK_EQUAL(StructMembers, info.member_count);
        for (int i = 0; i < StructMembers; ++i)
        {
            MOJOSHADER_symbolStructMember& member = info.members[i];
            CHECK_EQUAL("SSSS", member.name);
            MOJOSHADER_symbolTypeInfo& member_info = member.info;
            CHECK_EQUAL(MemberClass, member_info.parameter_class);
            CHECK_EQUAL(MemberType, member_info.parameter_type);
            CHECK_EQUAL(MemberRows, member_info.rows);
            CHECK_EQUAL(MemberColumns, member_info.columns);
            CHECK_EQUAL(MemberElements, member_info.elements);
            CHECK_EQUAL(MemberStructMembers, member_info.member_count);
        }
        MOJOSHADER_freeParseData(disasm);
    }
}

#endif // ENABLE_UNIT_TESTS
