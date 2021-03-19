#include "UnityPrefix.h"
#include "Runtime/Scripting/ScriptingExportUtility.h"



#include "UnityPrefix.h"
#include "Runtime/Scripting/ScriptingUtility.h"
#include "Runtime/Mono/MonoBehaviour.h"
#include "Runtime/Mono/MonoScopedThreadAttach.h"

#if PLATFORM_ANDROID

#if UNITY_DEVELOPER_BUILD
	static volatile bool DEBUGJNI = false;
#else
	#define DEBUGJNI 0
#endif

#define DEBUGJNI_VERBOSE 0

#include "PlatformDependent/AndroidPlayer/EntryPoint.h"

	static MonoDomain* s_MonoDomain = 0;
	jobject UnityJavaProxy_invoke(JNIEnv*, jobject thiz, jint delegate, jstring method, jobjectArray args)
	{
		ScopedThreadAttach mono(s_MonoDomain);
		MonoObject* proxy = mono_gchandle_get_target(delegate);
		void* params[] = {proxy, &method, &args};
		MonoObject* mresult = CallStaticMonoMethod("_AndroidJNIHelper", "InvokeJavaProxyMethod", params);
		if (!mresult)
			return 0;
		return ExtractMonoObjectData<jobject>(mresult);
	}
	void UnityJavaProxy_finalize(JNIEnv*, jobject thiz, jint jdelegate)
	{
		ScopedThreadAttach mono(s_MonoDomain);
		mono_gchandle_free(jdelegate);
	}

#if DEBUGJNI
	#define SCOPED_JNI(x) DalvikAttachThreadScoped jni_env(x)
#else
	#define SCOPED_JNI(x) DalvikAttachThreadScoped jni_env(0)
#endif

#define JNI_GET_ID( GetIDFunc )													\
	do {																		\
		SCOPED_JNI(__FUNCTION__);												\
		if (!jni_env) return 0;													\
		string methodName = name;							\
		string signature = sig;							\
		if (DEBUGJNI)															\
			printf_console("> %s(%08x, %s, %s)", __FUNCTION__, clazz, methodName.c_str(), signature.c_str());	\
		if (clazz == 0)															\
			return 0;															\
		return jni_env->GetIDFunc((jclass)clazz, methodName.c_str(), signature.c_str());	\
	} while(0)

#define ConvertToJNIArgs() \
		int numargs = mono_array_length(args); 									\
		jvalue* jni_args = (jvalue*)alloca(sizeof(jvalue) * numargs);			\
		for (int i = 0; i < numargs; ++i)										\
		{																		\
			jni_args[i] = GetMonoArrayElement<jvalue>(args, i);					\
			if (DEBUGJNI)														\
				printf_console(">\t\t\t, %08x", jni_args[i].i);					\
		}																		\
		if (DEBUGJNI && numargs)												\
			printf_console(">\t\t\t)");

#define JNI_CALL_METHOD( MethodFunc, ClassOrObj )								\
	do {																		\
		SCOPED_JNI(__FUNCTION__);												\
		if (!jni_env) return 0;													\
		if (DEBUGJNI)															\
			printf_console("> %s(%08x, %08x%s", __FUNCTION__, ClassOrObj,		\
					(jmethodID)methodID, mono_array_length(args) ? " " : ")");	\
		if (ClassOrObj == 0 || (jmethodID)methodID == 0)						\
			return 0;															\
		ConvertToJNIArgs();														\
		return jni_env->MethodFunc(ClassOrObj, (jmethodID)methodID, jni_args);	\
	} while(0)

#define JNI_CALL_VOID_METHOD( MethodFunc, ClassOrObj )							\
	do {																		\
		SCOPED_JNI(__FUNCTION__);												\
		if (!jni_env) return;													\
		if (DEBUGJNI)															\
			printf_console("> %s(%08x, %08x%s", __FUNCTION__, ClassOrObj,		\
				(jmethodID)methodID, mono_array_length(args) ? " " : ")");		\
		if (ClassOrObj == 0 || (jmethodID)methodID == 0)						\
			return;																\
		ConvertToJNIArgs();														\
		jni_env->MethodFunc(ClassOrObj, (jmethodID)methodID, jni_args);			\
	} while(0)

#define JNI_GET_FIELD( FieldFunc, ClassOrObj )									\
	do {																		\
		SCOPED_JNI(__FUNCTION__);												\
		if (!jni_env) return 0;													\
		if (DEBUGJNI)															\
			printf_console("> %s(%08x)", __FUNCTION__, ClassOrObj);				\
		if (ClassOrObj == 0 || (jmethodID)fieldID == 0)							\
			return 0;															\
		return jni_env->FieldFunc(ClassOrObj, (jfieldID)fieldID);				\
	} while(0)

#define JNI_SET_FIELD( FieldFunc, ClassOrObj, Value )							\
	do {																		\
		SCOPED_JNI(__FUNCTION__);												\
		if (!jni_env) return;													\
		if (DEBUGJNI)															\
			printf_console("> %s(%08x)", __FUNCTION__, ClassOrObj);				\
		if (ClassOrObj == 0 || (jmethodID)fieldID == 0)							\
			return;																\
		jni_env->FieldFunc(ClassOrObj, (jfieldID)fieldID, Value);				\
	} while(0)

#define JNI_PASS_VOID_VOID( Func )												\
	do {																		\
		SCOPED_JNI(__FUNCTION__);												\
		if (!jni_env) return;													\
		if (DEBUGJNI)															\
			printf_console("> %s()", __FUNCTION__);								\
		jni_env->Func();														\
	} while(0)

#define JNI_PASS_RETV_VOID( Func )												\
	do {																		\
		SCOPED_JNI(__FUNCTION__);												\
		if (!jni_env) return 0;													\
		if (DEBUGJNI)															\
			printf_console("> %s()", __FUNCTION__);								\
		return jni_env->Func();													\
	} while(0)

#define JNI_PASS_VOID_ARGS( Func, ...)											\
	do {																		\
		SCOPED_JNI(__FUNCTION__);												\
		if (!jni_env) return;													\
		if (DEBUGJNI)															\
			printf_console("> %s()", __FUNCTION__);								\
		jni_env->Func(__VA_ARGS__);												\
	} while(0)

#define JNI_PASS_RETV_ARGS( Func, ...)											\
	do {																		\
		SCOPED_JNI(__FUNCTION__);												\
		if (!jni_env) return 0;													\
		if (DEBUGJNI)															\
			printf_console("> %s()", __FUNCTION__);								\
		return jni_env->Func(__VA_ARGS__);										\
	} while(0)

#define JNI_GET_JSTR_FIELD( FieldFunc, ClassOrObj )								\
	do {																		\
		SCOPED_JNI(__FUNCTION__);												\
		if (!jni_env) return SCRIPTING_NULL;									\
		if (DEBUGJNI)															\
			printf_console("> %s(%08x)", __FUNCTION__, ClassOrObj);				\
		if (ClassOrObj == 0 || (jmethodID)fieldID == 0)							\
			return SCRIPTING_NULL;												\
		jstring str = (jstring)jni_env->FieldFunc(ClassOrObj, (jfieldID)fieldID);	\
		if (str == 0 || jni_env->ExceptionCheck())	return SCRIPTING_NULL;		\
		const char* cstring = jni_env->GetStringUTFChars(str, 0);				\
		if (cstring == 0 || jni_env->ExceptionCheck())							\
		{																		\
			jni_env->ReleaseStringUTFChars(str, cstring);						\
			jni_env->DeleteLocalRef(str);										\
			return SCRIPTING_NULL;												\
		}																		\
		MonoString* mstring = CreateScriptingString(cstring);					\
		jni_env->ReleaseStringUTFChars(str, cstring);							\
		jni_env->DeleteLocalRef(str);											\
		return mstring;															\
	} while(0)

#define JNI_CALL_JSTR_METHOD( MethodFunc, ClassOrObj )							\
	do {																		\
		SCOPED_JNI(__FUNCTION__);												\
		if (!jni_env) return SCRIPTING_NULL;									\
		if (DEBUGJNI)															\
			printf_console("> %s(%08x, %08x%s", __FUNCTION__, ClassOrObj,		\
				(jmethodID)methodID, mono_array_length(args) ? " " : ")");		\
		if (ClassOrObj == 0 || (jmethodID)methodID == 0)						\
			return SCRIPTING_NULL;												\
		ConvertToJNIArgs();														\
		jstring str = (jstring)jni_env->MethodFunc(ClassOrObj, (jmethodID)methodID, jni_args);		\
		if (str == 0  || jni_env->ExceptionCheck())	return SCRIPTING_NULL;		\
		const char* cstring = jni_env->GetStringUTFChars(str, 0);				\
		if (cstring == 0 || jni_env->ExceptionCheck())							\
		{																		\
			jni_env->ReleaseStringUTFChars(str, cstring);						\
			jni_env->DeleteLocalRef(str);										\
			return SCRIPTING_NULL;												\
		}																		\
		MonoString* mstring = CreateScriptingString(cstring);					\
		jni_env->ReleaseStringUTFChars(str, cstring);							\
		jni_env->DeleteLocalRef(str);											\
		return mstring;															\
	} while(0)

#define JNI_NEW_ARRAY( NewArrayType, Type, ArrayType )							\
	do {																		\
		SCOPED_JNI(__FUNCTION__);												\
		if (!jni_env) return SCRIPTING_NULL;									\
		int size = mono_array_length(array);									\
    	ArrayType jni_array = jni_env->New ## NewArrayType ## Array(size);		\
    	if (jni_array == 0 || jni_env->ExceptionCheck()) return SCRIPTING_NULL;		\
    	for (int i = 0; i < size; ++i)											\
    	{																		\
			Type val = GetMonoArrayElement<Type>(array, i);						\
    		jni_env->Set ## NewArrayType ## ArrayRegion(jni_array, i, 1, &val);	\
    		if (jni_env->ExceptionCheck()) return SCRIPTING_NULL;				\
    	}																		\
    	return jni_array;														\
	} while(0)

#define JNI_GET_ARRAY( GetArrayType, Type, ArrayType, ArrayClass )				\
	do {																		\
		SCOPED_JNI(__FUNCTION__);												\
		if (!jni_env) return SCRIPTING_NULL;									\
		jsize length = jni_env->GetArrayLength((jarray)array);					\
		if (jni_env->ExceptionCheck()) return SCRIPTING_NULL;					\
		MonoArray* csarray = mono_array_new(mono_domain_get(), ArrayClass, length);		\
		Type* mem = jni_env->Get ## GetArrayType ## ArrayElements((ArrayType)array, 0);	\
		if (jni_env->ExceptionCheck()) return SCRIPTING_NULL;					\
		for (int i = 0; i < length; ++i)										\
			SetScriptingArrayElement<Type>(csarray, i, mem[i]);					\
		jni_env->Release ## GetArrayType ## ArrayElements((ArrayType)array, mem, JNI_ABORT);	\
		return csarray;															\
	} while(0)

#define JNI_GET_ARRAY_ELEMENT( GetArrayType, Type, ArrayType )					\
	do {																		\
		SCOPED_JNI(__FUNCTION__);												\
		if (!jni_env) return 0;													\
		Type val;																\
		jni_env->Get ## GetArrayType ## ArrayRegion((ArrayType)array, index, 1, &val);			\
		return val;																\
	} while(0)

#define JNI_SET_ARRAY_ELEMENT( SetArrayType, Type, ArrayType )					\
	do {																		\
		SCOPED_JNI(__FUNCTION__);												\
		if (!jni_env) return;													\
		jni_env->Get ## SetArrayType ## ArrayRegion((ArrayType)array, index, 1, &val);			\
	} while(0)

struct jStringWrapper
{
	std::string cString;
	jstring javaString;
	jStringWrapper(ICallString& cs)
	: javaString(0)
	{
		cString = cs;
		if (DEBUGJNI_VERBOSE)
			printf_console("> [%s](\"%s\")", __FUNCTION__, cString.c_str());
	}

	~jStringWrapper()
	{
		if (DEBUGJNI_VERBOSE)
			printf_console("> [%s](\"%s\")", __FUNCTION__, cString.c_str());
		if (!javaString)
			return;
		SCOPED_JNI(__FUNCTION__);
		if (!jni_env) return;
		jni_env->DeleteLocalRef(javaString);
	}

	operator jstring()
	{
		if (DEBUGJNI_VERBOSE)
			printf_console("> [%s](\"%s\")", __FUNCTION__, cString.c_str());
		SCOPED_JNI(__FUNCTION__);
		if (!jni_env) return 0;
		javaString = jni_env->NewStringUTF(cString.c_str());
		return javaString;
	}
	operator const char*()
	{
		if (DEBUGJNI_VERBOSE)
			printf_console("> [%s](\"%s\")", __FUNCTION__, cString.c_str());
		return cString.c_str();
	}
};

#else 

#define JNI_GET_ID( GetIDFunc )	do { return 0; } while(0)
#define JNI_CALL_METHOD( MethodFunc, ClassOrObj )	do { return 0; } while(0)
#define JNI_CALL_VOID_METHOD( MethodFunc, ClassOrObj )	do { ; } while(0)
#define JNI_GET_FIELD( FieldFunc, ClassOrObj )	do { return 0; } while(0)
#define JNI_GET_JSTR_FIELD( FieldFunc, ClassOrObj )	do { return SCRIPTING_NULL; } while(0)
#define JNI_SET_FIELD( FieldFunc, ClassOrObj, Value )	do { ; } while(0)
#define JNI_PASS_VOID_VOID( Func )	do { ; } while(0)
#define JNI_PASS_RETV_VOID( Func)	do { return 0; } while(0)
#define JNI_PASS_VOID_ARGS( Func, ...)	do { ; } while(0)
#define JNI_PASS_RETV_ARGS( Func, ...)	do { return 0; } while(0)
#define JNI_CALL_JSTR_METHOD( MethodFunc, ClassOrObj )	do { return SCRIPTING_NULL; } while(0)
#define JNI_NEW_ARRAY( NewArrayFunc, Type, ArrayType )	do { return SCRIPTING_NULL; } while(0)
#define JNI_GET_ARRAY( GetArrayType, Type, ArrayType, ArrayClass )	do { return SCRIPTING_NULL; } while(0)
#define JNI_GET_ARRAY_ELEMENT( GetArrayType, Type, ArrayType )	do { return 0; } while(0)
#define JNI_SET_ARRAY_ELEMENT( SetArrayType, Type, ArrayType )	do { ; } while(0)

#endif
#if !PLATFORM_WINRT
SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION AndroidJNIHelper_Get_Custom_PropDebug()
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNIHelper_Get_Custom_PropDebug)
    SCRIPTINGAPI_STACK_CHECK(get_debug)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_debug)
    #if PLATFORM_ANDROID && UNITY_DEVELOPER_BUILD
    return DEBUGJNI;
    #else
    return false;
    #endif
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AndroidJNIHelper_Set_Custom_PropDebug(ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNIHelper_Set_Custom_PropDebug)
    SCRIPTINGAPI_STACK_CHECK(set_debug)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_debug)
    
    #if PLATFORM_ANDROID && UNITY_DEVELOPER_BUILD
    DEBUGJNI = value;
    SCOPED_JNI(__FUNCTION__);
    if (!jni_env) return;
    jclass cls_up = jni_env->FindClass("com/unity3d/player/ReflectionHelper");
    if (!cls_up || jni_env->ExceptionCheck())
    return;
    jfieldID fid_get = jni_env->GetStaticFieldID(cls_up, "LOG", "Z");
    if (!fid_get || jni_env->ExceptionCheck())
    {
    jni_env->DeleteLocalRef(cls_up);
    return;
    }
    jni_env->SetStaticBooleanField(cls_up, fid_get, value);
    jni_env->DeleteLocalRef(cls_up);
    #endif
    
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AndroidJNIHelper_CUSTOM_INTERNAL_CALL_CreateJavaProxy(ICallType_Object_Argument proxy_, ICallType_IntPtr_Return& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNIHelper_CUSTOM_INTERNAL_CALL_CreateJavaProxy)
    ICallType_Object_Local proxy(proxy_);
    UNUSED(proxy);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_CreateJavaProxy)
    
    	#if PLATFORM_ANDROID
    		s_MonoDomain = mono_domain_get();
    
    		int gcHandle = mono_gchandle_new(proxy, 0);
    		void* params[] = { &gcHandle, proxy };
    		MonoException* exception;
    		MonoObject* result = CallStaticMonoMethod("_AndroidJNIHelper", "CreateJavaProxy", params, &exception);
    		if (exception)
    		{
    			mono_gchandle_free(gcHandle);
    			mono_raise_exception(exception);
    			{ returnValue =(0); return; }
    		}
    		{ returnValue =(ExtractMonoObjectData<jobject>(result)); return; }
    	#else
    		{ returnValue =(0); return; }
    	#endif
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_AttachCurrentThread()
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_AttachCurrentThread)
    SCRIPTINGAPI_STACK_CHECK(AttachCurrentThread)
    
        #if PLATFORM_ANDROID
     		JavaVM* vm = GetJavaVm();
    		Assert(vm);
    		JNIEnv* env;
    		return vm->AttachCurrentThread(&env, 0);
    	#else
    		return 0;
    	#endif
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_DetachCurrentThread()
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_DetachCurrentThread)
    SCRIPTINGAPI_STACK_CHECK(DetachCurrentThread)
    
        #if PLATFORM_ANDROID
     		JavaVM* vm = GetJavaVm();
    		Assert(vm);
    		return vm->DetachCurrentThread();
    	#else
    		return 0;
    	#endif
        
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_GetVersion()
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_GetVersion)
    SCRIPTINGAPI_STACK_CHECK(GetVersion)
     JNI_PASS_RETV_VOID( GetVersion ); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_INTERNAL_CALL_FindClass(ICallType_String_Argument name_, ICallType_IntPtr_Return& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_INTERNAL_CALL_FindClass)
    ICallType_String_Local name(name_);
    UNUSED(name);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_FindClass)
     JNI_PASS_RETV_ARGS( FindClass, jStringWrapper(name) ); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_INTERNAL_CALL_FromReflectedMethod(ICallType_IntPtr_Argument refMethod_, ICallType_IntPtr_Return& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_INTERNAL_CALL_FromReflectedMethod)
    void* refMethod(refMethod_);
    UNUSED(refMethod);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_FromReflectedMethod)
     JNI_PASS_RETV_ARGS( FromReflectedMethod, (jobject)refMethod ); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_INTERNAL_CALL_FromReflectedField(ICallType_IntPtr_Argument refField_, ICallType_IntPtr_Return& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_INTERNAL_CALL_FromReflectedField)
    void* refField(refField_);
    UNUSED(refField);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_FromReflectedField)
     JNI_PASS_RETV_ARGS( FromReflectedField, (jobject)refField ); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_INTERNAL_CALL_ToReflectedMethod(ICallType_IntPtr_Argument clazz_, ICallType_IntPtr_Argument methodID_, ScriptingBool isStatic, ICallType_IntPtr_Return& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_INTERNAL_CALL_ToReflectedMethod)
    void* clazz(clazz_);
    UNUSED(clazz);
    void* methodID(methodID_);
    UNUSED(methodID);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_ToReflectedMethod)
     JNI_PASS_RETV_ARGS( ToReflectedMethod, (jclass)clazz, (jmethodID)methodID, isStatic ); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_INTERNAL_CALL_ToReflectedField(ICallType_IntPtr_Argument clazz_, ICallType_IntPtr_Argument fieldID_, ScriptingBool isStatic, ICallType_IntPtr_Return& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_INTERNAL_CALL_ToReflectedField)
    void* clazz(clazz_);
    UNUSED(clazz);
    void* fieldID(fieldID_);
    UNUSED(fieldID);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_ToReflectedField)
     JNI_PASS_RETV_ARGS( ToReflectedField, (jclass)clazz, (jfieldID)fieldID, isStatic ); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_INTERNAL_CALL_GetSuperclass(ICallType_IntPtr_Argument clazz_, ICallType_IntPtr_Return& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_INTERNAL_CALL_GetSuperclass)
    void* clazz(clazz_);
    UNUSED(clazz);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_GetSuperclass)
     JNI_PASS_RETV_ARGS( GetSuperclass, (jclass)clazz ); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_IsAssignableFrom(ICallType_IntPtr_Argument clazz1_, ICallType_IntPtr_Argument clazz2_)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_IsAssignableFrom)
    void* clazz1(clazz1_);
    UNUSED(clazz1);
    void* clazz2(clazz2_);
    UNUSED(clazz2);
    SCRIPTINGAPI_STACK_CHECK(IsAssignableFrom)
     JNI_PASS_RETV_ARGS( IsAssignableFrom, (jclass)clazz1, (jclass)clazz2 ); 
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_Throw(ICallType_IntPtr_Argument obj_)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_Throw)
    void* obj(obj_);
    UNUSED(obj);
    SCRIPTINGAPI_STACK_CHECK(Throw)
     JNI_PASS_RETV_ARGS( Throw, (jthrowable)obj ); 
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_ThrowNew(ICallType_IntPtr_Argument clazz_, ICallType_String_Argument message_)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_ThrowNew)
    void* clazz(clazz_);
    UNUSED(clazz);
    ICallType_String_Local message(message_);
    UNUSED(message);
    SCRIPTINGAPI_STACK_CHECK(ThrowNew)
     JNI_PASS_RETV_ARGS( ThrowNew, (jclass)clazz, jStringWrapper(message) ); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_INTERNAL_CALL_ExceptionOccurred(ICallType_IntPtr_Return& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_INTERNAL_CALL_ExceptionOccurred)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_ExceptionOccurred)
     JNI_PASS_RETV_VOID( ExceptionOccurred ); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_ExceptionDescribe()
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_ExceptionDescribe)
    SCRIPTINGAPI_STACK_CHECK(ExceptionDescribe)
     JNI_PASS_VOID_VOID( ExceptionDescribe ); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_ExceptionClear()
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_ExceptionClear)
    SCRIPTINGAPI_STACK_CHECK(ExceptionClear)
     JNI_PASS_VOID_VOID( ExceptionClear ); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_FatalError(ICallType_String_Argument message_)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_FatalError)
    ICallType_String_Local message(message_);
    UNUSED(message);
    SCRIPTINGAPI_STACK_CHECK(FatalError)
     JNI_PASS_VOID_ARGS( FatalError, jStringWrapper(message) ); 
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_PushLocalFrame(int capacity)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_PushLocalFrame)
    SCRIPTINGAPI_STACK_CHECK(PushLocalFrame)
     JNI_PASS_RETV_ARGS( PushLocalFrame, capacity ); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_INTERNAL_CALL_PopLocalFrame(ICallType_IntPtr_Argument result_, ICallType_IntPtr_Return& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_INTERNAL_CALL_PopLocalFrame)
    void* result(result_);
    UNUSED(result);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_PopLocalFrame)
     JNI_PASS_RETV_ARGS( PopLocalFrame, (jobject)result ); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_INTERNAL_CALL_NewGlobalRef(ICallType_IntPtr_Argument obj_, ICallType_IntPtr_Return& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_INTERNAL_CALL_NewGlobalRef)
    void* obj(obj_);
    UNUSED(obj);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_NewGlobalRef)
     JNI_PASS_RETV_ARGS( NewGlobalRef, (jobject)obj ); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_DeleteGlobalRef(ICallType_IntPtr_Argument obj_)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_DeleteGlobalRef)
    void* obj(obj_);
    UNUSED(obj);
    SCRIPTINGAPI_STACK_CHECK(DeleteGlobalRef)
     JNI_PASS_VOID_ARGS( DeleteGlobalRef, (jobject)obj ); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_INTERNAL_CALL_NewLocalRef(ICallType_IntPtr_Argument obj_, ICallType_IntPtr_Return& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_INTERNAL_CALL_NewLocalRef)
    void* obj(obj_);
    UNUSED(obj);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_NewLocalRef)
     JNI_PASS_RETV_ARGS( NewLocalRef, (jobject)obj ); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_DeleteLocalRef(ICallType_IntPtr_Argument obj_)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_DeleteLocalRef)
    void* obj(obj_);
    UNUSED(obj);
    SCRIPTINGAPI_STACK_CHECK(DeleteLocalRef)
     JNI_PASS_VOID_ARGS( DeleteLocalRef, (jobject)obj ); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_IsSameObject(ICallType_IntPtr_Argument obj1_, ICallType_IntPtr_Argument obj2_)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_IsSameObject)
    void* obj1(obj1_);
    UNUSED(obj1);
    void* obj2(obj2_);
    UNUSED(obj2);
    SCRIPTINGAPI_STACK_CHECK(IsSameObject)
     JNI_PASS_RETV_ARGS( IsSameObject, (jobject)obj1, (jobject)obj2 ); 
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_EnsureLocalCapacity(int capacity)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_EnsureLocalCapacity)
    SCRIPTINGAPI_STACK_CHECK(EnsureLocalCapacity)
     JNI_PASS_RETV_ARGS( EnsureLocalCapacity, capacity ); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_INTERNAL_CALL_AllocObject(ICallType_IntPtr_Argument clazz_, ICallType_IntPtr_Return& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_INTERNAL_CALL_AllocObject)
    void* clazz(clazz_);
    UNUSED(clazz);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_AllocObject)
     JNI_PASS_RETV_ARGS( AllocObject, (jclass)clazz ); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_INTERNAL_CALL_NewObject(ICallType_IntPtr_Argument clazz_, ICallType_IntPtr_Argument methodID_, ICallType_Array_Argument args_, ICallType_IntPtr_Return& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_INTERNAL_CALL_NewObject)
    void* clazz(clazz_);
    UNUSED(clazz);
    void* methodID(methodID_);
    UNUSED(methodID);
    ICallType_Array_Local args(args_);
    UNUSED(args);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_NewObject)
    		JNI_CALL_METHOD( NewObjectA, (jclass)clazz );			
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_INTERNAL_CALL_GetObjectClass(ICallType_IntPtr_Argument obj_, ICallType_IntPtr_Return& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_INTERNAL_CALL_GetObjectClass)
    void* obj(obj_);
    UNUSED(obj);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_GetObjectClass)
     JNI_PASS_RETV_ARGS( GetObjectClass, (jobject)obj ); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_IsInstanceOf(ICallType_IntPtr_Argument obj_, ICallType_IntPtr_Argument clazz_)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_IsInstanceOf)
    void* obj(obj_);
    UNUSED(obj);
    void* clazz(clazz_);
    UNUSED(clazz);
    SCRIPTINGAPI_STACK_CHECK(IsInstanceOf)
     JNI_PASS_RETV_ARGS( IsInstanceOf, (jobject)obj, (jclass)clazz ); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_INTERNAL_CALL_GetMethodID(ICallType_IntPtr_Argument clazz_, ICallType_String_Argument name_, ICallType_String_Argument sig_, ICallType_IntPtr_Return& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_INTERNAL_CALL_GetMethodID)
    void* clazz(clazz_);
    UNUSED(clazz);
    ICallType_String_Local name(name_);
    UNUSED(name);
    ICallType_String_Local sig(sig_);
    UNUSED(sig);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_GetMethodID)
    			JNI_GET_ID(GetMethodID);					
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_INTERNAL_CALL_GetFieldID(ICallType_IntPtr_Argument clazz_, ICallType_String_Argument name_, ICallType_String_Argument sig_, ICallType_IntPtr_Return& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_INTERNAL_CALL_GetFieldID)
    void* clazz(clazz_);
    UNUSED(clazz);
    ICallType_String_Local name(name_);
    UNUSED(name);
    ICallType_String_Local sig(sig_);
    UNUSED(sig);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_GetFieldID)
    			JNI_GET_ID(GetFieldID);						
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_INTERNAL_CALL_GetStaticMethodID(ICallType_IntPtr_Argument clazz_, ICallType_String_Argument name_, ICallType_String_Argument sig_, ICallType_IntPtr_Return& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_INTERNAL_CALL_GetStaticMethodID)
    void* clazz(clazz_);
    UNUSED(clazz);
    ICallType_String_Local name(name_);
    UNUSED(name);
    ICallType_String_Local sig(sig_);
    UNUSED(sig);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_GetStaticMethodID)
    			JNI_GET_ID(GetStaticMethodID);					
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_INTERNAL_CALL_GetStaticFieldID(ICallType_IntPtr_Argument clazz_, ICallType_String_Argument name_, ICallType_String_Argument sig_, ICallType_IntPtr_Return& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_INTERNAL_CALL_GetStaticFieldID)
    void* clazz(clazz_);
    UNUSED(clazz);
    ICallType_String_Local name(name_);
    UNUSED(name);
    ICallType_String_Local sig(sig_);
    UNUSED(sig);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_GetStaticFieldID)
    			JNI_GET_ID(GetStaticFieldID);					
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_INTERNAL_CALL_NewStringUTF(ICallType_String_Argument bytes_, ICallType_IntPtr_Return& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_INTERNAL_CALL_NewStringUTF)
    ICallType_String_Local bytes(bytes_);
    UNUSED(bytes);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_NewStringUTF)
     JNI_PASS_RETV_ARGS( NewStringUTF, jStringWrapper(bytes) ); 
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_GetStringUTFLength(ICallType_IntPtr_Argument str_)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_GetStringUTFLength)
    void* str(str_);
    UNUSED(str);
    SCRIPTINGAPI_STACK_CHECK(GetStringUTFLength)
     JNI_PASS_RETV_ARGS( GetStringUTFLength, (jstring) str); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_GetStringUTFChars(ICallType_IntPtr_Argument str_)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_GetStringUTFChars)
    void* str(str_);
    UNUSED(str);
    SCRIPTINGAPI_STACK_CHECK(GetStringUTFChars)
    
        #if PLATFORM_ANDROID
    		SCOPED_JNI(__FUNCTION__);
    		if (!jni_env) return 0;
    		if (DEBUGJNI)
    			printf_console("> %s()", __FUNCTION__);
    		const char* cstring = jni_env->GetStringUTFChars((jstring)str, 0);
    		if (cstring == 0 || jni_env->ExceptionCheck())
    		{
    			jni_env->ReleaseStringUTFChars((jstring)str, cstring);
    			return 0;
    		}
    		MonoString* mstring = CreateScriptingString(cstring);
    		jni_env->ReleaseStringUTFChars((jstring)str, cstring);
    		return mstring;
    	#else
    		return SCRIPTING_NULL;
    	#endif
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_CallStringMethod(ICallType_IntPtr_Argument obj_, ICallType_IntPtr_Argument methodID_, ICallType_Array_Argument args_)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_CallStringMethod)
    void* obj(obj_);
    UNUSED(obj);
    void* methodID(methodID_);
    UNUSED(methodID);
    ICallType_Array_Local args(args_);
    UNUSED(args);
    SCRIPTINGAPI_STACK_CHECK(CallStringMethod)
     JNI_CALL_JSTR_METHOD(CallObjectMethodA, (jobject)obj);		
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_INTERNAL_CALL_CallObjectMethod(ICallType_IntPtr_Argument obj_, ICallType_IntPtr_Argument methodID_, ICallType_Array_Argument args_, ICallType_IntPtr_Return& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_INTERNAL_CALL_CallObjectMethod)
    void* obj(obj_);
    UNUSED(obj);
    void* methodID(methodID_);
    UNUSED(methodID);
    ICallType_Array_Local args(args_);
    UNUSED(args);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_CallObjectMethod)
    		JNI_CALL_METHOD( CallObjectMethodA, (jobject)obj );		
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_CallIntMethod(ICallType_IntPtr_Argument obj_, ICallType_IntPtr_Argument methodID_, ICallType_Array_Argument args_)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_CallIntMethod)
    void* obj(obj_);
    UNUSED(obj);
    void* methodID(methodID_);
    UNUSED(methodID);
    ICallType_Array_Local args(args_);
    UNUSED(args);
    SCRIPTINGAPI_STACK_CHECK(CallIntMethod)
    			JNI_CALL_METHOD( CallIntMethodA, (jobject)obj );		
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_CallBooleanMethod(ICallType_IntPtr_Argument obj_, ICallType_IntPtr_Argument methodID_, ICallType_Array_Argument args_)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_CallBooleanMethod)
    void* obj(obj_);
    UNUSED(obj);
    void* methodID(methodID_);
    UNUSED(methodID);
    ICallType_Array_Local args(args_);
    UNUSED(args);
    SCRIPTINGAPI_STACK_CHECK(CallBooleanMethod)
    		JNI_CALL_METHOD( CallBooleanMethodA, (jobject)obj );	
}

SCRIPT_BINDINGS_EXPORT_DECL
short SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_CallShortMethod(ICallType_IntPtr_Argument obj_, ICallType_IntPtr_Argument methodID_, ICallType_Array_Argument args_)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_CallShortMethod)
    void* obj(obj_);
    UNUSED(obj);
    void* methodID(methodID_);
    UNUSED(methodID);
    ICallType_Array_Local args(args_);
    UNUSED(args);
    SCRIPTINGAPI_STACK_CHECK(CallShortMethod)
    		JNI_CALL_METHOD( CallShortMethodA, (jobject)obj );		
}

SCRIPT_BINDINGS_EXPORT_DECL
UInt8 SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_CallByteMethod(ICallType_IntPtr_Argument obj_, ICallType_IntPtr_Argument methodID_, ICallType_Array_Argument args_)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_CallByteMethod)
    void* obj(obj_);
    UNUSED(obj);
    void* methodID(methodID_);
    UNUSED(methodID);
    ICallType_Array_Local args(args_);
    UNUSED(args);
    SCRIPTINGAPI_STACK_CHECK(CallByteMethod)
    			JNI_CALL_METHOD( CallByteMethodA, (jobject)obj );		
}

SCRIPT_BINDINGS_EXPORT_DECL
UInt16 SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_CallCharMethod(ICallType_IntPtr_Argument obj_, ICallType_IntPtr_Argument methodID_, ICallType_Array_Argument args_)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_CallCharMethod)
    void* obj(obj_);
    UNUSED(obj);
    void* methodID(methodID_);
    UNUSED(methodID);
    ICallType_Array_Local args(args_);
    UNUSED(args);
    SCRIPTINGAPI_STACK_CHECK(CallCharMethod)
    			JNI_CALL_METHOD( CallCharMethodA, (jobject)obj );		
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_CallFloatMethod(ICallType_IntPtr_Argument obj_, ICallType_IntPtr_Argument methodID_, ICallType_Array_Argument args_)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_CallFloatMethod)
    void* obj(obj_);
    UNUSED(obj);
    void* methodID(methodID_);
    UNUSED(methodID);
    ICallType_Array_Local args(args_);
    UNUSED(args);
    SCRIPTINGAPI_STACK_CHECK(CallFloatMethod)
    		JNI_CALL_METHOD( CallFloatMethodA, (jobject)obj );		
}

SCRIPT_BINDINGS_EXPORT_DECL
double SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_CallDoubleMethod(ICallType_IntPtr_Argument obj_, ICallType_IntPtr_Argument methodID_, ICallType_Array_Argument args_)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_CallDoubleMethod)
    void* obj(obj_);
    UNUSED(obj);
    void* methodID(methodID_);
    UNUSED(methodID);
    ICallType_Array_Local args(args_);
    UNUSED(args);
    SCRIPTINGAPI_STACK_CHECK(CallDoubleMethod)
    		JNI_CALL_METHOD( CallDoubleMethodA, (jobject)obj );		
}

SCRIPT_BINDINGS_EXPORT_DECL
SInt64 SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_CallLongMethod(ICallType_IntPtr_Argument obj_, ICallType_IntPtr_Argument methodID_, ICallType_Array_Argument args_)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_CallLongMethod)
    void* obj(obj_);
    UNUSED(obj);
    void* methodID(methodID_);
    UNUSED(methodID);
    ICallType_Array_Local args(args_);
    UNUSED(args);
    SCRIPTINGAPI_STACK_CHECK(CallLongMethod)
    		JNI_CALL_METHOD( CallLongMethodA, (jobject)obj );		
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_CallVoidMethod(ICallType_IntPtr_Argument obj_, ICallType_IntPtr_Argument methodID_, ICallType_Array_Argument args_)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_CallVoidMethod)
    void* obj(obj_);
    UNUSED(obj);
    void* methodID(methodID_);
    UNUSED(methodID);
    ICallType_Array_Local args(args_);
    UNUSED(args);
    SCRIPTINGAPI_STACK_CHECK(CallVoidMethod)
    			JNI_CALL_VOID_METHOD( CallVoidMethodA, (jobject)obj );	
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_GetStringField(ICallType_IntPtr_Argument obj_, ICallType_IntPtr_Argument fieldID_)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_GetStringField)
    void* obj(obj_);
    UNUSED(obj);
    void* fieldID(fieldID_);
    UNUSED(fieldID);
    SCRIPTINGAPI_STACK_CHECK(GetStringField)
     JNI_GET_JSTR_FIELD( GetObjectField, (jobject)obj ); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_INTERNAL_CALL_GetObjectField(ICallType_IntPtr_Argument obj_, ICallType_IntPtr_Argument fieldID_, ICallType_IntPtr_Return& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_INTERNAL_CALL_GetObjectField)
    void* obj(obj_);
    UNUSED(obj);
    void* fieldID(fieldID_);
    UNUSED(fieldID);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_GetObjectField)
    	JNI_GET_FIELD( GetObjectField, (jobject)obj ); 	
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_GetBooleanField(ICallType_IntPtr_Argument obj_, ICallType_IntPtr_Argument fieldID_)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_GetBooleanField)
    void* obj(obj_);
    UNUSED(obj);
    void* fieldID(fieldID_);
    UNUSED(fieldID);
    SCRIPTINGAPI_STACK_CHECK(GetBooleanField)
    	JNI_GET_FIELD( GetBooleanField, (jobject)obj );	
}

SCRIPT_BINDINGS_EXPORT_DECL
UInt8 SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_GetByteField(ICallType_IntPtr_Argument obj_, ICallType_IntPtr_Argument fieldID_)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_GetByteField)
    void* obj(obj_);
    UNUSED(obj);
    void* fieldID(fieldID_);
    UNUSED(fieldID);
    SCRIPTINGAPI_STACK_CHECK(GetByteField)
    		JNI_GET_FIELD( GetByteField, (jobject)obj ); 	
}

SCRIPT_BINDINGS_EXPORT_DECL
UInt16 SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_GetCharField(ICallType_IntPtr_Argument obj_, ICallType_IntPtr_Argument fieldID_)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_GetCharField)
    void* obj(obj_);
    UNUSED(obj);
    void* fieldID(fieldID_);
    UNUSED(fieldID);
    SCRIPTINGAPI_STACK_CHECK(GetCharField)
    		JNI_GET_FIELD( GetCharField, (jobject)obj ); 	
}

SCRIPT_BINDINGS_EXPORT_DECL
short SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_GetShortField(ICallType_IntPtr_Argument obj_, ICallType_IntPtr_Argument fieldID_)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_GetShortField)
    void* obj(obj_);
    UNUSED(obj);
    void* fieldID(fieldID_);
    UNUSED(fieldID);
    SCRIPTINGAPI_STACK_CHECK(GetShortField)
    		JNI_GET_FIELD( GetShortField, (jobject)obj ); 	
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_GetIntField(ICallType_IntPtr_Argument obj_, ICallType_IntPtr_Argument fieldID_)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_GetIntField)
    void* obj(obj_);
    UNUSED(obj);
    void* fieldID(fieldID_);
    UNUSED(fieldID);
    SCRIPTINGAPI_STACK_CHECK(GetIntField)
    		JNI_GET_FIELD( GetIntField, (jobject)obj ); 	
}

SCRIPT_BINDINGS_EXPORT_DECL
SInt64 SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_GetLongField(ICallType_IntPtr_Argument obj_, ICallType_IntPtr_Argument fieldID_)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_GetLongField)
    void* obj(obj_);
    UNUSED(obj);
    void* fieldID(fieldID_);
    UNUSED(fieldID);
    SCRIPTINGAPI_STACK_CHECK(GetLongField)
    		JNI_GET_FIELD( GetLongField, (jobject)obj ); 	
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_GetFloatField(ICallType_IntPtr_Argument obj_, ICallType_IntPtr_Argument fieldID_)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_GetFloatField)
    void* obj(obj_);
    UNUSED(obj);
    void* fieldID(fieldID_);
    UNUSED(fieldID);
    SCRIPTINGAPI_STACK_CHECK(GetFloatField)
    		JNI_GET_FIELD( GetFloatField, (jobject)obj ); 	
}

SCRIPT_BINDINGS_EXPORT_DECL
double SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_GetDoubleField(ICallType_IntPtr_Argument obj_, ICallType_IntPtr_Argument fieldID_)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_GetDoubleField)
    void* obj(obj_);
    UNUSED(obj);
    void* fieldID(fieldID_);
    UNUSED(fieldID);
    SCRIPTINGAPI_STACK_CHECK(GetDoubleField)
    	JNI_GET_FIELD( GetDoubleField, (jobject)obj );	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_SetStringField(ICallType_IntPtr_Argument obj_, ICallType_IntPtr_Argument fieldID_, ICallType_String_Argument val_)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_SetStringField)
    void* obj(obj_);
    UNUSED(obj);
    void* fieldID(fieldID_);
    UNUSED(fieldID);
    ICallType_String_Local val(val_);
    UNUSED(val);
    SCRIPTINGAPI_STACK_CHECK(SetStringField)
     JNI_PASS_VOID_ARGS( SetObjectField, (jobject)obj, (jfieldID)fieldID, jStringWrapper(val) ); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_SetObjectField(ICallType_IntPtr_Argument obj_, ICallType_IntPtr_Argument fieldID_, ICallType_IntPtr_Argument val_)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_SetObjectField)
    void* obj(obj_);
    UNUSED(obj);
    void* fieldID(fieldID_);
    UNUSED(fieldID);
    void* val(val_);
    UNUSED(val);
    SCRIPTINGAPI_STACK_CHECK(SetObjectField)
    		JNI_SET_FIELD( SetObjectField, (jobject)obj, (jobject)val );	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_SetBooleanField(ICallType_IntPtr_Argument obj_, ICallType_IntPtr_Argument fieldID_, ScriptingBool val)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_SetBooleanField)
    void* obj(obj_);
    UNUSED(obj);
    void* fieldID(fieldID_);
    UNUSED(fieldID);
    SCRIPTINGAPI_STACK_CHECK(SetBooleanField)
    		JNI_SET_FIELD( SetBooleanField, (jobject)obj, val ); 			
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_SetByteField(ICallType_IntPtr_Argument obj_, ICallType_IntPtr_Argument fieldID_, UInt8 val)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_SetByteField)
    void* obj(obj_);
    UNUSED(obj);
    void* fieldID(fieldID_);
    UNUSED(fieldID);
    SCRIPTINGAPI_STACK_CHECK(SetByteField)
    			JNI_SET_FIELD( SetByteField, (jobject)obj, val ); 				
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_SetCharField(ICallType_IntPtr_Argument obj_, ICallType_IntPtr_Argument fieldID_, UInt16 val)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_SetCharField)
    void* obj(obj_);
    UNUSED(obj);
    void* fieldID(fieldID_);
    UNUSED(fieldID);
    SCRIPTINGAPI_STACK_CHECK(SetCharField)
    			JNI_SET_FIELD( SetCharField, (jobject)obj, val ); 				
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_SetShortField(ICallType_IntPtr_Argument obj_, ICallType_IntPtr_Argument fieldID_, short val)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_SetShortField)
    void* obj(obj_);
    UNUSED(obj);
    void* fieldID(fieldID_);
    UNUSED(fieldID);
    SCRIPTINGAPI_STACK_CHECK(SetShortField)
    		JNI_SET_FIELD( SetShortField, (jobject)obj, val ); 				
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_SetIntField(ICallType_IntPtr_Argument obj_, ICallType_IntPtr_Argument fieldID_, int val)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_SetIntField)
    void* obj(obj_);
    UNUSED(obj);
    void* fieldID(fieldID_);
    UNUSED(fieldID);
    SCRIPTINGAPI_STACK_CHECK(SetIntField)
    			JNI_SET_FIELD( SetIntField, (jobject)obj, val ); 				
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_SetLongField(ICallType_IntPtr_Argument obj_, ICallType_IntPtr_Argument fieldID_, SInt64 val)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_SetLongField)
    void* obj(obj_);
    UNUSED(obj);
    void* fieldID(fieldID_);
    UNUSED(fieldID);
    SCRIPTINGAPI_STACK_CHECK(SetLongField)
    		JNI_SET_FIELD( SetLongField, (jobject)obj, val ); 				
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_SetFloatField(ICallType_IntPtr_Argument obj_, ICallType_IntPtr_Argument fieldID_, float val)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_SetFloatField)
    void* obj(obj_);
    UNUSED(obj);
    void* fieldID(fieldID_);
    UNUSED(fieldID);
    SCRIPTINGAPI_STACK_CHECK(SetFloatField)
    		JNI_SET_FIELD( SetFloatField, (jobject)obj, val ); 				
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_SetDoubleField(ICallType_IntPtr_Argument obj_, ICallType_IntPtr_Argument fieldID_, double val)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_SetDoubleField)
    void* obj(obj_);
    UNUSED(obj);
    void* fieldID(fieldID_);
    UNUSED(fieldID);
    SCRIPTINGAPI_STACK_CHECK(SetDoubleField)
    		JNI_SET_FIELD( SetDoubleField, (jobject)obj, val ); 			
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_CallStaticStringMethod(ICallType_IntPtr_Argument clazz_, ICallType_IntPtr_Argument methodID_, ICallType_Array_Argument args_)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_CallStaticStringMethod)
    void* clazz(clazz_);
    UNUSED(clazz);
    void* methodID(methodID_);
    UNUSED(methodID);
    ICallType_Array_Local args(args_);
    UNUSED(args);
    SCRIPTINGAPI_STACK_CHECK(CallStaticStringMethod)
     JNI_CALL_JSTR_METHOD(CallStaticObjectMethodA, (jclass)clazz);		
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_INTERNAL_CALL_CallStaticObjectMethod(ICallType_IntPtr_Argument clazz_, ICallType_IntPtr_Argument methodID_, ICallType_Array_Argument args_, ICallType_IntPtr_Return& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_INTERNAL_CALL_CallStaticObjectMethod)
    void* clazz(clazz_);
    UNUSED(clazz);
    void* methodID(methodID_);
    UNUSED(methodID);
    ICallType_Array_Local args(args_);
    UNUSED(args);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_CallStaticObjectMethod)
    		JNI_CALL_METHOD( CallStaticObjectMethodA, (jclass)clazz );		
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_CallStaticIntMethod(ICallType_IntPtr_Argument clazz_, ICallType_IntPtr_Argument methodID_, ICallType_Array_Argument args_)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_CallStaticIntMethod)
    void* clazz(clazz_);
    UNUSED(clazz);
    void* methodID(methodID_);
    UNUSED(methodID);
    ICallType_Array_Local args(args_);
    UNUSED(args);
    SCRIPTINGAPI_STACK_CHECK(CallStaticIntMethod)
    			JNI_CALL_METHOD( CallStaticIntMethodA, (jclass)clazz );		
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_CallStaticBooleanMethod(ICallType_IntPtr_Argument clazz_, ICallType_IntPtr_Argument methodID_, ICallType_Array_Argument args_)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_CallStaticBooleanMethod)
    void* clazz(clazz_);
    UNUSED(clazz);
    void* methodID(methodID_);
    UNUSED(methodID);
    ICallType_Array_Local args(args_);
    UNUSED(args);
    SCRIPTINGAPI_STACK_CHECK(CallStaticBooleanMethod)
    		JNI_CALL_METHOD( CallStaticBooleanMethodA, (jclass)clazz );	
}

SCRIPT_BINDINGS_EXPORT_DECL
short SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_CallStaticShortMethod(ICallType_IntPtr_Argument clazz_, ICallType_IntPtr_Argument methodID_, ICallType_Array_Argument args_)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_CallStaticShortMethod)
    void* clazz(clazz_);
    UNUSED(clazz);
    void* methodID(methodID_);
    UNUSED(methodID);
    ICallType_Array_Local args(args_);
    UNUSED(args);
    SCRIPTINGAPI_STACK_CHECK(CallStaticShortMethod)
    		JNI_CALL_METHOD( CallStaticShortMethodA, (jclass)clazz );		
}

SCRIPT_BINDINGS_EXPORT_DECL
UInt8 SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_CallStaticByteMethod(ICallType_IntPtr_Argument clazz_, ICallType_IntPtr_Argument methodID_, ICallType_Array_Argument args_)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_CallStaticByteMethod)
    void* clazz(clazz_);
    UNUSED(clazz);
    void* methodID(methodID_);
    UNUSED(methodID);
    ICallType_Array_Local args(args_);
    UNUSED(args);
    SCRIPTINGAPI_STACK_CHECK(CallStaticByteMethod)
    			JNI_CALL_METHOD( CallStaticByteMethodA, (jclass)clazz );		
}

SCRIPT_BINDINGS_EXPORT_DECL
UInt16 SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_CallStaticCharMethod(ICallType_IntPtr_Argument clazz_, ICallType_IntPtr_Argument methodID_, ICallType_Array_Argument args_)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_CallStaticCharMethod)
    void* clazz(clazz_);
    UNUSED(clazz);
    void* methodID(methodID_);
    UNUSED(methodID);
    ICallType_Array_Local args(args_);
    UNUSED(args);
    SCRIPTINGAPI_STACK_CHECK(CallStaticCharMethod)
    			JNI_CALL_METHOD( CallStaticCharMethodA, (jclass)clazz );		
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_CallStaticFloatMethod(ICallType_IntPtr_Argument clazz_, ICallType_IntPtr_Argument methodID_, ICallType_Array_Argument args_)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_CallStaticFloatMethod)
    void* clazz(clazz_);
    UNUSED(clazz);
    void* methodID(methodID_);
    UNUSED(methodID);
    ICallType_Array_Local args(args_);
    UNUSED(args);
    SCRIPTINGAPI_STACK_CHECK(CallStaticFloatMethod)
    		JNI_CALL_METHOD( CallStaticFloatMethodA, (jclass)clazz );		
}

SCRIPT_BINDINGS_EXPORT_DECL
double SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_CallStaticDoubleMethod(ICallType_IntPtr_Argument clazz_, ICallType_IntPtr_Argument methodID_, ICallType_Array_Argument args_)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_CallStaticDoubleMethod)
    void* clazz(clazz_);
    UNUSED(clazz);
    void* methodID(methodID_);
    UNUSED(methodID);
    ICallType_Array_Local args(args_);
    UNUSED(args);
    SCRIPTINGAPI_STACK_CHECK(CallStaticDoubleMethod)
    		JNI_CALL_METHOD( CallStaticDoubleMethodA, (jclass)clazz );		
}

SCRIPT_BINDINGS_EXPORT_DECL
SInt64 SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_CallStaticLongMethod(ICallType_IntPtr_Argument clazz_, ICallType_IntPtr_Argument methodID_, ICallType_Array_Argument args_)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_CallStaticLongMethod)
    void* clazz(clazz_);
    UNUSED(clazz);
    void* methodID(methodID_);
    UNUSED(methodID);
    ICallType_Array_Local args(args_);
    UNUSED(args);
    SCRIPTINGAPI_STACK_CHECK(CallStaticLongMethod)
    		JNI_CALL_METHOD( CallStaticLongMethodA, (jclass)clazz );		
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_CallStaticVoidMethod(ICallType_IntPtr_Argument clazz_, ICallType_IntPtr_Argument methodID_, ICallType_Array_Argument args_)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_CallStaticVoidMethod)
    void* clazz(clazz_);
    UNUSED(clazz);
    void* methodID(methodID_);
    UNUSED(methodID);
    ICallType_Array_Local args(args_);
    UNUSED(args);
    SCRIPTINGAPI_STACK_CHECK(CallStaticVoidMethod)
    			JNI_CALL_VOID_METHOD( CallStaticVoidMethodA, (jclass)clazz );	
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_GetStaticStringField(ICallType_IntPtr_Argument clazz_, ICallType_IntPtr_Argument fieldID_)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_GetStaticStringField)
    void* clazz(clazz_);
    UNUSED(clazz);
    void* fieldID(fieldID_);
    UNUSED(fieldID);
    SCRIPTINGAPI_STACK_CHECK(GetStaticStringField)
     JNI_GET_JSTR_FIELD( GetStaticObjectField, (jclass)clazz ); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_INTERNAL_CALL_GetStaticObjectField(ICallType_IntPtr_Argument clazz_, ICallType_IntPtr_Argument fieldID_, ICallType_IntPtr_Return& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_INTERNAL_CALL_GetStaticObjectField)
    void* clazz(clazz_);
    UNUSED(clazz);
    void* fieldID(fieldID_);
    UNUSED(fieldID);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_GetStaticObjectField)
    	JNI_GET_FIELD( GetStaticObjectField, (jclass)clazz ); 	
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_GetStaticBooleanField(ICallType_IntPtr_Argument clazz_, ICallType_IntPtr_Argument fieldID_)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_GetStaticBooleanField)
    void* clazz(clazz_);
    UNUSED(clazz);
    void* fieldID(fieldID_);
    UNUSED(fieldID);
    SCRIPTINGAPI_STACK_CHECK(GetStaticBooleanField)
    	JNI_GET_FIELD( GetStaticBooleanField, (jclass)clazz );	
}

SCRIPT_BINDINGS_EXPORT_DECL
UInt8 SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_GetStaticByteField(ICallType_IntPtr_Argument clazz_, ICallType_IntPtr_Argument fieldID_)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_GetStaticByteField)
    void* clazz(clazz_);
    UNUSED(clazz);
    void* fieldID(fieldID_);
    UNUSED(fieldID);
    SCRIPTINGAPI_STACK_CHECK(GetStaticByteField)
    		JNI_GET_FIELD( GetStaticByteField, (jclass)clazz ); 	
}

SCRIPT_BINDINGS_EXPORT_DECL
UInt16 SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_GetStaticCharField(ICallType_IntPtr_Argument clazz_, ICallType_IntPtr_Argument fieldID_)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_GetStaticCharField)
    void* clazz(clazz_);
    UNUSED(clazz);
    void* fieldID(fieldID_);
    UNUSED(fieldID);
    SCRIPTINGAPI_STACK_CHECK(GetStaticCharField)
    		JNI_GET_FIELD( GetStaticCharField, (jclass)clazz ); 	
}

SCRIPT_BINDINGS_EXPORT_DECL
short SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_GetStaticShortField(ICallType_IntPtr_Argument clazz_, ICallType_IntPtr_Argument fieldID_)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_GetStaticShortField)
    void* clazz(clazz_);
    UNUSED(clazz);
    void* fieldID(fieldID_);
    UNUSED(fieldID);
    SCRIPTINGAPI_STACK_CHECK(GetStaticShortField)
    		JNI_GET_FIELD( GetStaticShortField, (jclass)clazz ); 	
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_GetStaticIntField(ICallType_IntPtr_Argument clazz_, ICallType_IntPtr_Argument fieldID_)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_GetStaticIntField)
    void* clazz(clazz_);
    UNUSED(clazz);
    void* fieldID(fieldID_);
    UNUSED(fieldID);
    SCRIPTINGAPI_STACK_CHECK(GetStaticIntField)
    		JNI_GET_FIELD( GetStaticIntField, (jclass)clazz ); 	
}

SCRIPT_BINDINGS_EXPORT_DECL
SInt64 SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_GetStaticLongField(ICallType_IntPtr_Argument clazz_, ICallType_IntPtr_Argument fieldID_)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_GetStaticLongField)
    void* clazz(clazz_);
    UNUSED(clazz);
    void* fieldID(fieldID_);
    UNUSED(fieldID);
    SCRIPTINGAPI_STACK_CHECK(GetStaticLongField)
    		JNI_GET_FIELD( GetStaticLongField, (jclass)clazz ); 	
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_GetStaticFloatField(ICallType_IntPtr_Argument clazz_, ICallType_IntPtr_Argument fieldID_)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_GetStaticFloatField)
    void* clazz(clazz_);
    UNUSED(clazz);
    void* fieldID(fieldID_);
    UNUSED(fieldID);
    SCRIPTINGAPI_STACK_CHECK(GetStaticFloatField)
    		JNI_GET_FIELD( GetStaticFloatField, (jclass)clazz ); 	
}

SCRIPT_BINDINGS_EXPORT_DECL
double SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_GetStaticDoubleField(ICallType_IntPtr_Argument clazz_, ICallType_IntPtr_Argument fieldID_)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_GetStaticDoubleField)
    void* clazz(clazz_);
    UNUSED(clazz);
    void* fieldID(fieldID_);
    UNUSED(fieldID);
    SCRIPTINGAPI_STACK_CHECK(GetStaticDoubleField)
    	JNI_GET_FIELD( GetStaticDoubleField, (jclass)clazz );	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_SetStaticStringField(ICallType_IntPtr_Argument clazz_, ICallType_IntPtr_Argument fieldID_, ICallType_String_Argument val_)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_SetStaticStringField)
    void* clazz(clazz_);
    UNUSED(clazz);
    void* fieldID(fieldID_);
    UNUSED(fieldID);
    ICallType_String_Local val(val_);
    UNUSED(val);
    SCRIPTINGAPI_STACK_CHECK(SetStaticStringField)
     JNI_PASS_VOID_ARGS( SetStaticObjectField, (jclass)clazz, (jfieldID)fieldID, jStringWrapper(val) ); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_SetStaticObjectField(ICallType_IntPtr_Argument clazz_, ICallType_IntPtr_Argument fieldID_, ICallType_IntPtr_Argument val_)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_SetStaticObjectField)
    void* clazz(clazz_);
    UNUSED(clazz);
    void* fieldID(fieldID_);
    UNUSED(fieldID);
    void* val(val_);
    UNUSED(val);
    SCRIPTINGAPI_STACK_CHECK(SetStaticObjectField)
    		JNI_SET_FIELD( SetStaticObjectField, (jclass)clazz, (jclass)val );	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_SetStaticBooleanField(ICallType_IntPtr_Argument clazz_, ICallType_IntPtr_Argument fieldID_, ScriptingBool val)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_SetStaticBooleanField)
    void* clazz(clazz_);
    UNUSED(clazz);
    void* fieldID(fieldID_);
    UNUSED(fieldID);
    SCRIPTINGAPI_STACK_CHECK(SetStaticBooleanField)
    		JNI_SET_FIELD( SetStaticBooleanField, (jclass)clazz, val ); 			
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_SetStaticByteField(ICallType_IntPtr_Argument clazz_, ICallType_IntPtr_Argument fieldID_, UInt8 val)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_SetStaticByteField)
    void* clazz(clazz_);
    UNUSED(clazz);
    void* fieldID(fieldID_);
    UNUSED(fieldID);
    SCRIPTINGAPI_STACK_CHECK(SetStaticByteField)
    			JNI_SET_FIELD( SetStaticByteField, (jclass)clazz, val ); 				
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_SetStaticCharField(ICallType_IntPtr_Argument clazz_, ICallType_IntPtr_Argument fieldID_, UInt16 val)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_SetStaticCharField)
    void* clazz(clazz_);
    UNUSED(clazz);
    void* fieldID(fieldID_);
    UNUSED(fieldID);
    SCRIPTINGAPI_STACK_CHECK(SetStaticCharField)
    			JNI_SET_FIELD( SetStaticCharField, (jclass)clazz, val ); 				
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_SetStaticShortField(ICallType_IntPtr_Argument clazz_, ICallType_IntPtr_Argument fieldID_, short val)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_SetStaticShortField)
    void* clazz(clazz_);
    UNUSED(clazz);
    void* fieldID(fieldID_);
    UNUSED(fieldID);
    SCRIPTINGAPI_STACK_CHECK(SetStaticShortField)
    		JNI_SET_FIELD( SetStaticShortField, (jclass)clazz, val ); 				
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_SetStaticIntField(ICallType_IntPtr_Argument clazz_, ICallType_IntPtr_Argument fieldID_, int val)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_SetStaticIntField)
    void* clazz(clazz_);
    UNUSED(clazz);
    void* fieldID(fieldID_);
    UNUSED(fieldID);
    SCRIPTINGAPI_STACK_CHECK(SetStaticIntField)
    			JNI_SET_FIELD( SetStaticIntField, (jclass)clazz, val ); 				
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_SetStaticLongField(ICallType_IntPtr_Argument clazz_, ICallType_IntPtr_Argument fieldID_, SInt64 val)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_SetStaticLongField)
    void* clazz(clazz_);
    UNUSED(clazz);
    void* fieldID(fieldID_);
    UNUSED(fieldID);
    SCRIPTINGAPI_STACK_CHECK(SetStaticLongField)
    		JNI_SET_FIELD( SetStaticLongField, (jclass)clazz, val ); 				
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_SetStaticFloatField(ICallType_IntPtr_Argument clazz_, ICallType_IntPtr_Argument fieldID_, float val)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_SetStaticFloatField)
    void* clazz(clazz_);
    UNUSED(clazz);
    void* fieldID(fieldID_);
    UNUSED(fieldID);
    SCRIPTINGAPI_STACK_CHECK(SetStaticFloatField)
    		JNI_SET_FIELD( SetStaticFloatField, (jclass)clazz, val ); 				
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_SetStaticDoubleField(ICallType_IntPtr_Argument clazz_, ICallType_IntPtr_Argument fieldID_, double val)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_SetStaticDoubleField)
    void* clazz(clazz_);
    UNUSED(clazz);
    void* fieldID(fieldID_);
    UNUSED(fieldID);
    SCRIPTINGAPI_STACK_CHECK(SetStaticDoubleField)
    		JNI_SET_FIELD( SetStaticDoubleField, (jclass)clazz, val ); 			
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_INTERNAL_CALL_ToBooleanArray(ICallType_Array_Argument array_, ICallType_IntPtr_Return& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_INTERNAL_CALL_ToBooleanArray)
    ICallType_Array_Local array(array_);
    UNUSED(array);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_ToBooleanArray)
     JNI_NEW_ARRAY( Boolean, jboolean, jbooleanArray );
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_INTERNAL_CALL_ToByteArray(ICallType_Array_Argument array_, ICallType_IntPtr_Return& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_INTERNAL_CALL_ToByteArray)
    ICallType_Array_Local array(array_);
    UNUSED(array);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_ToByteArray)
     JNI_NEW_ARRAY( Byte, jbyte, jbyteArray ); 		
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_INTERNAL_CALL_ToCharArray(ICallType_Array_Argument array_, ICallType_IntPtr_Return& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_INTERNAL_CALL_ToCharArray)
    ICallType_Array_Local array(array_);
    UNUSED(array);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_ToCharArray)
     JNI_NEW_ARRAY( Char, jchar, jcharArray );			
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_INTERNAL_CALL_ToShortArray(ICallType_Array_Argument array_, ICallType_IntPtr_Return& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_INTERNAL_CALL_ToShortArray)
    ICallType_Array_Local array(array_);
    UNUSED(array);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_ToShortArray)
     JNI_NEW_ARRAY( Short, jshort, jshortArray );		
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_INTERNAL_CALL_ToIntArray(ICallType_Array_Argument array_, ICallType_IntPtr_Return& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_INTERNAL_CALL_ToIntArray)
    ICallType_Array_Local array(array_);
    UNUSED(array);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_ToIntArray)
     JNI_NEW_ARRAY( Int, jint, jintArray );			
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_INTERNAL_CALL_ToLongArray(ICallType_Array_Argument array_, ICallType_IntPtr_Return& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_INTERNAL_CALL_ToLongArray)
    ICallType_Array_Local array(array_);
    UNUSED(array);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_ToLongArray)
     JNI_NEW_ARRAY( Long, jlong, jlongArray );			
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_INTERNAL_CALL_ToFloatArray(ICallType_Array_Argument array_, ICallType_IntPtr_Return& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_INTERNAL_CALL_ToFloatArray)
    ICallType_Array_Local array(array_);
    UNUSED(array);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_ToFloatArray)
     JNI_NEW_ARRAY( Float, jfloat, jfloatArray );		
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_INTERNAL_CALL_ToDoubleArray(ICallType_Array_Argument array_, ICallType_IntPtr_Return& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_INTERNAL_CALL_ToDoubleArray)
    ICallType_Array_Local array(array_);
    UNUSED(array);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_ToDoubleArray)
     JNI_NEW_ARRAY( Double, jdouble, jdoubleArray );	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_INTERNAL_CALL_ToObjectArray(ICallType_Array_Argument array_, ICallType_IntPtr_Return& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_INTERNAL_CALL_ToObjectArray)
    ICallType_Array_Local array(array_);
    UNUSED(array);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_ToObjectArray)
    
        #if PLATFORM_ANDROID
    		SCOPED_JNI(__FUNCTION__);
    		if (!jni_env) { returnValue =(0); return; }
    		int size = mono_array_length(array);
    		jclass obj_class = jni_env->FindClass("java/lang/Object");
    		if (obj_class == 0 || jni_env->ExceptionCheck()) { returnValue =(SCRIPTING_NULL); return; }
    		jobjectArray jni_array = jni_env->NewObjectArray(size, obj_class, 0);
    		if (jni_array == 0 || jni_env->ExceptionCheck())
    		{
    			jni_env->DeleteLocalRef(obj_class);
    			{ returnValue =(SCRIPTING_NULL); return; }
    		}
    		for (int i = 0; i < size; ++i)
    		{
    			void* val = GetMonoArrayElement<void*>(array, i);
    			jni_env->SetObjectArrayElement(jni_array, i, (jobject)val);
    			jni_env->DeleteLocalRef((jobject) val);
    			if (jni_env->ExceptionCheck())
    			{
    				jni_env->DeleteLocalRef(jni_array);
    				jni_env->DeleteLocalRef(obj_class);
    				{ returnValue =(SCRIPTING_NULL); return; }
    			}
        	}
        	jni_env->DeleteLocalRef(obj_class);
        	{ returnValue =(jni_array); return; }
       	#else
        	{ returnValue =(0); return; }
       	#endif
        
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_FromBooleanArray(ICallType_IntPtr_Argument array_)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_FromBooleanArray)
    void* array(array_);
    UNUSED(array);
    SCRIPTINGAPI_STACK_CHECK(FromBooleanArray)
     JNI_GET_ARRAY( Boolean, jboolean, jbooleanArray, mono_get_boolean_class() );	
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_FromByteArray(ICallType_IntPtr_Argument array_)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_FromByteArray)
    void* array(array_);
    UNUSED(array);
    SCRIPTINGAPI_STACK_CHECK(FromByteArray)
     JNI_GET_ARRAY( Byte, jbyte, jbyteArray, mono_get_byte_class() );				
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_FromCharArray(ICallType_IntPtr_Argument array_)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_FromCharArray)
    void* array(array_);
    UNUSED(array);
    SCRIPTINGAPI_STACK_CHECK(FromCharArray)
     JNI_GET_ARRAY( Char, jchar, jcharArray, mono_get_char_class() );				
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_FromShortArray(ICallType_IntPtr_Argument array_)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_FromShortArray)
    void* array(array_);
    UNUSED(array);
    SCRIPTINGAPI_STACK_CHECK(FromShortArray)
     JNI_GET_ARRAY( Short, jshort, jshortArray, mono_get_int16_class() );			
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_FromIntArray(ICallType_IntPtr_Argument array_)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_FromIntArray)
    void* array(array_);
    UNUSED(array);
    SCRIPTINGAPI_STACK_CHECK(FromIntArray)
     JNI_GET_ARRAY( Int, jint, jintArray, mono_get_int32_class() ); 				
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_FromLongArray(ICallType_IntPtr_Argument array_)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_FromLongArray)
    void* array(array_);
    UNUSED(array);
    SCRIPTINGAPI_STACK_CHECK(FromLongArray)
     JNI_GET_ARRAY( Long, jlong, jlongArray, mono_get_int64_class() );				
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_FromFloatArray(ICallType_IntPtr_Argument array_)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_FromFloatArray)
    void* array(array_);
    UNUSED(array);
    SCRIPTINGAPI_STACK_CHECK(FromFloatArray)
     JNI_GET_ARRAY( Float, jfloat, jfloatArray, mono_get_single_class() );			
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_FromDoubleArray(ICallType_IntPtr_Argument array_)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_FromDoubleArray)
    void* array(array_);
    UNUSED(array);
    SCRIPTINGAPI_STACK_CHECK(FromDoubleArray)
     JNI_GET_ARRAY( Double, jdouble, jdoubleArray, mono_get_double_class() );		
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_FromObjectArray(ICallType_IntPtr_Argument array_)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_FromObjectArray)
    void* array(array_);
    UNUSED(array);
    SCRIPTINGAPI_STACK_CHECK(FromObjectArray)
    
        #if PLATFORM_ANDROID
    		SCOPED_JNI(__FUNCTION__);
    		if (!jni_env) return 0;
    		jsize length = jni_env->GetArrayLength((jarray)array);
    		if (jni_env->ExceptionCheck()) return SCRIPTING_NULL;
    		MonoClass* clazz = mono_class_from_name (mono_get_corlib (), "System", "IntPtr");
    		MonoArray* csarray = mono_array_new(mono_domain_get(), clazz, length);
    		for (int i = 0; i < length; ++i)
    		{
    			jobject obj = jni_env->GetObjectArrayElement((jobjectArray)array, i);
    			if (jni_env->ExceptionCheck()) return SCRIPTING_NULL;
    			SetScriptingArrayElement<jobject>(csarray, i, obj);
    		}
    		return csarray;
    	#else
    		return SCRIPTING_NULL;
    	#endif
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_GetArrayLength(ICallType_IntPtr_Argument array_)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_GetArrayLength)
    void* array(array_);
    UNUSED(array);
    SCRIPTINGAPI_STACK_CHECK(GetArrayLength)
     JNI_PASS_RETV_ARGS( GetArrayLength, (jarray) array); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_INTERNAL_CALL_NewBooleanArray(int size, ICallType_IntPtr_Return& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_INTERNAL_CALL_NewBooleanArray)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_NewBooleanArray)
     JNI_PASS_RETV_ARGS( NewBooleanArray, (jsize)size);
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_INTERNAL_CALL_NewByteArray(int size, ICallType_IntPtr_Return& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_INTERNAL_CALL_NewByteArray)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_NewByteArray)
     JNI_PASS_RETV_ARGS( NewByteArray, (jsize)size); 	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_INTERNAL_CALL_NewCharArray(int size, ICallType_IntPtr_Return& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_INTERNAL_CALL_NewCharArray)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_NewCharArray)
     JNI_PASS_RETV_ARGS( NewCharArray, (jsize)size); 	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_INTERNAL_CALL_NewShortArray(int size, ICallType_IntPtr_Return& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_INTERNAL_CALL_NewShortArray)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_NewShortArray)
     JNI_PASS_RETV_ARGS( NewShortArray, (jsize)size); 	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_INTERNAL_CALL_NewIntArray(int size, ICallType_IntPtr_Return& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_INTERNAL_CALL_NewIntArray)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_NewIntArray)
     JNI_PASS_RETV_ARGS( NewIntArray, (jsize)size); 	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_INTERNAL_CALL_NewLongArray(int size, ICallType_IntPtr_Return& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_INTERNAL_CALL_NewLongArray)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_NewLongArray)
     JNI_PASS_RETV_ARGS( NewLongArray, (jsize)size); 	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_INTERNAL_CALL_NewFloatArray(int size, ICallType_IntPtr_Return& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_INTERNAL_CALL_NewFloatArray)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_NewFloatArray)
     JNI_PASS_RETV_ARGS( NewFloatArray, (jsize)size); 	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_INTERNAL_CALL_NewDoubleArray(int size, ICallType_IntPtr_Return& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_INTERNAL_CALL_NewDoubleArray)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_NewDoubleArray)
     JNI_PASS_RETV_ARGS( NewDoubleArray, (jsize)size); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_INTERNAL_CALL_NewObjectArray(int size, ICallType_IntPtr_Argument clazz_, ICallType_IntPtr_Argument obj_, ICallType_IntPtr_Return& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_INTERNAL_CALL_NewObjectArray)
    void* clazz(clazz_);
    UNUSED(clazz);
    void* obj(obj_);
    UNUSED(obj);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_NewObjectArray)
     JNI_PASS_RETV_ARGS( NewObjectArray, (jsize)size, (jclass)clazz, (jobject) obj); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_GetBooleanArrayElement(ICallType_IntPtr_Argument array_, int index)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_GetBooleanArrayElement)
    void* array(array_);
    UNUSED(array);
    SCRIPTINGAPI_STACK_CHECK(GetBooleanArrayElement)
     JNI_GET_ARRAY_ELEMENT( Boolean, jboolean, jbooleanArray ); 
}

SCRIPT_BINDINGS_EXPORT_DECL
UInt8 SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_GetByteArrayElement(ICallType_IntPtr_Argument array_, int index)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_GetByteArrayElement)
    void* array(array_);
    UNUSED(array);
    SCRIPTINGAPI_STACK_CHECK(GetByteArrayElement)
     JNI_GET_ARRAY_ELEMENT( Byte, jbyte, jbyteArray ); 
}

SCRIPT_BINDINGS_EXPORT_DECL
UInt16 SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_GetCharArrayElement(ICallType_IntPtr_Argument array_, int index)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_GetCharArrayElement)
    void* array(array_);
    UNUSED(array);
    SCRIPTINGAPI_STACK_CHECK(GetCharArrayElement)
     JNI_GET_ARRAY_ELEMENT( Char, jchar, jcharArray ); 
}

SCRIPT_BINDINGS_EXPORT_DECL
short SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_GetShortArrayElement(ICallType_IntPtr_Argument array_, int index)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_GetShortArrayElement)
    void* array(array_);
    UNUSED(array);
    SCRIPTINGAPI_STACK_CHECK(GetShortArrayElement)
     JNI_GET_ARRAY_ELEMENT( Short, jshort, jshortArray ); 
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_GetIntArrayElement(ICallType_IntPtr_Argument array_, int index)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_GetIntArrayElement)
    void* array(array_);
    UNUSED(array);
    SCRIPTINGAPI_STACK_CHECK(GetIntArrayElement)
     JNI_GET_ARRAY_ELEMENT( Int, jint, jintArray ); 
}

SCRIPT_BINDINGS_EXPORT_DECL
SInt64 SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_GetLongArrayElement(ICallType_IntPtr_Argument array_, int index)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_GetLongArrayElement)
    void* array(array_);
    UNUSED(array);
    SCRIPTINGAPI_STACK_CHECK(GetLongArrayElement)
     JNI_GET_ARRAY_ELEMENT( Long, jlong, jlongArray ); 
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_GetFloatArrayElement(ICallType_IntPtr_Argument array_, int index)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_GetFloatArrayElement)
    void* array(array_);
    UNUSED(array);
    SCRIPTINGAPI_STACK_CHECK(GetFloatArrayElement)
     JNI_GET_ARRAY_ELEMENT( Float, jfloat, jfloatArray ); 
}

SCRIPT_BINDINGS_EXPORT_DECL
double SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_GetDoubleArrayElement(ICallType_IntPtr_Argument array_, int index)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_GetDoubleArrayElement)
    void* array(array_);
    UNUSED(array);
    SCRIPTINGAPI_STACK_CHECK(GetDoubleArrayElement)
     JNI_GET_ARRAY_ELEMENT( Double, jdouble, jdoubleArray ); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_INTERNAL_CALL_GetObjectArrayElement(ICallType_IntPtr_Argument array_, int index, ICallType_IntPtr_Return& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_INTERNAL_CALL_GetObjectArrayElement)
    void* array(array_);
    UNUSED(array);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_GetObjectArrayElement)
     JNI_PASS_RETV_ARGS( GetObjectArrayElement, (jobjectArray)array, (jsize)index); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_SetBooleanArrayElement(ICallType_IntPtr_Argument array_, int index, UInt8 val)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_SetBooleanArrayElement)
    void* array(array_);
    UNUSED(array);
    SCRIPTINGAPI_STACK_CHECK(SetBooleanArrayElement)
     JNI_SET_ARRAY_ELEMENT( Boolean, jboolean, jbooleanArray ); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_SetByteArrayElement(ICallType_IntPtr_Argument array_, int index, SInt8 val)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_SetByteArrayElement)
    void* array(array_);
    UNUSED(array);
    SCRIPTINGAPI_STACK_CHECK(SetByteArrayElement)
     JNI_SET_ARRAY_ELEMENT( Byte, jbyte, jbyteArray ); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_SetCharArrayElement(ICallType_IntPtr_Argument array_, int index, UInt16 val)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_SetCharArrayElement)
    void* array(array_);
    UNUSED(array);
    SCRIPTINGAPI_STACK_CHECK(SetCharArrayElement)
     JNI_SET_ARRAY_ELEMENT( Char, jchar, jcharArray ); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_SetShortArrayElement(ICallType_IntPtr_Argument array_, int index, short val)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_SetShortArrayElement)
    void* array(array_);
    UNUSED(array);
    SCRIPTINGAPI_STACK_CHECK(SetShortArrayElement)
     JNI_SET_ARRAY_ELEMENT( Short, jshort, jshortArray ); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_SetIntArrayElement(ICallType_IntPtr_Argument array_, int index, int val)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_SetIntArrayElement)
    void* array(array_);
    UNUSED(array);
    SCRIPTINGAPI_STACK_CHECK(SetIntArrayElement)
     JNI_SET_ARRAY_ELEMENT( Int, jint, jintArray ); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_SetLongArrayElement(ICallType_IntPtr_Argument array_, int index, SInt64 val)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_SetLongArrayElement)
    void* array(array_);
    UNUSED(array);
    SCRIPTINGAPI_STACK_CHECK(SetLongArrayElement)
     JNI_SET_ARRAY_ELEMENT( Long, jlong, jlongArray ); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_SetFloatArrayElement(ICallType_IntPtr_Argument array_, int index, float val)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_SetFloatArrayElement)
    void* array(array_);
    UNUSED(array);
    SCRIPTINGAPI_STACK_CHECK(SetFloatArrayElement)
     JNI_SET_ARRAY_ELEMENT( Float, jfloat, jfloatArray ); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_SetDoubleArrayElement(ICallType_IntPtr_Argument array_, int index, double val)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_SetDoubleArrayElement)
    void* array(array_);
    UNUSED(array);
    SCRIPTINGAPI_STACK_CHECK(SetDoubleArrayElement)
     JNI_SET_ARRAY_ELEMENT( Double, jdouble, jdoubleArray ); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION AndroidJNI_CUSTOM_SetObjectArrayElement(ICallType_IntPtr_Argument array_, int index, ICallType_IntPtr_Argument obj_)
{
    SCRIPTINGAPI_ETW_ENTRY(AndroidJNI_CUSTOM_SetObjectArrayElement)
    void* array(array_);
    UNUSED(array);
    void* obj(obj_);
    UNUSED(obj);
    SCRIPTINGAPI_STACK_CHECK(SetObjectArrayElement)
     JNI_PASS_VOID_ARGS( SetObjectArrayElement, (jobjectArray)array, (jsize)index, (jobject)obj); 
}

#endif
#if !defined(INTERNAL_CALL_STRIPPING)
#   error must include unityconfigure.h
#endif
#if INTERNAL_CALL_STRIPPING
#if !PLATFORM_WINRT
void Register_UnityEngine_AndroidJNIHelper_get_debug()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNIHelper::get_debug" , (gpointer)& AndroidJNIHelper_Get_Custom_PropDebug );
}

void Register_UnityEngine_AndroidJNIHelper_set_debug()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNIHelper::set_debug" , (gpointer)& AndroidJNIHelper_Set_Custom_PropDebug );
}

void Register_UnityEngine_AndroidJNIHelper_INTERNAL_CALL_CreateJavaProxy()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNIHelper::INTERNAL_CALL_CreateJavaProxy" , (gpointer)& AndroidJNIHelper_CUSTOM_INTERNAL_CALL_CreateJavaProxy );
}

void Register_UnityEngine_AndroidJNI_AttachCurrentThread()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::AttachCurrentThread" , (gpointer)& AndroidJNI_CUSTOM_AttachCurrentThread );
}

void Register_UnityEngine_AndroidJNI_DetachCurrentThread()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::DetachCurrentThread" , (gpointer)& AndroidJNI_CUSTOM_DetachCurrentThread );
}

void Register_UnityEngine_AndroidJNI_GetVersion()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::GetVersion" , (gpointer)& AndroidJNI_CUSTOM_GetVersion );
}

void Register_UnityEngine_AndroidJNI_INTERNAL_CALL_FindClass()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::INTERNAL_CALL_FindClass" , (gpointer)& AndroidJNI_CUSTOM_INTERNAL_CALL_FindClass );
}

void Register_UnityEngine_AndroidJNI_INTERNAL_CALL_FromReflectedMethod()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::INTERNAL_CALL_FromReflectedMethod" , (gpointer)& AndroidJNI_CUSTOM_INTERNAL_CALL_FromReflectedMethod );
}

void Register_UnityEngine_AndroidJNI_INTERNAL_CALL_FromReflectedField()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::INTERNAL_CALL_FromReflectedField" , (gpointer)& AndroidJNI_CUSTOM_INTERNAL_CALL_FromReflectedField );
}

void Register_UnityEngine_AndroidJNI_INTERNAL_CALL_ToReflectedMethod()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::INTERNAL_CALL_ToReflectedMethod" , (gpointer)& AndroidJNI_CUSTOM_INTERNAL_CALL_ToReflectedMethod );
}

void Register_UnityEngine_AndroidJNI_INTERNAL_CALL_ToReflectedField()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::INTERNAL_CALL_ToReflectedField" , (gpointer)& AndroidJNI_CUSTOM_INTERNAL_CALL_ToReflectedField );
}

void Register_UnityEngine_AndroidJNI_INTERNAL_CALL_GetSuperclass()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::INTERNAL_CALL_GetSuperclass" , (gpointer)& AndroidJNI_CUSTOM_INTERNAL_CALL_GetSuperclass );
}

void Register_UnityEngine_AndroidJNI_IsAssignableFrom()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::IsAssignableFrom" , (gpointer)& AndroidJNI_CUSTOM_IsAssignableFrom );
}

void Register_UnityEngine_AndroidJNI_Throw()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::Throw" , (gpointer)& AndroidJNI_CUSTOM_Throw );
}

void Register_UnityEngine_AndroidJNI_ThrowNew()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::ThrowNew" , (gpointer)& AndroidJNI_CUSTOM_ThrowNew );
}

void Register_UnityEngine_AndroidJNI_INTERNAL_CALL_ExceptionOccurred()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::INTERNAL_CALL_ExceptionOccurred" , (gpointer)& AndroidJNI_CUSTOM_INTERNAL_CALL_ExceptionOccurred );
}

void Register_UnityEngine_AndroidJNI_ExceptionDescribe()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::ExceptionDescribe" , (gpointer)& AndroidJNI_CUSTOM_ExceptionDescribe );
}

void Register_UnityEngine_AndroidJNI_ExceptionClear()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::ExceptionClear" , (gpointer)& AndroidJNI_CUSTOM_ExceptionClear );
}

void Register_UnityEngine_AndroidJNI_FatalError()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::FatalError" , (gpointer)& AndroidJNI_CUSTOM_FatalError );
}

void Register_UnityEngine_AndroidJNI_PushLocalFrame()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::PushLocalFrame" , (gpointer)& AndroidJNI_CUSTOM_PushLocalFrame );
}

void Register_UnityEngine_AndroidJNI_INTERNAL_CALL_PopLocalFrame()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::INTERNAL_CALL_PopLocalFrame" , (gpointer)& AndroidJNI_CUSTOM_INTERNAL_CALL_PopLocalFrame );
}

void Register_UnityEngine_AndroidJNI_INTERNAL_CALL_NewGlobalRef()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::INTERNAL_CALL_NewGlobalRef" , (gpointer)& AndroidJNI_CUSTOM_INTERNAL_CALL_NewGlobalRef );
}

void Register_UnityEngine_AndroidJNI_DeleteGlobalRef()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::DeleteGlobalRef" , (gpointer)& AndroidJNI_CUSTOM_DeleteGlobalRef );
}

void Register_UnityEngine_AndroidJNI_INTERNAL_CALL_NewLocalRef()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::INTERNAL_CALL_NewLocalRef" , (gpointer)& AndroidJNI_CUSTOM_INTERNAL_CALL_NewLocalRef );
}

void Register_UnityEngine_AndroidJNI_DeleteLocalRef()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::DeleteLocalRef" , (gpointer)& AndroidJNI_CUSTOM_DeleteLocalRef );
}

void Register_UnityEngine_AndroidJNI_IsSameObject()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::IsSameObject" , (gpointer)& AndroidJNI_CUSTOM_IsSameObject );
}

void Register_UnityEngine_AndroidJNI_EnsureLocalCapacity()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::EnsureLocalCapacity" , (gpointer)& AndroidJNI_CUSTOM_EnsureLocalCapacity );
}

void Register_UnityEngine_AndroidJNI_INTERNAL_CALL_AllocObject()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::INTERNAL_CALL_AllocObject" , (gpointer)& AndroidJNI_CUSTOM_INTERNAL_CALL_AllocObject );
}

void Register_UnityEngine_AndroidJNI_INTERNAL_CALL_NewObject()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::INTERNAL_CALL_NewObject" , (gpointer)& AndroidJNI_CUSTOM_INTERNAL_CALL_NewObject );
}

void Register_UnityEngine_AndroidJNI_INTERNAL_CALL_GetObjectClass()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::INTERNAL_CALL_GetObjectClass" , (gpointer)& AndroidJNI_CUSTOM_INTERNAL_CALL_GetObjectClass );
}

void Register_UnityEngine_AndroidJNI_IsInstanceOf()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::IsInstanceOf" , (gpointer)& AndroidJNI_CUSTOM_IsInstanceOf );
}

void Register_UnityEngine_AndroidJNI_INTERNAL_CALL_GetMethodID()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::INTERNAL_CALL_GetMethodID" , (gpointer)& AndroidJNI_CUSTOM_INTERNAL_CALL_GetMethodID );
}

void Register_UnityEngine_AndroidJNI_INTERNAL_CALL_GetFieldID()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::INTERNAL_CALL_GetFieldID" , (gpointer)& AndroidJNI_CUSTOM_INTERNAL_CALL_GetFieldID );
}

void Register_UnityEngine_AndroidJNI_INTERNAL_CALL_GetStaticMethodID()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::INTERNAL_CALL_GetStaticMethodID" , (gpointer)& AndroidJNI_CUSTOM_INTERNAL_CALL_GetStaticMethodID );
}

void Register_UnityEngine_AndroidJNI_INTERNAL_CALL_GetStaticFieldID()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::INTERNAL_CALL_GetStaticFieldID" , (gpointer)& AndroidJNI_CUSTOM_INTERNAL_CALL_GetStaticFieldID );
}

void Register_UnityEngine_AndroidJNI_INTERNAL_CALL_NewStringUTF()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::INTERNAL_CALL_NewStringUTF" , (gpointer)& AndroidJNI_CUSTOM_INTERNAL_CALL_NewStringUTF );
}

void Register_UnityEngine_AndroidJNI_GetStringUTFLength()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::GetStringUTFLength" , (gpointer)& AndroidJNI_CUSTOM_GetStringUTFLength );
}

void Register_UnityEngine_AndroidJNI_GetStringUTFChars()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::GetStringUTFChars" , (gpointer)& AndroidJNI_CUSTOM_GetStringUTFChars );
}

void Register_UnityEngine_AndroidJNI_CallStringMethod()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::CallStringMethod" , (gpointer)& AndroidJNI_CUSTOM_CallStringMethod );
}

void Register_UnityEngine_AndroidJNI_INTERNAL_CALL_CallObjectMethod()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::INTERNAL_CALL_CallObjectMethod" , (gpointer)& AndroidJNI_CUSTOM_INTERNAL_CALL_CallObjectMethod );
}

void Register_UnityEngine_AndroidJNI_CallIntMethod()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::CallIntMethod" , (gpointer)& AndroidJNI_CUSTOM_CallIntMethod );
}

void Register_UnityEngine_AndroidJNI_CallBooleanMethod()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::CallBooleanMethod" , (gpointer)& AndroidJNI_CUSTOM_CallBooleanMethod );
}

void Register_UnityEngine_AndroidJNI_CallShortMethod()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::CallShortMethod" , (gpointer)& AndroidJNI_CUSTOM_CallShortMethod );
}

void Register_UnityEngine_AndroidJNI_CallByteMethod()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::CallByteMethod" , (gpointer)& AndroidJNI_CUSTOM_CallByteMethod );
}

void Register_UnityEngine_AndroidJNI_CallCharMethod()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::CallCharMethod" , (gpointer)& AndroidJNI_CUSTOM_CallCharMethod );
}

void Register_UnityEngine_AndroidJNI_CallFloatMethod()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::CallFloatMethod" , (gpointer)& AndroidJNI_CUSTOM_CallFloatMethod );
}

void Register_UnityEngine_AndroidJNI_CallDoubleMethod()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::CallDoubleMethod" , (gpointer)& AndroidJNI_CUSTOM_CallDoubleMethod );
}

void Register_UnityEngine_AndroidJNI_CallLongMethod()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::CallLongMethod" , (gpointer)& AndroidJNI_CUSTOM_CallLongMethod );
}

void Register_UnityEngine_AndroidJNI_CallVoidMethod()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::CallVoidMethod" , (gpointer)& AndroidJNI_CUSTOM_CallVoidMethod );
}

void Register_UnityEngine_AndroidJNI_GetStringField()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::GetStringField" , (gpointer)& AndroidJNI_CUSTOM_GetStringField );
}

void Register_UnityEngine_AndroidJNI_INTERNAL_CALL_GetObjectField()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::INTERNAL_CALL_GetObjectField" , (gpointer)& AndroidJNI_CUSTOM_INTERNAL_CALL_GetObjectField );
}

void Register_UnityEngine_AndroidJNI_GetBooleanField()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::GetBooleanField" , (gpointer)& AndroidJNI_CUSTOM_GetBooleanField );
}

void Register_UnityEngine_AndroidJNI_GetByteField()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::GetByteField" , (gpointer)& AndroidJNI_CUSTOM_GetByteField );
}

void Register_UnityEngine_AndroidJNI_GetCharField()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::GetCharField" , (gpointer)& AndroidJNI_CUSTOM_GetCharField );
}

void Register_UnityEngine_AndroidJNI_GetShortField()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::GetShortField" , (gpointer)& AndroidJNI_CUSTOM_GetShortField );
}

void Register_UnityEngine_AndroidJNI_GetIntField()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::GetIntField" , (gpointer)& AndroidJNI_CUSTOM_GetIntField );
}

void Register_UnityEngine_AndroidJNI_GetLongField()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::GetLongField" , (gpointer)& AndroidJNI_CUSTOM_GetLongField );
}

void Register_UnityEngine_AndroidJNI_GetFloatField()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::GetFloatField" , (gpointer)& AndroidJNI_CUSTOM_GetFloatField );
}

void Register_UnityEngine_AndroidJNI_GetDoubleField()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::GetDoubleField" , (gpointer)& AndroidJNI_CUSTOM_GetDoubleField );
}

void Register_UnityEngine_AndroidJNI_SetStringField()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::SetStringField" , (gpointer)& AndroidJNI_CUSTOM_SetStringField );
}

void Register_UnityEngine_AndroidJNI_SetObjectField()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::SetObjectField" , (gpointer)& AndroidJNI_CUSTOM_SetObjectField );
}

void Register_UnityEngine_AndroidJNI_SetBooleanField()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::SetBooleanField" , (gpointer)& AndroidJNI_CUSTOM_SetBooleanField );
}

void Register_UnityEngine_AndroidJNI_SetByteField()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::SetByteField" , (gpointer)& AndroidJNI_CUSTOM_SetByteField );
}

void Register_UnityEngine_AndroidJNI_SetCharField()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::SetCharField" , (gpointer)& AndroidJNI_CUSTOM_SetCharField );
}

void Register_UnityEngine_AndroidJNI_SetShortField()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::SetShortField" , (gpointer)& AndroidJNI_CUSTOM_SetShortField );
}

void Register_UnityEngine_AndroidJNI_SetIntField()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::SetIntField" , (gpointer)& AndroidJNI_CUSTOM_SetIntField );
}

void Register_UnityEngine_AndroidJNI_SetLongField()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::SetLongField" , (gpointer)& AndroidJNI_CUSTOM_SetLongField );
}

void Register_UnityEngine_AndroidJNI_SetFloatField()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::SetFloatField" , (gpointer)& AndroidJNI_CUSTOM_SetFloatField );
}

void Register_UnityEngine_AndroidJNI_SetDoubleField()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::SetDoubleField" , (gpointer)& AndroidJNI_CUSTOM_SetDoubleField );
}

void Register_UnityEngine_AndroidJNI_CallStaticStringMethod()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::CallStaticStringMethod" , (gpointer)& AndroidJNI_CUSTOM_CallStaticStringMethod );
}

void Register_UnityEngine_AndroidJNI_INTERNAL_CALL_CallStaticObjectMethod()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::INTERNAL_CALL_CallStaticObjectMethod" , (gpointer)& AndroidJNI_CUSTOM_INTERNAL_CALL_CallStaticObjectMethod );
}

void Register_UnityEngine_AndroidJNI_CallStaticIntMethod()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::CallStaticIntMethod" , (gpointer)& AndroidJNI_CUSTOM_CallStaticIntMethod );
}

void Register_UnityEngine_AndroidJNI_CallStaticBooleanMethod()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::CallStaticBooleanMethod" , (gpointer)& AndroidJNI_CUSTOM_CallStaticBooleanMethod );
}

void Register_UnityEngine_AndroidJNI_CallStaticShortMethod()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::CallStaticShortMethod" , (gpointer)& AndroidJNI_CUSTOM_CallStaticShortMethod );
}

void Register_UnityEngine_AndroidJNI_CallStaticByteMethod()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::CallStaticByteMethod" , (gpointer)& AndroidJNI_CUSTOM_CallStaticByteMethod );
}

void Register_UnityEngine_AndroidJNI_CallStaticCharMethod()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::CallStaticCharMethod" , (gpointer)& AndroidJNI_CUSTOM_CallStaticCharMethod );
}

void Register_UnityEngine_AndroidJNI_CallStaticFloatMethod()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::CallStaticFloatMethod" , (gpointer)& AndroidJNI_CUSTOM_CallStaticFloatMethod );
}

void Register_UnityEngine_AndroidJNI_CallStaticDoubleMethod()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::CallStaticDoubleMethod" , (gpointer)& AndroidJNI_CUSTOM_CallStaticDoubleMethod );
}

void Register_UnityEngine_AndroidJNI_CallStaticLongMethod()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::CallStaticLongMethod" , (gpointer)& AndroidJNI_CUSTOM_CallStaticLongMethod );
}

void Register_UnityEngine_AndroidJNI_CallStaticVoidMethod()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::CallStaticVoidMethod" , (gpointer)& AndroidJNI_CUSTOM_CallStaticVoidMethod );
}

void Register_UnityEngine_AndroidJNI_GetStaticStringField()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::GetStaticStringField" , (gpointer)& AndroidJNI_CUSTOM_GetStaticStringField );
}

void Register_UnityEngine_AndroidJNI_INTERNAL_CALL_GetStaticObjectField()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::INTERNAL_CALL_GetStaticObjectField" , (gpointer)& AndroidJNI_CUSTOM_INTERNAL_CALL_GetStaticObjectField );
}

void Register_UnityEngine_AndroidJNI_GetStaticBooleanField()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::GetStaticBooleanField" , (gpointer)& AndroidJNI_CUSTOM_GetStaticBooleanField );
}

void Register_UnityEngine_AndroidJNI_GetStaticByteField()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::GetStaticByteField" , (gpointer)& AndroidJNI_CUSTOM_GetStaticByteField );
}

void Register_UnityEngine_AndroidJNI_GetStaticCharField()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::GetStaticCharField" , (gpointer)& AndroidJNI_CUSTOM_GetStaticCharField );
}

void Register_UnityEngine_AndroidJNI_GetStaticShortField()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::GetStaticShortField" , (gpointer)& AndroidJNI_CUSTOM_GetStaticShortField );
}

void Register_UnityEngine_AndroidJNI_GetStaticIntField()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::GetStaticIntField" , (gpointer)& AndroidJNI_CUSTOM_GetStaticIntField );
}

void Register_UnityEngine_AndroidJNI_GetStaticLongField()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::GetStaticLongField" , (gpointer)& AndroidJNI_CUSTOM_GetStaticLongField );
}

void Register_UnityEngine_AndroidJNI_GetStaticFloatField()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::GetStaticFloatField" , (gpointer)& AndroidJNI_CUSTOM_GetStaticFloatField );
}

void Register_UnityEngine_AndroidJNI_GetStaticDoubleField()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::GetStaticDoubleField" , (gpointer)& AndroidJNI_CUSTOM_GetStaticDoubleField );
}

void Register_UnityEngine_AndroidJNI_SetStaticStringField()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::SetStaticStringField" , (gpointer)& AndroidJNI_CUSTOM_SetStaticStringField );
}

void Register_UnityEngine_AndroidJNI_SetStaticObjectField()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::SetStaticObjectField" , (gpointer)& AndroidJNI_CUSTOM_SetStaticObjectField );
}

void Register_UnityEngine_AndroidJNI_SetStaticBooleanField()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::SetStaticBooleanField" , (gpointer)& AndroidJNI_CUSTOM_SetStaticBooleanField );
}

void Register_UnityEngine_AndroidJNI_SetStaticByteField()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::SetStaticByteField" , (gpointer)& AndroidJNI_CUSTOM_SetStaticByteField );
}

void Register_UnityEngine_AndroidJNI_SetStaticCharField()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::SetStaticCharField" , (gpointer)& AndroidJNI_CUSTOM_SetStaticCharField );
}

void Register_UnityEngine_AndroidJNI_SetStaticShortField()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::SetStaticShortField" , (gpointer)& AndroidJNI_CUSTOM_SetStaticShortField );
}

void Register_UnityEngine_AndroidJNI_SetStaticIntField()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::SetStaticIntField" , (gpointer)& AndroidJNI_CUSTOM_SetStaticIntField );
}

void Register_UnityEngine_AndroidJNI_SetStaticLongField()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::SetStaticLongField" , (gpointer)& AndroidJNI_CUSTOM_SetStaticLongField );
}

void Register_UnityEngine_AndroidJNI_SetStaticFloatField()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::SetStaticFloatField" , (gpointer)& AndroidJNI_CUSTOM_SetStaticFloatField );
}

void Register_UnityEngine_AndroidJNI_SetStaticDoubleField()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::SetStaticDoubleField" , (gpointer)& AndroidJNI_CUSTOM_SetStaticDoubleField );
}

void Register_UnityEngine_AndroidJNI_INTERNAL_CALL_ToBooleanArray()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::INTERNAL_CALL_ToBooleanArray" , (gpointer)& AndroidJNI_CUSTOM_INTERNAL_CALL_ToBooleanArray );
}

void Register_UnityEngine_AndroidJNI_INTERNAL_CALL_ToByteArray()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::INTERNAL_CALL_ToByteArray" , (gpointer)& AndroidJNI_CUSTOM_INTERNAL_CALL_ToByteArray );
}

void Register_UnityEngine_AndroidJNI_INTERNAL_CALL_ToCharArray()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::INTERNAL_CALL_ToCharArray" , (gpointer)& AndroidJNI_CUSTOM_INTERNAL_CALL_ToCharArray );
}

void Register_UnityEngine_AndroidJNI_INTERNAL_CALL_ToShortArray()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::INTERNAL_CALL_ToShortArray" , (gpointer)& AndroidJNI_CUSTOM_INTERNAL_CALL_ToShortArray );
}

void Register_UnityEngine_AndroidJNI_INTERNAL_CALL_ToIntArray()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::INTERNAL_CALL_ToIntArray" , (gpointer)& AndroidJNI_CUSTOM_INTERNAL_CALL_ToIntArray );
}

void Register_UnityEngine_AndroidJNI_INTERNAL_CALL_ToLongArray()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::INTERNAL_CALL_ToLongArray" , (gpointer)& AndroidJNI_CUSTOM_INTERNAL_CALL_ToLongArray );
}

void Register_UnityEngine_AndroidJNI_INTERNAL_CALL_ToFloatArray()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::INTERNAL_CALL_ToFloatArray" , (gpointer)& AndroidJNI_CUSTOM_INTERNAL_CALL_ToFloatArray );
}

void Register_UnityEngine_AndroidJNI_INTERNAL_CALL_ToDoubleArray()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::INTERNAL_CALL_ToDoubleArray" , (gpointer)& AndroidJNI_CUSTOM_INTERNAL_CALL_ToDoubleArray );
}

void Register_UnityEngine_AndroidJNI_INTERNAL_CALL_ToObjectArray()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::INTERNAL_CALL_ToObjectArray" , (gpointer)& AndroidJNI_CUSTOM_INTERNAL_CALL_ToObjectArray );
}

void Register_UnityEngine_AndroidJNI_FromBooleanArray()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::FromBooleanArray" , (gpointer)& AndroidJNI_CUSTOM_FromBooleanArray );
}

void Register_UnityEngine_AndroidJNI_FromByteArray()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::FromByteArray" , (gpointer)& AndroidJNI_CUSTOM_FromByteArray );
}

void Register_UnityEngine_AndroidJNI_FromCharArray()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::FromCharArray" , (gpointer)& AndroidJNI_CUSTOM_FromCharArray );
}

void Register_UnityEngine_AndroidJNI_FromShortArray()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::FromShortArray" , (gpointer)& AndroidJNI_CUSTOM_FromShortArray );
}

void Register_UnityEngine_AndroidJNI_FromIntArray()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::FromIntArray" , (gpointer)& AndroidJNI_CUSTOM_FromIntArray );
}

void Register_UnityEngine_AndroidJNI_FromLongArray()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::FromLongArray" , (gpointer)& AndroidJNI_CUSTOM_FromLongArray );
}

void Register_UnityEngine_AndroidJNI_FromFloatArray()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::FromFloatArray" , (gpointer)& AndroidJNI_CUSTOM_FromFloatArray );
}

void Register_UnityEngine_AndroidJNI_FromDoubleArray()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::FromDoubleArray" , (gpointer)& AndroidJNI_CUSTOM_FromDoubleArray );
}

void Register_UnityEngine_AndroidJNI_FromObjectArray()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::FromObjectArray" , (gpointer)& AndroidJNI_CUSTOM_FromObjectArray );
}

void Register_UnityEngine_AndroidJNI_GetArrayLength()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::GetArrayLength" , (gpointer)& AndroidJNI_CUSTOM_GetArrayLength );
}

void Register_UnityEngine_AndroidJNI_INTERNAL_CALL_NewBooleanArray()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::INTERNAL_CALL_NewBooleanArray" , (gpointer)& AndroidJNI_CUSTOM_INTERNAL_CALL_NewBooleanArray );
}

void Register_UnityEngine_AndroidJNI_INTERNAL_CALL_NewByteArray()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::INTERNAL_CALL_NewByteArray" , (gpointer)& AndroidJNI_CUSTOM_INTERNAL_CALL_NewByteArray );
}

void Register_UnityEngine_AndroidJNI_INTERNAL_CALL_NewCharArray()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::INTERNAL_CALL_NewCharArray" , (gpointer)& AndroidJNI_CUSTOM_INTERNAL_CALL_NewCharArray );
}

void Register_UnityEngine_AndroidJNI_INTERNAL_CALL_NewShortArray()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::INTERNAL_CALL_NewShortArray" , (gpointer)& AndroidJNI_CUSTOM_INTERNAL_CALL_NewShortArray );
}

void Register_UnityEngine_AndroidJNI_INTERNAL_CALL_NewIntArray()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::INTERNAL_CALL_NewIntArray" , (gpointer)& AndroidJNI_CUSTOM_INTERNAL_CALL_NewIntArray );
}

void Register_UnityEngine_AndroidJNI_INTERNAL_CALL_NewLongArray()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::INTERNAL_CALL_NewLongArray" , (gpointer)& AndroidJNI_CUSTOM_INTERNAL_CALL_NewLongArray );
}

void Register_UnityEngine_AndroidJNI_INTERNAL_CALL_NewFloatArray()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::INTERNAL_CALL_NewFloatArray" , (gpointer)& AndroidJNI_CUSTOM_INTERNAL_CALL_NewFloatArray );
}

void Register_UnityEngine_AndroidJNI_INTERNAL_CALL_NewDoubleArray()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::INTERNAL_CALL_NewDoubleArray" , (gpointer)& AndroidJNI_CUSTOM_INTERNAL_CALL_NewDoubleArray );
}

void Register_UnityEngine_AndroidJNI_INTERNAL_CALL_NewObjectArray()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::INTERNAL_CALL_NewObjectArray" , (gpointer)& AndroidJNI_CUSTOM_INTERNAL_CALL_NewObjectArray );
}

void Register_UnityEngine_AndroidJNI_GetBooleanArrayElement()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::GetBooleanArrayElement" , (gpointer)& AndroidJNI_CUSTOM_GetBooleanArrayElement );
}

void Register_UnityEngine_AndroidJNI_GetByteArrayElement()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::GetByteArrayElement" , (gpointer)& AndroidJNI_CUSTOM_GetByteArrayElement );
}

void Register_UnityEngine_AndroidJNI_GetCharArrayElement()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::GetCharArrayElement" , (gpointer)& AndroidJNI_CUSTOM_GetCharArrayElement );
}

void Register_UnityEngine_AndroidJNI_GetShortArrayElement()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::GetShortArrayElement" , (gpointer)& AndroidJNI_CUSTOM_GetShortArrayElement );
}

void Register_UnityEngine_AndroidJNI_GetIntArrayElement()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::GetIntArrayElement" , (gpointer)& AndroidJNI_CUSTOM_GetIntArrayElement );
}

void Register_UnityEngine_AndroidJNI_GetLongArrayElement()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::GetLongArrayElement" , (gpointer)& AndroidJNI_CUSTOM_GetLongArrayElement );
}

void Register_UnityEngine_AndroidJNI_GetFloatArrayElement()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::GetFloatArrayElement" , (gpointer)& AndroidJNI_CUSTOM_GetFloatArrayElement );
}

void Register_UnityEngine_AndroidJNI_GetDoubleArrayElement()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::GetDoubleArrayElement" , (gpointer)& AndroidJNI_CUSTOM_GetDoubleArrayElement );
}

void Register_UnityEngine_AndroidJNI_INTERNAL_CALL_GetObjectArrayElement()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::INTERNAL_CALL_GetObjectArrayElement" , (gpointer)& AndroidJNI_CUSTOM_INTERNAL_CALL_GetObjectArrayElement );
}

void Register_UnityEngine_AndroidJNI_SetBooleanArrayElement()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::SetBooleanArrayElement" , (gpointer)& AndroidJNI_CUSTOM_SetBooleanArrayElement );
}

void Register_UnityEngine_AndroidJNI_SetByteArrayElement()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::SetByteArrayElement" , (gpointer)& AndroidJNI_CUSTOM_SetByteArrayElement );
}

void Register_UnityEngine_AndroidJNI_SetCharArrayElement()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::SetCharArrayElement" , (gpointer)& AndroidJNI_CUSTOM_SetCharArrayElement );
}

void Register_UnityEngine_AndroidJNI_SetShortArrayElement()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::SetShortArrayElement" , (gpointer)& AndroidJNI_CUSTOM_SetShortArrayElement );
}

void Register_UnityEngine_AndroidJNI_SetIntArrayElement()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::SetIntArrayElement" , (gpointer)& AndroidJNI_CUSTOM_SetIntArrayElement );
}

void Register_UnityEngine_AndroidJNI_SetLongArrayElement()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::SetLongArrayElement" , (gpointer)& AndroidJNI_CUSTOM_SetLongArrayElement );
}

void Register_UnityEngine_AndroidJNI_SetFloatArrayElement()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::SetFloatArrayElement" , (gpointer)& AndroidJNI_CUSTOM_SetFloatArrayElement );
}

void Register_UnityEngine_AndroidJNI_SetDoubleArrayElement()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::SetDoubleArrayElement" , (gpointer)& AndroidJNI_CUSTOM_SetDoubleArrayElement );
}

void Register_UnityEngine_AndroidJNI_SetObjectArrayElement()
{
    scripting_add_internal_call( "UnityEngine.AndroidJNI::SetObjectArrayElement" , (gpointer)& AndroidJNI_CUSTOM_SetObjectArrayElement );
}

#endif
#elif ENABLE_MONO || ENABLE_IL2CPP
static const char* s_AndroidJNI_IcallNames [] =
{
#if !PLATFORM_WINRT
    "UnityEngine.AndroidJNIHelper::get_debug",    // -> AndroidJNIHelper_Get_Custom_PropDebug
    "UnityEngine.AndroidJNIHelper::set_debug",    // -> AndroidJNIHelper_Set_Custom_PropDebug
    "UnityEngine.AndroidJNIHelper::INTERNAL_CALL_CreateJavaProxy",    // -> AndroidJNIHelper_CUSTOM_INTERNAL_CALL_CreateJavaProxy
    "UnityEngine.AndroidJNI::AttachCurrentThread",    // -> AndroidJNI_CUSTOM_AttachCurrentThread
    "UnityEngine.AndroidJNI::DetachCurrentThread",    // -> AndroidJNI_CUSTOM_DetachCurrentThread
    "UnityEngine.AndroidJNI::GetVersion"    ,    // -> AndroidJNI_CUSTOM_GetVersion
    "UnityEngine.AndroidJNI::INTERNAL_CALL_FindClass",    // -> AndroidJNI_CUSTOM_INTERNAL_CALL_FindClass
    "UnityEngine.AndroidJNI::INTERNAL_CALL_FromReflectedMethod",    // -> AndroidJNI_CUSTOM_INTERNAL_CALL_FromReflectedMethod
    "UnityEngine.AndroidJNI::INTERNAL_CALL_FromReflectedField",    // -> AndroidJNI_CUSTOM_INTERNAL_CALL_FromReflectedField
    "UnityEngine.AndroidJNI::INTERNAL_CALL_ToReflectedMethod",    // -> AndroidJNI_CUSTOM_INTERNAL_CALL_ToReflectedMethod
    "UnityEngine.AndroidJNI::INTERNAL_CALL_ToReflectedField",    // -> AndroidJNI_CUSTOM_INTERNAL_CALL_ToReflectedField
    "UnityEngine.AndroidJNI::INTERNAL_CALL_GetSuperclass",    // -> AndroidJNI_CUSTOM_INTERNAL_CALL_GetSuperclass
    "UnityEngine.AndroidJNI::IsAssignableFrom",    // -> AndroidJNI_CUSTOM_IsAssignableFrom
    "UnityEngine.AndroidJNI::Throw"         ,    // -> AndroidJNI_CUSTOM_Throw
    "UnityEngine.AndroidJNI::ThrowNew"      ,    // -> AndroidJNI_CUSTOM_ThrowNew
    "UnityEngine.AndroidJNI::INTERNAL_CALL_ExceptionOccurred",    // -> AndroidJNI_CUSTOM_INTERNAL_CALL_ExceptionOccurred
    "UnityEngine.AndroidJNI::ExceptionDescribe",    // -> AndroidJNI_CUSTOM_ExceptionDescribe
    "UnityEngine.AndroidJNI::ExceptionClear",    // -> AndroidJNI_CUSTOM_ExceptionClear
    "UnityEngine.AndroidJNI::FatalError"    ,    // -> AndroidJNI_CUSTOM_FatalError
    "UnityEngine.AndroidJNI::PushLocalFrame",    // -> AndroidJNI_CUSTOM_PushLocalFrame
    "UnityEngine.AndroidJNI::INTERNAL_CALL_PopLocalFrame",    // -> AndroidJNI_CUSTOM_INTERNAL_CALL_PopLocalFrame
    "UnityEngine.AndroidJNI::INTERNAL_CALL_NewGlobalRef",    // -> AndroidJNI_CUSTOM_INTERNAL_CALL_NewGlobalRef
    "UnityEngine.AndroidJNI::DeleteGlobalRef",    // -> AndroidJNI_CUSTOM_DeleteGlobalRef
    "UnityEngine.AndroidJNI::INTERNAL_CALL_NewLocalRef",    // -> AndroidJNI_CUSTOM_INTERNAL_CALL_NewLocalRef
    "UnityEngine.AndroidJNI::DeleteLocalRef",    // -> AndroidJNI_CUSTOM_DeleteLocalRef
    "UnityEngine.AndroidJNI::IsSameObject"  ,    // -> AndroidJNI_CUSTOM_IsSameObject
    "UnityEngine.AndroidJNI::EnsureLocalCapacity",    // -> AndroidJNI_CUSTOM_EnsureLocalCapacity
    "UnityEngine.AndroidJNI::INTERNAL_CALL_AllocObject",    // -> AndroidJNI_CUSTOM_INTERNAL_CALL_AllocObject
    "UnityEngine.AndroidJNI::INTERNAL_CALL_NewObject",    // -> AndroidJNI_CUSTOM_INTERNAL_CALL_NewObject
    "UnityEngine.AndroidJNI::INTERNAL_CALL_GetObjectClass",    // -> AndroidJNI_CUSTOM_INTERNAL_CALL_GetObjectClass
    "UnityEngine.AndroidJNI::IsInstanceOf"  ,    // -> AndroidJNI_CUSTOM_IsInstanceOf
    "UnityEngine.AndroidJNI::INTERNAL_CALL_GetMethodID",    // -> AndroidJNI_CUSTOM_INTERNAL_CALL_GetMethodID
    "UnityEngine.AndroidJNI::INTERNAL_CALL_GetFieldID",    // -> AndroidJNI_CUSTOM_INTERNAL_CALL_GetFieldID
    "UnityEngine.AndroidJNI::INTERNAL_CALL_GetStaticMethodID",    // -> AndroidJNI_CUSTOM_INTERNAL_CALL_GetStaticMethodID
    "UnityEngine.AndroidJNI::INTERNAL_CALL_GetStaticFieldID",    // -> AndroidJNI_CUSTOM_INTERNAL_CALL_GetStaticFieldID
    "UnityEngine.AndroidJNI::INTERNAL_CALL_NewStringUTF",    // -> AndroidJNI_CUSTOM_INTERNAL_CALL_NewStringUTF
    "UnityEngine.AndroidJNI::GetStringUTFLength",    // -> AndroidJNI_CUSTOM_GetStringUTFLength
    "UnityEngine.AndroidJNI::GetStringUTFChars",    // -> AndroidJNI_CUSTOM_GetStringUTFChars
    "UnityEngine.AndroidJNI::CallStringMethod",    // -> AndroidJNI_CUSTOM_CallStringMethod
    "UnityEngine.AndroidJNI::INTERNAL_CALL_CallObjectMethod",    // -> AndroidJNI_CUSTOM_INTERNAL_CALL_CallObjectMethod
    "UnityEngine.AndroidJNI::CallIntMethod" ,    // -> AndroidJNI_CUSTOM_CallIntMethod
    "UnityEngine.AndroidJNI::CallBooleanMethod",    // -> AndroidJNI_CUSTOM_CallBooleanMethod
    "UnityEngine.AndroidJNI::CallShortMethod",    // -> AndroidJNI_CUSTOM_CallShortMethod
    "UnityEngine.AndroidJNI::CallByteMethod",    // -> AndroidJNI_CUSTOM_CallByteMethod
    "UnityEngine.AndroidJNI::CallCharMethod",    // -> AndroidJNI_CUSTOM_CallCharMethod
    "UnityEngine.AndroidJNI::CallFloatMethod",    // -> AndroidJNI_CUSTOM_CallFloatMethod
    "UnityEngine.AndroidJNI::CallDoubleMethod",    // -> AndroidJNI_CUSTOM_CallDoubleMethod
    "UnityEngine.AndroidJNI::CallLongMethod",    // -> AndroidJNI_CUSTOM_CallLongMethod
    "UnityEngine.AndroidJNI::CallVoidMethod",    // -> AndroidJNI_CUSTOM_CallVoidMethod
    "UnityEngine.AndroidJNI::GetStringField",    // -> AndroidJNI_CUSTOM_GetStringField
    "UnityEngine.AndroidJNI::INTERNAL_CALL_GetObjectField",    // -> AndroidJNI_CUSTOM_INTERNAL_CALL_GetObjectField
    "UnityEngine.AndroidJNI::GetBooleanField",    // -> AndroidJNI_CUSTOM_GetBooleanField
    "UnityEngine.AndroidJNI::GetByteField"  ,    // -> AndroidJNI_CUSTOM_GetByteField
    "UnityEngine.AndroidJNI::GetCharField"  ,    // -> AndroidJNI_CUSTOM_GetCharField
    "UnityEngine.AndroidJNI::GetShortField" ,    // -> AndroidJNI_CUSTOM_GetShortField
    "UnityEngine.AndroidJNI::GetIntField"   ,    // -> AndroidJNI_CUSTOM_GetIntField
    "UnityEngine.AndroidJNI::GetLongField"  ,    // -> AndroidJNI_CUSTOM_GetLongField
    "UnityEngine.AndroidJNI::GetFloatField" ,    // -> AndroidJNI_CUSTOM_GetFloatField
    "UnityEngine.AndroidJNI::GetDoubleField",    // -> AndroidJNI_CUSTOM_GetDoubleField
    "UnityEngine.AndroidJNI::SetStringField",    // -> AndroidJNI_CUSTOM_SetStringField
    "UnityEngine.AndroidJNI::SetObjectField",    // -> AndroidJNI_CUSTOM_SetObjectField
    "UnityEngine.AndroidJNI::SetBooleanField",    // -> AndroidJNI_CUSTOM_SetBooleanField
    "UnityEngine.AndroidJNI::SetByteField"  ,    // -> AndroidJNI_CUSTOM_SetByteField
    "UnityEngine.AndroidJNI::SetCharField"  ,    // -> AndroidJNI_CUSTOM_SetCharField
    "UnityEngine.AndroidJNI::SetShortField" ,    // -> AndroidJNI_CUSTOM_SetShortField
    "UnityEngine.AndroidJNI::SetIntField"   ,    // -> AndroidJNI_CUSTOM_SetIntField
    "UnityEngine.AndroidJNI::SetLongField"  ,    // -> AndroidJNI_CUSTOM_SetLongField
    "UnityEngine.AndroidJNI::SetFloatField" ,    // -> AndroidJNI_CUSTOM_SetFloatField
    "UnityEngine.AndroidJNI::SetDoubleField",    // -> AndroidJNI_CUSTOM_SetDoubleField
    "UnityEngine.AndroidJNI::CallStaticStringMethod",    // -> AndroidJNI_CUSTOM_CallStaticStringMethod
    "UnityEngine.AndroidJNI::INTERNAL_CALL_CallStaticObjectMethod",    // -> AndroidJNI_CUSTOM_INTERNAL_CALL_CallStaticObjectMethod
    "UnityEngine.AndroidJNI::CallStaticIntMethod",    // -> AndroidJNI_CUSTOM_CallStaticIntMethod
    "UnityEngine.AndroidJNI::CallStaticBooleanMethod",    // -> AndroidJNI_CUSTOM_CallStaticBooleanMethod
    "UnityEngine.AndroidJNI::CallStaticShortMethod",    // -> AndroidJNI_CUSTOM_CallStaticShortMethod
    "UnityEngine.AndroidJNI::CallStaticByteMethod",    // -> AndroidJNI_CUSTOM_CallStaticByteMethod
    "UnityEngine.AndroidJNI::CallStaticCharMethod",    // -> AndroidJNI_CUSTOM_CallStaticCharMethod
    "UnityEngine.AndroidJNI::CallStaticFloatMethod",    // -> AndroidJNI_CUSTOM_CallStaticFloatMethod
    "UnityEngine.AndroidJNI::CallStaticDoubleMethod",    // -> AndroidJNI_CUSTOM_CallStaticDoubleMethod
    "UnityEngine.AndroidJNI::CallStaticLongMethod",    // -> AndroidJNI_CUSTOM_CallStaticLongMethod
    "UnityEngine.AndroidJNI::CallStaticVoidMethod",    // -> AndroidJNI_CUSTOM_CallStaticVoidMethod
    "UnityEngine.AndroidJNI::GetStaticStringField",    // -> AndroidJNI_CUSTOM_GetStaticStringField
    "UnityEngine.AndroidJNI::INTERNAL_CALL_GetStaticObjectField",    // -> AndroidJNI_CUSTOM_INTERNAL_CALL_GetStaticObjectField
    "UnityEngine.AndroidJNI::GetStaticBooleanField",    // -> AndroidJNI_CUSTOM_GetStaticBooleanField
    "UnityEngine.AndroidJNI::GetStaticByteField",    // -> AndroidJNI_CUSTOM_GetStaticByteField
    "UnityEngine.AndroidJNI::GetStaticCharField",    // -> AndroidJNI_CUSTOM_GetStaticCharField
    "UnityEngine.AndroidJNI::GetStaticShortField",    // -> AndroidJNI_CUSTOM_GetStaticShortField
    "UnityEngine.AndroidJNI::GetStaticIntField",    // -> AndroidJNI_CUSTOM_GetStaticIntField
    "UnityEngine.AndroidJNI::GetStaticLongField",    // -> AndroidJNI_CUSTOM_GetStaticLongField
    "UnityEngine.AndroidJNI::GetStaticFloatField",    // -> AndroidJNI_CUSTOM_GetStaticFloatField
    "UnityEngine.AndroidJNI::GetStaticDoubleField",    // -> AndroidJNI_CUSTOM_GetStaticDoubleField
    "UnityEngine.AndroidJNI::SetStaticStringField",    // -> AndroidJNI_CUSTOM_SetStaticStringField
    "UnityEngine.AndroidJNI::SetStaticObjectField",    // -> AndroidJNI_CUSTOM_SetStaticObjectField
    "UnityEngine.AndroidJNI::SetStaticBooleanField",    // -> AndroidJNI_CUSTOM_SetStaticBooleanField
    "UnityEngine.AndroidJNI::SetStaticByteField",    // -> AndroidJNI_CUSTOM_SetStaticByteField
    "UnityEngine.AndroidJNI::SetStaticCharField",    // -> AndroidJNI_CUSTOM_SetStaticCharField
    "UnityEngine.AndroidJNI::SetStaticShortField",    // -> AndroidJNI_CUSTOM_SetStaticShortField
    "UnityEngine.AndroidJNI::SetStaticIntField",    // -> AndroidJNI_CUSTOM_SetStaticIntField
    "UnityEngine.AndroidJNI::SetStaticLongField",    // -> AndroidJNI_CUSTOM_SetStaticLongField
    "UnityEngine.AndroidJNI::SetStaticFloatField",    // -> AndroidJNI_CUSTOM_SetStaticFloatField
    "UnityEngine.AndroidJNI::SetStaticDoubleField",    // -> AndroidJNI_CUSTOM_SetStaticDoubleField
    "UnityEngine.AndroidJNI::INTERNAL_CALL_ToBooleanArray",    // -> AndroidJNI_CUSTOM_INTERNAL_CALL_ToBooleanArray
    "UnityEngine.AndroidJNI::INTERNAL_CALL_ToByteArray",    // -> AndroidJNI_CUSTOM_INTERNAL_CALL_ToByteArray
    "UnityEngine.AndroidJNI::INTERNAL_CALL_ToCharArray",    // -> AndroidJNI_CUSTOM_INTERNAL_CALL_ToCharArray
    "UnityEngine.AndroidJNI::INTERNAL_CALL_ToShortArray",    // -> AndroidJNI_CUSTOM_INTERNAL_CALL_ToShortArray
    "UnityEngine.AndroidJNI::INTERNAL_CALL_ToIntArray",    // -> AndroidJNI_CUSTOM_INTERNAL_CALL_ToIntArray
    "UnityEngine.AndroidJNI::INTERNAL_CALL_ToLongArray",    // -> AndroidJNI_CUSTOM_INTERNAL_CALL_ToLongArray
    "UnityEngine.AndroidJNI::INTERNAL_CALL_ToFloatArray",    // -> AndroidJNI_CUSTOM_INTERNAL_CALL_ToFloatArray
    "UnityEngine.AndroidJNI::INTERNAL_CALL_ToDoubleArray",    // -> AndroidJNI_CUSTOM_INTERNAL_CALL_ToDoubleArray
    "UnityEngine.AndroidJNI::INTERNAL_CALL_ToObjectArray",    // -> AndroidJNI_CUSTOM_INTERNAL_CALL_ToObjectArray
    "UnityEngine.AndroidJNI::FromBooleanArray",    // -> AndroidJNI_CUSTOM_FromBooleanArray
    "UnityEngine.AndroidJNI::FromByteArray" ,    // -> AndroidJNI_CUSTOM_FromByteArray
    "UnityEngine.AndroidJNI::FromCharArray" ,    // -> AndroidJNI_CUSTOM_FromCharArray
    "UnityEngine.AndroidJNI::FromShortArray",    // -> AndroidJNI_CUSTOM_FromShortArray
    "UnityEngine.AndroidJNI::FromIntArray"  ,    // -> AndroidJNI_CUSTOM_FromIntArray
    "UnityEngine.AndroidJNI::FromLongArray" ,    // -> AndroidJNI_CUSTOM_FromLongArray
    "UnityEngine.AndroidJNI::FromFloatArray",    // -> AndroidJNI_CUSTOM_FromFloatArray
    "UnityEngine.AndroidJNI::FromDoubleArray",    // -> AndroidJNI_CUSTOM_FromDoubleArray
    "UnityEngine.AndroidJNI::FromObjectArray",    // -> AndroidJNI_CUSTOM_FromObjectArray
    "UnityEngine.AndroidJNI::GetArrayLength",    // -> AndroidJNI_CUSTOM_GetArrayLength
    "UnityEngine.AndroidJNI::INTERNAL_CALL_NewBooleanArray",    // -> AndroidJNI_CUSTOM_INTERNAL_CALL_NewBooleanArray
    "UnityEngine.AndroidJNI::INTERNAL_CALL_NewByteArray",    // -> AndroidJNI_CUSTOM_INTERNAL_CALL_NewByteArray
    "UnityEngine.AndroidJNI::INTERNAL_CALL_NewCharArray",    // -> AndroidJNI_CUSTOM_INTERNAL_CALL_NewCharArray
    "UnityEngine.AndroidJNI::INTERNAL_CALL_NewShortArray",    // -> AndroidJNI_CUSTOM_INTERNAL_CALL_NewShortArray
    "UnityEngine.AndroidJNI::INTERNAL_CALL_NewIntArray",    // -> AndroidJNI_CUSTOM_INTERNAL_CALL_NewIntArray
    "UnityEngine.AndroidJNI::INTERNAL_CALL_NewLongArray",    // -> AndroidJNI_CUSTOM_INTERNAL_CALL_NewLongArray
    "UnityEngine.AndroidJNI::INTERNAL_CALL_NewFloatArray",    // -> AndroidJNI_CUSTOM_INTERNAL_CALL_NewFloatArray
    "UnityEngine.AndroidJNI::INTERNAL_CALL_NewDoubleArray",    // -> AndroidJNI_CUSTOM_INTERNAL_CALL_NewDoubleArray
    "UnityEngine.AndroidJNI::INTERNAL_CALL_NewObjectArray",    // -> AndroidJNI_CUSTOM_INTERNAL_CALL_NewObjectArray
    "UnityEngine.AndroidJNI::GetBooleanArrayElement",    // -> AndroidJNI_CUSTOM_GetBooleanArrayElement
    "UnityEngine.AndroidJNI::GetByteArrayElement",    // -> AndroidJNI_CUSTOM_GetByteArrayElement
    "UnityEngine.AndroidJNI::GetCharArrayElement",    // -> AndroidJNI_CUSTOM_GetCharArrayElement
    "UnityEngine.AndroidJNI::GetShortArrayElement",    // -> AndroidJNI_CUSTOM_GetShortArrayElement
    "UnityEngine.AndroidJNI::GetIntArrayElement",    // -> AndroidJNI_CUSTOM_GetIntArrayElement
    "UnityEngine.AndroidJNI::GetLongArrayElement",    // -> AndroidJNI_CUSTOM_GetLongArrayElement
    "UnityEngine.AndroidJNI::GetFloatArrayElement",    // -> AndroidJNI_CUSTOM_GetFloatArrayElement
    "UnityEngine.AndroidJNI::GetDoubleArrayElement",    // -> AndroidJNI_CUSTOM_GetDoubleArrayElement
    "UnityEngine.AndroidJNI::INTERNAL_CALL_GetObjectArrayElement",    // -> AndroidJNI_CUSTOM_INTERNAL_CALL_GetObjectArrayElement
    "UnityEngine.AndroidJNI::SetBooleanArrayElement",    // -> AndroidJNI_CUSTOM_SetBooleanArrayElement
    "UnityEngine.AndroidJNI::SetByteArrayElement",    // -> AndroidJNI_CUSTOM_SetByteArrayElement
    "UnityEngine.AndroidJNI::SetCharArrayElement",    // -> AndroidJNI_CUSTOM_SetCharArrayElement
    "UnityEngine.AndroidJNI::SetShortArrayElement",    // -> AndroidJNI_CUSTOM_SetShortArrayElement
    "UnityEngine.AndroidJNI::SetIntArrayElement",    // -> AndroidJNI_CUSTOM_SetIntArrayElement
    "UnityEngine.AndroidJNI::SetLongArrayElement",    // -> AndroidJNI_CUSTOM_SetLongArrayElement
    "UnityEngine.AndroidJNI::SetFloatArrayElement",    // -> AndroidJNI_CUSTOM_SetFloatArrayElement
    "UnityEngine.AndroidJNI::SetDoubleArrayElement",    // -> AndroidJNI_CUSTOM_SetDoubleArrayElement
    "UnityEngine.AndroidJNI::SetObjectArrayElement",    // -> AndroidJNI_CUSTOM_SetObjectArrayElement
#endif
    NULL
};

static const void* s_AndroidJNI_IcallFuncs [] =
{
#if !PLATFORM_WINRT
    (const void*)&AndroidJNIHelper_Get_Custom_PropDebug   ,  //  <- UnityEngine.AndroidJNIHelper::get_debug
    (const void*)&AndroidJNIHelper_Set_Custom_PropDebug   ,  //  <- UnityEngine.AndroidJNIHelper::set_debug
    (const void*)&AndroidJNIHelper_CUSTOM_INTERNAL_CALL_CreateJavaProxy,  //  <- UnityEngine.AndroidJNIHelper::INTERNAL_CALL_CreateJavaProxy
    (const void*)&AndroidJNI_CUSTOM_AttachCurrentThread   ,  //  <- UnityEngine.AndroidJNI::AttachCurrentThread
    (const void*)&AndroidJNI_CUSTOM_DetachCurrentThread   ,  //  <- UnityEngine.AndroidJNI::DetachCurrentThread
    (const void*)&AndroidJNI_CUSTOM_GetVersion            ,  //  <- UnityEngine.AndroidJNI::GetVersion
    (const void*)&AndroidJNI_CUSTOM_INTERNAL_CALL_FindClass,  //  <- UnityEngine.AndroidJNI::INTERNAL_CALL_FindClass
    (const void*)&AndroidJNI_CUSTOM_INTERNAL_CALL_FromReflectedMethod,  //  <- UnityEngine.AndroidJNI::INTERNAL_CALL_FromReflectedMethod
    (const void*)&AndroidJNI_CUSTOM_INTERNAL_CALL_FromReflectedField,  //  <- UnityEngine.AndroidJNI::INTERNAL_CALL_FromReflectedField
    (const void*)&AndroidJNI_CUSTOM_INTERNAL_CALL_ToReflectedMethod,  //  <- UnityEngine.AndroidJNI::INTERNAL_CALL_ToReflectedMethod
    (const void*)&AndroidJNI_CUSTOM_INTERNAL_CALL_ToReflectedField,  //  <- UnityEngine.AndroidJNI::INTERNAL_CALL_ToReflectedField
    (const void*)&AndroidJNI_CUSTOM_INTERNAL_CALL_GetSuperclass,  //  <- UnityEngine.AndroidJNI::INTERNAL_CALL_GetSuperclass
    (const void*)&AndroidJNI_CUSTOM_IsAssignableFrom      ,  //  <- UnityEngine.AndroidJNI::IsAssignableFrom
    (const void*)&AndroidJNI_CUSTOM_Throw                 ,  //  <- UnityEngine.AndroidJNI::Throw
    (const void*)&AndroidJNI_CUSTOM_ThrowNew              ,  //  <- UnityEngine.AndroidJNI::ThrowNew
    (const void*)&AndroidJNI_CUSTOM_INTERNAL_CALL_ExceptionOccurred,  //  <- UnityEngine.AndroidJNI::INTERNAL_CALL_ExceptionOccurred
    (const void*)&AndroidJNI_CUSTOM_ExceptionDescribe     ,  //  <- UnityEngine.AndroidJNI::ExceptionDescribe
    (const void*)&AndroidJNI_CUSTOM_ExceptionClear        ,  //  <- UnityEngine.AndroidJNI::ExceptionClear
    (const void*)&AndroidJNI_CUSTOM_FatalError            ,  //  <- UnityEngine.AndroidJNI::FatalError
    (const void*)&AndroidJNI_CUSTOM_PushLocalFrame        ,  //  <- UnityEngine.AndroidJNI::PushLocalFrame
    (const void*)&AndroidJNI_CUSTOM_INTERNAL_CALL_PopLocalFrame,  //  <- UnityEngine.AndroidJNI::INTERNAL_CALL_PopLocalFrame
    (const void*)&AndroidJNI_CUSTOM_INTERNAL_CALL_NewGlobalRef,  //  <- UnityEngine.AndroidJNI::INTERNAL_CALL_NewGlobalRef
    (const void*)&AndroidJNI_CUSTOM_DeleteGlobalRef       ,  //  <- UnityEngine.AndroidJNI::DeleteGlobalRef
    (const void*)&AndroidJNI_CUSTOM_INTERNAL_CALL_NewLocalRef,  //  <- UnityEngine.AndroidJNI::INTERNAL_CALL_NewLocalRef
    (const void*)&AndroidJNI_CUSTOM_DeleteLocalRef        ,  //  <- UnityEngine.AndroidJNI::DeleteLocalRef
    (const void*)&AndroidJNI_CUSTOM_IsSameObject          ,  //  <- UnityEngine.AndroidJNI::IsSameObject
    (const void*)&AndroidJNI_CUSTOM_EnsureLocalCapacity   ,  //  <- UnityEngine.AndroidJNI::EnsureLocalCapacity
    (const void*)&AndroidJNI_CUSTOM_INTERNAL_CALL_AllocObject,  //  <- UnityEngine.AndroidJNI::INTERNAL_CALL_AllocObject
    (const void*)&AndroidJNI_CUSTOM_INTERNAL_CALL_NewObject,  //  <- UnityEngine.AndroidJNI::INTERNAL_CALL_NewObject
    (const void*)&AndroidJNI_CUSTOM_INTERNAL_CALL_GetObjectClass,  //  <- UnityEngine.AndroidJNI::INTERNAL_CALL_GetObjectClass
    (const void*)&AndroidJNI_CUSTOM_IsInstanceOf          ,  //  <- UnityEngine.AndroidJNI::IsInstanceOf
    (const void*)&AndroidJNI_CUSTOM_INTERNAL_CALL_GetMethodID,  //  <- UnityEngine.AndroidJNI::INTERNAL_CALL_GetMethodID
    (const void*)&AndroidJNI_CUSTOM_INTERNAL_CALL_GetFieldID,  //  <- UnityEngine.AndroidJNI::INTERNAL_CALL_GetFieldID
    (const void*)&AndroidJNI_CUSTOM_INTERNAL_CALL_GetStaticMethodID,  //  <- UnityEngine.AndroidJNI::INTERNAL_CALL_GetStaticMethodID
    (const void*)&AndroidJNI_CUSTOM_INTERNAL_CALL_GetStaticFieldID,  //  <- UnityEngine.AndroidJNI::INTERNAL_CALL_GetStaticFieldID
    (const void*)&AndroidJNI_CUSTOM_INTERNAL_CALL_NewStringUTF,  //  <- UnityEngine.AndroidJNI::INTERNAL_CALL_NewStringUTF
    (const void*)&AndroidJNI_CUSTOM_GetStringUTFLength    ,  //  <- UnityEngine.AndroidJNI::GetStringUTFLength
    (const void*)&AndroidJNI_CUSTOM_GetStringUTFChars     ,  //  <- UnityEngine.AndroidJNI::GetStringUTFChars
    (const void*)&AndroidJNI_CUSTOM_CallStringMethod      ,  //  <- UnityEngine.AndroidJNI::CallStringMethod
    (const void*)&AndroidJNI_CUSTOM_INTERNAL_CALL_CallObjectMethod,  //  <- UnityEngine.AndroidJNI::INTERNAL_CALL_CallObjectMethod
    (const void*)&AndroidJNI_CUSTOM_CallIntMethod         ,  //  <- UnityEngine.AndroidJNI::CallIntMethod
    (const void*)&AndroidJNI_CUSTOM_CallBooleanMethod     ,  //  <- UnityEngine.AndroidJNI::CallBooleanMethod
    (const void*)&AndroidJNI_CUSTOM_CallShortMethod       ,  //  <- UnityEngine.AndroidJNI::CallShortMethod
    (const void*)&AndroidJNI_CUSTOM_CallByteMethod        ,  //  <- UnityEngine.AndroidJNI::CallByteMethod
    (const void*)&AndroidJNI_CUSTOM_CallCharMethod        ,  //  <- UnityEngine.AndroidJNI::CallCharMethod
    (const void*)&AndroidJNI_CUSTOM_CallFloatMethod       ,  //  <- UnityEngine.AndroidJNI::CallFloatMethod
    (const void*)&AndroidJNI_CUSTOM_CallDoubleMethod      ,  //  <- UnityEngine.AndroidJNI::CallDoubleMethod
    (const void*)&AndroidJNI_CUSTOM_CallLongMethod        ,  //  <- UnityEngine.AndroidJNI::CallLongMethod
    (const void*)&AndroidJNI_CUSTOM_CallVoidMethod        ,  //  <- UnityEngine.AndroidJNI::CallVoidMethod
    (const void*)&AndroidJNI_CUSTOM_GetStringField        ,  //  <- UnityEngine.AndroidJNI::GetStringField
    (const void*)&AndroidJNI_CUSTOM_INTERNAL_CALL_GetObjectField,  //  <- UnityEngine.AndroidJNI::INTERNAL_CALL_GetObjectField
    (const void*)&AndroidJNI_CUSTOM_GetBooleanField       ,  //  <- UnityEngine.AndroidJNI::GetBooleanField
    (const void*)&AndroidJNI_CUSTOM_GetByteField          ,  //  <- UnityEngine.AndroidJNI::GetByteField
    (const void*)&AndroidJNI_CUSTOM_GetCharField          ,  //  <- UnityEngine.AndroidJNI::GetCharField
    (const void*)&AndroidJNI_CUSTOM_GetShortField         ,  //  <- UnityEngine.AndroidJNI::GetShortField
    (const void*)&AndroidJNI_CUSTOM_GetIntField           ,  //  <- UnityEngine.AndroidJNI::GetIntField
    (const void*)&AndroidJNI_CUSTOM_GetLongField          ,  //  <- UnityEngine.AndroidJNI::GetLongField
    (const void*)&AndroidJNI_CUSTOM_GetFloatField         ,  //  <- UnityEngine.AndroidJNI::GetFloatField
    (const void*)&AndroidJNI_CUSTOM_GetDoubleField        ,  //  <- UnityEngine.AndroidJNI::GetDoubleField
    (const void*)&AndroidJNI_CUSTOM_SetStringField        ,  //  <- UnityEngine.AndroidJNI::SetStringField
    (const void*)&AndroidJNI_CUSTOM_SetObjectField        ,  //  <- UnityEngine.AndroidJNI::SetObjectField
    (const void*)&AndroidJNI_CUSTOM_SetBooleanField       ,  //  <- UnityEngine.AndroidJNI::SetBooleanField
    (const void*)&AndroidJNI_CUSTOM_SetByteField          ,  //  <- UnityEngine.AndroidJNI::SetByteField
    (const void*)&AndroidJNI_CUSTOM_SetCharField          ,  //  <- UnityEngine.AndroidJNI::SetCharField
    (const void*)&AndroidJNI_CUSTOM_SetShortField         ,  //  <- UnityEngine.AndroidJNI::SetShortField
    (const void*)&AndroidJNI_CUSTOM_SetIntField           ,  //  <- UnityEngine.AndroidJNI::SetIntField
    (const void*)&AndroidJNI_CUSTOM_SetLongField          ,  //  <- UnityEngine.AndroidJNI::SetLongField
    (const void*)&AndroidJNI_CUSTOM_SetFloatField         ,  //  <- UnityEngine.AndroidJNI::SetFloatField
    (const void*)&AndroidJNI_CUSTOM_SetDoubleField        ,  //  <- UnityEngine.AndroidJNI::SetDoubleField
    (const void*)&AndroidJNI_CUSTOM_CallStaticStringMethod,  //  <- UnityEngine.AndroidJNI::CallStaticStringMethod
    (const void*)&AndroidJNI_CUSTOM_INTERNAL_CALL_CallStaticObjectMethod,  //  <- UnityEngine.AndroidJNI::INTERNAL_CALL_CallStaticObjectMethod
    (const void*)&AndroidJNI_CUSTOM_CallStaticIntMethod   ,  //  <- UnityEngine.AndroidJNI::CallStaticIntMethod
    (const void*)&AndroidJNI_CUSTOM_CallStaticBooleanMethod,  //  <- UnityEngine.AndroidJNI::CallStaticBooleanMethod
    (const void*)&AndroidJNI_CUSTOM_CallStaticShortMethod ,  //  <- UnityEngine.AndroidJNI::CallStaticShortMethod
    (const void*)&AndroidJNI_CUSTOM_CallStaticByteMethod  ,  //  <- UnityEngine.AndroidJNI::CallStaticByteMethod
    (const void*)&AndroidJNI_CUSTOM_CallStaticCharMethod  ,  //  <- UnityEngine.AndroidJNI::CallStaticCharMethod
    (const void*)&AndroidJNI_CUSTOM_CallStaticFloatMethod ,  //  <- UnityEngine.AndroidJNI::CallStaticFloatMethod
    (const void*)&AndroidJNI_CUSTOM_CallStaticDoubleMethod,  //  <- UnityEngine.AndroidJNI::CallStaticDoubleMethod
    (const void*)&AndroidJNI_CUSTOM_CallStaticLongMethod  ,  //  <- UnityEngine.AndroidJNI::CallStaticLongMethod
    (const void*)&AndroidJNI_CUSTOM_CallStaticVoidMethod  ,  //  <- UnityEngine.AndroidJNI::CallStaticVoidMethod
    (const void*)&AndroidJNI_CUSTOM_GetStaticStringField  ,  //  <- UnityEngine.AndroidJNI::GetStaticStringField
    (const void*)&AndroidJNI_CUSTOM_INTERNAL_CALL_GetStaticObjectField,  //  <- UnityEngine.AndroidJNI::INTERNAL_CALL_GetStaticObjectField
    (const void*)&AndroidJNI_CUSTOM_GetStaticBooleanField ,  //  <- UnityEngine.AndroidJNI::GetStaticBooleanField
    (const void*)&AndroidJNI_CUSTOM_GetStaticByteField    ,  //  <- UnityEngine.AndroidJNI::GetStaticByteField
    (const void*)&AndroidJNI_CUSTOM_GetStaticCharField    ,  //  <- UnityEngine.AndroidJNI::GetStaticCharField
    (const void*)&AndroidJNI_CUSTOM_GetStaticShortField   ,  //  <- UnityEngine.AndroidJNI::GetStaticShortField
    (const void*)&AndroidJNI_CUSTOM_GetStaticIntField     ,  //  <- UnityEngine.AndroidJNI::GetStaticIntField
    (const void*)&AndroidJNI_CUSTOM_GetStaticLongField    ,  //  <- UnityEngine.AndroidJNI::GetStaticLongField
    (const void*)&AndroidJNI_CUSTOM_GetStaticFloatField   ,  //  <- UnityEngine.AndroidJNI::GetStaticFloatField
    (const void*)&AndroidJNI_CUSTOM_GetStaticDoubleField  ,  //  <- UnityEngine.AndroidJNI::GetStaticDoubleField
    (const void*)&AndroidJNI_CUSTOM_SetStaticStringField  ,  //  <- UnityEngine.AndroidJNI::SetStaticStringField
    (const void*)&AndroidJNI_CUSTOM_SetStaticObjectField  ,  //  <- UnityEngine.AndroidJNI::SetStaticObjectField
    (const void*)&AndroidJNI_CUSTOM_SetStaticBooleanField ,  //  <- UnityEngine.AndroidJNI::SetStaticBooleanField
    (const void*)&AndroidJNI_CUSTOM_SetStaticByteField    ,  //  <- UnityEngine.AndroidJNI::SetStaticByteField
    (const void*)&AndroidJNI_CUSTOM_SetStaticCharField    ,  //  <- UnityEngine.AndroidJNI::SetStaticCharField
    (const void*)&AndroidJNI_CUSTOM_SetStaticShortField   ,  //  <- UnityEngine.AndroidJNI::SetStaticShortField
    (const void*)&AndroidJNI_CUSTOM_SetStaticIntField     ,  //  <- UnityEngine.AndroidJNI::SetStaticIntField
    (const void*)&AndroidJNI_CUSTOM_SetStaticLongField    ,  //  <- UnityEngine.AndroidJNI::SetStaticLongField
    (const void*)&AndroidJNI_CUSTOM_SetStaticFloatField   ,  //  <- UnityEngine.AndroidJNI::SetStaticFloatField
    (const void*)&AndroidJNI_CUSTOM_SetStaticDoubleField  ,  //  <- UnityEngine.AndroidJNI::SetStaticDoubleField
    (const void*)&AndroidJNI_CUSTOM_INTERNAL_CALL_ToBooleanArray,  //  <- UnityEngine.AndroidJNI::INTERNAL_CALL_ToBooleanArray
    (const void*)&AndroidJNI_CUSTOM_INTERNAL_CALL_ToByteArray,  //  <- UnityEngine.AndroidJNI::INTERNAL_CALL_ToByteArray
    (const void*)&AndroidJNI_CUSTOM_INTERNAL_CALL_ToCharArray,  //  <- UnityEngine.AndroidJNI::INTERNAL_CALL_ToCharArray
    (const void*)&AndroidJNI_CUSTOM_INTERNAL_CALL_ToShortArray,  //  <- UnityEngine.AndroidJNI::INTERNAL_CALL_ToShortArray
    (const void*)&AndroidJNI_CUSTOM_INTERNAL_CALL_ToIntArray,  //  <- UnityEngine.AndroidJNI::INTERNAL_CALL_ToIntArray
    (const void*)&AndroidJNI_CUSTOM_INTERNAL_CALL_ToLongArray,  //  <- UnityEngine.AndroidJNI::INTERNAL_CALL_ToLongArray
    (const void*)&AndroidJNI_CUSTOM_INTERNAL_CALL_ToFloatArray,  //  <- UnityEngine.AndroidJNI::INTERNAL_CALL_ToFloatArray
    (const void*)&AndroidJNI_CUSTOM_INTERNAL_CALL_ToDoubleArray,  //  <- UnityEngine.AndroidJNI::INTERNAL_CALL_ToDoubleArray
    (const void*)&AndroidJNI_CUSTOM_INTERNAL_CALL_ToObjectArray,  //  <- UnityEngine.AndroidJNI::INTERNAL_CALL_ToObjectArray
    (const void*)&AndroidJNI_CUSTOM_FromBooleanArray      ,  //  <- UnityEngine.AndroidJNI::FromBooleanArray
    (const void*)&AndroidJNI_CUSTOM_FromByteArray         ,  //  <- UnityEngine.AndroidJNI::FromByteArray
    (const void*)&AndroidJNI_CUSTOM_FromCharArray         ,  //  <- UnityEngine.AndroidJNI::FromCharArray
    (const void*)&AndroidJNI_CUSTOM_FromShortArray        ,  //  <- UnityEngine.AndroidJNI::FromShortArray
    (const void*)&AndroidJNI_CUSTOM_FromIntArray          ,  //  <- UnityEngine.AndroidJNI::FromIntArray
    (const void*)&AndroidJNI_CUSTOM_FromLongArray         ,  //  <- UnityEngine.AndroidJNI::FromLongArray
    (const void*)&AndroidJNI_CUSTOM_FromFloatArray        ,  //  <- UnityEngine.AndroidJNI::FromFloatArray
    (const void*)&AndroidJNI_CUSTOM_FromDoubleArray       ,  //  <- UnityEngine.AndroidJNI::FromDoubleArray
    (const void*)&AndroidJNI_CUSTOM_FromObjectArray       ,  //  <- UnityEngine.AndroidJNI::FromObjectArray
    (const void*)&AndroidJNI_CUSTOM_GetArrayLength        ,  //  <- UnityEngine.AndroidJNI::GetArrayLength
    (const void*)&AndroidJNI_CUSTOM_INTERNAL_CALL_NewBooleanArray,  //  <- UnityEngine.AndroidJNI::INTERNAL_CALL_NewBooleanArray
    (const void*)&AndroidJNI_CUSTOM_INTERNAL_CALL_NewByteArray,  //  <- UnityEngine.AndroidJNI::INTERNAL_CALL_NewByteArray
    (const void*)&AndroidJNI_CUSTOM_INTERNAL_CALL_NewCharArray,  //  <- UnityEngine.AndroidJNI::INTERNAL_CALL_NewCharArray
    (const void*)&AndroidJNI_CUSTOM_INTERNAL_CALL_NewShortArray,  //  <- UnityEngine.AndroidJNI::INTERNAL_CALL_NewShortArray
    (const void*)&AndroidJNI_CUSTOM_INTERNAL_CALL_NewIntArray,  //  <- UnityEngine.AndroidJNI::INTERNAL_CALL_NewIntArray
    (const void*)&AndroidJNI_CUSTOM_INTERNAL_CALL_NewLongArray,  //  <- UnityEngine.AndroidJNI::INTERNAL_CALL_NewLongArray
    (const void*)&AndroidJNI_CUSTOM_INTERNAL_CALL_NewFloatArray,  //  <- UnityEngine.AndroidJNI::INTERNAL_CALL_NewFloatArray
    (const void*)&AndroidJNI_CUSTOM_INTERNAL_CALL_NewDoubleArray,  //  <- UnityEngine.AndroidJNI::INTERNAL_CALL_NewDoubleArray
    (const void*)&AndroidJNI_CUSTOM_INTERNAL_CALL_NewObjectArray,  //  <- UnityEngine.AndroidJNI::INTERNAL_CALL_NewObjectArray
    (const void*)&AndroidJNI_CUSTOM_GetBooleanArrayElement,  //  <- UnityEngine.AndroidJNI::GetBooleanArrayElement
    (const void*)&AndroidJNI_CUSTOM_GetByteArrayElement   ,  //  <- UnityEngine.AndroidJNI::GetByteArrayElement
    (const void*)&AndroidJNI_CUSTOM_GetCharArrayElement   ,  //  <- UnityEngine.AndroidJNI::GetCharArrayElement
    (const void*)&AndroidJNI_CUSTOM_GetShortArrayElement  ,  //  <- UnityEngine.AndroidJNI::GetShortArrayElement
    (const void*)&AndroidJNI_CUSTOM_GetIntArrayElement    ,  //  <- UnityEngine.AndroidJNI::GetIntArrayElement
    (const void*)&AndroidJNI_CUSTOM_GetLongArrayElement   ,  //  <- UnityEngine.AndroidJNI::GetLongArrayElement
    (const void*)&AndroidJNI_CUSTOM_GetFloatArrayElement  ,  //  <- UnityEngine.AndroidJNI::GetFloatArrayElement
    (const void*)&AndroidJNI_CUSTOM_GetDoubleArrayElement ,  //  <- UnityEngine.AndroidJNI::GetDoubleArrayElement
    (const void*)&AndroidJNI_CUSTOM_INTERNAL_CALL_GetObjectArrayElement,  //  <- UnityEngine.AndroidJNI::INTERNAL_CALL_GetObjectArrayElement
    (const void*)&AndroidJNI_CUSTOM_SetBooleanArrayElement,  //  <- UnityEngine.AndroidJNI::SetBooleanArrayElement
    (const void*)&AndroidJNI_CUSTOM_SetByteArrayElement   ,  //  <- UnityEngine.AndroidJNI::SetByteArrayElement
    (const void*)&AndroidJNI_CUSTOM_SetCharArrayElement   ,  //  <- UnityEngine.AndroidJNI::SetCharArrayElement
    (const void*)&AndroidJNI_CUSTOM_SetShortArrayElement  ,  //  <- UnityEngine.AndroidJNI::SetShortArrayElement
    (const void*)&AndroidJNI_CUSTOM_SetIntArrayElement    ,  //  <- UnityEngine.AndroidJNI::SetIntArrayElement
    (const void*)&AndroidJNI_CUSTOM_SetLongArrayElement   ,  //  <- UnityEngine.AndroidJNI::SetLongArrayElement
    (const void*)&AndroidJNI_CUSTOM_SetFloatArrayElement  ,  //  <- UnityEngine.AndroidJNI::SetFloatArrayElement
    (const void*)&AndroidJNI_CUSTOM_SetDoubleArrayElement ,  //  <- UnityEngine.AndroidJNI::SetDoubleArrayElement
    (const void*)&AndroidJNI_CUSTOM_SetObjectArrayElement ,  //  <- UnityEngine.AndroidJNI::SetObjectArrayElement
#endif
    NULL
};

void ExportAndroidJNIBindings();
void ExportAndroidJNIBindings()
{
    for (int i = 0; s_AndroidJNI_IcallNames [i] != NULL; ++i )
        scripting_add_internal_call( s_AndroidJNI_IcallNames [i], s_AndroidJNI_IcallFuncs [i] );
}

#elif ENABLE_DOTNET
#include "Runtime/Scripting/WinRTHelper.h"
void ExportAndroidJNIBindings()
{
    #if UNITY_WP8
    extern intptr_t g_WinRTFuncPtrs[];
    #define SET_METRO_BINDING(Name) g_WinRTFuncPtrs[k##Name##FuncDef] = reinterpret_cast<intptr_t>(Name);
    #else
    long long* p = GetWinRTFuncDefsPointers();
    #define SET_METRO_BINDING(Name) p[k##Name##Func] = (long long)Name;
    #endif
#if !PLATFORM_WINRT
    SET_METRO_BINDING(AndroidJNIHelper_Get_Custom_PropDebug); //  <- UnityEngine.AndroidJNIHelper::get_debug
    SET_METRO_BINDING(AndroidJNIHelper_Set_Custom_PropDebug); //  <- UnityEngine.AndroidJNIHelper::set_debug
    SET_METRO_BINDING(AndroidJNIHelper_CUSTOM_INTERNAL_CALL_CreateJavaProxy); //  <- UnityEngine.AndroidJNIHelper::INTERNAL_CALL_CreateJavaProxy
    SET_METRO_BINDING(AndroidJNI_CUSTOM_AttachCurrentThread); //  <- UnityEngine.AndroidJNI::AttachCurrentThread
    SET_METRO_BINDING(AndroidJNI_CUSTOM_DetachCurrentThread); //  <- UnityEngine.AndroidJNI::DetachCurrentThread
    SET_METRO_BINDING(AndroidJNI_CUSTOM_GetVersion); //  <- UnityEngine.AndroidJNI::GetVersion
    SET_METRO_BINDING(AndroidJNI_CUSTOM_INTERNAL_CALL_FindClass); //  <- UnityEngine.AndroidJNI::INTERNAL_CALL_FindClass
    SET_METRO_BINDING(AndroidJNI_CUSTOM_INTERNAL_CALL_FromReflectedMethod); //  <- UnityEngine.AndroidJNI::INTERNAL_CALL_FromReflectedMethod
    SET_METRO_BINDING(AndroidJNI_CUSTOM_INTERNAL_CALL_FromReflectedField); //  <- UnityEngine.AndroidJNI::INTERNAL_CALL_FromReflectedField
    SET_METRO_BINDING(AndroidJNI_CUSTOM_INTERNAL_CALL_ToReflectedMethod); //  <- UnityEngine.AndroidJNI::INTERNAL_CALL_ToReflectedMethod
    SET_METRO_BINDING(AndroidJNI_CUSTOM_INTERNAL_CALL_ToReflectedField); //  <- UnityEngine.AndroidJNI::INTERNAL_CALL_ToReflectedField
    SET_METRO_BINDING(AndroidJNI_CUSTOM_INTERNAL_CALL_GetSuperclass); //  <- UnityEngine.AndroidJNI::INTERNAL_CALL_GetSuperclass
    SET_METRO_BINDING(AndroidJNI_CUSTOM_IsAssignableFrom); //  <- UnityEngine.AndroidJNI::IsAssignableFrom
    SET_METRO_BINDING(AndroidJNI_CUSTOM_Throw); //  <- UnityEngine.AndroidJNI::Throw
    SET_METRO_BINDING(AndroidJNI_CUSTOM_ThrowNew); //  <- UnityEngine.AndroidJNI::ThrowNew
    SET_METRO_BINDING(AndroidJNI_CUSTOM_INTERNAL_CALL_ExceptionOccurred); //  <- UnityEngine.AndroidJNI::INTERNAL_CALL_ExceptionOccurred
    SET_METRO_BINDING(AndroidJNI_CUSTOM_ExceptionDescribe); //  <- UnityEngine.AndroidJNI::ExceptionDescribe
    SET_METRO_BINDING(AndroidJNI_CUSTOM_ExceptionClear); //  <- UnityEngine.AndroidJNI::ExceptionClear
    SET_METRO_BINDING(AndroidJNI_CUSTOM_FatalError); //  <- UnityEngine.AndroidJNI::FatalError
    SET_METRO_BINDING(AndroidJNI_CUSTOM_PushLocalFrame); //  <- UnityEngine.AndroidJNI::PushLocalFrame
    SET_METRO_BINDING(AndroidJNI_CUSTOM_INTERNAL_CALL_PopLocalFrame); //  <- UnityEngine.AndroidJNI::INTERNAL_CALL_PopLocalFrame
    SET_METRO_BINDING(AndroidJNI_CUSTOM_INTERNAL_CALL_NewGlobalRef); //  <- UnityEngine.AndroidJNI::INTERNAL_CALL_NewGlobalRef
    SET_METRO_BINDING(AndroidJNI_CUSTOM_DeleteGlobalRef); //  <- UnityEngine.AndroidJNI::DeleteGlobalRef
    SET_METRO_BINDING(AndroidJNI_CUSTOM_INTERNAL_CALL_NewLocalRef); //  <- UnityEngine.AndroidJNI::INTERNAL_CALL_NewLocalRef
    SET_METRO_BINDING(AndroidJNI_CUSTOM_DeleteLocalRef); //  <- UnityEngine.AndroidJNI::DeleteLocalRef
    SET_METRO_BINDING(AndroidJNI_CUSTOM_IsSameObject); //  <- UnityEngine.AndroidJNI::IsSameObject
    SET_METRO_BINDING(AndroidJNI_CUSTOM_EnsureLocalCapacity); //  <- UnityEngine.AndroidJNI::EnsureLocalCapacity
    SET_METRO_BINDING(AndroidJNI_CUSTOM_INTERNAL_CALL_AllocObject); //  <- UnityEngine.AndroidJNI::INTERNAL_CALL_AllocObject
    SET_METRO_BINDING(AndroidJNI_CUSTOM_INTERNAL_CALL_NewObject); //  <- UnityEngine.AndroidJNI::INTERNAL_CALL_NewObject
    SET_METRO_BINDING(AndroidJNI_CUSTOM_INTERNAL_CALL_GetObjectClass); //  <- UnityEngine.AndroidJNI::INTERNAL_CALL_GetObjectClass
    SET_METRO_BINDING(AndroidJNI_CUSTOM_IsInstanceOf); //  <- UnityEngine.AndroidJNI::IsInstanceOf
    SET_METRO_BINDING(AndroidJNI_CUSTOM_INTERNAL_CALL_GetMethodID); //  <- UnityEngine.AndroidJNI::INTERNAL_CALL_GetMethodID
    SET_METRO_BINDING(AndroidJNI_CUSTOM_INTERNAL_CALL_GetFieldID); //  <- UnityEngine.AndroidJNI::INTERNAL_CALL_GetFieldID
    SET_METRO_BINDING(AndroidJNI_CUSTOM_INTERNAL_CALL_GetStaticMethodID); //  <- UnityEngine.AndroidJNI::INTERNAL_CALL_GetStaticMethodID
    SET_METRO_BINDING(AndroidJNI_CUSTOM_INTERNAL_CALL_GetStaticFieldID); //  <- UnityEngine.AndroidJNI::INTERNAL_CALL_GetStaticFieldID
    SET_METRO_BINDING(AndroidJNI_CUSTOM_INTERNAL_CALL_NewStringUTF); //  <- UnityEngine.AndroidJNI::INTERNAL_CALL_NewStringUTF
    SET_METRO_BINDING(AndroidJNI_CUSTOM_GetStringUTFLength); //  <- UnityEngine.AndroidJNI::GetStringUTFLength
    SET_METRO_BINDING(AndroidJNI_CUSTOM_GetStringUTFChars); //  <- UnityEngine.AndroidJNI::GetStringUTFChars
    SET_METRO_BINDING(AndroidJNI_CUSTOM_CallStringMethod); //  <- UnityEngine.AndroidJNI::CallStringMethod
    SET_METRO_BINDING(AndroidJNI_CUSTOM_INTERNAL_CALL_CallObjectMethod); //  <- UnityEngine.AndroidJNI::INTERNAL_CALL_CallObjectMethod
    SET_METRO_BINDING(AndroidJNI_CUSTOM_CallIntMethod); //  <- UnityEngine.AndroidJNI::CallIntMethod
    SET_METRO_BINDING(AndroidJNI_CUSTOM_CallBooleanMethod); //  <- UnityEngine.AndroidJNI::CallBooleanMethod
    SET_METRO_BINDING(AndroidJNI_CUSTOM_CallShortMethod); //  <- UnityEngine.AndroidJNI::CallShortMethod
    SET_METRO_BINDING(AndroidJNI_CUSTOM_CallByteMethod); //  <- UnityEngine.AndroidJNI::CallByteMethod
    SET_METRO_BINDING(AndroidJNI_CUSTOM_CallCharMethod); //  <- UnityEngine.AndroidJNI::CallCharMethod
    SET_METRO_BINDING(AndroidJNI_CUSTOM_CallFloatMethod); //  <- UnityEngine.AndroidJNI::CallFloatMethod
    SET_METRO_BINDING(AndroidJNI_CUSTOM_CallDoubleMethod); //  <- UnityEngine.AndroidJNI::CallDoubleMethod
    SET_METRO_BINDING(AndroidJNI_CUSTOM_CallLongMethod); //  <- UnityEngine.AndroidJNI::CallLongMethod
    SET_METRO_BINDING(AndroidJNI_CUSTOM_CallVoidMethod); //  <- UnityEngine.AndroidJNI::CallVoidMethod
    SET_METRO_BINDING(AndroidJNI_CUSTOM_GetStringField); //  <- UnityEngine.AndroidJNI::GetStringField
    SET_METRO_BINDING(AndroidJNI_CUSTOM_INTERNAL_CALL_GetObjectField); //  <- UnityEngine.AndroidJNI::INTERNAL_CALL_GetObjectField
    SET_METRO_BINDING(AndroidJNI_CUSTOM_GetBooleanField); //  <- UnityEngine.AndroidJNI::GetBooleanField
    SET_METRO_BINDING(AndroidJNI_CUSTOM_GetByteField); //  <- UnityEngine.AndroidJNI::GetByteField
    SET_METRO_BINDING(AndroidJNI_CUSTOM_GetCharField); //  <- UnityEngine.AndroidJNI::GetCharField
    SET_METRO_BINDING(AndroidJNI_CUSTOM_GetShortField); //  <- UnityEngine.AndroidJNI::GetShortField
    SET_METRO_BINDING(AndroidJNI_CUSTOM_GetIntField); //  <- UnityEngine.AndroidJNI::GetIntField
    SET_METRO_BINDING(AndroidJNI_CUSTOM_GetLongField); //  <- UnityEngine.AndroidJNI::GetLongField
    SET_METRO_BINDING(AndroidJNI_CUSTOM_GetFloatField); //  <- UnityEngine.AndroidJNI::GetFloatField
    SET_METRO_BINDING(AndroidJNI_CUSTOM_GetDoubleField); //  <- UnityEngine.AndroidJNI::GetDoubleField
    SET_METRO_BINDING(AndroidJNI_CUSTOM_SetStringField); //  <- UnityEngine.AndroidJNI::SetStringField
    SET_METRO_BINDING(AndroidJNI_CUSTOM_SetObjectField); //  <- UnityEngine.AndroidJNI::SetObjectField
    SET_METRO_BINDING(AndroidJNI_CUSTOM_SetBooleanField); //  <- UnityEngine.AndroidJNI::SetBooleanField
    SET_METRO_BINDING(AndroidJNI_CUSTOM_SetByteField); //  <- UnityEngine.AndroidJNI::SetByteField
    SET_METRO_BINDING(AndroidJNI_CUSTOM_SetCharField); //  <- UnityEngine.AndroidJNI::SetCharField
    SET_METRO_BINDING(AndroidJNI_CUSTOM_SetShortField); //  <- UnityEngine.AndroidJNI::SetShortField
    SET_METRO_BINDING(AndroidJNI_CUSTOM_SetIntField); //  <- UnityEngine.AndroidJNI::SetIntField
    SET_METRO_BINDING(AndroidJNI_CUSTOM_SetLongField); //  <- UnityEngine.AndroidJNI::SetLongField
    SET_METRO_BINDING(AndroidJNI_CUSTOM_SetFloatField); //  <- UnityEngine.AndroidJNI::SetFloatField
    SET_METRO_BINDING(AndroidJNI_CUSTOM_SetDoubleField); //  <- UnityEngine.AndroidJNI::SetDoubleField
    SET_METRO_BINDING(AndroidJNI_CUSTOM_CallStaticStringMethod); //  <- UnityEngine.AndroidJNI::CallStaticStringMethod
    SET_METRO_BINDING(AndroidJNI_CUSTOM_INTERNAL_CALL_CallStaticObjectMethod); //  <- UnityEngine.AndroidJNI::INTERNAL_CALL_CallStaticObjectMethod
    SET_METRO_BINDING(AndroidJNI_CUSTOM_CallStaticIntMethod); //  <- UnityEngine.AndroidJNI::CallStaticIntMethod
    SET_METRO_BINDING(AndroidJNI_CUSTOM_CallStaticBooleanMethod); //  <- UnityEngine.AndroidJNI::CallStaticBooleanMethod
    SET_METRO_BINDING(AndroidJNI_CUSTOM_CallStaticShortMethod); //  <- UnityEngine.AndroidJNI::CallStaticShortMethod
    SET_METRO_BINDING(AndroidJNI_CUSTOM_CallStaticByteMethod); //  <- UnityEngine.AndroidJNI::CallStaticByteMethod
    SET_METRO_BINDING(AndroidJNI_CUSTOM_CallStaticCharMethod); //  <- UnityEngine.AndroidJNI::CallStaticCharMethod
    SET_METRO_BINDING(AndroidJNI_CUSTOM_CallStaticFloatMethod); //  <- UnityEngine.AndroidJNI::CallStaticFloatMethod
    SET_METRO_BINDING(AndroidJNI_CUSTOM_CallStaticDoubleMethod); //  <- UnityEngine.AndroidJNI::CallStaticDoubleMethod
    SET_METRO_BINDING(AndroidJNI_CUSTOM_CallStaticLongMethod); //  <- UnityEngine.AndroidJNI::CallStaticLongMethod
    SET_METRO_BINDING(AndroidJNI_CUSTOM_CallStaticVoidMethod); //  <- UnityEngine.AndroidJNI::CallStaticVoidMethod
    SET_METRO_BINDING(AndroidJNI_CUSTOM_GetStaticStringField); //  <- UnityEngine.AndroidJNI::GetStaticStringField
    SET_METRO_BINDING(AndroidJNI_CUSTOM_INTERNAL_CALL_GetStaticObjectField); //  <- UnityEngine.AndroidJNI::INTERNAL_CALL_GetStaticObjectField
    SET_METRO_BINDING(AndroidJNI_CUSTOM_GetStaticBooleanField); //  <- UnityEngine.AndroidJNI::GetStaticBooleanField
    SET_METRO_BINDING(AndroidJNI_CUSTOM_GetStaticByteField); //  <- UnityEngine.AndroidJNI::GetStaticByteField
    SET_METRO_BINDING(AndroidJNI_CUSTOM_GetStaticCharField); //  <- UnityEngine.AndroidJNI::GetStaticCharField
    SET_METRO_BINDING(AndroidJNI_CUSTOM_GetStaticShortField); //  <- UnityEngine.AndroidJNI::GetStaticShortField
    SET_METRO_BINDING(AndroidJNI_CUSTOM_GetStaticIntField); //  <- UnityEngine.AndroidJNI::GetStaticIntField
    SET_METRO_BINDING(AndroidJNI_CUSTOM_GetStaticLongField); //  <- UnityEngine.AndroidJNI::GetStaticLongField
    SET_METRO_BINDING(AndroidJNI_CUSTOM_GetStaticFloatField); //  <- UnityEngine.AndroidJNI::GetStaticFloatField
    SET_METRO_BINDING(AndroidJNI_CUSTOM_GetStaticDoubleField); //  <- UnityEngine.AndroidJNI::GetStaticDoubleField
    SET_METRO_BINDING(AndroidJNI_CUSTOM_SetStaticStringField); //  <- UnityEngine.AndroidJNI::SetStaticStringField
    SET_METRO_BINDING(AndroidJNI_CUSTOM_SetStaticObjectField); //  <- UnityEngine.AndroidJNI::SetStaticObjectField
    SET_METRO_BINDING(AndroidJNI_CUSTOM_SetStaticBooleanField); //  <- UnityEngine.AndroidJNI::SetStaticBooleanField
    SET_METRO_BINDING(AndroidJNI_CUSTOM_SetStaticByteField); //  <- UnityEngine.AndroidJNI::SetStaticByteField
    SET_METRO_BINDING(AndroidJNI_CUSTOM_SetStaticCharField); //  <- UnityEngine.AndroidJNI::SetStaticCharField
    SET_METRO_BINDING(AndroidJNI_CUSTOM_SetStaticShortField); //  <- UnityEngine.AndroidJNI::SetStaticShortField
    SET_METRO_BINDING(AndroidJNI_CUSTOM_SetStaticIntField); //  <- UnityEngine.AndroidJNI::SetStaticIntField
    SET_METRO_BINDING(AndroidJNI_CUSTOM_SetStaticLongField); //  <- UnityEngine.AndroidJNI::SetStaticLongField
    SET_METRO_BINDING(AndroidJNI_CUSTOM_SetStaticFloatField); //  <- UnityEngine.AndroidJNI::SetStaticFloatField
    SET_METRO_BINDING(AndroidJNI_CUSTOM_SetStaticDoubleField); //  <- UnityEngine.AndroidJNI::SetStaticDoubleField
    SET_METRO_BINDING(AndroidJNI_CUSTOM_INTERNAL_CALL_ToBooleanArray); //  <- UnityEngine.AndroidJNI::INTERNAL_CALL_ToBooleanArray
    SET_METRO_BINDING(AndroidJNI_CUSTOM_INTERNAL_CALL_ToByteArray); //  <- UnityEngine.AndroidJNI::INTERNAL_CALL_ToByteArray
    SET_METRO_BINDING(AndroidJNI_CUSTOM_INTERNAL_CALL_ToCharArray); //  <- UnityEngine.AndroidJNI::INTERNAL_CALL_ToCharArray
    SET_METRO_BINDING(AndroidJNI_CUSTOM_INTERNAL_CALL_ToShortArray); //  <- UnityEngine.AndroidJNI::INTERNAL_CALL_ToShortArray
    SET_METRO_BINDING(AndroidJNI_CUSTOM_INTERNAL_CALL_ToIntArray); //  <- UnityEngine.AndroidJNI::INTERNAL_CALL_ToIntArray
    SET_METRO_BINDING(AndroidJNI_CUSTOM_INTERNAL_CALL_ToLongArray); //  <- UnityEngine.AndroidJNI::INTERNAL_CALL_ToLongArray
    SET_METRO_BINDING(AndroidJNI_CUSTOM_INTERNAL_CALL_ToFloatArray); //  <- UnityEngine.AndroidJNI::INTERNAL_CALL_ToFloatArray
    SET_METRO_BINDING(AndroidJNI_CUSTOM_INTERNAL_CALL_ToDoubleArray); //  <- UnityEngine.AndroidJNI::INTERNAL_CALL_ToDoubleArray
    SET_METRO_BINDING(AndroidJNI_CUSTOM_INTERNAL_CALL_ToObjectArray); //  <- UnityEngine.AndroidJNI::INTERNAL_CALL_ToObjectArray
    SET_METRO_BINDING(AndroidJNI_CUSTOM_FromBooleanArray); //  <- UnityEngine.AndroidJNI::FromBooleanArray
    SET_METRO_BINDING(AndroidJNI_CUSTOM_FromByteArray); //  <- UnityEngine.AndroidJNI::FromByteArray
    SET_METRO_BINDING(AndroidJNI_CUSTOM_FromCharArray); //  <- UnityEngine.AndroidJNI::FromCharArray
    SET_METRO_BINDING(AndroidJNI_CUSTOM_FromShortArray); //  <- UnityEngine.AndroidJNI::FromShortArray
    SET_METRO_BINDING(AndroidJNI_CUSTOM_FromIntArray); //  <- UnityEngine.AndroidJNI::FromIntArray
    SET_METRO_BINDING(AndroidJNI_CUSTOM_FromLongArray); //  <- UnityEngine.AndroidJNI::FromLongArray
    SET_METRO_BINDING(AndroidJNI_CUSTOM_FromFloatArray); //  <- UnityEngine.AndroidJNI::FromFloatArray
    SET_METRO_BINDING(AndroidJNI_CUSTOM_FromDoubleArray); //  <- UnityEngine.AndroidJNI::FromDoubleArray
    SET_METRO_BINDING(AndroidJNI_CUSTOM_FromObjectArray); //  <- UnityEngine.AndroidJNI::FromObjectArray
    SET_METRO_BINDING(AndroidJNI_CUSTOM_GetArrayLength); //  <- UnityEngine.AndroidJNI::GetArrayLength
    SET_METRO_BINDING(AndroidJNI_CUSTOM_INTERNAL_CALL_NewBooleanArray); //  <- UnityEngine.AndroidJNI::INTERNAL_CALL_NewBooleanArray
    SET_METRO_BINDING(AndroidJNI_CUSTOM_INTERNAL_CALL_NewByteArray); //  <- UnityEngine.AndroidJNI::INTERNAL_CALL_NewByteArray
    SET_METRO_BINDING(AndroidJNI_CUSTOM_INTERNAL_CALL_NewCharArray); //  <- UnityEngine.AndroidJNI::INTERNAL_CALL_NewCharArray
    SET_METRO_BINDING(AndroidJNI_CUSTOM_INTERNAL_CALL_NewShortArray); //  <- UnityEngine.AndroidJNI::INTERNAL_CALL_NewShortArray
    SET_METRO_BINDING(AndroidJNI_CUSTOM_INTERNAL_CALL_NewIntArray); //  <- UnityEngine.AndroidJNI::INTERNAL_CALL_NewIntArray
    SET_METRO_BINDING(AndroidJNI_CUSTOM_INTERNAL_CALL_NewLongArray); //  <- UnityEngine.AndroidJNI::INTERNAL_CALL_NewLongArray
    SET_METRO_BINDING(AndroidJNI_CUSTOM_INTERNAL_CALL_NewFloatArray); //  <- UnityEngine.AndroidJNI::INTERNAL_CALL_NewFloatArray
    SET_METRO_BINDING(AndroidJNI_CUSTOM_INTERNAL_CALL_NewDoubleArray); //  <- UnityEngine.AndroidJNI::INTERNAL_CALL_NewDoubleArray
    SET_METRO_BINDING(AndroidJNI_CUSTOM_INTERNAL_CALL_NewObjectArray); //  <- UnityEngine.AndroidJNI::INTERNAL_CALL_NewObjectArray
    SET_METRO_BINDING(AndroidJNI_CUSTOM_GetBooleanArrayElement); //  <- UnityEngine.AndroidJNI::GetBooleanArrayElement
    SET_METRO_BINDING(AndroidJNI_CUSTOM_GetByteArrayElement); //  <- UnityEngine.AndroidJNI::GetByteArrayElement
    SET_METRO_BINDING(AndroidJNI_CUSTOM_GetCharArrayElement); //  <- UnityEngine.AndroidJNI::GetCharArrayElement
    SET_METRO_BINDING(AndroidJNI_CUSTOM_GetShortArrayElement); //  <- UnityEngine.AndroidJNI::GetShortArrayElement
    SET_METRO_BINDING(AndroidJNI_CUSTOM_GetIntArrayElement); //  <- UnityEngine.AndroidJNI::GetIntArrayElement
    SET_METRO_BINDING(AndroidJNI_CUSTOM_GetLongArrayElement); //  <- UnityEngine.AndroidJNI::GetLongArrayElement
    SET_METRO_BINDING(AndroidJNI_CUSTOM_GetFloatArrayElement); //  <- UnityEngine.AndroidJNI::GetFloatArrayElement
    SET_METRO_BINDING(AndroidJNI_CUSTOM_GetDoubleArrayElement); //  <- UnityEngine.AndroidJNI::GetDoubleArrayElement
    SET_METRO_BINDING(AndroidJNI_CUSTOM_INTERNAL_CALL_GetObjectArrayElement); //  <- UnityEngine.AndroidJNI::INTERNAL_CALL_GetObjectArrayElement
    SET_METRO_BINDING(AndroidJNI_CUSTOM_SetBooleanArrayElement); //  <- UnityEngine.AndroidJNI::SetBooleanArrayElement
    SET_METRO_BINDING(AndroidJNI_CUSTOM_SetByteArrayElement); //  <- UnityEngine.AndroidJNI::SetByteArrayElement
    SET_METRO_BINDING(AndroidJNI_CUSTOM_SetCharArrayElement); //  <- UnityEngine.AndroidJNI::SetCharArrayElement
    SET_METRO_BINDING(AndroidJNI_CUSTOM_SetShortArrayElement); //  <- UnityEngine.AndroidJNI::SetShortArrayElement
    SET_METRO_BINDING(AndroidJNI_CUSTOM_SetIntArrayElement); //  <- UnityEngine.AndroidJNI::SetIntArrayElement
    SET_METRO_BINDING(AndroidJNI_CUSTOM_SetLongArrayElement); //  <- UnityEngine.AndroidJNI::SetLongArrayElement
    SET_METRO_BINDING(AndroidJNI_CUSTOM_SetFloatArrayElement); //  <- UnityEngine.AndroidJNI::SetFloatArrayElement
    SET_METRO_BINDING(AndroidJNI_CUSTOM_SetDoubleArrayElement); //  <- UnityEngine.AndroidJNI::SetDoubleArrayElement
    SET_METRO_BINDING(AndroidJNI_CUSTOM_SetObjectArrayElement); //  <- UnityEngine.AndroidJNI::SetObjectArrayElement
#endif
}

#endif
