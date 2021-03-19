#include "UnityPrefix.h"
#include "Runtime/Scripting/ScriptingExportUtility.h"


#include "UnityPrefix.h"
#include "Runtime/Mono/MonoBehaviour.h"
#include "Runtime/Mono/MonoScript.h"

#include "Runtime/Scripting/ScriptingManager.h"
#include "Runtime/Scripting/ScriptingUtility.h"
#include "Runtime/Scripting/ScriptingExportUtility.h"

#include "Runtime/Graphics/Transform.h"
#include "Runtime/Animation/AvatarBuilder.h"
#include "Runtime/Animation/Avatar.h"

using namespace Unity;


struct MonoSkeletonBone {
	ScriptingStringPtr name;
	Vector3f	position;
	Quaternionf rotation;
	Vector3f	scale;
	int			transformModified;
};

void SkeletonBoneToMono (const SkeletonBone &src, MonoSkeletonBone &dest) {
	dest.name = CreateScriptingString(src.m_Name);
	dest.position = src.m_Position;
	dest.rotation = src.m_Rotation;
	dest.scale = src.m_Scale;
	dest.transformModified = src.m_TransformModified ? 1 : 0;
}

void SkeletonBoneFromMono (const MonoSkeletonBone &src, SkeletonBone &dest) {
	dest.m_Name = scripting_cpp_string_for(src.name);
	dest.m_Position = src.position;
	dest.m_Rotation = src.rotation;
	dest.m_Scale = src.scale;
	dest.m_TransformModified = src.transformModified != 0;
}


struct MonoHumanLimit {
	
	Vector3f	m_Min;
	Vector3f	m_Max;
	Vector3f	m_Center;
	Quaternionf m_PreQ;
	Quaternionf m_PostQ;	
	float		m_AxisLength;
	int			m_UseDefaultValues;
};

void HumanLimitToMono (const SkeletonBoneLimit &src, MonoHumanLimit &dest) {
	dest.m_UseDefaultValues = src.m_Modified ? 0 : 1;
	dest.m_Min = src.m_Min;
	dest.m_Max = src.m_Max;
	dest.m_Center = src.m_Value;
	dest.m_AxisLength = src.m_Length;
}

void HumanLimitFromMono (const MonoHumanLimit &src, SkeletonBoneLimit &dest) {
	dest.m_Modified = src.m_UseDefaultValues == 1 ? false : true;
	dest.m_Min = src.m_Min;
	dest.m_Max = src.m_Max;
	dest.m_Value = src.m_Center;
	dest.m_Length = src.m_AxisLength;
}


struct MonoHumanBone {
	ScriptingStringPtr	m_BoneName;
	ScriptingStringPtr	m_HumanName;
	MonoHumanLimit		m_Limit;
};

void HumanBoneToMono (const HumanBone &src, MonoHumanBone &dest) 
{
	dest.m_BoneName = CreateScriptingString(src.m_BoneName);
	dest.m_HumanName = CreateScriptingString(src.m_HumanName);
	HumanLimitToMono(src.m_Limit, dest.m_Limit);
}

void HumanBoneFromMono (const MonoHumanBone &src, HumanBone &dest) 
{
	dest.m_BoneName = scripting_cpp_string_for(src.m_BoneName);
	dest.m_HumanName = scripting_cpp_string_for(src.m_HumanName);
	HumanLimitFromMono(src.m_Limit, dest.m_Limit);
}


struct MonoHumanDescription {
	ScriptingArrayPtr	m_Human;
	ScriptingArrayPtr	m_Skeleton;

	float				m_ArmTwist;
	float				m_ForeArmTwist;
	float				m_UpperLegTwist;
	float				m_LegTwist;
	float				m_ArmStretch;
	float				m_LegStretch;
	float				m_FeetSpacing;
};


void HumanDescriptionToMono (const HumanDescription &src, MonoHumanDescription &dest) 
{
	if (src.m_Skeleton.size() <= 0)
		dest.m_Skeleton = CreateEmptyStructArray(GetCommonScriptingClasses ().skeletonBone);
	else
		dest.m_Skeleton = CreateScriptingArray(&src.m_Skeleton[0], src.m_Skeleton.size(), GetCommonScriptingClasses ().skeletonBone);

	if (src.m_Human.size() <= 0)
		dest.m_Human = CreateEmptyStructArray(GetCommonScriptingClasses ().humanBone);
	else
		dest.m_Human = CreateScriptingArray(&src.m_Human[0], src.m_Human.size(), GetCommonScriptingClasses ().humanBone);	

	dest.m_ArmTwist = src.m_ArmTwist;
	dest.m_ForeArmTwist = src.m_ForeArmTwist;

	dest.m_UpperLegTwist = src.m_UpperLegTwist;
	dest.m_LegTwist = src.m_LegTwist;
	dest.m_ArmStretch = src.m_ArmStretch;
	dest.m_LegStretch = src.m_LegStretch;
	dest.m_FeetSpacing = src.m_FeetSpacing;
}

void HumanDescriptionFromMono (const MonoHumanDescription &src, HumanDescription &dest) 
{
	ScriptingStructArrayToVector<SkeletonBone, MonoSkeletonBone>(src.m_Skeleton, dest.m_Skeleton, SkeletonBoneFromMono);
	ScriptingStructArrayToVector<HumanBone, MonoHumanBone>(src.m_Human, dest.m_Human, HumanBoneFromMono);

	dest.m_ArmTwist = src.m_ArmTwist;
	dest.m_ForeArmTwist = src.m_ForeArmTwist;

	dest.m_UpperLegTwist = src.m_UpperLegTwist;
	dest.m_LegTwist = src.m_LegTwist;
	dest.m_ArmStretch = src.m_ArmStretch;
	dest.m_LegStretch = src.m_LegStretch;
	dest.m_FeetSpacing = src.m_FeetSpacing;
}
SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION AvatarBuilder_CUSTOM_INTERNAL_CALL_BuildHumanAvatar(ICallType_ReadOnlyUnityEngineObject_Argument go_, const MonoHumanDescription& monoHumanDescription)
{
    SCRIPTINGAPI_ETW_ENTRY(AvatarBuilder_CUSTOM_INTERNAL_CALL_BuildHumanAvatar)
    ReadOnlyScriptingObjectOfType<GameObject> go(go_);
    UNUSED(go);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_BuildHumanAvatar)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_BuildHumanAvatar)
    
    		Avatar* avatar = NEW_OBJECT(Avatar);
    		avatar->Reset();	
    
    		HumanDescription humanDescription;
    		HumanDescriptionFromMono(monoHumanDescription, humanDescription);
    
    		AvatarBuilder::Options options;
    		options.avatarType = kHumanoid;
    		options.useMask = true;
    
    		std::string error = AvatarBuilder::BuildAvatar(*avatar, *go, humanDescription, options);
    		if(!error.empty())
    			ErrorString(error);
    
    		avatar->AwakeFromLoad(kInstantiateOrCreateFromCodeAwakeFromLoad);
    
    		return Scripting::ScriptingWrapperFor(avatar);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION AvatarBuilder_CUSTOM_BuildGenericAvatar(ICallType_ReadOnlyUnityEngineObject_Argument go_, ICallType_String_Argument rootMotionTransformName_)
{
    SCRIPTINGAPI_ETW_ENTRY(AvatarBuilder_CUSTOM_BuildGenericAvatar)
    ReadOnlyScriptingObjectOfType<GameObject> go(go_);
    UNUSED(go);
    ICallType_String_Local rootMotionTransformName(rootMotionTransformName_);
    UNUSED(rootMotionTransformName);
    SCRIPTINGAPI_STACK_CHECK(BuildGenericAvatar)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(BuildGenericAvatar)
    
    		Avatar* avatar = NEW_OBJECT(Avatar);
    		avatar->Reset();	
    
    		HumanDescription humanDescription;
    		humanDescription.m_RootMotionBoneName = rootMotionTransformName.ToUTF8().c_str();
    		std::string error = AvatarBuilder::BuildAvatar(*avatar, *go, humanDescription);
    		if(!error.empty())
    			ErrorString(error);
    
    		avatar->AwakeFromLoad(kInstantiateOrCreateFromCodeAwakeFromLoad);
    
    		return Scripting::ScriptingWrapperFor(avatar);
    	
}

#if !defined(INTERNAL_CALL_STRIPPING)
#   error must include unityconfigure.h
#endif
#if INTERNAL_CALL_STRIPPING
void Register_UnityEngine_AvatarBuilder_INTERNAL_CALL_BuildHumanAvatar()
{
    scripting_add_internal_call( "UnityEngine.AvatarBuilder::INTERNAL_CALL_BuildHumanAvatar" , (gpointer)& AvatarBuilder_CUSTOM_INTERNAL_CALL_BuildHumanAvatar );
}

void Register_UnityEngine_AvatarBuilder_BuildGenericAvatar()
{
    scripting_add_internal_call( "UnityEngine.AvatarBuilder::BuildGenericAvatar" , (gpointer)& AvatarBuilder_CUSTOM_BuildGenericAvatar );
}

#elif ENABLE_MONO || ENABLE_IL2CPP
static const char* s_AvatarBuilder_IcallNames [] =
{
    "UnityEngine.AvatarBuilder::INTERNAL_CALL_BuildHumanAvatar",    // -> AvatarBuilder_CUSTOM_INTERNAL_CALL_BuildHumanAvatar
    "UnityEngine.AvatarBuilder::BuildGenericAvatar",    // -> AvatarBuilder_CUSTOM_BuildGenericAvatar
    NULL
};

static const void* s_AvatarBuilder_IcallFuncs [] =
{
    (const void*)&AvatarBuilder_CUSTOM_INTERNAL_CALL_BuildHumanAvatar,  //  <- UnityEngine.AvatarBuilder::INTERNAL_CALL_BuildHumanAvatar
    (const void*)&AvatarBuilder_CUSTOM_BuildGenericAvatar ,  //  <- UnityEngine.AvatarBuilder::BuildGenericAvatar
    NULL
};

void ExportAvatarBuilderBindings();
void ExportAvatarBuilderBindings()
{
    for (int i = 0; s_AvatarBuilder_IcallNames [i] != NULL; ++i )
        scripting_add_internal_call( s_AvatarBuilder_IcallNames [i], s_AvatarBuilder_IcallFuncs [i] );
}

#elif ENABLE_DOTNET
// This comment is here on purpose, so Jam won't pick WinRTHelper.h as dependency for non WinRT targets, thus not doing unneeded recompilation.
//#include "Runtime/Scripting/WinRTHelper.h"
void ExportAvatarBuilderBindings()
{
    #if UNITY_WP8
    extern intptr_t g_WinRTFuncPtrs[];
    #define SET_METRO_BINDING(Name) g_WinRTFuncPtrs[k##Name##FuncDef] = reinterpret_cast<intptr_t>(Name);
    #else
    long long* p = GetWinRTFuncDefsPointers();
    #define SET_METRO_BINDING(Name) p[k##Name##Func] = (long long)Name;
    #endif
    SET_METRO_BINDING(AvatarBuilder_CUSTOM_INTERNAL_CALL_BuildHumanAvatar); //  <- UnityEngine.AvatarBuilder::INTERNAL_CALL_BuildHumanAvatar
    SET_METRO_BINDING(AvatarBuilder_CUSTOM_BuildGenericAvatar); //  <- UnityEngine.AvatarBuilder::BuildGenericAvatar
}

#endif
