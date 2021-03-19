#include "UnityPrefix.h"
#include "GLESCompilerUtils.h"

#if ENABLE_UNIT_TESTS

#include "Runtime/Testing/Testing.h"

UNIT_TEST_SUITE(GLESCompilerUtils)
{
    TEST(ParseGLSLVersion_Empty_Returns120)
    {
        int ver = 0; bool es = false;
        ParseGLSLVersion("", ver, es);
        CHECK_EQUAL(ver, 120);
        CHECK_EQUAL(es, false);
    }
    TEST(ParseGLSLVersion_ValidVersionFound_Works)
    {
        int ver = 0; bool es = false;
        ParseGLSLVersion("   #version 100\nasd\u02C6flkj\u02C6.lkasjdfa\nsdf\u02C6lkj\n\n\n", ver, es);
        CHECK_EQUAL(ver, 100);
        CHECK_EQUAL(es, true);

        ParseGLSLVersion("#version 300 es    \nvoid main(asd\u02C6)\n{\n\n}\n", ver, es);
        CHECK_EQUAL(ver, 300);
        CHECK_EQUAL(es, true);

        ParseGLSLVersion("#version 420\nvoid main(asd\u02C6)\n{\n\n}\n", ver, es);
        CHECK_EQUAL(ver, 420);
        CHECK_EQUAL(es, false);

        ParseGLSLVersion("#version 123456 es\nvoid main(asd\u02C6)\n{\n\n}\n", ver, es);
        CHECK_EQUAL(ver, 123456);
        CHECK_EQUAL(es, true);
    }
    TEST(ParseGLSLVersion_NotFound_Returns120)
    {
        int ver = 0; bool es = false;
        ParseGLSLVersion("#ve    \nvoid main(asd\u02C6)\n{\n\n}\n", ver, es);
        CHECK_EQUAL(ver, 120);
        CHECK_EQUAL(es, false);
        ParseGLSLVersion("#pragma MutsisOli    \nvoid main(asd\u02C6)\n{\n\n}\n", ver, es);
        CHECK_EQUAL(ver, 120);
        CHECK_EQUAL(es, false);
    }

    TEST(FindShaderSplitPoint_Works)
    {
        CHECK_EQUAL(0, FindShaderSplitPoint(""));
        CHECK_EQUAL(0, FindShaderSplitPoint("foobar"));
        CHECK_EQUAL(15, FindShaderSplitPoint("#extension aaa\n"));
        CHECK_EQUAL(30, FindShaderSplitPoint("#extension aaa\n#extension bbb\n"));
    }

    TEST(TranslateGLSLToGL_SkipsOverExtensionStatements)
    {
        core::string s =
            "#ifdef VERTEX\n"
            "#extension GL_foobar : require\n"
            "void main() {\n"
            " gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
            "}\n"
            "#endif\n"
            "#ifdef FRAGMENT\n"
            "#extension GL_harhar : require\n"
            "void main() {\n"
            " gl_FragColor = gl_Color;\n"
            "}\n"
            "#endif";
        core::string res = TranslateGLSLToGLUnifiedShader(s);
        core::string expected =
            "#version 150\n\n#ifdef VERTEX\n\n"
            "#extension GL_foobar : require\n"
            "#define gl_Vertex _glesVertex\n"
            "in vec4 _glesVertex;\n"
            "void main() {\n"
            " gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
            "}\n"
            "\n#endif\n"
            "#ifdef FRAGMENT\n\n"
            "#extension GL_harhar : require\n"
            "#define gl_Color _glesFrontColor\n"
            "in vec4 _glesFrontColor;\n"
            "#define gl_FragColor _glesFragColor\n"
            "out vec4 _glesFragColor;\n"
            "void main() {\n"
            " gl_FragColor = gl_Color;\n"
            "}\n"
            "\n#endif";

        CHECK_EQUAL(expected.size(), res.size());
        CHECK_EQUAL(expected, res);
    }

    TEST(TranslateGLSLToGLES3_AddsInKeywords_AndNotVaryings)
    {
        core::string s =
            "#ifdef VERTEX\n"
            "void main() {\n"
            " gl_Position = gl_Vertex;\n"
            "}\n"
            "#endif\n"
            "#ifdef FRAGMENT\n"
            "void main() {\n"
            " gl_FragColor = gl_Color;\n"
            "}\n"
            "#endif";
        core::string res = TranslateGLSLToGLES30Plus(s);
        CHECK(res.find("#define gl_Color _glesFrontColor") != core::string::npos);
        CHECK(res.find("in lowp vec4 _glesFrontColor;") != core::string::npos);
    }

    TEST(FindAttributeDefinition_Works)
    {
        size_t pos;
        core::string attrName, attrDef;
        pos = FindAttributeDefinition(false, "moo; attribute highp vec3 Tangent; boo", "TANGENT", &attrName, &attrDef);
        CHECK(pos == 5);
        CHECK(attrName == "Tangent");
        CHECK(Trim(attrDef) == "attribute highp vec3 Tangent;");
        pos = FindAttributeDefinition(false, "attribute highp vec3 Tangent   ;", "TANGENT", &attrName, &attrDef);
        CHECK(pos == 0);
        CHECK(attrName == "Tangent");
        CHECK(Trim(attrDef) == "attribute highp vec3 Tangent   ;");
        CHECK(pos == 0);
        CHECK(attrName == "Tangent");

        pos = FindAttributeDefinition(false, "moo; attribute highp vec3 Tangents; boo", "TANGENT", &attrName, &attrDef);
        CHECK(pos == core::string::npos);
        pos = FindAttributeDefinition(false, "moo; attribute highp vec3 Normal; boo", "TANGENT", &attrName, &attrDef);
        CHECK(pos == core::string::npos);
        pos = FindAttributeDefinition(false, "uniform highp vec3 Normal;", "TANGENT", &attrName, &attrDef);
        CHECK(pos == core::string::npos);

        pos = FindAttributeDefinition(false, "moo; attribute highp vec3 Normal; attribute highp vec3 Tangent ; boo", "TANGENT", &attrName, &attrDef);
        CHECK(pos == 34);
        CHECK(attrName == "Tangent");
        CHECK(Trim(attrDef) == "attribute highp vec3 Tangent ;");

        pos = FindAttributeDefinition(false, "moo; attribute highp vec3 Normal; attribute highp vec3 TaNgEnT ; boo", "TANGENT", &attrName, &attrDef);
        CHECK(attrName == "TaNgEnT");
        CHECK(Trim(attrDef) == "attribute highp vec3 TaNgEnT ;");

        pos = FindAttributeDefinition(true, "#version 300 es\nmoo; in highp vec3 Tangent; boo", "TANGENT", &attrName, &attrDef);
        CHECK(pos == 21);
        CHECK(attrName == "Tangent");
        CHECK(Trim(attrDef) == "in highp vec3 Tangent;");

        // used to be a bug when for GLES3 we were finding attribute inside a "#define" since the "in" part looked like
        // a keyword... so we used to find "ine A 1\nin vec4 tangent" as the definition.
        pos = FindAttributeDefinition(true, "#version 300 es\n#define A 1\nin vec4 tangent;\n", "TANGENT", &attrName, &attrDef);
        CHECK(pos == 28);
        CHECK(attrName == "tangent");
        CHECK(Trim(attrDef) == "in vec4 tangent;");
    }
}

#endif // ENABLE_UNIT_TESTS
