#ifndef UNITY_PREFIX_H
#define UNITY_PREFIX_H

#include <assert.h>

#include <string>
#include <vector>
#include <set>
#include <map>
#include <list>

#include "Modules/Baselib/Public/Baselib.h"

#define UNITY_EXTERNAL_TOOL 1

#if defined(_DEBUG) && !defined(ENABLE_UNIT_TESTS)
#define ENABLE_UNIT_TESTS 1
#endif

#if PLATFORM_OSX
#include "Projects/PrecompiledHeaders/OSXPrefix.h"
#endif

#include "Runtime/Allocator/MemoryMacros.h"
#include "Runtime/Utilities/Word.h"
#include "Configuration/UnityConfigure.h"
#include "Runtime/Logging/LogAssert.h"
#include "Runtime/Math/FloatConversion.h"

#endif
