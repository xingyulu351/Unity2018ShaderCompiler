#include "UnityPrefix.h"
#include "Runtime/Utilities/RapidJSONConfig.h"
#include "rapidjson/document.h"
#include "Runtime/Testing/Testing.h"

#include "RapidJsonIoUtil.h"
#include "TocCombiner.h"
#include "IndexCombiner.h"
#include <string>
#include <list>


using namespace Unity::rapidjson;

int main()
{
    return UnitTest::RunAllTests();
}
