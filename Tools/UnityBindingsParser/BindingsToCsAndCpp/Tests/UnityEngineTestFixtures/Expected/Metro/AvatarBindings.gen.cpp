#include "UnityPrefix.h"
#include "Runtime/Scripting/ScriptingExportUtility.h"


#include "UnityPrefix.h"
#include "Runtime/Mono/MonoBehaviour.h"
#include "Runtime/Mono/MonoScript.h"
#include "Runtime/Graphics/Transform.h"
#include "Runtime/Animation/Avatar.h"
#include "Runtime/mecanim/human/human.h"

using namespace Unity;
SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Avatar_Get_Custom_PropIsValid(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Avatar_Get_Custom_PropIsValid)
    ReadOnlyScriptingObjectOfType<Avatar> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_isValid)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_isValid)
    return self->IsValid();
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Avatar_Get_Custom_PropIsHuman(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Avatar_Get_Custom_PropIsHuman)
    ReadOnlyScriptingObjectOfType<Avatar> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_isHuman)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_isHuman)
    return self->IsValid() && self->GetAsset()->isHuman();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Avatar_CUSTOM_SetMuscleMinMax(ICallType_ReadOnlyUnityEngineObject_Argument self_, int muscleId, float min, float max)
{
    SCRIPTINGAPI_ETW_ENTRY(Avatar_CUSTOM_SetMuscleMinMax)
    ReadOnlyScriptingObjectOfType<Avatar> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(SetMuscleMinMax)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SetMuscleMinMax)
    
    		self->SetMuscleMinMax(muscleId, min, max);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Avatar_CUSTOM_SetParameter(ICallType_ReadOnlyUnityEngineObject_Argument self_, int parameterId, float value)
{
    SCRIPTINGAPI_ETW_ENTRY(Avatar_CUSTOM_SetParameter)
    ReadOnlyScriptingObjectOfType<Avatar> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(SetParameter)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SetParameter)
    
    		self->SetParameter(parameterId, value);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION HumanTrait_Get_Custom_PropMuscleCount()
{
    SCRIPTINGAPI_ETW_ENTRY(HumanTrait_Get_Custom_PropMuscleCount)
    SCRIPTINGAPI_STACK_CHECK(get_MuscleCount)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_MuscleCount)
    return HumanTrait::MuscleCount;
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION HumanTrait_Get_Custom_PropMuscleName()
{
    SCRIPTINGAPI_ETW_ENTRY(HumanTrait_Get_Custom_PropMuscleName)
    SCRIPTINGAPI_STACK_CHECK(get_MuscleName)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_MuscleName)
    return StringVectorToScripting( HumanTrait::GetMuscleName() );
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION HumanTrait_Get_Custom_PropBoneCount()
{
    SCRIPTINGAPI_ETW_ENTRY(HumanTrait_Get_Custom_PropBoneCount)
    SCRIPTINGAPI_STACK_CHECK(get_BoneCount)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_BoneCount)
    return HumanTrait::BoneCount;
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION HumanTrait_Get_Custom_PropBoneName()
{
    SCRIPTINGAPI_ETW_ENTRY(HumanTrait_Get_Custom_PropBoneName)
    SCRIPTINGAPI_STACK_CHECK(get_BoneName)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_BoneName)
    return StringVectorToScripting( HumanTrait::GetBoneName() );
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION HumanTrait_CUSTOM_MuscleFromBone(int i, int dofIndex)
{
    SCRIPTINGAPI_ETW_ENTRY(HumanTrait_CUSTOM_MuscleFromBone)
    SCRIPTINGAPI_STACK_CHECK(MuscleFromBone)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(MuscleFromBone)
    
    		return HumanTrait::MuscleFromBone(i, dofIndex);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION HumanTrait_CUSTOM_BoneFromMuscle(int i)
{
    SCRIPTINGAPI_ETW_ENTRY(HumanTrait_CUSTOM_BoneFromMuscle)
    SCRIPTINGAPI_STACK_CHECK(BoneFromMuscle)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(BoneFromMuscle)
    
    		return HumanTrait::BoneFromMuscle(i);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION HumanTrait_CUSTOM_RequiredBone(int i)
{
    SCRIPTINGAPI_ETW_ENTRY(HumanTrait_CUSTOM_RequiredBone)
    SCRIPTINGAPI_STACK_CHECK(RequiredBone)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(RequiredBone)
    
    		return HumanTrait::RequiredBone(i);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION HumanTrait_Get_Custom_PropRequiredBoneCount()
{
    SCRIPTINGAPI_ETW_ENTRY(HumanTrait_Get_Custom_PropRequiredBoneCount)
    SCRIPTINGAPI_STACK_CHECK(get_RequiredBoneCount)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_RequiredBoneCount)
    return HumanTrait::RequiredBoneCount();
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION HumanTrait_CUSTOM_HasCollider(ICallType_ReadOnlyUnityEngineObject_Argument avatar_, int i)
{
    SCRIPTINGAPI_ETW_ENTRY(HumanTrait_CUSTOM_HasCollider)
    ReadOnlyScriptingObjectOfType<Avatar> avatar(avatar_);
    UNUSED(avatar);
    SCRIPTINGAPI_STACK_CHECK(HasCollider)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(HasCollider)
    
    		return HumanTrait::HasCollider(*avatar, i);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION HumanTrait_CUSTOM_GetMuscleDefaultMin(int i)
{
    SCRIPTINGAPI_ETW_ENTRY(HumanTrait_CUSTOM_GetMuscleDefaultMin)
    SCRIPTINGAPI_STACK_CHECK(GetMuscleDefaultMin)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetMuscleDefaultMin)
    
    		return HumanTrait::GetMuscleDefaultMin(i);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION HumanTrait_CUSTOM_GetMuscleDefaultMax(int i)
{
    SCRIPTINGAPI_ETW_ENTRY(HumanTrait_CUSTOM_GetMuscleDefaultMax)
    SCRIPTINGAPI_STACK_CHECK(GetMuscleDefaultMax)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(GetMuscleDefaultMax)
    
    		return HumanTrait::GetMuscleDefaultMax(i);
    	
}

#if !defined(INTERNAL_CALL_STRIPPING)
#   error must include unityconfigure.h
#endif
#if INTERNAL_CALL_STRIPPING
void Register_UnityEngine_Avatar_get_isValid()
{
    scripting_add_internal_call( "UnityEngine.Avatar::get_isValid" , (gpointer)& Avatar_Get_Custom_PropIsValid );
}

void Register_UnityEngine_Avatar_get_isHuman()
{
    scripting_add_internal_call( "UnityEngine.Avatar::get_isHuman" , (gpointer)& Avatar_Get_Custom_PropIsHuman );
}

void Register_UnityEngine_Avatar_SetMuscleMinMax()
{
    scripting_add_internal_call( "UnityEngine.Avatar::SetMuscleMinMax" , (gpointer)& Avatar_CUSTOM_SetMuscleMinMax );
}

void Register_UnityEngine_Avatar_SetParameter()
{
    scripting_add_internal_call( "UnityEngine.Avatar::SetParameter" , (gpointer)& Avatar_CUSTOM_SetParameter );
}

void Register_UnityEngine_HumanTrait_get_MuscleCount()
{
    scripting_add_internal_call( "UnityEngine.HumanTrait::get_MuscleCount" , (gpointer)& HumanTrait_Get_Custom_PropMuscleCount );
}

void Register_UnityEngine_HumanTrait_get_MuscleName()
{
    scripting_add_internal_call( "UnityEngine.HumanTrait::get_MuscleName" , (gpointer)& HumanTrait_Get_Custom_PropMuscleName );
}

void Register_UnityEngine_HumanTrait_get_BoneCount()
{
    scripting_add_internal_call( "UnityEngine.HumanTrait::get_BoneCount" , (gpointer)& HumanTrait_Get_Custom_PropBoneCount );
}

void Register_UnityEngine_HumanTrait_get_BoneName()
{
    scripting_add_internal_call( "UnityEngine.HumanTrait::get_BoneName" , (gpointer)& HumanTrait_Get_Custom_PropBoneName );
}

void Register_UnityEngine_HumanTrait_MuscleFromBone()
{
    scripting_add_internal_call( "UnityEngine.HumanTrait::MuscleFromBone" , (gpointer)& HumanTrait_CUSTOM_MuscleFromBone );
}

void Register_UnityEngine_HumanTrait_BoneFromMuscle()
{
    scripting_add_internal_call( "UnityEngine.HumanTrait::BoneFromMuscle" , (gpointer)& HumanTrait_CUSTOM_BoneFromMuscle );
}

void Register_UnityEngine_HumanTrait_RequiredBone()
{
    scripting_add_internal_call( "UnityEngine.HumanTrait::RequiredBone" , (gpointer)& HumanTrait_CUSTOM_RequiredBone );
}

void Register_UnityEngine_HumanTrait_get_RequiredBoneCount()
{
    scripting_add_internal_call( "UnityEngine.HumanTrait::get_RequiredBoneCount" , (gpointer)& HumanTrait_Get_Custom_PropRequiredBoneCount );
}

void Register_UnityEngine_HumanTrait_HasCollider()
{
    scripting_add_internal_call( "UnityEngine.HumanTrait::HasCollider" , (gpointer)& HumanTrait_CUSTOM_HasCollider );
}

void Register_UnityEngine_HumanTrait_GetMuscleDefaultMin()
{
    scripting_add_internal_call( "UnityEngine.HumanTrait::GetMuscleDefaultMin" , (gpointer)& HumanTrait_CUSTOM_GetMuscleDefaultMin );
}

void Register_UnityEngine_HumanTrait_GetMuscleDefaultMax()
{
    scripting_add_internal_call( "UnityEngine.HumanTrait::GetMuscleDefaultMax" , (gpointer)& HumanTrait_CUSTOM_GetMuscleDefaultMax );
}

#elif ENABLE_MONO || ENABLE_IL2CPP
static const char* s_Avatar_IcallNames [] =
{
    "UnityEngine.Avatar::get_isValid"       ,    // -> Avatar_Get_Custom_PropIsValid
    "UnityEngine.Avatar::get_isHuman"       ,    // -> Avatar_Get_Custom_PropIsHuman
    "UnityEngine.Avatar::SetMuscleMinMax"   ,    // -> Avatar_CUSTOM_SetMuscleMinMax
    "UnityEngine.Avatar::SetParameter"      ,    // -> Avatar_CUSTOM_SetParameter
    "UnityEngine.HumanTrait::get_MuscleCount",    // -> HumanTrait_Get_Custom_PropMuscleCount
    "UnityEngine.HumanTrait::get_MuscleName",    // -> HumanTrait_Get_Custom_PropMuscleName
    "UnityEngine.HumanTrait::get_BoneCount" ,    // -> HumanTrait_Get_Custom_PropBoneCount
    "UnityEngine.HumanTrait::get_BoneName"  ,    // -> HumanTrait_Get_Custom_PropBoneName
    "UnityEngine.HumanTrait::MuscleFromBone",    // -> HumanTrait_CUSTOM_MuscleFromBone
    "UnityEngine.HumanTrait::BoneFromMuscle",    // -> HumanTrait_CUSTOM_BoneFromMuscle
    "UnityEngine.HumanTrait::RequiredBone"  ,    // -> HumanTrait_CUSTOM_RequiredBone
    "UnityEngine.HumanTrait::get_RequiredBoneCount",    // -> HumanTrait_Get_Custom_PropRequiredBoneCount
    "UnityEngine.HumanTrait::HasCollider"   ,    // -> HumanTrait_CUSTOM_HasCollider
    "UnityEngine.HumanTrait::GetMuscleDefaultMin",    // -> HumanTrait_CUSTOM_GetMuscleDefaultMin
    "UnityEngine.HumanTrait::GetMuscleDefaultMax",    // -> HumanTrait_CUSTOM_GetMuscleDefaultMax
    NULL
};

static const void* s_Avatar_IcallFuncs [] =
{
    (const void*)&Avatar_Get_Custom_PropIsValid           ,  //  <- UnityEngine.Avatar::get_isValid
    (const void*)&Avatar_Get_Custom_PropIsHuman           ,  //  <- UnityEngine.Avatar::get_isHuman
    (const void*)&Avatar_CUSTOM_SetMuscleMinMax           ,  //  <- UnityEngine.Avatar::SetMuscleMinMax
    (const void*)&Avatar_CUSTOM_SetParameter              ,  //  <- UnityEngine.Avatar::SetParameter
    (const void*)&HumanTrait_Get_Custom_PropMuscleCount   ,  //  <- UnityEngine.HumanTrait::get_MuscleCount
    (const void*)&HumanTrait_Get_Custom_PropMuscleName    ,  //  <- UnityEngine.HumanTrait::get_MuscleName
    (const void*)&HumanTrait_Get_Custom_PropBoneCount     ,  //  <- UnityEngine.HumanTrait::get_BoneCount
    (const void*)&HumanTrait_Get_Custom_PropBoneName      ,  //  <- UnityEngine.HumanTrait::get_BoneName
    (const void*)&HumanTrait_CUSTOM_MuscleFromBone        ,  //  <- UnityEngine.HumanTrait::MuscleFromBone
    (const void*)&HumanTrait_CUSTOM_BoneFromMuscle        ,  //  <- UnityEngine.HumanTrait::BoneFromMuscle
    (const void*)&HumanTrait_CUSTOM_RequiredBone          ,  //  <- UnityEngine.HumanTrait::RequiredBone
    (const void*)&HumanTrait_Get_Custom_PropRequiredBoneCount,  //  <- UnityEngine.HumanTrait::get_RequiredBoneCount
    (const void*)&HumanTrait_CUSTOM_HasCollider           ,  //  <- UnityEngine.HumanTrait::HasCollider
    (const void*)&HumanTrait_CUSTOM_GetMuscleDefaultMin   ,  //  <- UnityEngine.HumanTrait::GetMuscleDefaultMin
    (const void*)&HumanTrait_CUSTOM_GetMuscleDefaultMax   ,  //  <- UnityEngine.HumanTrait::GetMuscleDefaultMax
    NULL
};

void ExportAvatarBindings();
void ExportAvatarBindings()
{
    for (int i = 0; s_Avatar_IcallNames [i] != NULL; ++i )
        scripting_add_internal_call( s_Avatar_IcallNames [i], s_Avatar_IcallFuncs [i] );
}

#elif ENABLE_DOTNET
#include "Runtime/Scripting/WinRTHelper.h"
void ExportAvatarBindings()
{
    #if UNITY_WP8
    extern intptr_t g_WinRTFuncPtrs[];
    #define SET_METRO_BINDING(Name) g_WinRTFuncPtrs[k##Name##FuncDef] = reinterpret_cast<intptr_t>(Name);
    #else
    long long* p = GetWinRTFuncDefsPointers();
    #define SET_METRO_BINDING(Name) p[k##Name##Func] = (long long)Name;
    #endif
    SET_METRO_BINDING(Avatar_Get_Custom_PropIsValid); //  <- UnityEngine.Avatar::get_isValid
    SET_METRO_BINDING(Avatar_Get_Custom_PropIsHuman); //  <- UnityEngine.Avatar::get_isHuman
    SET_METRO_BINDING(Avatar_CUSTOM_SetMuscleMinMax); //  <- UnityEngine.Avatar::SetMuscleMinMax
    SET_METRO_BINDING(Avatar_CUSTOM_SetParameter); //  <- UnityEngine.Avatar::SetParameter
    SET_METRO_BINDING(HumanTrait_Get_Custom_PropMuscleCount); //  <- UnityEngine.HumanTrait::get_MuscleCount
    SET_METRO_BINDING(HumanTrait_Get_Custom_PropMuscleName); //  <- UnityEngine.HumanTrait::get_MuscleName
    SET_METRO_BINDING(HumanTrait_Get_Custom_PropBoneCount); //  <- UnityEngine.HumanTrait::get_BoneCount
    SET_METRO_BINDING(HumanTrait_Get_Custom_PropBoneName); //  <- UnityEngine.HumanTrait::get_BoneName
    SET_METRO_BINDING(HumanTrait_CUSTOM_MuscleFromBone); //  <- UnityEngine.HumanTrait::MuscleFromBone
    SET_METRO_BINDING(HumanTrait_CUSTOM_BoneFromMuscle); //  <- UnityEngine.HumanTrait::BoneFromMuscle
    SET_METRO_BINDING(HumanTrait_CUSTOM_RequiredBone); //  <- UnityEngine.HumanTrait::RequiredBone
    SET_METRO_BINDING(HumanTrait_Get_Custom_PropRequiredBoneCount); //  <- UnityEngine.HumanTrait::get_RequiredBoneCount
    SET_METRO_BINDING(HumanTrait_CUSTOM_HasCollider); //  <- UnityEngine.HumanTrait::HasCollider
    SET_METRO_BINDING(HumanTrait_CUSTOM_GetMuscleDefaultMin); //  <- UnityEngine.HumanTrait::GetMuscleDefaultMin
    SET_METRO_BINDING(HumanTrait_CUSTOM_GetMuscleDefaultMax); //  <- UnityEngine.HumanTrait::GetMuscleDefaultMax
}

#endif
