#include "UnityPrefix.h"
#include "Runtime/Utilities/RapidJSONConfig.h"
#include <RapidJSON/document.h>

#include "Runtime/Testing/Testing.h"
#include "TocCombiner.h"
#include "RapidJsonIoUtil.h"
#include "JsonTestUtils.h"
#include <iostream>

#if ENABLE_UNIT_TESTS

using namespace Unity::rapidjson;

UNIT_TEST_SUITE(TocCombiner)
{
    const char APrimitiveToc[] =
        "{\"link\":\"toc\",\"title\":\"toc\",\"children\":null}";

    const char ASimpleToc[] =
        "{\"link\":\"toc\",\"title\":\"toc\",\"children\":["
        "\t{\"link\":\"null\",\"title\":\"Classes\",\"children\":["
        "\t\t{\"link\":\"A\",\"title\":\"A\",\"children\":null}"
        "\t]}"
        "]}";

    TEST(TryGetJsonDocFromString_ValidString_ReturnsTrue)
    {
        Document main, other, result;
        bool success = RapidJsonIoUtil::TryGetJsonDocFromString(APrimitiveToc, main);
        CHECK(success);
        success = RapidJsonIoUtil::TryGetJsonDocFromString(ASimpleToc, other);
        CHECK(success);
    }

    TEST(AppendJsonDocument_OneValueJson_ProducesThatJson)
    {
        Document A;

        TocCombiner jsonCombiner;
        RapidJsonIoUtil::TryGetJsonDocFromString(APrimitiveToc, A);

        // add module info to all documents
        //make new test to show that adding a platform prefix to a trivial toc produces no change
        //jsonCombiner.AddPlatformPrefix(A, 0);

        jsonCombiner.AppendJsonDocument(A);

        Document& actual = jsonCombiner.GetCombinedDoc();
        std::string actualSt = RapidJsonIoUtil::OutputJsonValueToString(actual);

        std::string expectedSt = "{\"link\":\"toc\",\"title\":\"toc\",\"children\":null}";

        CHECK_EQUAL(expectedSt, actualSt);
    }

    TEST(AppendJsonDocument_TrivialAndSimpleJson_ProducesSimpleJson)
    {
        std::string expectedSt =
            "{\"link\":\"toc\",\"title\":\"toc\",\"children\":["
            "{\"link\":\"null\",\"title\":\"Classes\",\"children\":["
            "{\"link\":\"A\",\"title\":\"A\",\"children\":null,\"module\":1}"
            "],\"module\":1}"
            "],\"module\":0}";

        TocCombiner jsonCombiner;
        jsonCombiner.SetModulePathAtIndex(0, "module0");
        jsonCombiner.SetModulePathAtIndex(1, "module1");

        VerifyCombineJsons(jsonCombiner, APrimitiveToc, 0, ASimpleToc, 1, expectedSt);
    }
}

#endif // ENABLE_UNIT_TESTS
