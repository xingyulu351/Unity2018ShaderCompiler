#include "UnityPrefix.h"
#include "Runtime/Scripting/ScriptingExportUtility.h"



#include "UnityPrefix.h"
#include "Runtime/Math/Random/Random.h"
#include "Runtime/Scripting/ScriptingUtility.h"
#include <ctime>
	
using namespace std;

Rand gScriptingRand = Rand::GetRandomized();
	
SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Random_Get_Custom_PropSeed()
{
    SCRIPTINGAPI_ETW_ENTRY(Random_Get_Custom_PropSeed)
    SCRIPTINGAPI_STACK_CHECK(get_seed)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_seed)
    return gScriptingRand.GetSeed();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Random_Set_Custom_PropSeed(int value)
{
    SCRIPTINGAPI_ETW_ENTRY(Random_Set_Custom_PropSeed)
    SCRIPTINGAPI_STACK_CHECK(set_seed)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_seed)
     gScriptingRand.SetSeed(value); 
}


SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Random_CUSTOM_Range(float min, float max)
{
    SCRIPTINGAPI_ETW_ENTRY(Random_CUSTOM_Range)
    SCRIPTINGAPI_STACK_CHECK(Range)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Range)
     return RangedRandom (gScriptingRand, min, max); 
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Random_CUSTOM_RandomRangeInt(int min, int max)
{
    SCRIPTINGAPI_ETW_ENTRY(Random_CUSTOM_RandomRangeInt)
    SCRIPTINGAPI_STACK_CHECK(RandomRangeInt)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(RandomRangeInt)
     return RangedRandom (gScriptingRand, min, max); 
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Random_Get_Custom_PropValue()
{
    SCRIPTINGAPI_ETW_ENTRY(Random_Get_Custom_PropValue)
    SCRIPTINGAPI_STACK_CHECK(get_value)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_value)
    return Random01 (gScriptingRand);
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Random_CUSTOM_INTERNAL_get_insideUnitSphere(Vector3f* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Random_CUSTOM_INTERNAL_get_insideUnitSphere)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_insideUnitSphere)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_insideUnitSphere)
    { *returnValue =(RandomPointInsideUnitSphere (gScriptingRand)); return;};
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Random_CUSTOM_GetRandomUnitCircle(Vector2fIcall* output)
{
    SCRIPTINGAPI_ETW_ENTRY(Random_CUSTOM_GetRandomUnitCircle)
    SCRIPTINGAPI_STACK_CHECK(GetRandomUnitCircle)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetRandomUnitCircle)
    
    		*output = RandomPointInsideUnitCircle (gScriptingRand);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Random_CUSTOM_INTERNAL_get_onUnitSphere(Vector3f* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Random_CUSTOM_INTERNAL_get_onUnitSphere)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_onUnitSphere)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_onUnitSphere)
    { *returnValue =(RandomUnitVector (gScriptingRand)); return;};
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Random_CUSTOM_INTERNAL_get_rotation(Quaternionf* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Random_CUSTOM_INTERNAL_get_rotation)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_rotation)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_rotation)
    { *returnValue =(RandomQuaternion (gScriptingRand)); return;};
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Random_CUSTOM_INTERNAL_get_rotationUniform(Quaternionf* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Random_CUSTOM_INTERNAL_get_rotationUniform)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_rotationUniform)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_rotationUniform)
    { *returnValue =(RandomQuaternionUniformDistribution (gScriptingRand)); return;};
}

#if !defined(INTERNAL_CALL_STRIPPING)
#   error must include unityconfigure.h
#endif
#if INTERNAL_CALL_STRIPPING
void Register_UnityEngine_Random_get_seed()
{
    scripting_add_internal_call( "UnityEngine.Random::get_seed" , (gpointer)& Random_Get_Custom_PropSeed );
}

void Register_UnityEngine_Random_set_seed()
{
    scripting_add_internal_call( "UnityEngine.Random::set_seed" , (gpointer)& Random_Set_Custom_PropSeed );
}

void Register_UnityEngine_Random_Range()
{
    scripting_add_internal_call( "UnityEngine.Random::Range" , (gpointer)& Random_CUSTOM_Range );
}

void Register_UnityEngine_Random_RandomRangeInt()
{
    scripting_add_internal_call( "UnityEngine.Random::RandomRangeInt" , (gpointer)& Random_CUSTOM_RandomRangeInt );
}

void Register_UnityEngine_Random_get_value()
{
    scripting_add_internal_call( "UnityEngine.Random::get_value" , (gpointer)& Random_Get_Custom_PropValue );
}

void Register_UnityEngine_Random_INTERNAL_get_insideUnitSphere()
{
    scripting_add_internal_call( "UnityEngine.Random::INTERNAL_get_insideUnitSphere" , (gpointer)& Random_CUSTOM_INTERNAL_get_insideUnitSphere );
}

void Register_UnityEngine_Random_GetRandomUnitCircle()
{
    scripting_add_internal_call( "UnityEngine.Random::GetRandomUnitCircle" , (gpointer)& Random_CUSTOM_GetRandomUnitCircle );
}

void Register_UnityEngine_Random_INTERNAL_get_onUnitSphere()
{
    scripting_add_internal_call( "UnityEngine.Random::INTERNAL_get_onUnitSphere" , (gpointer)& Random_CUSTOM_INTERNAL_get_onUnitSphere );
}

void Register_UnityEngine_Random_INTERNAL_get_rotation()
{
    scripting_add_internal_call( "UnityEngine.Random::INTERNAL_get_rotation" , (gpointer)& Random_CUSTOM_INTERNAL_get_rotation );
}

void Register_UnityEngine_Random_INTERNAL_get_rotationUniform()
{
    scripting_add_internal_call( "UnityEngine.Random::INTERNAL_get_rotationUniform" , (gpointer)& Random_CUSTOM_INTERNAL_get_rotationUniform );
}

#elif ENABLE_MONO || ENABLE_IL2CPP
static const char* s_UnityEngineRandom_IcallNames [] =
{
    "UnityEngine.Random::get_seed"          ,    // -> Random_Get_Custom_PropSeed
    "UnityEngine.Random::set_seed"          ,    // -> Random_Set_Custom_PropSeed
    "UnityEngine.Random::Range"             ,    // -> Random_CUSTOM_Range
    "UnityEngine.Random::RandomRangeInt"    ,    // -> Random_CUSTOM_RandomRangeInt
    "UnityEngine.Random::get_value"         ,    // -> Random_Get_Custom_PropValue
    "UnityEngine.Random::INTERNAL_get_insideUnitSphere",    // -> Random_CUSTOM_INTERNAL_get_insideUnitSphere
    "UnityEngine.Random::GetRandomUnitCircle",    // -> Random_CUSTOM_GetRandomUnitCircle
    "UnityEngine.Random::INTERNAL_get_onUnitSphere",    // -> Random_CUSTOM_INTERNAL_get_onUnitSphere
    "UnityEngine.Random::INTERNAL_get_rotation",    // -> Random_CUSTOM_INTERNAL_get_rotation
    "UnityEngine.Random::INTERNAL_get_rotationUniform",    // -> Random_CUSTOM_INTERNAL_get_rotationUniform
    NULL
};

static const void* s_UnityEngineRandom_IcallFuncs [] =
{
    (const void*)&Random_Get_Custom_PropSeed              ,  //  <- UnityEngine.Random::get_seed
    (const void*)&Random_Set_Custom_PropSeed              ,  //  <- UnityEngine.Random::set_seed
    (const void*)&Random_CUSTOM_Range                     ,  //  <- UnityEngine.Random::Range
    (const void*)&Random_CUSTOM_RandomRangeInt            ,  //  <- UnityEngine.Random::RandomRangeInt
    (const void*)&Random_Get_Custom_PropValue             ,  //  <- UnityEngine.Random::get_value
    (const void*)&Random_CUSTOM_INTERNAL_get_insideUnitSphere,  //  <- UnityEngine.Random::INTERNAL_get_insideUnitSphere
    (const void*)&Random_CUSTOM_GetRandomUnitCircle       ,  //  <- UnityEngine.Random::GetRandomUnitCircle
    (const void*)&Random_CUSTOM_INTERNAL_get_onUnitSphere ,  //  <- UnityEngine.Random::INTERNAL_get_onUnitSphere
    (const void*)&Random_CUSTOM_INTERNAL_get_rotation     ,  //  <- UnityEngine.Random::INTERNAL_get_rotation
    (const void*)&Random_CUSTOM_INTERNAL_get_rotationUniform,  //  <- UnityEngine.Random::INTERNAL_get_rotationUniform
    NULL
};

void ExportUnityEngineRandomBindings();
void ExportUnityEngineRandomBindings()
{
    for (int i = 0; s_UnityEngineRandom_IcallNames [i] != NULL; ++i )
        scripting_add_internal_call( s_UnityEngineRandom_IcallNames [i], s_UnityEngineRandom_IcallFuncs [i] );
}

#elif ENABLE_DOTNET
// This comment is here on purpose, so Jam won't pick WinRTHelper.h as dependency for non WinRT targets, thus not doing unneeded recompilation.
//#include "Runtime/Scripting/WinRTHelper.h"
void ExportUnityEngineRandomBindings()
{
    #if UNITY_WP8
    extern intptr_t g_WinRTFuncPtrs[];
    #define SET_METRO_BINDING(Name) g_WinRTFuncPtrs[k##Name##FuncDef] = reinterpret_cast<intptr_t>(Name);
    #else
    long long* p = GetWinRTFuncDefsPointers();
    #define SET_METRO_BINDING(Name) p[k##Name##Func] = (long long)Name;
    #endif
    SET_METRO_BINDING(Random_Get_Custom_PropSeed); //  <- UnityEngine.Random::get_seed
    SET_METRO_BINDING(Random_Set_Custom_PropSeed); //  <- UnityEngine.Random::set_seed
    SET_METRO_BINDING(Random_CUSTOM_Range); //  <- UnityEngine.Random::Range
    SET_METRO_BINDING(Random_CUSTOM_RandomRangeInt); //  <- UnityEngine.Random::RandomRangeInt
    SET_METRO_BINDING(Random_Get_Custom_PropValue); //  <- UnityEngine.Random::get_value
    SET_METRO_BINDING(Random_CUSTOM_INTERNAL_get_insideUnitSphere); //  <- UnityEngine.Random::INTERNAL_get_insideUnitSphere
    SET_METRO_BINDING(Random_CUSTOM_GetRandomUnitCircle); //  <- UnityEngine.Random::GetRandomUnitCircle
    SET_METRO_BINDING(Random_CUSTOM_INTERNAL_get_onUnitSphere); //  <- UnityEngine.Random::INTERNAL_get_onUnitSphere
    SET_METRO_BINDING(Random_CUSTOM_INTERNAL_get_rotation); //  <- UnityEngine.Random::INTERNAL_get_rotation
    SET_METRO_BINDING(Random_CUSTOM_INTERNAL_get_rotationUniform); //  <- UnityEngine.Random::INTERNAL_get_rotationUniform
}

#endif
