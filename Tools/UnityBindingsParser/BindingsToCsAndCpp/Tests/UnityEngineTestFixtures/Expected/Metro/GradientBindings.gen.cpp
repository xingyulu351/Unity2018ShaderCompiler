#include "UnityPrefix.h"
#include "Runtime/Scripting/ScriptingExportUtility.h"


#include "UnityPrefix.h"
#include "Runtime/Math/Gradient.h"
#include "Runtime/Math/Color.h"
#include "Runtime/Scripting/ScriptingUtility.h"
#include "Runtime/Scripting/ScriptingExportUtility.h"
using namespace Unity;



struct MonoGradientColorKey
{
	ColorRGBAf color;
	float time;
};


struct MonoGradientAlphaKey
{
	float alpha;
	float time;
};

 static void CleanupGradient(void* gradient){ delete ((Gradient*)gradient); } ;
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Gradient_CUSTOM_Init(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Gradient_CUSTOM_Init)
    ScriptingObjectWithIntPtrField<Gradient> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(Init)
    
    		self.SetPtr(new Gradient(), CleanupGradient);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Gradient_CUSTOM_Cleanup(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Gradient_CUSTOM_Cleanup)
    ScriptingObjectWithIntPtrField<Gradient> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(Cleanup)
     CleanupGradient(self.GetPtr()); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Gradient_CUSTOM_INTERNAL_CALL_Evaluate(ICallType_Object_Argument self_, float time, ColorRGBAf& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Gradient_CUSTOM_INTERNAL_CALL_Evaluate)
    ScriptingObjectWithIntPtrField<Gradient> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_Evaluate)
    
    		time = clamp01(time);
    		{ returnValue =(self->Evaluate (time)); return; }
    	
}


	int ConvertColorKeyArray (ScriptingArrayPtr scriptColorKeys, Gradient::ColorKey* colorKeys)
	{
		if (scriptColorKeys == SCRIPTING_NULL)
		{
			ErrorString("SetKeys: Invalid input ColorKey array");
			return -1;
		}
	
		int size = GetScriptingArraySize (scriptColorKeys);
		if (size > kGradientMaxNumKeys)
		{
			ErrorString(Format("Max number of color keys is %d (given %d)", kGradientMaxNumKeys, size));
			return -1;
		}
			
		for (int i=0; i < size ; i++)
		{
			MonoGradientColorKey &key = GetScriptingArrayElement<MonoGradientColorKey> (scriptColorKeys, i);
			colorKeys[i].m_Color = key.color;
			colorKeys[i].m_Time = key.time;
		}
		
		return size;
	}
	

	int ConvertAlphaKeyArray (ScriptingArrayPtr scriptAlphaKeys, Gradient::AlphaKey* alphaKeys)
	{
		if (scriptAlphaKeys == SCRIPTING_NULL)
		{
			ErrorString("SetKeys: Invalid input AlphaKey array");
			return -1;
		}
	
		int size = GetScriptingArraySize (scriptAlphaKeys);
		if (size > kGradientMaxNumKeys)
		{
			ErrorString(Format("Max number of alpha keys is %d (given %d)", kGradientMaxNumKeys, size));
			return -1;
		}
			
		for (int i=0; i < size ; i++)
		{
			MonoGradientAlphaKey &key = GetScriptingArrayElement<MonoGradientAlphaKey> (scriptAlphaKeys, i);
			alphaKeys[i].m_Alpha = key.alpha;
			alphaKeys[i].m_Time = key.time;
		}
		
		return size;
	}	
SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION Gradient_Get_Custom_PropColorKeys(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Gradient_Get_Custom_PropColorKeys)
    ScriptingObjectWithIntPtrField<Gradient> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_colorKeys)
    int numColorKeys = self->GetNumColorKeys ();
    Gradient::ColorKey colorKeys[kGradientMaxNumKeys];
    ColorRGBA32& firstKey = self->GetKey(0);
    UInt16& firstColorTime = self->GetColorTime(0);
    for(int i = 0; i < kGradientMaxNumKeys; i++)
    {
    colorKeys[i].m_Color = (&firstKey)[i];
    colorKeys[i].m_Color.a = 255;
    colorKeys[i].m_Time = WordToNormalized((&firstColorTime)[i]);
    }
    return CreateScriptingArray (colorKeys, numColorKeys, GetCommonScriptingClasses ().gradientColorKey);
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Gradient_Set_Custom_PropColorKeys(ICallType_Object_Argument self_, ICallType_Array_Argument value_)
{
    SCRIPTINGAPI_ETW_ENTRY(Gradient_Set_Custom_PropColorKeys)
    ScriptingObjectWithIntPtrField<Gradient> self(self_);
    UNUSED(self);
    ICallType_Array_Local value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(set_colorKeys)
    
    Gradient::ColorKey colorKeys[kGradientMaxNumKeys];
    int numColorKeys = ConvertColorKeyArray (value, colorKeys);
    if (numColorKeys == -1)
    return;
    self->SetColorKeys(&colorKeys[0], numColorKeys);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION Gradient_Get_Custom_PropAlphaKeys(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Gradient_Get_Custom_PropAlphaKeys)
    ScriptingObjectWithIntPtrField<Gradient> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_alphaKeys)
    int numAlphaKeys = self->GetNumAlphaKeys ();
    Gradient::AlphaKey alphaKeys[kGradientMaxNumKeys];
    const ColorRGBA32& firstKey = self->GetKey(0);
    const UInt16& firstAlphaTime = self->GetAlphaTime(0);
    for(int i = 0; i < kGradientMaxNumKeys; i++)
    {
    alphaKeys[i].m_Alpha = ByteToNormalized((&firstKey)[i].a);
    alphaKeys[i].m_Time = WordToNormalized((&firstAlphaTime)[i]);
    }
    return CreateScriptingArray (alphaKeys, numAlphaKeys, GetCommonScriptingClasses ().gradientAlphaKey);
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Gradient_Set_Custom_PropAlphaKeys(ICallType_Object_Argument self_, ICallType_Array_Argument value_)
{
    SCRIPTINGAPI_ETW_ENTRY(Gradient_Set_Custom_PropAlphaKeys)
    ScriptingObjectWithIntPtrField<Gradient> self(self_);
    UNUSED(self);
    ICallType_Array_Local value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(set_alphaKeys)
    
    Gradient::AlphaKey alphaKeys[kGradientMaxNumKeys];
    int numAlphaKeys = ConvertAlphaKeyArray (value, alphaKeys);
    if (numAlphaKeys == -1)
    return;
    self->SetAlphaKeys(&alphaKeys[0], numAlphaKeys);
    
}

#if UNITY_EDITOR
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Gradient_CUSTOM_INTERNAL_get_constantColor(ICallType_Object_Argument self_, ColorRGBAf* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Gradient_CUSTOM_INTERNAL_get_constantColor)
    ScriptingObjectWithIntPtrField<Gradient> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_constantColor)
    { *returnValue =(self->GetConstantColor ()); return;};
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Gradient_CUSTOM_INTERNAL_set_constantColor(ICallType_Object_Argument self_, const ColorRGBAf& value)
{
    SCRIPTINGAPI_ETW_ENTRY(Gradient_CUSTOM_INTERNAL_set_constantColor)
    ScriptingObjectWithIntPtrField<Gradient> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_set_constantColor)
    
    self->SetConstantColor (value);
    
}

#endif

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Gradient_CUSTOM_SetKeys(ICallType_Object_Argument self_, ICallType_Array_Argument colorKeys_, ICallType_Array_Argument alphaKeys_)
{
    SCRIPTINGAPI_ETW_ENTRY(Gradient_CUSTOM_SetKeys)
    ScriptingObjectWithIntPtrField<Gradient> self(self_);
    UNUSED(self);
    ICallType_Array_Local colorKeys(colorKeys_);
    UNUSED(colorKeys);
    ICallType_Array_Local alphaKeys(alphaKeys_);
    UNUSED(alphaKeys);
    SCRIPTINGAPI_STACK_CHECK(SetKeys)
    
    		Gradient::ColorKey colors[kGradientMaxNumKeys];
    		int numColorKeys = ConvertColorKeyArray (colorKeys, colors);
    		if (numColorKeys == -1)
    			return;
    	
    		Gradient::AlphaKey alphas[kGradientMaxNumKeys];
    		int numAlphaKeys = ConvertAlphaKeyArray (alphaKeys, alphas);
    		if (numAlphaKeys == -1)
    			return;	
    		
    		return self->SetKeys (&colors[0], numColorKeys, &alphas[0], numAlphaKeys);
    	
}

#if !defined(INTERNAL_CALL_STRIPPING)
#   error must include unityconfigure.h
#endif
#if INTERNAL_CALL_STRIPPING
void Register_UnityEngine_Gradient_Init()
{
    scripting_add_internal_call( "UnityEngine.Gradient::Init" , (gpointer)& Gradient_CUSTOM_Init );
}

void Register_UnityEngine_Gradient_Cleanup()
{
    scripting_add_internal_call( "UnityEngine.Gradient::Cleanup" , (gpointer)& Gradient_CUSTOM_Cleanup );
}

void Register_UnityEngine_Gradient_INTERNAL_CALL_Evaluate()
{
    scripting_add_internal_call( "UnityEngine.Gradient::INTERNAL_CALL_Evaluate" , (gpointer)& Gradient_CUSTOM_INTERNAL_CALL_Evaluate );
}

void Register_UnityEngine_Gradient_get_colorKeys()
{
    scripting_add_internal_call( "UnityEngine.Gradient::get_colorKeys" , (gpointer)& Gradient_Get_Custom_PropColorKeys );
}

void Register_UnityEngine_Gradient_set_colorKeys()
{
    scripting_add_internal_call( "UnityEngine.Gradient::set_colorKeys" , (gpointer)& Gradient_Set_Custom_PropColorKeys );
}

void Register_UnityEngine_Gradient_get_alphaKeys()
{
    scripting_add_internal_call( "UnityEngine.Gradient::get_alphaKeys" , (gpointer)& Gradient_Get_Custom_PropAlphaKeys );
}

void Register_UnityEngine_Gradient_set_alphaKeys()
{
    scripting_add_internal_call( "UnityEngine.Gradient::set_alphaKeys" , (gpointer)& Gradient_Set_Custom_PropAlphaKeys );
}

#if UNITY_EDITOR
void Register_UnityEngine_Gradient_INTERNAL_get_constantColor()
{
    scripting_add_internal_call( "UnityEngine.Gradient::INTERNAL_get_constantColor" , (gpointer)& Gradient_CUSTOM_INTERNAL_get_constantColor );
}

void Register_UnityEngine_Gradient_INTERNAL_set_constantColor()
{
    scripting_add_internal_call( "UnityEngine.Gradient::INTERNAL_set_constantColor" , (gpointer)& Gradient_CUSTOM_INTERNAL_set_constantColor );
}

#endif
void Register_UnityEngine_Gradient_SetKeys()
{
    scripting_add_internal_call( "UnityEngine.Gradient::SetKeys" , (gpointer)& Gradient_CUSTOM_SetKeys );
}

#elif ENABLE_MONO || ENABLE_IL2CPP
static const char* s_Gradient_IcallNames [] =
{
    "UnityEngine.Gradient::Init"            ,    // -> Gradient_CUSTOM_Init
    "UnityEngine.Gradient::Cleanup"         ,    // -> Gradient_CUSTOM_Cleanup
    "UnityEngine.Gradient::INTERNAL_CALL_Evaluate",    // -> Gradient_CUSTOM_INTERNAL_CALL_Evaluate
    "UnityEngine.Gradient::get_colorKeys"   ,    // -> Gradient_Get_Custom_PropColorKeys
    "UnityEngine.Gradient::set_colorKeys"   ,    // -> Gradient_Set_Custom_PropColorKeys
    "UnityEngine.Gradient::get_alphaKeys"   ,    // -> Gradient_Get_Custom_PropAlphaKeys
    "UnityEngine.Gradient::set_alphaKeys"   ,    // -> Gradient_Set_Custom_PropAlphaKeys
#if UNITY_EDITOR
    "UnityEngine.Gradient::INTERNAL_get_constantColor",    // -> Gradient_CUSTOM_INTERNAL_get_constantColor
    "UnityEngine.Gradient::INTERNAL_set_constantColor",    // -> Gradient_CUSTOM_INTERNAL_set_constantColor
#endif
    "UnityEngine.Gradient::SetKeys"         ,    // -> Gradient_CUSTOM_SetKeys
    NULL
};

static const void* s_Gradient_IcallFuncs [] =
{
    (const void*)&Gradient_CUSTOM_Init                    ,  //  <- UnityEngine.Gradient::Init
    (const void*)&Gradient_CUSTOM_Cleanup                 ,  //  <- UnityEngine.Gradient::Cleanup
    (const void*)&Gradient_CUSTOM_INTERNAL_CALL_Evaluate  ,  //  <- UnityEngine.Gradient::INTERNAL_CALL_Evaluate
    (const void*)&Gradient_Get_Custom_PropColorKeys       ,  //  <- UnityEngine.Gradient::get_colorKeys
    (const void*)&Gradient_Set_Custom_PropColorKeys       ,  //  <- UnityEngine.Gradient::set_colorKeys
    (const void*)&Gradient_Get_Custom_PropAlphaKeys       ,  //  <- UnityEngine.Gradient::get_alphaKeys
    (const void*)&Gradient_Set_Custom_PropAlphaKeys       ,  //  <- UnityEngine.Gradient::set_alphaKeys
#if UNITY_EDITOR
    (const void*)&Gradient_CUSTOM_INTERNAL_get_constantColor,  //  <- UnityEngine.Gradient::INTERNAL_get_constantColor
    (const void*)&Gradient_CUSTOM_INTERNAL_set_constantColor,  //  <- UnityEngine.Gradient::INTERNAL_set_constantColor
#endif
    (const void*)&Gradient_CUSTOM_SetKeys                 ,  //  <- UnityEngine.Gradient::SetKeys
    NULL
};

void ExportGradientBindings();
void ExportGradientBindings()
{
    for (int i = 0; s_Gradient_IcallNames [i] != NULL; ++i )
        scripting_add_internal_call( s_Gradient_IcallNames [i], s_Gradient_IcallFuncs [i] );
}

#elif ENABLE_DOTNET
#include "Runtime/Scripting/WinRTHelper.h"
void ExportGradientBindings()
{
    #if UNITY_WP8
    extern intptr_t g_WinRTFuncPtrs[];
    #define SET_METRO_BINDING(Name) g_WinRTFuncPtrs[k##Name##FuncDef] = reinterpret_cast<intptr_t>(Name);
    #else
    long long* p = GetWinRTFuncDefsPointers();
    #define SET_METRO_BINDING(Name) p[k##Name##Func] = (long long)Name;
    #endif
    SET_METRO_BINDING(Gradient_CUSTOM_Init); //  <- UnityEngine.Gradient::Init
    SET_METRO_BINDING(Gradient_CUSTOM_Cleanup); //  <- UnityEngine.Gradient::Cleanup
    SET_METRO_BINDING(Gradient_CUSTOM_INTERNAL_CALL_Evaluate); //  <- UnityEngine.Gradient::INTERNAL_CALL_Evaluate
    SET_METRO_BINDING(Gradient_Get_Custom_PropColorKeys); //  <- UnityEngine.Gradient::get_colorKeys
    SET_METRO_BINDING(Gradient_Set_Custom_PropColorKeys); //  <- UnityEngine.Gradient::set_colorKeys
    SET_METRO_BINDING(Gradient_Get_Custom_PropAlphaKeys); //  <- UnityEngine.Gradient::get_alphaKeys
    SET_METRO_BINDING(Gradient_Set_Custom_PropAlphaKeys); //  <- UnityEngine.Gradient::set_alphaKeys
#if UNITY_EDITOR
    SET_METRO_BINDING(Gradient_CUSTOM_INTERNAL_get_constantColor); //  <- UnityEngine.Gradient::INTERNAL_get_constantColor
    SET_METRO_BINDING(Gradient_CUSTOM_INTERNAL_set_constantColor); //  <- UnityEngine.Gradient::INTERNAL_set_constantColor
#endif
    SET_METRO_BINDING(Gradient_CUSTOM_SetKeys); //  <- UnityEngine.Gradient::SetKeys
}

#endif
