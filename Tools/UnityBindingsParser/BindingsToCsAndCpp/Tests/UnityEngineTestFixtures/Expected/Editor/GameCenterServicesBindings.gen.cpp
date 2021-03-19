#include "UnityPrefix.h"
#include "Runtime/Scripting/ScriptingExportUtility.h"


#include "UnityPrefix.h"
#include "Runtime/Scripting/ScriptingUtility.h"
#include "Runtime/Utilities/Utility.h"
#include <vector>

#if ENABLE_GAMECENTER
#include "External/GameKit/GameCenter.h"
#endif

#if ENABLE_GAMECENTER
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GameCenterPlatform_CUSTOM_Internal_Authenticate()
{
    SCRIPTINGAPI_ETW_ENTRY(GameCenterPlatform_CUSTOM_Internal_Authenticate)
    SCRIPTINGAPI_STACK_CHECK(Internal_Authenticate)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_Authenticate)
    
    			#if ENABLE_GAMECENTER
    			GameCenter::GcLocalUser::GetInstance()->Authenticate();
    			#endif
    		
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION GameCenterPlatform_CUSTOM_Internal_Authenticated()
{
    SCRIPTINGAPI_ETW_ENTRY(GameCenterPlatform_CUSTOM_Internal_Authenticated)
    SCRIPTINGAPI_STACK_CHECK(Internal_Authenticated)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_Authenticated)
    
    			#if ENABLE_GAMECENTER
    			return GameCenter::GcLocalUser::GetInstance()->GetAuthenticated();
    			#else
    			return false;
    			#endif
    		
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION GameCenterPlatform_CUSTOM_Internal_UserName()
{
    SCRIPTINGAPI_ETW_ENTRY(GameCenterPlatform_CUSTOM_Internal_UserName)
    SCRIPTINGAPI_STACK_CHECK(Internal_UserName)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_UserName)
    
    			#if ENABLE_GAMECENTER
    			GameCenter::GcLocalUser *user = GameCenter::GcLocalUser::GetInstance();
    			std::string name = user->GetUserName();
    			return CreateScriptingString(name);
    			#else
    			return CreateScriptingString("");
    			#endif
    		
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION GameCenterPlatform_CUSTOM_Internal_UserID()
{
    SCRIPTINGAPI_ETW_ENTRY(GameCenterPlatform_CUSTOM_Internal_UserID)
    SCRIPTINGAPI_STACK_CHECK(Internal_UserID)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_UserID)
    
    			#if ENABLE_GAMECENTER
    			return CreateScriptingString(GameCenter::GcLocalUser::GetInstance()->GetUserID());
    			#else
    			return CreateScriptingString("");
    			#endif
    		
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION GameCenterPlatform_CUSTOM_Internal_Underage()
{
    SCRIPTINGAPI_ETW_ENTRY(GameCenterPlatform_CUSTOM_Internal_Underage)
    SCRIPTINGAPI_STACK_CHECK(Internal_Underage)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_Underage)
    
    			#if ENABLE_GAMECENTER
    			return GameCenter::GcLocalUser::GetInstance()->GetIsUnderage();
    			#else
    			return false;
    			#endif
    		
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION GameCenterPlatform_CUSTOM_Internal_UserImage()
{
    SCRIPTINGAPI_ETW_ENTRY(GameCenterPlatform_CUSTOM_Internal_UserImage)
    SCRIPTINGAPI_STACK_CHECK(Internal_UserImage)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_UserImage)
    
    			#if ENABLE_GAMECENTER
    			return GameCenter::GcLocalUser::GetInstance()->GetUserImage();
    			#else
    			return SCRIPTING_NULL;
    			#endif
    		
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GameCenterPlatform_CUSTOM_Internal_LoadFriends()
{
    SCRIPTINGAPI_ETW_ENTRY(GameCenterPlatform_CUSTOM_Internal_LoadFriends)
    SCRIPTINGAPI_STACK_CHECK(Internal_LoadFriends)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_LoadFriends)
    
    			#if ENABLE_GAMECENTER
    			GameCenter::GcLocalUser::GetInstance()->LoadFriends();
    			#endif
    		
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GameCenterPlatform_CUSTOM_Internal_LoadAchievementDescriptions()
{
    SCRIPTINGAPI_ETW_ENTRY(GameCenterPlatform_CUSTOM_Internal_LoadAchievementDescriptions)
    SCRIPTINGAPI_STACK_CHECK(Internal_LoadAchievementDescriptions)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_LoadAchievementDescriptions)
    
    			#if ENABLE_GAMECENTER
    			GameCenter::GcAchievementDescription::LoadAchievementDescriptions();
    			#endif
    		
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GameCenterPlatform_CUSTOM_Internal_LoadAchievements()
{
    SCRIPTINGAPI_ETW_ENTRY(GameCenterPlatform_CUSTOM_Internal_LoadAchievements)
    SCRIPTINGAPI_STACK_CHECK(Internal_LoadAchievements)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_LoadAchievements)
    
    			#if ENABLE_GAMECENTER
    			GameCenter::GcAchievement::LoadAchievements();
    			#endif
    		
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GameCenterPlatform_CUSTOM_Internal_ReportProgress(ICallType_String_Argument id_, double progress)
{
    SCRIPTINGAPI_ETW_ENTRY(GameCenterPlatform_CUSTOM_Internal_ReportProgress)
    ICallType_String_Local id(id_);
    UNUSED(id);
    SCRIPTINGAPI_STACK_CHECK(Internal_ReportProgress)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_ReportProgress)
    
    			#if ENABLE_GAMECENTER
    			GameCenter::GcAchievement::ReportProgress(id, progress);
    			#endif
    		
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GameCenterPlatform_CUSTOM_Internal_ReportScore(SInt64 score, ICallType_String_Argument category_)
{
    SCRIPTINGAPI_ETW_ENTRY(GameCenterPlatform_CUSTOM_Internal_ReportScore)
    ICallType_String_Local category(category_);
    UNUSED(category);
    SCRIPTINGAPI_STACK_CHECK(Internal_ReportScore)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_ReportScore)
    
    			#if ENABLE_GAMECENTER
    			GameCenter::GcScore::ReportScore(score, category);
    			#endif
    		
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GameCenterPlatform_CUSTOM_Internal_LoadScores(ICallType_String_Argument category_)
{
    SCRIPTINGAPI_ETW_ENTRY(GameCenterPlatform_CUSTOM_Internal_LoadScores)
    ICallType_String_Local category(category_);
    UNUSED(category);
    SCRIPTINGAPI_STACK_CHECK(Internal_LoadScores)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_LoadScores)
    
    			#if ENABLE_GAMECENTER
    			GameCenter::GcScore::LoadScores(category);
    			#endif
    		
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GameCenterPlatform_CUSTOM_Internal_ShowAchievementsUI()
{
    SCRIPTINGAPI_ETW_ENTRY(GameCenterPlatform_CUSTOM_Internal_ShowAchievementsUI)
    SCRIPTINGAPI_STACK_CHECK(Internal_ShowAchievementsUI)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_ShowAchievementsUI)
    
    			#if ENABLE_GAMECENTER
    			GameCenter::GcAchievementDescription::ShowAchievementsUI();
    			#endif
    		
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GameCenterPlatform_CUSTOM_Internal_ShowLeaderboardUI()
{
    SCRIPTINGAPI_ETW_ENTRY(GameCenterPlatform_CUSTOM_Internal_ShowLeaderboardUI)
    SCRIPTINGAPI_STACK_CHECK(Internal_ShowLeaderboardUI)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_ShowLeaderboardUI)
    
    			#if ENABLE_GAMECENTER
    			GameCenter::GcLeaderboard::ShowLeaderboardUI();
    			#endif
    		
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GameCenterPlatform_CUSTOM_Internal_LoadUsers(ICallType_Array_Argument userIds_)
{
    SCRIPTINGAPI_ETW_ENTRY(GameCenterPlatform_CUSTOM_Internal_LoadUsers)
    ICallType_Array_Local userIds(userIds_);
    UNUSED(userIds);
    SCRIPTINGAPI_STACK_CHECK(Internal_LoadUsers)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_LoadUsers)
    
    			#if ENABLE_GAMECENTER
    			GameCenter::GcLocalUser::LoadUsers(userIds);
    			#endif
    		
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GameCenterPlatform_CUSTOM_Internal_ResetAllAchievements()
{
    SCRIPTINGAPI_ETW_ENTRY(GameCenterPlatform_CUSTOM_Internal_ResetAllAchievements)
    SCRIPTINGAPI_STACK_CHECK(Internal_ResetAllAchievements)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_ResetAllAchievements)
    
    			#if ENABLE_GAMECENTER
    			GameCenter::GcAchievement::ResetAllAchievements();
    			#endif
    		
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GameCenterPlatform_CUSTOM_Internal_ShowDefaultAchievementBanner(ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(GameCenterPlatform_CUSTOM_Internal_ShowDefaultAchievementBanner)
    SCRIPTINGAPI_STACK_CHECK(Internal_ShowDefaultAchievementBanner)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_ShowDefaultAchievementBanner)
    
    			#if ENABLE_GAMECENTER
    			GameCenter::GcAchievement::ShowDefaultAchievementBanner(value);
    			#endif
    		
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GameCenterPlatform_CUSTOM_Internal_ShowSpecificLeaderboardUI(ICallType_String_Argument leaderboardID_, int timeScope)
{
    SCRIPTINGAPI_ETW_ENTRY(GameCenterPlatform_CUSTOM_Internal_ShowSpecificLeaderboardUI)
    ICallType_String_Local leaderboardID(leaderboardID_);
    UNUSED(leaderboardID);
    SCRIPTINGAPI_STACK_CHECK(Internal_ShowSpecificLeaderboardUI)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_ShowSpecificLeaderboardUI)
    
    			#if ENABLE_GAMECENTER
    			GameCenter::GcLeaderboard::ShowLeaderboardUI(leaderboardID, timeScope);
    			#endif
    		
}

#endif

		#if ENABLE_GAMECENTER
		struct GcLeaderboardToMono
		{
			GameCenter::GcLeaderboard* leaderboard;
			ScriptingObjectPtr genericLeaderboard;
		};
		#endif
		
#if ENABLE_GAMECENTER
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GcLeaderboard_CUSTOM_Internal_LoadScores(ICallType_Object_Argument self_, ICallType_String_Argument category_, int from, int count, int playerScope, int timeScope)
{
    SCRIPTINGAPI_ETW_ENTRY(GcLeaderboard_CUSTOM_Internal_LoadScores)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    ICallType_String_Local category(category_);
    UNUSED(category);
    SCRIPTINGAPI_STACK_CHECK(Internal_LoadScores)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_LoadScores)
    
    			#if ENABLE_GAMECENTER
    			GcLeaderboardToMono& monoBoard = ExtractMonoObjectData<GcLeaderboardToMono>(self);
    			monoBoard.leaderboard = new GameCenter::GcLeaderboard();
    			monoBoard.leaderboard->Create();
    			monoBoard.leaderboard->RegisterManagedSelf(self);
    			monoBoard.leaderboard->LoadScores(category, from, count, playerScope, timeScope);
    			#endif
    		
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GcLeaderboard_CUSTOM_Internal_LoadScoresWithUsers(ICallType_Object_Argument self_, ICallType_String_Argument category_, int timeScope, ICallType_Array_Argument userIDs_)
{
    SCRIPTINGAPI_ETW_ENTRY(GcLeaderboard_CUSTOM_Internal_LoadScoresWithUsers)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    ICallType_String_Local category(category_);
    UNUSED(category);
    ICallType_Array_Local userIDs(userIDs_);
    UNUSED(userIDs);
    SCRIPTINGAPI_STACK_CHECK(Internal_LoadScoresWithUsers)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_LoadScoresWithUsers)
    
    			#if ENABLE_GAMECENTER
    			GcLeaderboardToMono& monoBoard = ExtractMonoObjectData<GcLeaderboardToMono>(self);
    			monoBoard.leaderboard = new GameCenter::GcLeaderboard();
    			monoBoard.leaderboard->Create(userIDs);
    			monoBoard.leaderboard->RegisterManagedSelf(self);
    			monoBoard.leaderboard->LoadScores(category, 1, 10, 0, timeScope);
    			#endif
    		
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION GcLeaderboard_CUSTOM_Loading(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(GcLeaderboard_CUSTOM_Loading)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(Loading)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Loading)
    
    			#if ENABLE_GAMECENTER
    			GcLeaderboardToMono& monoBoard = ExtractMonoObjectData<GcLeaderboardToMono>(self);
    			return monoBoard.leaderboard->Loading();
    			#else
    			return false;
    			#endif
    		
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION GcLeaderboard_CUSTOM_Dispose(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(GcLeaderboard_CUSTOM_Dispose)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(Dispose)
    
    			#if ENABLE_GAMECENTER
    			GcLeaderboardToMono& monoBoard = ExtractMonoObjectData<GcLeaderboardToMono>(self);
    			delete monoBoard.leaderboard;
    			monoBoard.leaderboard = NULL;
    			#endif
    		
}

#endif
#if !defined(INTERNAL_CALL_STRIPPING)
#   error must include unityconfigure.h
#endif
#if INTERNAL_CALL_STRIPPING
#if ENABLE_GAMECENTER
void Register_UnityEngine_SocialPlatforms_GameCenter_GameCenterPlatform_Internal_Authenticate()
{
    scripting_add_internal_call( "UnityEngine.SocialPlatforms.GameCenter.GameCenterPlatform::Internal_Authenticate" , (gpointer)& GameCenterPlatform_CUSTOM_Internal_Authenticate );
}

void Register_UnityEngine_SocialPlatforms_GameCenter_GameCenterPlatform_Internal_Authenticated()
{
    scripting_add_internal_call( "UnityEngine.SocialPlatforms.GameCenter.GameCenterPlatform::Internal_Authenticated" , (gpointer)& GameCenterPlatform_CUSTOM_Internal_Authenticated );
}

void Register_UnityEngine_SocialPlatforms_GameCenter_GameCenterPlatform_Internal_UserName()
{
    scripting_add_internal_call( "UnityEngine.SocialPlatforms.GameCenter.GameCenterPlatform::Internal_UserName" , (gpointer)& GameCenterPlatform_CUSTOM_Internal_UserName );
}

void Register_UnityEngine_SocialPlatforms_GameCenter_GameCenterPlatform_Internal_UserID()
{
    scripting_add_internal_call( "UnityEngine.SocialPlatforms.GameCenter.GameCenterPlatform::Internal_UserID" , (gpointer)& GameCenterPlatform_CUSTOM_Internal_UserID );
}

void Register_UnityEngine_SocialPlatforms_GameCenter_GameCenterPlatform_Internal_Underage()
{
    scripting_add_internal_call( "UnityEngine.SocialPlatforms.GameCenter.GameCenterPlatform::Internal_Underage" , (gpointer)& GameCenterPlatform_CUSTOM_Internal_Underage );
}

void Register_UnityEngine_SocialPlatforms_GameCenter_GameCenterPlatform_Internal_UserImage()
{
    scripting_add_internal_call( "UnityEngine.SocialPlatforms.GameCenter.GameCenterPlatform::Internal_UserImage" , (gpointer)& GameCenterPlatform_CUSTOM_Internal_UserImage );
}

void Register_UnityEngine_SocialPlatforms_GameCenter_GameCenterPlatform_Internal_LoadFriends()
{
    scripting_add_internal_call( "UnityEngine.SocialPlatforms.GameCenter.GameCenterPlatform::Internal_LoadFriends" , (gpointer)& GameCenterPlatform_CUSTOM_Internal_LoadFriends );
}

void Register_UnityEngine_SocialPlatforms_GameCenter_GameCenterPlatform_Internal_LoadAchievementDescriptions()
{
    scripting_add_internal_call( "UnityEngine.SocialPlatforms.GameCenter.GameCenterPlatform::Internal_LoadAchievementDescriptions" , (gpointer)& GameCenterPlatform_CUSTOM_Internal_LoadAchievementDescriptions );
}

void Register_UnityEngine_SocialPlatforms_GameCenter_GameCenterPlatform_Internal_LoadAchievements()
{
    scripting_add_internal_call( "UnityEngine.SocialPlatforms.GameCenter.GameCenterPlatform::Internal_LoadAchievements" , (gpointer)& GameCenterPlatform_CUSTOM_Internal_LoadAchievements );
}

void Register_UnityEngine_SocialPlatforms_GameCenter_GameCenterPlatform_Internal_ReportProgress()
{
    scripting_add_internal_call( "UnityEngine.SocialPlatforms.GameCenter.GameCenterPlatform::Internal_ReportProgress" , (gpointer)& GameCenterPlatform_CUSTOM_Internal_ReportProgress );
}

void Register_UnityEngine_SocialPlatforms_GameCenter_GameCenterPlatform_Internal_ReportScore()
{
    scripting_add_internal_call( "UnityEngine.SocialPlatforms.GameCenter.GameCenterPlatform::Internal_ReportScore" , (gpointer)& GameCenterPlatform_CUSTOM_Internal_ReportScore );
}

void Register_UnityEngine_SocialPlatforms_GameCenter_GameCenterPlatform_Internal_LoadScores()
{
    scripting_add_internal_call( "UnityEngine.SocialPlatforms.GameCenter.GameCenterPlatform::Internal_LoadScores" , (gpointer)& GameCenterPlatform_CUSTOM_Internal_LoadScores );
}

void Register_UnityEngine_SocialPlatforms_GameCenter_GameCenterPlatform_Internal_ShowAchievementsUI()
{
    scripting_add_internal_call( "UnityEngine.SocialPlatforms.GameCenter.GameCenterPlatform::Internal_ShowAchievementsUI" , (gpointer)& GameCenterPlatform_CUSTOM_Internal_ShowAchievementsUI );
}

void Register_UnityEngine_SocialPlatforms_GameCenter_GameCenterPlatform_Internal_ShowLeaderboardUI()
{
    scripting_add_internal_call( "UnityEngine.SocialPlatforms.GameCenter.GameCenterPlatform::Internal_ShowLeaderboardUI" , (gpointer)& GameCenterPlatform_CUSTOM_Internal_ShowLeaderboardUI );
}

void Register_UnityEngine_SocialPlatforms_GameCenter_GameCenterPlatform_Internal_LoadUsers()
{
    scripting_add_internal_call( "UnityEngine.SocialPlatforms.GameCenter.GameCenterPlatform::Internal_LoadUsers" , (gpointer)& GameCenterPlatform_CUSTOM_Internal_LoadUsers );
}

void Register_UnityEngine_SocialPlatforms_GameCenter_GameCenterPlatform_Internal_ResetAllAchievements()
{
    scripting_add_internal_call( "UnityEngine.SocialPlatforms.GameCenter.GameCenterPlatform::Internal_ResetAllAchievements" , (gpointer)& GameCenterPlatform_CUSTOM_Internal_ResetAllAchievements );
}

void Register_UnityEngine_SocialPlatforms_GameCenter_GameCenterPlatform_Internal_ShowDefaultAchievementBanner()
{
    scripting_add_internal_call( "UnityEngine.SocialPlatforms.GameCenter.GameCenterPlatform::Internal_ShowDefaultAchievementBanner" , (gpointer)& GameCenterPlatform_CUSTOM_Internal_ShowDefaultAchievementBanner );
}

void Register_UnityEngine_SocialPlatforms_GameCenter_GameCenterPlatform_Internal_ShowSpecificLeaderboardUI()
{
    scripting_add_internal_call( "UnityEngine.SocialPlatforms.GameCenter.GameCenterPlatform::Internal_ShowSpecificLeaderboardUI" , (gpointer)& GameCenterPlatform_CUSTOM_Internal_ShowSpecificLeaderboardUI );
}

void Register_UnityEngine_SocialPlatforms_GameCenter_GcLeaderboard_Internal_LoadScores()
{
    scripting_add_internal_call( "UnityEngine.SocialPlatforms.GameCenter.GcLeaderboard::Internal_LoadScores" , (gpointer)& GcLeaderboard_CUSTOM_Internal_LoadScores );
}

void Register_UnityEngine_SocialPlatforms_GameCenter_GcLeaderboard_Internal_LoadScoresWithUsers()
{
    scripting_add_internal_call( "UnityEngine.SocialPlatforms.GameCenter.GcLeaderboard::Internal_LoadScoresWithUsers" , (gpointer)& GcLeaderboard_CUSTOM_Internal_LoadScoresWithUsers );
}

void Register_UnityEngine_SocialPlatforms_GameCenter_GcLeaderboard_Loading()
{
    scripting_add_internal_call( "UnityEngine.SocialPlatforms.GameCenter.GcLeaderboard::Loading" , (gpointer)& GcLeaderboard_CUSTOM_Loading );
}

void Register_UnityEngine_SocialPlatforms_GameCenter_GcLeaderboard_Dispose()
{
    scripting_add_internal_call( "UnityEngine.SocialPlatforms.GameCenter.GcLeaderboard::Dispose" , (gpointer)& GcLeaderboard_CUSTOM_Dispose );
}

#endif
#elif ENABLE_MONO || ENABLE_IL2CPP
static const char* s_GameCenterServices_IcallNames [] =
{
#if ENABLE_GAMECENTER
    "UnityEngine.SocialPlatforms.GameCenter.GameCenterPlatform::Internal_Authenticate",    // -> GameCenterPlatform_CUSTOM_Internal_Authenticate
    "UnityEngine.SocialPlatforms.GameCenter.GameCenterPlatform::Internal_Authenticated",    // -> GameCenterPlatform_CUSTOM_Internal_Authenticated
    "UnityEngine.SocialPlatforms.GameCenter.GameCenterPlatform::Internal_UserName",    // -> GameCenterPlatform_CUSTOM_Internal_UserName
    "UnityEngine.SocialPlatforms.GameCenter.GameCenterPlatform::Internal_UserID",    // -> GameCenterPlatform_CUSTOM_Internal_UserID
    "UnityEngine.SocialPlatforms.GameCenter.GameCenterPlatform::Internal_Underage",    // -> GameCenterPlatform_CUSTOM_Internal_Underage
    "UnityEngine.SocialPlatforms.GameCenter.GameCenterPlatform::Internal_UserImage",    // -> GameCenterPlatform_CUSTOM_Internal_UserImage
    "UnityEngine.SocialPlatforms.GameCenter.GameCenterPlatform::Internal_LoadFriends",    // -> GameCenterPlatform_CUSTOM_Internal_LoadFriends
    "UnityEngine.SocialPlatforms.GameCenter.GameCenterPlatform::Internal_LoadAchievementDescriptions",    // -> GameCenterPlatform_CUSTOM_Internal_LoadAchievementDescriptions
    "UnityEngine.SocialPlatforms.GameCenter.GameCenterPlatform::Internal_LoadAchievements",    // -> GameCenterPlatform_CUSTOM_Internal_LoadAchievements
    "UnityEngine.SocialPlatforms.GameCenter.GameCenterPlatform::Internal_ReportProgress",    // -> GameCenterPlatform_CUSTOM_Internal_ReportProgress
    "UnityEngine.SocialPlatforms.GameCenter.GameCenterPlatform::Internal_ReportScore",    // -> GameCenterPlatform_CUSTOM_Internal_ReportScore
    "UnityEngine.SocialPlatforms.GameCenter.GameCenterPlatform::Internal_LoadScores",    // -> GameCenterPlatform_CUSTOM_Internal_LoadScores
    "UnityEngine.SocialPlatforms.GameCenter.GameCenterPlatform::Internal_ShowAchievementsUI",    // -> GameCenterPlatform_CUSTOM_Internal_ShowAchievementsUI
    "UnityEngine.SocialPlatforms.GameCenter.GameCenterPlatform::Internal_ShowLeaderboardUI",    // -> GameCenterPlatform_CUSTOM_Internal_ShowLeaderboardUI
    "UnityEngine.SocialPlatforms.GameCenter.GameCenterPlatform::Internal_LoadUsers",    // -> GameCenterPlatform_CUSTOM_Internal_LoadUsers
    "UnityEngine.SocialPlatforms.GameCenter.GameCenterPlatform::Internal_ResetAllAchievements",    // -> GameCenterPlatform_CUSTOM_Internal_ResetAllAchievements
    "UnityEngine.SocialPlatforms.GameCenter.GameCenterPlatform::Internal_ShowDefaultAchievementBanner",    // -> GameCenterPlatform_CUSTOM_Internal_ShowDefaultAchievementBanner
    "UnityEngine.SocialPlatforms.GameCenter.GameCenterPlatform::Internal_ShowSpecificLeaderboardUI",    // -> GameCenterPlatform_CUSTOM_Internal_ShowSpecificLeaderboardUI
    "UnityEngine.SocialPlatforms.GameCenter.GcLeaderboard::Internal_LoadScores",    // -> GcLeaderboard_CUSTOM_Internal_LoadScores
    "UnityEngine.SocialPlatforms.GameCenter.GcLeaderboard::Internal_LoadScoresWithUsers",    // -> GcLeaderboard_CUSTOM_Internal_LoadScoresWithUsers
    "UnityEngine.SocialPlatforms.GameCenter.GcLeaderboard::Loading",    // -> GcLeaderboard_CUSTOM_Loading
    "UnityEngine.SocialPlatforms.GameCenter.GcLeaderboard::Dispose",    // -> GcLeaderboard_CUSTOM_Dispose
#endif
    NULL
};

static const void* s_GameCenterServices_IcallFuncs [] =
{
#if ENABLE_GAMECENTER
    (const void*)&GameCenterPlatform_CUSTOM_Internal_Authenticate,  //  <- UnityEngine.SocialPlatforms.GameCenter.GameCenterPlatform::Internal_Authenticate
    (const void*)&GameCenterPlatform_CUSTOM_Internal_Authenticated,  //  <- UnityEngine.SocialPlatforms.GameCenter.GameCenterPlatform::Internal_Authenticated
    (const void*)&GameCenterPlatform_CUSTOM_Internal_UserName,  //  <- UnityEngine.SocialPlatforms.GameCenter.GameCenterPlatform::Internal_UserName
    (const void*)&GameCenterPlatform_CUSTOM_Internal_UserID,  //  <- UnityEngine.SocialPlatforms.GameCenter.GameCenterPlatform::Internal_UserID
    (const void*)&GameCenterPlatform_CUSTOM_Internal_Underage,  //  <- UnityEngine.SocialPlatforms.GameCenter.GameCenterPlatform::Internal_Underage
    (const void*)&GameCenterPlatform_CUSTOM_Internal_UserImage,  //  <- UnityEngine.SocialPlatforms.GameCenter.GameCenterPlatform::Internal_UserImage
    (const void*)&GameCenterPlatform_CUSTOM_Internal_LoadFriends,  //  <- UnityEngine.SocialPlatforms.GameCenter.GameCenterPlatform::Internal_LoadFriends
    (const void*)&GameCenterPlatform_CUSTOM_Internal_LoadAchievementDescriptions,  //  <- UnityEngine.SocialPlatforms.GameCenter.GameCenterPlatform::Internal_LoadAchievementDescriptions
    (const void*)&GameCenterPlatform_CUSTOM_Internal_LoadAchievements,  //  <- UnityEngine.SocialPlatforms.GameCenter.GameCenterPlatform::Internal_LoadAchievements
    (const void*)&GameCenterPlatform_CUSTOM_Internal_ReportProgress,  //  <- UnityEngine.SocialPlatforms.GameCenter.GameCenterPlatform::Internal_ReportProgress
    (const void*)&GameCenterPlatform_CUSTOM_Internal_ReportScore,  //  <- UnityEngine.SocialPlatforms.GameCenter.GameCenterPlatform::Internal_ReportScore
    (const void*)&GameCenterPlatform_CUSTOM_Internal_LoadScores,  //  <- UnityEngine.SocialPlatforms.GameCenter.GameCenterPlatform::Internal_LoadScores
    (const void*)&GameCenterPlatform_CUSTOM_Internal_ShowAchievementsUI,  //  <- UnityEngine.SocialPlatforms.GameCenter.GameCenterPlatform::Internal_ShowAchievementsUI
    (const void*)&GameCenterPlatform_CUSTOM_Internal_ShowLeaderboardUI,  //  <- UnityEngine.SocialPlatforms.GameCenter.GameCenterPlatform::Internal_ShowLeaderboardUI
    (const void*)&GameCenterPlatform_CUSTOM_Internal_LoadUsers,  //  <- UnityEngine.SocialPlatforms.GameCenter.GameCenterPlatform::Internal_LoadUsers
    (const void*)&GameCenterPlatform_CUSTOM_Internal_ResetAllAchievements,  //  <- UnityEngine.SocialPlatforms.GameCenter.GameCenterPlatform::Internal_ResetAllAchievements
    (const void*)&GameCenterPlatform_CUSTOM_Internal_ShowDefaultAchievementBanner,  //  <- UnityEngine.SocialPlatforms.GameCenter.GameCenterPlatform::Internal_ShowDefaultAchievementBanner
    (const void*)&GameCenterPlatform_CUSTOM_Internal_ShowSpecificLeaderboardUI,  //  <- UnityEngine.SocialPlatforms.GameCenter.GameCenterPlatform::Internal_ShowSpecificLeaderboardUI
    (const void*)&GcLeaderboard_CUSTOM_Internal_LoadScores,  //  <- UnityEngine.SocialPlatforms.GameCenter.GcLeaderboard::Internal_LoadScores
    (const void*)&GcLeaderboard_CUSTOM_Internal_LoadScoresWithUsers,  //  <- UnityEngine.SocialPlatforms.GameCenter.GcLeaderboard::Internal_LoadScoresWithUsers
    (const void*)&GcLeaderboard_CUSTOM_Loading            ,  //  <- UnityEngine.SocialPlatforms.GameCenter.GcLeaderboard::Loading
    (const void*)&GcLeaderboard_CUSTOM_Dispose            ,  //  <- UnityEngine.SocialPlatforms.GameCenter.GcLeaderboard::Dispose
#endif
    NULL
};

void ExportGameCenterServicesBindings();
void ExportGameCenterServicesBindings()
{
    for (int i = 0; s_GameCenterServices_IcallNames [i] != NULL; ++i )
        scripting_add_internal_call( s_GameCenterServices_IcallNames [i], s_GameCenterServices_IcallFuncs [i] );
}

#elif ENABLE_DOTNET
// This comment is here on purpose, so Jam won't pick WinRTHelper.h as dependency for non WinRT targets, thus not doing unneeded recompilation.
//#include "Runtime/Scripting/WinRTHelper.h"
void ExportGameCenterServicesBindings()
{
    #if UNITY_WP8
    extern intptr_t g_WinRTFuncPtrs[];
    #define SET_METRO_BINDING(Name) g_WinRTFuncPtrs[k##Name##FuncDef] = reinterpret_cast<intptr_t>(Name);
    #else
    long long* p = GetWinRTFuncDefsPointers();
    #define SET_METRO_BINDING(Name) p[k##Name##Func] = (long long)Name;
    #endif
#if ENABLE_GAMECENTER
    SET_METRO_BINDING(GameCenterPlatform_CUSTOM_Internal_Authenticate); //  <- UnityEngine.SocialPlatforms.GameCenter.GameCenterPlatform::Internal_Authenticate
    SET_METRO_BINDING(GameCenterPlatform_CUSTOM_Internal_Authenticated); //  <- UnityEngine.SocialPlatforms.GameCenter.GameCenterPlatform::Internal_Authenticated
    SET_METRO_BINDING(GameCenterPlatform_CUSTOM_Internal_UserName); //  <- UnityEngine.SocialPlatforms.GameCenter.GameCenterPlatform::Internal_UserName
    SET_METRO_BINDING(GameCenterPlatform_CUSTOM_Internal_UserID); //  <- UnityEngine.SocialPlatforms.GameCenter.GameCenterPlatform::Internal_UserID
    SET_METRO_BINDING(GameCenterPlatform_CUSTOM_Internal_Underage); //  <- UnityEngine.SocialPlatforms.GameCenter.GameCenterPlatform::Internal_Underage
    SET_METRO_BINDING(GameCenterPlatform_CUSTOM_Internal_UserImage); //  <- UnityEngine.SocialPlatforms.GameCenter.GameCenterPlatform::Internal_UserImage
    SET_METRO_BINDING(GameCenterPlatform_CUSTOM_Internal_LoadFriends); //  <- UnityEngine.SocialPlatforms.GameCenter.GameCenterPlatform::Internal_LoadFriends
    SET_METRO_BINDING(GameCenterPlatform_CUSTOM_Internal_LoadAchievementDescriptions); //  <- UnityEngine.SocialPlatforms.GameCenter.GameCenterPlatform::Internal_LoadAchievementDescriptions
    SET_METRO_BINDING(GameCenterPlatform_CUSTOM_Internal_LoadAchievements); //  <- UnityEngine.SocialPlatforms.GameCenter.GameCenterPlatform::Internal_LoadAchievements
    SET_METRO_BINDING(GameCenterPlatform_CUSTOM_Internal_ReportProgress); //  <- UnityEngine.SocialPlatforms.GameCenter.GameCenterPlatform::Internal_ReportProgress
    SET_METRO_BINDING(GameCenterPlatform_CUSTOM_Internal_ReportScore); //  <- UnityEngine.SocialPlatforms.GameCenter.GameCenterPlatform::Internal_ReportScore
    SET_METRO_BINDING(GameCenterPlatform_CUSTOM_Internal_LoadScores); //  <- UnityEngine.SocialPlatforms.GameCenter.GameCenterPlatform::Internal_LoadScores
    SET_METRO_BINDING(GameCenterPlatform_CUSTOM_Internal_ShowAchievementsUI); //  <- UnityEngine.SocialPlatforms.GameCenter.GameCenterPlatform::Internal_ShowAchievementsUI
    SET_METRO_BINDING(GameCenterPlatform_CUSTOM_Internal_ShowLeaderboardUI); //  <- UnityEngine.SocialPlatforms.GameCenter.GameCenterPlatform::Internal_ShowLeaderboardUI
    SET_METRO_BINDING(GameCenterPlatform_CUSTOM_Internal_LoadUsers); //  <- UnityEngine.SocialPlatforms.GameCenter.GameCenterPlatform::Internal_LoadUsers
    SET_METRO_BINDING(GameCenterPlatform_CUSTOM_Internal_ResetAllAchievements); //  <- UnityEngine.SocialPlatforms.GameCenter.GameCenterPlatform::Internal_ResetAllAchievements
    SET_METRO_BINDING(GameCenterPlatform_CUSTOM_Internal_ShowDefaultAchievementBanner); //  <- UnityEngine.SocialPlatforms.GameCenter.GameCenterPlatform::Internal_ShowDefaultAchievementBanner
    SET_METRO_BINDING(GameCenterPlatform_CUSTOM_Internal_ShowSpecificLeaderboardUI); //  <- UnityEngine.SocialPlatforms.GameCenter.GameCenterPlatform::Internal_ShowSpecificLeaderboardUI
    SET_METRO_BINDING(GcLeaderboard_CUSTOM_Internal_LoadScores); //  <- UnityEngine.SocialPlatforms.GameCenter.GcLeaderboard::Internal_LoadScores
    SET_METRO_BINDING(GcLeaderboard_CUSTOM_Internal_LoadScoresWithUsers); //  <- UnityEngine.SocialPlatforms.GameCenter.GcLeaderboard::Internal_LoadScoresWithUsers
    SET_METRO_BINDING(GcLeaderboard_CUSTOM_Loading); //  <- UnityEngine.SocialPlatforms.GameCenter.GcLeaderboard::Loading
    SET_METRO_BINDING(GcLeaderboard_CUSTOM_Dispose); //  <- UnityEngine.SocialPlatforms.GameCenter.GcLeaderboard::Dispose
#endif
}

#endif
