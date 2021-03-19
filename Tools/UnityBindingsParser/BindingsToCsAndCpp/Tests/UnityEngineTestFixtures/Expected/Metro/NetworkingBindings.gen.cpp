#include "UnityPrefix.h"
#include "Runtime/Scripting/ScriptingExportUtility.h"


#include "UnityPrefix.h"
#include "Configuration/UnityConfigure.h"
#include "Runtime/Math/Quaternion.h"
#include "Runtime/Utilities/Utility.h"
#include "Runtime/Network/NetworkManager.h"
#include "Runtime/Network/NetworkUtility.h"
#include "Runtime/Network/BitStreamPacker.h"
#include "Runtime/Network/MasterServerInterface.h"
#include "Runtime/Mono/MonoBehaviour.h"
#if ENABLE_NETWORK
SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION NetworkPlayer_CUSTOM_Internal_GetIPAddress(int index)
{
    SCRIPTINGAPI_ETW_ENTRY(NetworkPlayer_CUSTOM_Internal_GetIPAddress)
    SCRIPTINGAPI_STACK_CHECK(Internal_GetIPAddress)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_GetIPAddress)
     return CreateScriptingString(GetNetworkManager().GetIPAddress(index));  
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION NetworkPlayer_CUSTOM_Internal_GetPort(int index)
{
    SCRIPTINGAPI_ETW_ENTRY(NetworkPlayer_CUSTOM_Internal_GetPort)
    SCRIPTINGAPI_STACK_CHECK(Internal_GetPort)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_GetPort)
     return GetNetworkManager().GetPort(index); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION NetworkPlayer_CUSTOM_Internal_GetExternalIP()
{
    SCRIPTINGAPI_ETW_ENTRY(NetworkPlayer_CUSTOM_Internal_GetExternalIP)
    SCRIPTINGAPI_STACK_CHECK(Internal_GetExternalIP)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_GetExternalIP)
     return CreateScriptingString(GetNetworkManager().GetExternalIP()); 
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION NetworkPlayer_CUSTOM_Internal_GetExternalPort()
{
    SCRIPTINGAPI_ETW_ENTRY(NetworkPlayer_CUSTOM_Internal_GetExternalPort)
    SCRIPTINGAPI_STACK_CHECK(Internal_GetExternalPort)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_GetExternalPort)
     return GetNetworkManager().GetExternalPort(); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION NetworkPlayer_CUSTOM_Internal_GetLocalIP()
{
    SCRIPTINGAPI_ETW_ENTRY(NetworkPlayer_CUSTOM_Internal_GetLocalIP)
    SCRIPTINGAPI_STACK_CHECK(Internal_GetLocalIP)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_GetLocalIP)
     return CreateScriptingString(GetLocalIP()); 
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION NetworkPlayer_CUSTOM_Internal_GetLocalPort()
{
    SCRIPTINGAPI_ETW_ENTRY(NetworkPlayer_CUSTOM_Internal_GetLocalPort)
    SCRIPTINGAPI_STACK_CHECK(Internal_GetLocalPort)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_GetLocalPort)
     return GetNetworkManager().GetPort(); 
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION NetworkPlayer_CUSTOM_Internal_GetPlayerIndex()
{
    SCRIPTINGAPI_ETW_ENTRY(NetworkPlayer_CUSTOM_Internal_GetPlayerIndex)
    SCRIPTINGAPI_STACK_CHECK(Internal_GetPlayerIndex)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_GetPlayerIndex)
     return GetNetworkManager().GetPlayerID(); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION NetworkPlayer_CUSTOM_Internal_GetGUID(int index)
{
    SCRIPTINGAPI_ETW_ENTRY(NetworkPlayer_CUSTOM_Internal_GetGUID)
    SCRIPTINGAPI_STACK_CHECK(Internal_GetGUID)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_GetGUID)
     return CreateScriptingString(GetNetworkManager().GetGUID(index)); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION NetworkPlayer_CUSTOM_Internal_GetLocalGUID()
{
    SCRIPTINGAPI_ETW_ENTRY(NetworkPlayer_CUSTOM_Internal_GetLocalGUID)
    SCRIPTINGAPI_STACK_CHECK(Internal_GetLocalGUID)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_GetLocalGUID)
     return CreateScriptingString(GetNetworkManager().GetGUID()); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION NetworkViewID_CUSTOM_INTERNAL_get_unassigned(NetworkViewID* returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(NetworkViewID_CUSTOM_INTERNAL_get_unassigned)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_get_unassigned)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_get_unassigned)
    { *returnValue =(NetworkViewID::GetUnassignedViewID()); return;};
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION NetworkViewID_CUSTOM_INTERNAL_CALL_Internal_IsMine(const NetworkViewID& value)
{
    SCRIPTINGAPI_ETW_ENTRY(NetworkViewID_CUSTOM_INTERNAL_CALL_Internal_IsMine)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_Internal_IsMine)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_Internal_IsMine)
     return GetNetworkManager().WasViewIdAllocatedByMe(value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION NetworkViewID_CUSTOM_INTERNAL_CALL_Internal_GetOwner(const NetworkViewID& value, int* player)
{
    SCRIPTINGAPI_ETW_ENTRY(NetworkViewID_CUSTOM_INTERNAL_CALL_Internal_GetOwner)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_Internal_GetOwner)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_Internal_GetOwner)
     *player = GetNetworkManager().GetNetworkViewIDOwner(value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION NetworkViewID_CUSTOM_INTERNAL_CALL_Internal_GetString(const NetworkViewID& value)
{
    SCRIPTINGAPI_ETW_ENTRY(NetworkViewID_CUSTOM_INTERNAL_CALL_Internal_GetString)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_Internal_GetString)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_Internal_GetString)
     return CreateScriptingString(value.ToString()); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION NetworkViewID_CUSTOM_INTERNAL_CALL_Internal_Compare(const NetworkViewID& lhs, const NetworkViewID& rhs)
{
    SCRIPTINGAPI_ETW_ENTRY(NetworkViewID_CUSTOM_INTERNAL_CALL_Internal_Compare)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_Internal_Compare)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_Internal_Compare)
     return rhs == lhs; 
}

#endif

 #define GET ExtractMonoObjectData<Ping*> (self)
	
#if ENABLE_NETWORK
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Ping_CUSTOM_Ping(ICallType_Object_Argument self_, ICallType_String_Argument address_)
{
    SCRIPTINGAPI_ETW_ENTRY(Ping_CUSTOM_Ping)
    ScriptingObjectWithIntPtrField<Ping> self(self_);
    UNUSED(self);
    ICallType_String_Local address(address_);
    UNUSED(address);
    SCRIPTINGAPI_STACK_CHECK(.ctor)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(.ctor)
    
    		GET = new Ping(address);
    		GetNetworkManager().PingWrapper(GET);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Ping_CUSTOM_DestroyPing(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Ping_CUSTOM_DestroyPing)
    ScriptingObjectWithIntPtrField<Ping> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(DestroyPing)
    
    		if (GET)
    		{
    			GET->Release();
    			GET = 0;
    		}
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Ping_Get_Custom_PropIsDone(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Ping_Get_Custom_PropIsDone)
    ScriptingObjectWithIntPtrField<Ping> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_isDone)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_isDone)
    if (GET) return (short)GET->GetIsDone();
    else return false;
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Ping_Get_Custom_PropTime(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Ping_Get_Custom_PropTime)
    ScriptingObjectWithIntPtrField<Ping> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_time)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_time)
    return GET->GetTime();
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION Ping_Get_Custom_PropIp(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(Ping_Get_Custom_PropIp)
    ScriptingObjectWithIntPtrField<Ping> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_ip)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_ip)
    return CreateScriptingString(GET->GetIP());
}

#endif

 #undef GET

#if ENABLE_NETWORK
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION NetworkView_CUSTOM_Internal_RPC(ICallType_ReadOnlyUnityEngineObject_Argument view_, ICallType_String_Argument name_, int mode, ICallType_Array_Argument args_)
{
    SCRIPTINGAPI_ETW_ENTRY(NetworkView_CUSTOM_Internal_RPC)
    ReadOnlyScriptingObjectOfType<NetworkView> view(view_);
    UNUSED(view);
    ICallType_String_Local name(name_);
    UNUSED(name);
    ICallType_Array_Local args(args_);
    UNUSED(args);
    SCRIPTINGAPI_STACK_CHECK(Internal_RPC)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_RPC)
    
    		view->RPCCall(name, mode, args);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION NetworkView_CUSTOM_INTERNAL_CALL_Internal_RPC_Target(ICallType_ReadOnlyUnityEngineObject_Argument view_, ICallType_String_Argument name_, const int& target, ICallType_Array_Argument args_)
{
    SCRIPTINGAPI_ETW_ENTRY(NetworkView_CUSTOM_INTERNAL_CALL_Internal_RPC_Target)
    ReadOnlyScriptingObjectOfType<NetworkView> view(view_);
    UNUSED(view);
    ICallType_String_Local name(name_);
    UNUSED(name);
    ICallType_Array_Local args(args_);
    UNUSED(args);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_Internal_RPC_Target)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_Internal_RPC_Target)
    
    		view->RPCCallSpecificTarget(name, target, args);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION NetworkView_Get_Custom_PropObserved(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(NetworkView_Get_Custom_PropObserved)
    ReadOnlyScriptingObjectOfType<NetworkView> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_observed)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_observed)
    return Scripting::ScriptingWrapperFor(self->GetObserved());
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION NetworkView_Set_Custom_PropObserved(ICallType_ReadOnlyUnityEngineObject_Argument self_, ICallType_ReadOnlyUnityEngineObject_Argument val_)
{
    SCRIPTINGAPI_ETW_ENTRY(NetworkView_Set_Custom_PropObserved)
    ReadOnlyScriptingObjectOfType<NetworkView> self(self_);
    UNUSED(self);
    ReadOnlyScriptingObjectOfType<Unity::Component> val(val_);
    UNUSED(val);
    SCRIPTINGAPI_STACK_CHECK(set_observed)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_observed)
    
    self->SetObserved (val);
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION NetworkView_Get_Custom_PropStateSynchronization(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(NetworkView_Get_Custom_PropStateSynchronization)
    ReadOnlyScriptingObjectOfType<NetworkView> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_stateSynchronization)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_stateSynchronization)
    return self->GetStateSynchronization();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION NetworkView_Set_Custom_PropStateSynchronization(ICallType_ReadOnlyUnityEngineObject_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(NetworkView_Set_Custom_PropStateSynchronization)
    ReadOnlyScriptingObjectOfType<NetworkView> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_stateSynchronization)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_stateSynchronization)
     self->SetStateSynchronization(value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION NetworkView_CUSTOM_Internal_GetViewID(ICallType_ReadOnlyUnityEngineObject_Argument self_, NetworkViewID* viewID)
{
    SCRIPTINGAPI_ETW_ENTRY(NetworkView_CUSTOM_Internal_GetViewID)
    ReadOnlyScriptingObjectOfType<NetworkView> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(Internal_GetViewID)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_GetViewID)
    
    		*viewID = self->GetViewID();
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION NetworkView_CUSTOM_INTERNAL_CALL_Internal_SetViewID(ICallType_ReadOnlyUnityEngineObject_Argument self_, const NetworkViewID& viewID)
{
    SCRIPTINGAPI_ETW_ENTRY(NetworkView_CUSTOM_INTERNAL_CALL_Internal_SetViewID)
    ReadOnlyScriptingObjectOfType<NetworkView> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_Internal_SetViewID)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_Internal_SetViewID)
     self->SetViewID(viewID); 
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION NetworkView_Get_Custom_PropGroup(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(NetworkView_Get_Custom_PropGroup)
    ReadOnlyScriptingObjectOfType<NetworkView> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_group)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_group)
    return self->GetGroup ();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION NetworkView_Set_Custom_PropGroup(ICallType_ReadOnlyUnityEngineObject_Argument self_, int value)
{
    SCRIPTINGAPI_ETW_ENTRY(NetworkView_Set_Custom_PropGroup)
    ReadOnlyScriptingObjectOfType<NetworkView> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(set_group)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_group)
    
    self->SetGroup (value);
    
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION NetworkView_CUSTOM_INTERNAL_CALL_SetScope(ICallType_ReadOnlyUnityEngineObject_Argument self_, const int& player, ScriptingBool relevancy)
{
    SCRIPTINGAPI_ETW_ENTRY(NetworkView_CUSTOM_INTERNAL_CALL_SetScope)
    ReadOnlyScriptingObjectOfType<NetworkView> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_SetScope)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_SetScope)
     
    		return self->SetPlayerScope(player, relevancy); 
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION NetworkView_CUSTOM_INTERNAL_CALL_Find(const NetworkViewID& viewID)
{
    SCRIPTINGAPI_ETW_ENTRY(NetworkView_CUSTOM_INTERNAL_CALL_Find)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_Find)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_Find)
     return Scripting::ScriptingWrapperFor(GetNetworkManager().ViewIDToNetworkView (viewID)); 
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Network_CUSTOM_InitializeServer(int connections, int listenPort, ScriptingBool useNat)
{
    SCRIPTINGAPI_ETW_ENTRY(Network_CUSTOM_InitializeServer)
    SCRIPTINGAPI_STACK_CHECK(InitializeServer)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(InitializeServer)
     return GetNetworkManager().InitializeServer(connections, listenPort, useNat); 
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Network_CUSTOM_Internal_InitializeServerDeprecated(int connections, int listenPort)
{
    SCRIPTINGAPI_ETW_ENTRY(Network_CUSTOM_Internal_InitializeServerDeprecated)
    SCRIPTINGAPI_STACK_CHECK(Internal_InitializeServerDeprecated)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_InitializeServerDeprecated)
     return GetNetworkManager().InitializeServer(connections, listenPort, false); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION Network_Get_Custom_PropIncomingPassword()
{
    SCRIPTINGAPI_ETW_ENTRY(Network_Get_Custom_PropIncomingPassword)
    SCRIPTINGAPI_STACK_CHECK(get_incomingPassword)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_incomingPassword)
    return CreateScriptingString(GetNetworkManager().GetIncomingPassword());
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Network_Set_Custom_PropIncomingPassword(ICallType_String_Argument value_)
{
    SCRIPTINGAPI_ETW_ENTRY(Network_Set_Custom_PropIncomingPassword)
    ICallType_String_Local value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(set_incomingPassword)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_incomingPassword)
     GetNetworkManager().SetIncomingPassword(value);
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Network_Get_Custom_PropLogLevel()
{
    SCRIPTINGAPI_ETW_ENTRY(Network_Get_Custom_PropLogLevel)
    SCRIPTINGAPI_STACK_CHECK(get_logLevel)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_logLevel)
    return GetNetworkManager().GetDebugLevel();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Network_Set_Custom_PropLogLevel(int value)
{
    SCRIPTINGAPI_ETW_ENTRY(Network_Set_Custom_PropLogLevel)
    SCRIPTINGAPI_STACK_CHECK(set_logLevel)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_logLevel)
     GetNetworkManager().SetDebugLevel(value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Network_CUSTOM_InitializeSecurity()
{
    SCRIPTINGAPI_ETW_ENTRY(Network_CUSTOM_InitializeSecurity)
    SCRIPTINGAPI_STACK_CHECK(InitializeSecurity)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(InitializeSecurity)
     GetNetworkManager().InitializeSecurity(); 
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Network_CUSTOM_Internal_ConnectToSingleIP(ICallType_String_Argument IP_, int remotePort, int localPort, ICallType_String_Argument password_)
{
    SCRIPTINGAPI_ETW_ENTRY(Network_CUSTOM_Internal_ConnectToSingleIP)
    ICallType_String_Local IP(IP_);
    UNUSED(IP);
    ICallType_String_Local password(password_);
    UNUSED(password);
    SCRIPTINGAPI_STACK_CHECK(Internal_ConnectToSingleIP)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_ConnectToSingleIP)
     
    		return GetNetworkManager().Connect(IP, remotePort, localPort, password);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Network_CUSTOM_Internal_ConnectToGuid(ICallType_String_Argument guid_, ICallType_String_Argument password_)
{
    SCRIPTINGAPI_ETW_ENTRY(Network_CUSTOM_Internal_ConnectToGuid)
    ICallType_String_Local guid(guid_);
    UNUSED(guid);
    ICallType_String_Local password(password_);
    UNUSED(password);
    SCRIPTINGAPI_STACK_CHECK(Internal_ConnectToGuid)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_ConnectToGuid)
     
    		RakNetGUID remoteGuid;
    		remoteGuid.FromString(guid.ToUTF8().c_str());
    		return GetNetworkManager().Connect(remoteGuid, 0, password);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Network_CUSTOM_Internal_ConnectToIPs(ICallType_Array_Argument IP_, int remotePort, int localPort, ICallType_String_Argument password_)
{
    SCRIPTINGAPI_ETW_ENTRY(Network_CUSTOM_Internal_ConnectToIPs)
    ICallType_Array_Local IP(IP_);
    UNUSED(IP);
    ICallType_String_Local password(password_);
    UNUSED(password);
    SCRIPTINGAPI_STACK_CHECK(Internal_ConnectToIPs)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_ConnectToIPs)
    
    		std::vector<string> ipvector;
    		for (int i=0; i < mono_array_length_safe(IP); i++)
    		{
    			ipvector.push_back(scripting_cpp_string_for(GetMonoArrayElement<MonoString*> (IP, i)));
    		}
    		return GetNetworkManager().Connect(ipvector, remotePort, localPort, password);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Network_CUSTOM_Disconnect(int timeout)
{
    SCRIPTINGAPI_ETW_ENTRY(Network_CUSTOM_Disconnect)
    SCRIPTINGAPI_STACK_CHECK(Disconnect)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Disconnect)
     GetNetworkManager().Disconnect(timeout); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Network_CUSTOM_INTERNAL_CALL_CloseConnection(const int& target, ScriptingBool sendDisconnectionNotification)
{
    SCRIPTINGAPI_ETW_ENTRY(Network_CUSTOM_INTERNAL_CALL_CloseConnection)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_CloseConnection)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_CloseConnection)
     GetNetworkManager().CloseConnection(target, sendDisconnectionNotification); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION Network_Get_Custom_PropConnections()
{
    SCRIPTINGAPI_ETW_ENTRY(Network_Get_Custom_PropConnections)
    SCRIPTINGAPI_STACK_CHECK(get_connections)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_connections)
    MonoArray* array = mono_array_new (mono_domain_get (), GetCommonScriptingClasses ().networkPlayer, GetNetworkManager().GetConnectionCount());
    GetNetworkManager().GetConnections(&GetMonoArrayElement<int> (array,0));
    return array;
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Network_CUSTOM_Internal_GetPlayer()
{
    SCRIPTINGAPI_ETW_ENTRY(Network_CUSTOM_Internal_GetPlayer)
    SCRIPTINGAPI_STACK_CHECK(Internal_GetPlayer)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_GetPlayer)
     return GetNetworkManager().GetPlayerID(); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Network_CUSTOM_Internal_AllocateViewID(NetworkViewID* viewID)
{
    SCRIPTINGAPI_ETW_ENTRY(Network_CUSTOM_Internal_AllocateViewID)
    SCRIPTINGAPI_STACK_CHECK(Internal_AllocateViewID)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_AllocateViewID)
     *viewID = GetNetworkManager().AllocateViewID(); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION Network_CUSTOM_INTERNAL_CALL_Instantiate(ICallType_ReadOnlyUnityEngineObject_Argument prefab_, const Vector3f& position, const Quaternionf& rotation, int group)
{
    SCRIPTINGAPI_ETW_ENTRY(Network_CUSTOM_INTERNAL_CALL_Instantiate)
    ReadOnlyScriptingObjectOfType<Object> prefab(prefab_);
    UNUSED(prefab);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_Instantiate)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_Instantiate)
     return Scripting::ScriptingWrapperFor(GetNetworkManager().Instantiate (*prefab, position, rotation, group)); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Network_CUSTOM_INTERNAL_CALL_Destroy(const NetworkViewID& viewID)
{
    SCRIPTINGAPI_ETW_ENTRY(Network_CUSTOM_INTERNAL_CALL_Destroy)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_Destroy)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_Destroy)
     GetNetworkManager().DestroyDelayed(viewID); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Network_CUSTOM_INTERNAL_CALL_DestroyPlayerObjects(const int& playerID)
{
    SCRIPTINGAPI_ETW_ENTRY(Network_CUSTOM_INTERNAL_CALL_DestroyPlayerObjects)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_DestroyPlayerObjects)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_DestroyPlayerObjects)
     GetNetworkManager().DestroyPlayerObjects(playerID); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Network_CUSTOM_INTERNAL_CALL_Internal_RemoveRPCs(const int& playerID, const NetworkViewID& viewID, UInt32 channelMask)
{
    SCRIPTINGAPI_ETW_ENTRY(Network_CUSTOM_INTERNAL_CALL_Internal_RemoveRPCs)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_Internal_RemoveRPCs)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_Internal_RemoveRPCs)
     GetNetworkManager().RemoveRPCs(playerID, viewID, channelMask); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Network_Get_Custom_PropIsClient()
{
    SCRIPTINGAPI_ETW_ENTRY(Network_Get_Custom_PropIsClient)
    SCRIPTINGAPI_STACK_CHECK(get_isClient)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_isClient)
    return GetNetworkManager().IsClient();
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Network_Get_Custom_PropIsServer()
{
    SCRIPTINGAPI_ETW_ENTRY(Network_Get_Custom_PropIsServer)
    SCRIPTINGAPI_STACK_CHECK(get_isServer)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_isServer)
    return GetNetworkManager().IsServer();
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Network_Get_Custom_PropPeerType()
{
    SCRIPTINGAPI_ETW_ENTRY(Network_Get_Custom_PropPeerType)
    SCRIPTINGAPI_STACK_CHECK(get_peerType)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_peerType)
    return GetNetworkManager().GetPeerType();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Network_CUSTOM_SetLevelPrefix(int prefix)
{
    SCRIPTINGAPI_ETW_ENTRY(Network_CUSTOM_SetLevelPrefix)
    SCRIPTINGAPI_STACK_CHECK(SetLevelPrefix)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(SetLevelPrefix)
     return GetNetworkManager().SetLevelPrefix(prefix); 
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Network_CUSTOM_INTERNAL_CALL_GetLastPing(const int& player)
{
    SCRIPTINGAPI_ETW_ENTRY(Network_CUSTOM_INTERNAL_CALL_GetLastPing)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_GetLastPing)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_GetLastPing)
     return GetNetworkManager().GetLastPing(player); 
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Network_CUSTOM_INTERNAL_CALL_GetAveragePing(const int& player)
{
    SCRIPTINGAPI_ETW_ENTRY(Network_CUSTOM_INTERNAL_CALL_GetAveragePing)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_GetAveragePing)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_GetAveragePing)
     return GetNetworkManager().GetAveragePing(player); 
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Network_Get_Custom_PropSendRate()
{
    SCRIPTINGAPI_ETW_ENTRY(Network_Get_Custom_PropSendRate)
    SCRIPTINGAPI_STACK_CHECK(get_sendRate)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_sendRate)
    return GetNetworkManager().GetSendRate();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Network_Set_Custom_PropSendRate(float value)
{
    SCRIPTINGAPI_ETW_ENTRY(Network_Set_Custom_PropSendRate)
    SCRIPTINGAPI_STACK_CHECK(set_sendRate)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_sendRate)
     GetNetworkManager().SetSendRate(value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Network_Get_Custom_PropIsMessageQueueRunning()
{
    SCRIPTINGAPI_ETW_ENTRY(Network_Get_Custom_PropIsMessageQueueRunning)
    SCRIPTINGAPI_STACK_CHECK(get_isMessageQueueRunning)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_isMessageQueueRunning)
    return GetNetworkManager().GetMessageQueueRunning();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Network_Set_Custom_PropIsMessageQueueRunning(ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(Network_Set_Custom_PropIsMessageQueueRunning)
    SCRIPTINGAPI_STACK_CHECK(set_isMessageQueueRunning)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_isMessageQueueRunning)
     GetNetworkManager().SetMessageQueueRunning(value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Network_CUSTOM_INTERNAL_CALL_SetReceivingEnabled(const int& player, int group, ScriptingBool enabled)
{
    SCRIPTINGAPI_ETW_ENTRY(Network_CUSTOM_INTERNAL_CALL_SetReceivingEnabled)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_SetReceivingEnabled)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_SetReceivingEnabled)
     GetNetworkManager().SetReceivingGroupEnabled(player, group, enabled); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Network_CUSTOM_Internal_SetSendingGlobal(int group, ScriptingBool enabled)
{
    SCRIPTINGAPI_ETW_ENTRY(Network_CUSTOM_Internal_SetSendingGlobal)
    SCRIPTINGAPI_STACK_CHECK(Internal_SetSendingGlobal)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_SetSendingGlobal)
     GetNetworkManager().SetSendingGroupEnabled(group, enabled); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Network_CUSTOM_INTERNAL_CALL_Internal_SetSendingSpecific(const int& player, int group, ScriptingBool enabled)
{
    SCRIPTINGAPI_ETW_ENTRY(Network_CUSTOM_INTERNAL_CALL_Internal_SetSendingSpecific)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_Internal_SetSendingSpecific)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_Internal_SetSendingSpecific)
     GetNetworkManager().SetSendingGroupEnabled(player, group, enabled); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Network_CUSTOM_Internal_GetTime(double* t)
{
    SCRIPTINGAPI_ETW_ENTRY(Network_CUSTOM_Internal_GetTime)
    SCRIPTINGAPI_STACK_CHECK(Internal_GetTime)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Internal_GetTime)
     *t = GetNetworkManager().GetTime();
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Network_Get_Custom_PropMinimumAllocatableViewIDs()
{
    SCRIPTINGAPI_ETW_ENTRY(Network_Get_Custom_PropMinimumAllocatableViewIDs)
    SCRIPTINGAPI_STACK_CHECK(get_minimumAllocatableViewIDs)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_minimumAllocatableViewIDs)
    return GetNetworkManager().GetMinimumAllocatableViewIDs();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Network_Set_Custom_PropMinimumAllocatableViewIDs(int value)
{
    SCRIPTINGAPI_ETW_ENTRY(Network_Set_Custom_PropMinimumAllocatableViewIDs)
    SCRIPTINGAPI_STACK_CHECK(set_minimumAllocatableViewIDs)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_minimumAllocatableViewIDs)
     GetNetworkManager().SetMinimumAllocatableViewIDs(value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Network_Get_Custom_PropUseNat()
{
    SCRIPTINGAPI_ETW_ENTRY(Network_Get_Custom_PropUseNat)
    SCRIPTINGAPI_STACK_CHECK(get_useNat)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_useNat)
    return GetNetworkManager().GetUseNat();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Network_Set_Custom_PropUseNat(ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(Network_Set_Custom_PropUseNat)
    SCRIPTINGAPI_STACK_CHECK(set_useNat)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_useNat)
     GetNetworkManager().SetUseNat(value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION Network_Get_Custom_PropNatFacilitatorIP()
{
    SCRIPTINGAPI_ETW_ENTRY(Network_Get_Custom_PropNatFacilitatorIP)
    SCRIPTINGAPI_STACK_CHECK(get_natFacilitatorIP)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_natFacilitatorIP)
    return CreateScriptingString(GetNetworkManager().GetFacilitatorAddress().ToString());
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Network_Set_Custom_PropNatFacilitatorIP(ICallType_String_Argument value_)
{
    SCRIPTINGAPI_ETW_ENTRY(Network_Set_Custom_PropNatFacilitatorIP)
    ICallType_String_Local value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(set_natFacilitatorIP)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_natFacilitatorIP)
     GetNetworkManager().GetFacilitatorAddress(false).SetBinaryAddress(value.ToUTF8().c_str());
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Network_Get_Custom_PropNatFacilitatorPort()
{
    SCRIPTINGAPI_ETW_ENTRY(Network_Get_Custom_PropNatFacilitatorPort)
    SCRIPTINGAPI_STACK_CHECK(get_natFacilitatorPort)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_natFacilitatorPort)
    return GetNetworkManager().GetFacilitatorAddress().port;
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Network_Set_Custom_PropNatFacilitatorPort(int value)
{
    SCRIPTINGAPI_ETW_ENTRY(Network_Set_Custom_PropNatFacilitatorPort)
    SCRIPTINGAPI_STACK_CHECK(set_natFacilitatorPort)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_natFacilitatorPort)
    
    GetNetworkManager().GetFacilitatorAddress(false).port = value;
    
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Network_CUSTOM_TestConnection(ScriptingBool forceTest)
{
    SCRIPTINGAPI_ETW_ENTRY(Network_CUSTOM_TestConnection)
    SCRIPTINGAPI_STACK_CHECK(TestConnection)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(TestConnection)
     return GetNetworkManager().TestConnection(false, forceTest); 
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Network_CUSTOM_TestConnectionNAT(ScriptingBool forceTest)
{
    SCRIPTINGAPI_ETW_ENTRY(Network_CUSTOM_TestConnectionNAT)
    SCRIPTINGAPI_STACK_CHECK(TestConnectionNAT)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(TestConnectionNAT)
     return GetNetworkManager().TestConnection(true, forceTest); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION Network_Get_Custom_PropConnectionTesterIP()
{
    SCRIPTINGAPI_ETW_ENTRY(Network_Get_Custom_PropConnectionTesterIP)
    SCRIPTINGAPI_STACK_CHECK(get_connectionTesterIP)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_connectionTesterIP)
    return CreateScriptingString(GetNetworkManager().GetConnTesterAddress().ToString(false));
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Network_Set_Custom_PropConnectionTesterIP(ICallType_String_Argument value_)
{
    SCRIPTINGAPI_ETW_ENTRY(Network_Set_Custom_PropConnectionTesterIP)
    ICallType_String_Local value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(set_connectionTesterIP)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_connectionTesterIP)
     SystemAddress address = GetNetworkManager().GetConnTesterAddress(); address.SetBinaryAddress(value.ToUTF8().c_str()); GetNetworkManager().SetConnTesterAddress(address);
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Network_Get_Custom_PropConnectionTesterPort()
{
    SCRIPTINGAPI_ETW_ENTRY(Network_Get_Custom_PropConnectionTesterPort)
    SCRIPTINGAPI_STACK_CHECK(get_connectionTesterPort)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_connectionTesterPort)
    return GetNetworkManager().GetConnTesterAddress().port;
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Network_Set_Custom_PropConnectionTesterPort(int value)
{
    SCRIPTINGAPI_ETW_ENTRY(Network_Set_Custom_PropConnectionTesterPort)
    SCRIPTINGAPI_STACK_CHECK(set_connectionTesterPort)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_connectionTesterPort)
     SystemAddress address = GetNetworkManager().GetConnTesterAddress(); address.port = value; GetNetworkManager().SetConnTesterAddress(address);
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Network_CUSTOM_HavePublicAddress()
{
    SCRIPTINGAPI_ETW_ENTRY(Network_CUSTOM_HavePublicAddress)
    SCRIPTINGAPI_STACK_CHECK(HavePublicAddress)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(HavePublicAddress)
     return CheckForPublicAddress(); 
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Network_Get_Custom_PropMaxConnections()
{
    SCRIPTINGAPI_ETW_ENTRY(Network_Get_Custom_PropMaxConnections)
    SCRIPTINGAPI_STACK_CHECK(get_maxConnections)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_maxConnections)
    return GetNetworkManager().GetMaxConnections();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Network_Set_Custom_PropMaxConnections(int value)
{
    SCRIPTINGAPI_ETW_ENTRY(Network_Set_Custom_PropMaxConnections)
    SCRIPTINGAPI_STACK_CHECK(set_maxConnections)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_maxConnections)
     GetNetworkManager().SetMaxConnections(value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION Network_Get_Custom_PropProxyIP()
{
    SCRIPTINGAPI_ETW_ENTRY(Network_Get_Custom_PropProxyIP)
    SCRIPTINGAPI_STACK_CHECK(get_proxyIP)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_proxyIP)
    return CreateScriptingString(GetNetworkManager().GetProxyIP());
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Network_Set_Custom_PropProxyIP(ICallType_String_Argument value_)
{
    SCRIPTINGAPI_ETW_ENTRY(Network_Set_Custom_PropProxyIP)
    ICallType_String_Local value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(set_proxyIP)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_proxyIP)
     GetNetworkManager().SetProxyIP(value);
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Network_Get_Custom_PropProxyPort()
{
    SCRIPTINGAPI_ETW_ENTRY(Network_Get_Custom_PropProxyPort)
    SCRIPTINGAPI_STACK_CHECK(get_proxyPort)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_proxyPort)
    return GetNetworkManager().GetProxyPort();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Network_Set_Custom_PropProxyPort(int value)
{
    SCRIPTINGAPI_ETW_ENTRY(Network_Set_Custom_PropProxyPort)
    SCRIPTINGAPI_STACK_CHECK(set_proxyPort)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_proxyPort)
     GetNetworkManager().SetProxyPort(value);
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Network_Get_Custom_PropUseProxy()
{
    SCRIPTINGAPI_ETW_ENTRY(Network_Get_Custom_PropUseProxy)
    SCRIPTINGAPI_STACK_CHECK(get_useProxy)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_useProxy)
    return GetNetworkManager().GetUseProxy();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Network_Set_Custom_PropUseProxy(ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(Network_Set_Custom_PropUseProxy)
    SCRIPTINGAPI_STACK_CHECK(set_useProxy)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_useProxy)
     GetNetworkManager().SetUseProxy(value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION Network_Get_Custom_PropProxyPassword()
{
    SCRIPTINGAPI_ETW_ENTRY(Network_Get_Custom_PropProxyPassword)
    SCRIPTINGAPI_STACK_CHECK(get_proxyPassword)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_proxyPassword)
    return CreateScriptingString(GetNetworkManager().GetProxyPassword());
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Network_Set_Custom_PropProxyPassword(ICallType_String_Argument value_)
{
    SCRIPTINGAPI_ETW_ENTRY(Network_Set_Custom_PropProxyPassword)
    ICallType_String_Local value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(set_proxyPassword)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_proxyPassword)
    
    GetNetworkManager().SetProxyPassword(value);
    
}

#endif

 #define GET ExtractMonoObjectData<BitstreamPacker*> (self)

 #define CHECK_PTR if (ExtractMonoObjectData<BitstreamPacker*> (self) == NULL) RaiseNullException("");
#if ENABLE_NETWORK
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION BitStream_CUSTOM_Serializeb(ICallType_Object_Argument self_, int& value)
{
    SCRIPTINGAPI_ETW_ENTRY(BitStream_CUSTOM_Serializeb)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(Serializeb)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Serializeb)
     CHECK_PTR bool temp = value; GET->Serialize(temp); value = temp; 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION BitStream_CUSTOM_Serializec(ICallType_Object_Argument self_, UInt16& value)
{
    SCRIPTINGAPI_ETW_ENTRY(BitStream_CUSTOM_Serializec)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(Serializec)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Serializec)
    
    		#if UNITY_BIG_ENDIAN
    		char* hack = reinterpret_cast<char*>(&value)+1;
    		#else
    		char* hack = reinterpret_cast<char*>(&value);
    		#endif
    		CHECK_PTR GET->Serialize(*hack);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION BitStream_CUSTOM_Serializes(ICallType_Object_Argument self_, short& value)
{
    SCRIPTINGAPI_ETW_ENTRY(BitStream_CUSTOM_Serializes)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(Serializes)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Serializes)
     CHECK_PTR GET->Serialize(value); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION BitStream_CUSTOM_Serializei(ICallType_Object_Argument self_, int& value)
{
    SCRIPTINGAPI_ETW_ENTRY(BitStream_CUSTOM_Serializei)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(Serializei)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Serializei)
     CHECK_PTR GET->Serialize(reinterpret_cast<SInt32&>(value)); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION BitStream_CUSTOM_Serializef(ICallType_Object_Argument self_, float& value, float maximumDelta)
{
    SCRIPTINGAPI_ETW_ENTRY(BitStream_CUSTOM_Serializef)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(Serializef)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Serializef)
     CHECK_PTR GET->Serialize(value, maximumDelta); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION BitStream_CUSTOM_INTERNAL_CALL_Serializeq(ICallType_Object_Argument self_, Quaternionf& value, float maximumDelta)
{
    SCRIPTINGAPI_ETW_ENTRY(BitStream_CUSTOM_INTERNAL_CALL_Serializeq)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_Serializeq)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_Serializeq)
     CHECK_PTR GET->Serialize(value, maximumDelta); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION BitStream_CUSTOM_INTERNAL_CALL_Serializev(ICallType_Object_Argument self_, Vector3f& value, float maximumDelta)
{
    SCRIPTINGAPI_ETW_ENTRY(BitStream_CUSTOM_INTERNAL_CALL_Serializev)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_Serializev)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_Serializev)
     CHECK_PTR GET->Serialize(value, maximumDelta); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION BitStream_CUSTOM_INTERNAL_CALL_Serializen(ICallType_Object_Argument self_, NetworkViewID& viewID)
{
    SCRIPTINGAPI_ETW_ENTRY(BitStream_CUSTOM_INTERNAL_CALL_Serializen)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_Serializen)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(INTERNAL_CALL_Serializen)
     CHECK_PTR GET->Serialize(viewID); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION BitStream_Get_Custom_PropIsReading(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(BitStream_Get_Custom_PropIsReading)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_isReading)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_isReading)
    CHECK_PTR return GET->IsReading();
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION BitStream_Get_Custom_PropIsWriting(ICallType_Object_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(BitStream_Get_Custom_PropIsWriting)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(get_isWriting)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_isWriting)
    CHECK_PTR return GET->IsWriting();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION BitStream_CUSTOM_Serialize(ICallType_Object_Argument self_, ICallType_String_Argument_Ref value_)
{
    SCRIPTINGAPI_ETW_ENTRY(BitStream_CUSTOM_Serialize)
    ICallType_Object_Local self(self_);
    UNUSED(self);
    ICallType_String_Local_Ref value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(Serialize)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(Serialize)
    
    		CHECK_PTR 
    		std::string cppValue;
    		if (GET->IsWriting())
    			cppValue = value;
    		
    		GET->Serialize(cppValue);
    		
    		if (GET->IsReading())
    			value.str = CreateScriptingString (cppValue);
    	
}

#endif

 #undef GET
	

 #undef CHECK



struct HostDataCpp
{
		int			useNat;
        MonoString* gameType;
        MonoString* gameName;
        int			connectedPlayers;
        int			playerLimit;
        MonoArray*	IP;
        int			port;
        int			passwordProtected;
		MonoString* comment;
		MonoString* guid;
};

#if ENABLE_NETWORK
SCRIPT_BINDINGS_EXPORT_DECL
ICallType_String_Return SCRIPT_CALL_CONVENTION MasterServer_Get_Custom_PropIpAddress()
{
    SCRIPTINGAPI_ETW_ENTRY(MasterServer_Get_Custom_PropIpAddress)
    SCRIPTINGAPI_STACK_CHECK(get_ipAddress)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_ipAddress)
    return CreateScriptingString(GetMasterServerInterface().GetIPAddress());
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION MasterServer_Set_Custom_PropIpAddress(ICallType_String_Argument value_)
{
    SCRIPTINGAPI_ETW_ENTRY(MasterServer_Set_Custom_PropIpAddress)
    ICallType_String_Local value(value_);
    UNUSED(value);
    SCRIPTINGAPI_STACK_CHECK(set_ipAddress)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_ipAddress)
     GetMasterServerInterface().SetIPAddress(value);
}

#endif

	
#if ENABLE_NETWORK
SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION MasterServer_Get_Custom_PropPort()
{
    SCRIPTINGAPI_ETW_ENTRY(MasterServer_Get_Custom_PropPort)
    SCRIPTINGAPI_STACK_CHECK(get_port)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_port)
    return GetMasterServerInterface().GetPort();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION MasterServer_Set_Custom_PropPort(int value)
{
    SCRIPTINGAPI_ETW_ENTRY(MasterServer_Set_Custom_PropPort)
    SCRIPTINGAPI_STACK_CHECK(set_port)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_port)
     GetMasterServerInterface().SetPort(value);
}

#endif

#if ENABLE_NETWORK
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION MasterServer_CUSTOM_RequestHostList(ICallType_String_Argument gameTypeName_)
{
    SCRIPTINGAPI_ETW_ENTRY(MasterServer_CUSTOM_RequestHostList)
    ICallType_String_Local gameTypeName(gameTypeName_);
    UNUSED(gameTypeName);
    SCRIPTINGAPI_STACK_CHECK(RequestHostList)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(RequestHostList)
     GetMasterServerInterface().QueryHostList(gameTypeName); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Array_Return SCRIPT_CALL_CONVENTION MasterServer_CUSTOM_PollHostList()
{
    SCRIPTINGAPI_ETW_ENTRY(MasterServer_CUSTOM_PollHostList)
    SCRIPTINGAPI_STACK_CHECK(PollHostList)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(PollHostList)
    
    		const std::vector<HostData>& data = GetMasterServerInterface().PollHostList();
    		MonoClass* hostType = GetMonoManager ().GetCommonClasses ().hostData;
    
    
    
    		MonoArray* array = mono_array_new (mono_domain_get (), hostType, data.size());
    		for (int i = 0; i < data.size(); i++)
    		{
    				MonoObject* element = mono_object_new(mono_domain_get(), hostType);
    				GetMonoArrayElement<MonoObject*> (array, i) = element;
    				HostDataCpp& dst = ExtractMonoObjectData<HostDataCpp> (element);
    				HostData src = data[i];
    
    				dst.useNat = src.useNat;
    				dst.gameType = CreateScriptingString(src.gameType);
    				dst.gameName = CreateScriptingString(src.gameName);
    				dst.connectedPlayers = src.connectedPlayers;
    				dst.playerLimit = src.playerLimit;
    				MonoArray* ips = mono_array_new (mono_domain_get (), mono_get_string_class(), src.IP.size());
    				for (int i = 0; i < src.IP.size(); i++)
    				{
    					GetMonoArrayElement<MonoString*> (ips, i) = CreateScriptingString(src.IP[i]);
    				}
    				dst.IP = ips;
    				dst.port = src.port;
    				dst.passwordProtected = src.passwordProtected;
    				dst.comment = CreateScriptingString(src.comment);
    				dst.guid = CreateScriptingString(src.guid);
    		}
    
    		return array;
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION MasterServer_CUSTOM_RegisterHost(ICallType_String_Argument gameTypeName_, ICallType_String_Argument gameName_, ICallType_String_Argument comment_)
{
    SCRIPTINGAPI_ETW_ENTRY(MasterServer_CUSTOM_RegisterHost)
    ICallType_String_Local gameTypeName(gameTypeName_);
    UNUSED(gameTypeName);
    ICallType_String_Local gameName(gameName_);
    UNUSED(gameName);
    ICallType_String_Local comment(comment_);
    UNUSED(comment);
    SCRIPTINGAPI_STACK_CHECK(RegisterHost)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(RegisterHost)
     GetMasterServerInterface().RegisterHost(gameTypeName, gameName, comment); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION MasterServer_CUSTOM_UnregisterHost()
{
    SCRIPTINGAPI_ETW_ENTRY(MasterServer_CUSTOM_UnregisterHost)
    SCRIPTINGAPI_STACK_CHECK(UnregisterHost)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(UnregisterHost)
     GetMasterServerInterface().UnregisterHost(); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION MasterServer_CUSTOM_ClearHostList()
{
    SCRIPTINGAPI_ETW_ENTRY(MasterServer_CUSTOM_ClearHostList)
    SCRIPTINGAPI_STACK_CHECK(ClearHostList)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(ClearHostList)
     GetMasterServerInterface().ClearHostList(); 
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION MasterServer_Get_Custom_PropUpdateRate()
{
    SCRIPTINGAPI_ETW_ENTRY(MasterServer_Get_Custom_PropUpdateRate)
    SCRIPTINGAPI_STACK_CHECK(get_updateRate)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_updateRate)
    return GetMasterServerInterface().GetUpdateRate();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION MasterServer_Set_Custom_PropUpdateRate(int value)
{
    SCRIPTINGAPI_ETW_ENTRY(MasterServer_Set_Custom_PropUpdateRate)
    SCRIPTINGAPI_STACK_CHECK(set_updateRate)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_updateRate)
     GetMasterServerInterface().SetUpdateRate(value);
}

#endif

	
#if ENABLE_NETWORK
SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION MasterServer_Get_Custom_PropDedicatedServer()
{
    SCRIPTINGAPI_ETW_ENTRY(MasterServer_Get_Custom_PropDedicatedServer)
    SCRIPTINGAPI_STACK_CHECK(get_dedicatedServer)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(get_dedicatedServer)
    return GetMasterServerInterface().GetDedicatedServer();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION MasterServer_Set_Custom_PropDedicatedServer(ScriptingBool value)
{
    SCRIPTINGAPI_ETW_ENTRY(MasterServer_Set_Custom_PropDedicatedServer)
    SCRIPTINGAPI_STACK_CHECK(set_dedicatedServer)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(set_dedicatedServer)
     GetMasterServerInterface().SetDedicatedServer(value); 
}

#endif
#if ENABLE_NETWORK
SCRIPT_BINDINGS_EXPORT_DECL
ICallType_Object_Return SCRIPT_CALL_CONVENTION NetworkMessageInfo_CUSTOM_NullNetworkView(ICallType_ReadOnlyUnityEngineObject_Argument self_)
{
    SCRIPTINGAPI_ETW_ENTRY(NetworkMessageInfo_CUSTOM_NullNetworkView)
    ReadOnlyScriptingObjectOfType<NetworkMessageInfo> self(self_);
    UNUSED(self);
    SCRIPTINGAPI_STACK_CHECK(NullNetworkView)
    SCRIPTINGAPI_THREAD_AND_SERIALIZATION_CHECK(NullNetworkView)
     return MonoObjectNULL(ClassID(NetworkView)); 
}

#endif
#if !defined(INTERNAL_CALL_STRIPPING)
#   error must include unityconfigure.h
#endif
#if INTERNAL_CALL_STRIPPING
#if ENABLE_NETWORK
void Register_UnityEngine_NetworkPlayer_Internal_GetIPAddress()
{
    scripting_add_internal_call( "UnityEngine.NetworkPlayer::Internal_GetIPAddress" , (gpointer)& NetworkPlayer_CUSTOM_Internal_GetIPAddress );
}

void Register_UnityEngine_NetworkPlayer_Internal_GetPort()
{
    scripting_add_internal_call( "UnityEngine.NetworkPlayer::Internal_GetPort" , (gpointer)& NetworkPlayer_CUSTOM_Internal_GetPort );
}

void Register_UnityEngine_NetworkPlayer_Internal_GetExternalIP()
{
    scripting_add_internal_call( "UnityEngine.NetworkPlayer::Internal_GetExternalIP" , (gpointer)& NetworkPlayer_CUSTOM_Internal_GetExternalIP );
}

void Register_UnityEngine_NetworkPlayer_Internal_GetExternalPort()
{
    scripting_add_internal_call( "UnityEngine.NetworkPlayer::Internal_GetExternalPort" , (gpointer)& NetworkPlayer_CUSTOM_Internal_GetExternalPort );
}

void Register_UnityEngine_NetworkPlayer_Internal_GetLocalIP()
{
    scripting_add_internal_call( "UnityEngine.NetworkPlayer::Internal_GetLocalIP" , (gpointer)& NetworkPlayer_CUSTOM_Internal_GetLocalIP );
}

void Register_UnityEngine_NetworkPlayer_Internal_GetLocalPort()
{
    scripting_add_internal_call( "UnityEngine.NetworkPlayer::Internal_GetLocalPort" , (gpointer)& NetworkPlayer_CUSTOM_Internal_GetLocalPort );
}

void Register_UnityEngine_NetworkPlayer_Internal_GetPlayerIndex()
{
    scripting_add_internal_call( "UnityEngine.NetworkPlayer::Internal_GetPlayerIndex" , (gpointer)& NetworkPlayer_CUSTOM_Internal_GetPlayerIndex );
}

void Register_UnityEngine_NetworkPlayer_Internal_GetGUID()
{
    scripting_add_internal_call( "UnityEngine.NetworkPlayer::Internal_GetGUID" , (gpointer)& NetworkPlayer_CUSTOM_Internal_GetGUID );
}

void Register_UnityEngine_NetworkPlayer_Internal_GetLocalGUID()
{
    scripting_add_internal_call( "UnityEngine.NetworkPlayer::Internal_GetLocalGUID" , (gpointer)& NetworkPlayer_CUSTOM_Internal_GetLocalGUID );
}

void Register_UnityEngine_NetworkViewID_INTERNAL_get_unassigned()
{
    scripting_add_internal_call( "UnityEngine.NetworkViewID::INTERNAL_get_unassigned" , (gpointer)& NetworkViewID_CUSTOM_INTERNAL_get_unassigned );
}

void Register_UnityEngine_NetworkViewID_INTERNAL_CALL_Internal_IsMine()
{
    scripting_add_internal_call( "UnityEngine.NetworkViewID::INTERNAL_CALL_Internal_IsMine" , (gpointer)& NetworkViewID_CUSTOM_INTERNAL_CALL_Internal_IsMine );
}

void Register_UnityEngine_NetworkViewID_INTERNAL_CALL_Internal_GetOwner()
{
    scripting_add_internal_call( "UnityEngine.NetworkViewID::INTERNAL_CALL_Internal_GetOwner" , (gpointer)& NetworkViewID_CUSTOM_INTERNAL_CALL_Internal_GetOwner );
}

void Register_UnityEngine_NetworkViewID_INTERNAL_CALL_Internal_GetString()
{
    scripting_add_internal_call( "UnityEngine.NetworkViewID::INTERNAL_CALL_Internal_GetString" , (gpointer)& NetworkViewID_CUSTOM_INTERNAL_CALL_Internal_GetString );
}

void Register_UnityEngine_NetworkViewID_INTERNAL_CALL_Internal_Compare()
{
    scripting_add_internal_call( "UnityEngine.NetworkViewID::INTERNAL_CALL_Internal_Compare" , (gpointer)& NetworkViewID_CUSTOM_INTERNAL_CALL_Internal_Compare );
}

void Register_UnityEngine_Ping__ctor()
{
    scripting_add_internal_call( "UnityEngine.Ping::.ctor" , (gpointer)& Ping_CUSTOM_Ping );
}

void Register_UnityEngine_Ping_DestroyPing()
{
    scripting_add_internal_call( "UnityEngine.Ping::DestroyPing" , (gpointer)& Ping_CUSTOM_DestroyPing );
}

void Register_UnityEngine_Ping_get_isDone()
{
    scripting_add_internal_call( "UnityEngine.Ping::get_isDone" , (gpointer)& Ping_Get_Custom_PropIsDone );
}

void Register_UnityEngine_Ping_get_time()
{
    scripting_add_internal_call( "UnityEngine.Ping::get_time" , (gpointer)& Ping_Get_Custom_PropTime );
}

void Register_UnityEngine_Ping_get_ip()
{
    scripting_add_internal_call( "UnityEngine.Ping::get_ip" , (gpointer)& Ping_Get_Custom_PropIp );
}

void Register_UnityEngine_NetworkView_Internal_RPC()
{
    scripting_add_internal_call( "UnityEngine.NetworkView::Internal_RPC" , (gpointer)& NetworkView_CUSTOM_Internal_RPC );
}

void Register_UnityEngine_NetworkView_INTERNAL_CALL_Internal_RPC_Target()
{
    scripting_add_internal_call( "UnityEngine.NetworkView::INTERNAL_CALL_Internal_RPC_Target" , (gpointer)& NetworkView_CUSTOM_INTERNAL_CALL_Internal_RPC_Target );
}

void Register_UnityEngine_NetworkView_get_observed()
{
    scripting_add_internal_call( "UnityEngine.NetworkView::get_observed" , (gpointer)& NetworkView_Get_Custom_PropObserved );
}

void Register_UnityEngine_NetworkView_set_observed()
{
    scripting_add_internal_call( "UnityEngine.NetworkView::set_observed" , (gpointer)& NetworkView_Set_Custom_PropObserved );
}

void Register_UnityEngine_NetworkView_get_stateSynchronization()
{
    scripting_add_internal_call( "UnityEngine.NetworkView::get_stateSynchronization" , (gpointer)& NetworkView_Get_Custom_PropStateSynchronization );
}

void Register_UnityEngine_NetworkView_set_stateSynchronization()
{
    scripting_add_internal_call( "UnityEngine.NetworkView::set_stateSynchronization" , (gpointer)& NetworkView_Set_Custom_PropStateSynchronization );
}

void Register_UnityEngine_NetworkView_Internal_GetViewID()
{
    scripting_add_internal_call( "UnityEngine.NetworkView::Internal_GetViewID" , (gpointer)& NetworkView_CUSTOM_Internal_GetViewID );
}

void Register_UnityEngine_NetworkView_INTERNAL_CALL_Internal_SetViewID()
{
    scripting_add_internal_call( "UnityEngine.NetworkView::INTERNAL_CALL_Internal_SetViewID" , (gpointer)& NetworkView_CUSTOM_INTERNAL_CALL_Internal_SetViewID );
}

void Register_UnityEngine_NetworkView_get_group()
{
    scripting_add_internal_call( "UnityEngine.NetworkView::get_group" , (gpointer)& NetworkView_Get_Custom_PropGroup );
}

void Register_UnityEngine_NetworkView_set_group()
{
    scripting_add_internal_call( "UnityEngine.NetworkView::set_group" , (gpointer)& NetworkView_Set_Custom_PropGroup );
}

void Register_UnityEngine_NetworkView_INTERNAL_CALL_SetScope()
{
    scripting_add_internal_call( "UnityEngine.NetworkView::INTERNAL_CALL_SetScope" , (gpointer)& NetworkView_CUSTOM_INTERNAL_CALL_SetScope );
}

void Register_UnityEngine_NetworkView_INTERNAL_CALL_Find()
{
    scripting_add_internal_call( "UnityEngine.NetworkView::INTERNAL_CALL_Find" , (gpointer)& NetworkView_CUSTOM_INTERNAL_CALL_Find );
}

void Register_UnityEngine_Network_InitializeServer()
{
    scripting_add_internal_call( "UnityEngine.Network::InitializeServer" , (gpointer)& Network_CUSTOM_InitializeServer );
}

void Register_UnityEngine_Network_Internal_InitializeServerDeprecated()
{
    scripting_add_internal_call( "UnityEngine.Network::Internal_InitializeServerDeprecated" , (gpointer)& Network_CUSTOM_Internal_InitializeServerDeprecated );
}

void Register_UnityEngine_Network_get_incomingPassword()
{
    scripting_add_internal_call( "UnityEngine.Network::get_incomingPassword" , (gpointer)& Network_Get_Custom_PropIncomingPassword );
}

void Register_UnityEngine_Network_set_incomingPassword()
{
    scripting_add_internal_call( "UnityEngine.Network::set_incomingPassword" , (gpointer)& Network_Set_Custom_PropIncomingPassword );
}

void Register_UnityEngine_Network_get_logLevel()
{
    scripting_add_internal_call( "UnityEngine.Network::get_logLevel" , (gpointer)& Network_Get_Custom_PropLogLevel );
}

void Register_UnityEngine_Network_set_logLevel()
{
    scripting_add_internal_call( "UnityEngine.Network::set_logLevel" , (gpointer)& Network_Set_Custom_PropLogLevel );
}

void Register_UnityEngine_Network_InitializeSecurity()
{
    scripting_add_internal_call( "UnityEngine.Network::InitializeSecurity" , (gpointer)& Network_CUSTOM_InitializeSecurity );
}

void Register_UnityEngine_Network_Internal_ConnectToSingleIP()
{
    scripting_add_internal_call( "UnityEngine.Network::Internal_ConnectToSingleIP" , (gpointer)& Network_CUSTOM_Internal_ConnectToSingleIP );
}

void Register_UnityEngine_Network_Internal_ConnectToGuid()
{
    scripting_add_internal_call( "UnityEngine.Network::Internal_ConnectToGuid" , (gpointer)& Network_CUSTOM_Internal_ConnectToGuid );
}

void Register_UnityEngine_Network_Internal_ConnectToIPs()
{
    scripting_add_internal_call( "UnityEngine.Network::Internal_ConnectToIPs" , (gpointer)& Network_CUSTOM_Internal_ConnectToIPs );
}

void Register_UnityEngine_Network_Disconnect()
{
    scripting_add_internal_call( "UnityEngine.Network::Disconnect" , (gpointer)& Network_CUSTOM_Disconnect );
}

void Register_UnityEngine_Network_INTERNAL_CALL_CloseConnection()
{
    scripting_add_internal_call( "UnityEngine.Network::INTERNAL_CALL_CloseConnection" , (gpointer)& Network_CUSTOM_INTERNAL_CALL_CloseConnection );
}

void Register_UnityEngine_Network_get_connections()
{
    scripting_add_internal_call( "UnityEngine.Network::get_connections" , (gpointer)& Network_Get_Custom_PropConnections );
}

void Register_UnityEngine_Network_Internal_GetPlayer()
{
    scripting_add_internal_call( "UnityEngine.Network::Internal_GetPlayer" , (gpointer)& Network_CUSTOM_Internal_GetPlayer );
}

void Register_UnityEngine_Network_Internal_AllocateViewID()
{
    scripting_add_internal_call( "UnityEngine.Network::Internal_AllocateViewID" , (gpointer)& Network_CUSTOM_Internal_AllocateViewID );
}

void Register_UnityEngine_Network_INTERNAL_CALL_Instantiate()
{
    scripting_add_internal_call( "UnityEngine.Network::INTERNAL_CALL_Instantiate" , (gpointer)& Network_CUSTOM_INTERNAL_CALL_Instantiate );
}

void Register_UnityEngine_Network_INTERNAL_CALL_Destroy()
{
    scripting_add_internal_call( "UnityEngine.Network::INTERNAL_CALL_Destroy" , (gpointer)& Network_CUSTOM_INTERNAL_CALL_Destroy );
}

void Register_UnityEngine_Network_INTERNAL_CALL_DestroyPlayerObjects()
{
    scripting_add_internal_call( "UnityEngine.Network::INTERNAL_CALL_DestroyPlayerObjects" , (gpointer)& Network_CUSTOM_INTERNAL_CALL_DestroyPlayerObjects );
}

void Register_UnityEngine_Network_INTERNAL_CALL_Internal_RemoveRPCs()
{
    scripting_add_internal_call( "UnityEngine.Network::INTERNAL_CALL_Internal_RemoveRPCs" , (gpointer)& Network_CUSTOM_INTERNAL_CALL_Internal_RemoveRPCs );
}

void Register_UnityEngine_Network_get_isClient()
{
    scripting_add_internal_call( "UnityEngine.Network::get_isClient" , (gpointer)& Network_Get_Custom_PropIsClient );
}

void Register_UnityEngine_Network_get_isServer()
{
    scripting_add_internal_call( "UnityEngine.Network::get_isServer" , (gpointer)& Network_Get_Custom_PropIsServer );
}

void Register_UnityEngine_Network_get_peerType()
{
    scripting_add_internal_call( "UnityEngine.Network::get_peerType" , (gpointer)& Network_Get_Custom_PropPeerType );
}

void Register_UnityEngine_Network_SetLevelPrefix()
{
    scripting_add_internal_call( "UnityEngine.Network::SetLevelPrefix" , (gpointer)& Network_CUSTOM_SetLevelPrefix );
}

void Register_UnityEngine_Network_INTERNAL_CALL_GetLastPing()
{
    scripting_add_internal_call( "UnityEngine.Network::INTERNAL_CALL_GetLastPing" , (gpointer)& Network_CUSTOM_INTERNAL_CALL_GetLastPing );
}

void Register_UnityEngine_Network_INTERNAL_CALL_GetAveragePing()
{
    scripting_add_internal_call( "UnityEngine.Network::INTERNAL_CALL_GetAveragePing" , (gpointer)& Network_CUSTOM_INTERNAL_CALL_GetAveragePing );
}

void Register_UnityEngine_Network_get_sendRate()
{
    scripting_add_internal_call( "UnityEngine.Network::get_sendRate" , (gpointer)& Network_Get_Custom_PropSendRate );
}

void Register_UnityEngine_Network_set_sendRate()
{
    scripting_add_internal_call( "UnityEngine.Network::set_sendRate" , (gpointer)& Network_Set_Custom_PropSendRate );
}

void Register_UnityEngine_Network_get_isMessageQueueRunning()
{
    scripting_add_internal_call( "UnityEngine.Network::get_isMessageQueueRunning" , (gpointer)& Network_Get_Custom_PropIsMessageQueueRunning );
}

void Register_UnityEngine_Network_set_isMessageQueueRunning()
{
    scripting_add_internal_call( "UnityEngine.Network::set_isMessageQueueRunning" , (gpointer)& Network_Set_Custom_PropIsMessageQueueRunning );
}

void Register_UnityEngine_Network_INTERNAL_CALL_SetReceivingEnabled()
{
    scripting_add_internal_call( "UnityEngine.Network::INTERNAL_CALL_SetReceivingEnabled" , (gpointer)& Network_CUSTOM_INTERNAL_CALL_SetReceivingEnabled );
}

void Register_UnityEngine_Network_Internal_SetSendingGlobal()
{
    scripting_add_internal_call( "UnityEngine.Network::Internal_SetSendingGlobal" , (gpointer)& Network_CUSTOM_Internal_SetSendingGlobal );
}

void Register_UnityEngine_Network_INTERNAL_CALL_Internal_SetSendingSpecific()
{
    scripting_add_internal_call( "UnityEngine.Network::INTERNAL_CALL_Internal_SetSendingSpecific" , (gpointer)& Network_CUSTOM_INTERNAL_CALL_Internal_SetSendingSpecific );
}

void Register_UnityEngine_Network_Internal_GetTime()
{
    scripting_add_internal_call( "UnityEngine.Network::Internal_GetTime" , (gpointer)& Network_CUSTOM_Internal_GetTime );
}

void Register_UnityEngine_Network_get_minimumAllocatableViewIDs()
{
    scripting_add_internal_call( "UnityEngine.Network::get_minimumAllocatableViewIDs" , (gpointer)& Network_Get_Custom_PropMinimumAllocatableViewIDs );
}

void Register_UnityEngine_Network_set_minimumAllocatableViewIDs()
{
    scripting_add_internal_call( "UnityEngine.Network::set_minimumAllocatableViewIDs" , (gpointer)& Network_Set_Custom_PropMinimumAllocatableViewIDs );
}

void Register_UnityEngine_Network_get_useNat()
{
    scripting_add_internal_call( "UnityEngine.Network::get_useNat" , (gpointer)& Network_Get_Custom_PropUseNat );
}

void Register_UnityEngine_Network_set_useNat()
{
    scripting_add_internal_call( "UnityEngine.Network::set_useNat" , (gpointer)& Network_Set_Custom_PropUseNat );
}

void Register_UnityEngine_Network_get_natFacilitatorIP()
{
    scripting_add_internal_call( "UnityEngine.Network::get_natFacilitatorIP" , (gpointer)& Network_Get_Custom_PropNatFacilitatorIP );
}

void Register_UnityEngine_Network_set_natFacilitatorIP()
{
    scripting_add_internal_call( "UnityEngine.Network::set_natFacilitatorIP" , (gpointer)& Network_Set_Custom_PropNatFacilitatorIP );
}

void Register_UnityEngine_Network_get_natFacilitatorPort()
{
    scripting_add_internal_call( "UnityEngine.Network::get_natFacilitatorPort" , (gpointer)& Network_Get_Custom_PropNatFacilitatorPort );
}

void Register_UnityEngine_Network_set_natFacilitatorPort()
{
    scripting_add_internal_call( "UnityEngine.Network::set_natFacilitatorPort" , (gpointer)& Network_Set_Custom_PropNatFacilitatorPort );
}

void Register_UnityEngine_Network_TestConnection()
{
    scripting_add_internal_call( "UnityEngine.Network::TestConnection" , (gpointer)& Network_CUSTOM_TestConnection );
}

void Register_UnityEngine_Network_TestConnectionNAT()
{
    scripting_add_internal_call( "UnityEngine.Network::TestConnectionNAT" , (gpointer)& Network_CUSTOM_TestConnectionNAT );
}

void Register_UnityEngine_Network_get_connectionTesterIP()
{
    scripting_add_internal_call( "UnityEngine.Network::get_connectionTesterIP" , (gpointer)& Network_Get_Custom_PropConnectionTesterIP );
}

void Register_UnityEngine_Network_set_connectionTesterIP()
{
    scripting_add_internal_call( "UnityEngine.Network::set_connectionTesterIP" , (gpointer)& Network_Set_Custom_PropConnectionTesterIP );
}

void Register_UnityEngine_Network_get_connectionTesterPort()
{
    scripting_add_internal_call( "UnityEngine.Network::get_connectionTesterPort" , (gpointer)& Network_Get_Custom_PropConnectionTesterPort );
}

void Register_UnityEngine_Network_set_connectionTesterPort()
{
    scripting_add_internal_call( "UnityEngine.Network::set_connectionTesterPort" , (gpointer)& Network_Set_Custom_PropConnectionTesterPort );
}

void Register_UnityEngine_Network_HavePublicAddress()
{
    scripting_add_internal_call( "UnityEngine.Network::HavePublicAddress" , (gpointer)& Network_CUSTOM_HavePublicAddress );
}

void Register_UnityEngine_Network_get_maxConnections()
{
    scripting_add_internal_call( "UnityEngine.Network::get_maxConnections" , (gpointer)& Network_Get_Custom_PropMaxConnections );
}

void Register_UnityEngine_Network_set_maxConnections()
{
    scripting_add_internal_call( "UnityEngine.Network::set_maxConnections" , (gpointer)& Network_Set_Custom_PropMaxConnections );
}

void Register_UnityEngine_Network_get_proxyIP()
{
    scripting_add_internal_call( "UnityEngine.Network::get_proxyIP" , (gpointer)& Network_Get_Custom_PropProxyIP );
}

void Register_UnityEngine_Network_set_proxyIP()
{
    scripting_add_internal_call( "UnityEngine.Network::set_proxyIP" , (gpointer)& Network_Set_Custom_PropProxyIP );
}

void Register_UnityEngine_Network_get_proxyPort()
{
    scripting_add_internal_call( "UnityEngine.Network::get_proxyPort" , (gpointer)& Network_Get_Custom_PropProxyPort );
}

void Register_UnityEngine_Network_set_proxyPort()
{
    scripting_add_internal_call( "UnityEngine.Network::set_proxyPort" , (gpointer)& Network_Set_Custom_PropProxyPort );
}

void Register_UnityEngine_Network_get_useProxy()
{
    scripting_add_internal_call( "UnityEngine.Network::get_useProxy" , (gpointer)& Network_Get_Custom_PropUseProxy );
}

void Register_UnityEngine_Network_set_useProxy()
{
    scripting_add_internal_call( "UnityEngine.Network::set_useProxy" , (gpointer)& Network_Set_Custom_PropUseProxy );
}

void Register_UnityEngine_Network_get_proxyPassword()
{
    scripting_add_internal_call( "UnityEngine.Network::get_proxyPassword" , (gpointer)& Network_Get_Custom_PropProxyPassword );
}

void Register_UnityEngine_Network_set_proxyPassword()
{
    scripting_add_internal_call( "UnityEngine.Network::set_proxyPassword" , (gpointer)& Network_Set_Custom_PropProxyPassword );
}

void Register_UnityEngine_BitStream_Serializeb()
{
    scripting_add_internal_call( "UnityEngine.BitStream::Serializeb" , (gpointer)& BitStream_CUSTOM_Serializeb );
}

void Register_UnityEngine_BitStream_Serializec()
{
    scripting_add_internal_call( "UnityEngine.BitStream::Serializec" , (gpointer)& BitStream_CUSTOM_Serializec );
}

void Register_UnityEngine_BitStream_Serializes()
{
    scripting_add_internal_call( "UnityEngine.BitStream::Serializes" , (gpointer)& BitStream_CUSTOM_Serializes );
}

void Register_UnityEngine_BitStream_Serializei()
{
    scripting_add_internal_call( "UnityEngine.BitStream::Serializei" , (gpointer)& BitStream_CUSTOM_Serializei );
}

void Register_UnityEngine_BitStream_Serializef()
{
    scripting_add_internal_call( "UnityEngine.BitStream::Serializef" , (gpointer)& BitStream_CUSTOM_Serializef );
}

void Register_UnityEngine_BitStream_INTERNAL_CALL_Serializeq()
{
    scripting_add_internal_call( "UnityEngine.BitStream::INTERNAL_CALL_Serializeq" , (gpointer)& BitStream_CUSTOM_INTERNAL_CALL_Serializeq );
}

void Register_UnityEngine_BitStream_INTERNAL_CALL_Serializev()
{
    scripting_add_internal_call( "UnityEngine.BitStream::INTERNAL_CALL_Serializev" , (gpointer)& BitStream_CUSTOM_INTERNAL_CALL_Serializev );
}

void Register_UnityEngine_BitStream_INTERNAL_CALL_Serializen()
{
    scripting_add_internal_call( "UnityEngine.BitStream::INTERNAL_CALL_Serializen" , (gpointer)& BitStream_CUSTOM_INTERNAL_CALL_Serializen );
}

void Register_UnityEngine_BitStream_get_isReading()
{
    scripting_add_internal_call( "UnityEngine.BitStream::get_isReading" , (gpointer)& BitStream_Get_Custom_PropIsReading );
}

void Register_UnityEngine_BitStream_get_isWriting()
{
    scripting_add_internal_call( "UnityEngine.BitStream::get_isWriting" , (gpointer)& BitStream_Get_Custom_PropIsWriting );
}

void Register_UnityEngine_BitStream_Serialize()
{
    scripting_add_internal_call( "UnityEngine.BitStream::Serialize" , (gpointer)& BitStream_CUSTOM_Serialize );
}

void Register_UnityEngine_MasterServer_get_ipAddress()
{
    scripting_add_internal_call( "UnityEngine.MasterServer::get_ipAddress" , (gpointer)& MasterServer_Get_Custom_PropIpAddress );
}

void Register_UnityEngine_MasterServer_set_ipAddress()
{
    scripting_add_internal_call( "UnityEngine.MasterServer::set_ipAddress" , (gpointer)& MasterServer_Set_Custom_PropIpAddress );
}

void Register_UnityEngine_MasterServer_get_port()
{
    scripting_add_internal_call( "UnityEngine.MasterServer::get_port" , (gpointer)& MasterServer_Get_Custom_PropPort );
}

void Register_UnityEngine_MasterServer_set_port()
{
    scripting_add_internal_call( "UnityEngine.MasterServer::set_port" , (gpointer)& MasterServer_Set_Custom_PropPort );
}

void Register_UnityEngine_MasterServer_RequestHostList()
{
    scripting_add_internal_call( "UnityEngine.MasterServer::RequestHostList" , (gpointer)& MasterServer_CUSTOM_RequestHostList );
}

void Register_UnityEngine_MasterServer_PollHostList()
{
    scripting_add_internal_call( "UnityEngine.MasterServer::PollHostList" , (gpointer)& MasterServer_CUSTOM_PollHostList );
}

void Register_UnityEngine_MasterServer_RegisterHost()
{
    scripting_add_internal_call( "UnityEngine.MasterServer::RegisterHost" , (gpointer)& MasterServer_CUSTOM_RegisterHost );
}

void Register_UnityEngine_MasterServer_UnregisterHost()
{
    scripting_add_internal_call( "UnityEngine.MasterServer::UnregisterHost" , (gpointer)& MasterServer_CUSTOM_UnregisterHost );
}

void Register_UnityEngine_MasterServer_ClearHostList()
{
    scripting_add_internal_call( "UnityEngine.MasterServer::ClearHostList" , (gpointer)& MasterServer_CUSTOM_ClearHostList );
}

void Register_UnityEngine_MasterServer_get_updateRate()
{
    scripting_add_internal_call( "UnityEngine.MasterServer::get_updateRate" , (gpointer)& MasterServer_Get_Custom_PropUpdateRate );
}

void Register_UnityEngine_MasterServer_set_updateRate()
{
    scripting_add_internal_call( "UnityEngine.MasterServer::set_updateRate" , (gpointer)& MasterServer_Set_Custom_PropUpdateRate );
}

void Register_UnityEngine_MasterServer_get_dedicatedServer()
{
    scripting_add_internal_call( "UnityEngine.MasterServer::get_dedicatedServer" , (gpointer)& MasterServer_Get_Custom_PropDedicatedServer );
}

void Register_UnityEngine_MasterServer_set_dedicatedServer()
{
    scripting_add_internal_call( "UnityEngine.MasterServer::set_dedicatedServer" , (gpointer)& MasterServer_Set_Custom_PropDedicatedServer );
}

void Register_UnityEngine_NetworkMessageInfo_NullNetworkView()
{
    scripting_add_internal_call( "UnityEngine.NetworkMessageInfo::NullNetworkView" , (gpointer)& NetworkMessageInfo_CUSTOM_NullNetworkView );
}

#endif
#elif ENABLE_MONO || ENABLE_IL2CPP
static const char* s_Networking_IcallNames [] =
{
#if ENABLE_NETWORK
    "UnityEngine.NetworkPlayer::Internal_GetIPAddress",    // -> NetworkPlayer_CUSTOM_Internal_GetIPAddress
    "UnityEngine.NetworkPlayer::Internal_GetPort",    // -> NetworkPlayer_CUSTOM_Internal_GetPort
    "UnityEngine.NetworkPlayer::Internal_GetExternalIP",    // -> NetworkPlayer_CUSTOM_Internal_GetExternalIP
    "UnityEngine.NetworkPlayer::Internal_GetExternalPort",    // -> NetworkPlayer_CUSTOM_Internal_GetExternalPort
    "UnityEngine.NetworkPlayer::Internal_GetLocalIP",    // -> NetworkPlayer_CUSTOM_Internal_GetLocalIP
    "UnityEngine.NetworkPlayer::Internal_GetLocalPort",    // -> NetworkPlayer_CUSTOM_Internal_GetLocalPort
    "UnityEngine.NetworkPlayer::Internal_GetPlayerIndex",    // -> NetworkPlayer_CUSTOM_Internal_GetPlayerIndex
    "UnityEngine.NetworkPlayer::Internal_GetGUID",    // -> NetworkPlayer_CUSTOM_Internal_GetGUID
    "UnityEngine.NetworkPlayer::Internal_GetLocalGUID",    // -> NetworkPlayer_CUSTOM_Internal_GetLocalGUID
    "UnityEngine.NetworkViewID::INTERNAL_get_unassigned",    // -> NetworkViewID_CUSTOM_INTERNAL_get_unassigned
    "UnityEngine.NetworkViewID::INTERNAL_CALL_Internal_IsMine",    // -> NetworkViewID_CUSTOM_INTERNAL_CALL_Internal_IsMine
    "UnityEngine.NetworkViewID::INTERNAL_CALL_Internal_GetOwner",    // -> NetworkViewID_CUSTOM_INTERNAL_CALL_Internal_GetOwner
    "UnityEngine.NetworkViewID::INTERNAL_CALL_Internal_GetString",    // -> NetworkViewID_CUSTOM_INTERNAL_CALL_Internal_GetString
    "UnityEngine.NetworkViewID::INTERNAL_CALL_Internal_Compare",    // -> NetworkViewID_CUSTOM_INTERNAL_CALL_Internal_Compare
    "UnityEngine.Ping::.ctor"               ,    // -> Ping_CUSTOM_Ping
    "UnityEngine.Ping::DestroyPing"         ,    // -> Ping_CUSTOM_DestroyPing
    "UnityEngine.Ping::get_isDone"          ,    // -> Ping_Get_Custom_PropIsDone
    "UnityEngine.Ping::get_time"            ,    // -> Ping_Get_Custom_PropTime
    "UnityEngine.Ping::get_ip"              ,    // -> Ping_Get_Custom_PropIp
    "UnityEngine.NetworkView::Internal_RPC" ,    // -> NetworkView_CUSTOM_Internal_RPC
    "UnityEngine.NetworkView::INTERNAL_CALL_Internal_RPC_Target",    // -> NetworkView_CUSTOM_INTERNAL_CALL_Internal_RPC_Target
    "UnityEngine.NetworkView::get_observed" ,    // -> NetworkView_Get_Custom_PropObserved
    "UnityEngine.NetworkView::set_observed" ,    // -> NetworkView_Set_Custom_PropObserved
    "UnityEngine.NetworkView::get_stateSynchronization",    // -> NetworkView_Get_Custom_PropStateSynchronization
    "UnityEngine.NetworkView::set_stateSynchronization",    // -> NetworkView_Set_Custom_PropStateSynchronization
    "UnityEngine.NetworkView::Internal_GetViewID",    // -> NetworkView_CUSTOM_Internal_GetViewID
    "UnityEngine.NetworkView::INTERNAL_CALL_Internal_SetViewID",    // -> NetworkView_CUSTOM_INTERNAL_CALL_Internal_SetViewID
    "UnityEngine.NetworkView::get_group"    ,    // -> NetworkView_Get_Custom_PropGroup
    "UnityEngine.NetworkView::set_group"    ,    // -> NetworkView_Set_Custom_PropGroup
    "UnityEngine.NetworkView::INTERNAL_CALL_SetScope",    // -> NetworkView_CUSTOM_INTERNAL_CALL_SetScope
    "UnityEngine.NetworkView::INTERNAL_CALL_Find",    // -> NetworkView_CUSTOM_INTERNAL_CALL_Find
    "UnityEngine.Network::InitializeServer" ,    // -> Network_CUSTOM_InitializeServer
    "UnityEngine.Network::Internal_InitializeServerDeprecated",    // -> Network_CUSTOM_Internal_InitializeServerDeprecated
    "UnityEngine.Network::get_incomingPassword",    // -> Network_Get_Custom_PropIncomingPassword
    "UnityEngine.Network::set_incomingPassword",    // -> Network_Set_Custom_PropIncomingPassword
    "UnityEngine.Network::get_logLevel"     ,    // -> Network_Get_Custom_PropLogLevel
    "UnityEngine.Network::set_logLevel"     ,    // -> Network_Set_Custom_PropLogLevel
    "UnityEngine.Network::InitializeSecurity",    // -> Network_CUSTOM_InitializeSecurity
    "UnityEngine.Network::Internal_ConnectToSingleIP",    // -> Network_CUSTOM_Internal_ConnectToSingleIP
    "UnityEngine.Network::Internal_ConnectToGuid",    // -> Network_CUSTOM_Internal_ConnectToGuid
    "UnityEngine.Network::Internal_ConnectToIPs",    // -> Network_CUSTOM_Internal_ConnectToIPs
    "UnityEngine.Network::Disconnect"       ,    // -> Network_CUSTOM_Disconnect
    "UnityEngine.Network::INTERNAL_CALL_CloseConnection",    // -> Network_CUSTOM_INTERNAL_CALL_CloseConnection
    "UnityEngine.Network::get_connections"  ,    // -> Network_Get_Custom_PropConnections
    "UnityEngine.Network::Internal_GetPlayer",    // -> Network_CUSTOM_Internal_GetPlayer
    "UnityEngine.Network::Internal_AllocateViewID",    // -> Network_CUSTOM_Internal_AllocateViewID
    "UnityEngine.Network::INTERNAL_CALL_Instantiate",    // -> Network_CUSTOM_INTERNAL_CALL_Instantiate
    "UnityEngine.Network::INTERNAL_CALL_Destroy",    // -> Network_CUSTOM_INTERNAL_CALL_Destroy
    "UnityEngine.Network::INTERNAL_CALL_DestroyPlayerObjects",    // -> Network_CUSTOM_INTERNAL_CALL_DestroyPlayerObjects
    "UnityEngine.Network::INTERNAL_CALL_Internal_RemoveRPCs",    // -> Network_CUSTOM_INTERNAL_CALL_Internal_RemoveRPCs
    "UnityEngine.Network::get_isClient"     ,    // -> Network_Get_Custom_PropIsClient
    "UnityEngine.Network::get_isServer"     ,    // -> Network_Get_Custom_PropIsServer
    "UnityEngine.Network::get_peerType"     ,    // -> Network_Get_Custom_PropPeerType
    "UnityEngine.Network::SetLevelPrefix"   ,    // -> Network_CUSTOM_SetLevelPrefix
    "UnityEngine.Network::INTERNAL_CALL_GetLastPing",    // -> Network_CUSTOM_INTERNAL_CALL_GetLastPing
    "UnityEngine.Network::INTERNAL_CALL_GetAveragePing",    // -> Network_CUSTOM_INTERNAL_CALL_GetAveragePing
    "UnityEngine.Network::get_sendRate"     ,    // -> Network_Get_Custom_PropSendRate
    "UnityEngine.Network::set_sendRate"     ,    // -> Network_Set_Custom_PropSendRate
    "UnityEngine.Network::get_isMessageQueueRunning",    // -> Network_Get_Custom_PropIsMessageQueueRunning
    "UnityEngine.Network::set_isMessageQueueRunning",    // -> Network_Set_Custom_PropIsMessageQueueRunning
    "UnityEngine.Network::INTERNAL_CALL_SetReceivingEnabled",    // -> Network_CUSTOM_INTERNAL_CALL_SetReceivingEnabled
    "UnityEngine.Network::Internal_SetSendingGlobal",    // -> Network_CUSTOM_Internal_SetSendingGlobal
    "UnityEngine.Network::INTERNAL_CALL_Internal_SetSendingSpecific",    // -> Network_CUSTOM_INTERNAL_CALL_Internal_SetSendingSpecific
    "UnityEngine.Network::Internal_GetTime" ,    // -> Network_CUSTOM_Internal_GetTime
    "UnityEngine.Network::get_minimumAllocatableViewIDs",    // -> Network_Get_Custom_PropMinimumAllocatableViewIDs
    "UnityEngine.Network::set_minimumAllocatableViewIDs",    // -> Network_Set_Custom_PropMinimumAllocatableViewIDs
    "UnityEngine.Network::get_useNat"       ,    // -> Network_Get_Custom_PropUseNat
    "UnityEngine.Network::set_useNat"       ,    // -> Network_Set_Custom_PropUseNat
    "UnityEngine.Network::get_natFacilitatorIP",    // -> Network_Get_Custom_PropNatFacilitatorIP
    "UnityEngine.Network::set_natFacilitatorIP",    // -> Network_Set_Custom_PropNatFacilitatorIP
    "UnityEngine.Network::get_natFacilitatorPort",    // -> Network_Get_Custom_PropNatFacilitatorPort
    "UnityEngine.Network::set_natFacilitatorPort",    // -> Network_Set_Custom_PropNatFacilitatorPort
    "UnityEngine.Network::TestConnection"   ,    // -> Network_CUSTOM_TestConnection
    "UnityEngine.Network::TestConnectionNAT",    // -> Network_CUSTOM_TestConnectionNAT
    "UnityEngine.Network::get_connectionTesterIP",    // -> Network_Get_Custom_PropConnectionTesterIP
    "UnityEngine.Network::set_connectionTesterIP",    // -> Network_Set_Custom_PropConnectionTesterIP
    "UnityEngine.Network::get_connectionTesterPort",    // -> Network_Get_Custom_PropConnectionTesterPort
    "UnityEngine.Network::set_connectionTesterPort",    // -> Network_Set_Custom_PropConnectionTesterPort
    "UnityEngine.Network::HavePublicAddress",    // -> Network_CUSTOM_HavePublicAddress
    "UnityEngine.Network::get_maxConnections",    // -> Network_Get_Custom_PropMaxConnections
    "UnityEngine.Network::set_maxConnections",    // -> Network_Set_Custom_PropMaxConnections
    "UnityEngine.Network::get_proxyIP"      ,    // -> Network_Get_Custom_PropProxyIP
    "UnityEngine.Network::set_proxyIP"      ,    // -> Network_Set_Custom_PropProxyIP
    "UnityEngine.Network::get_proxyPort"    ,    // -> Network_Get_Custom_PropProxyPort
    "UnityEngine.Network::set_proxyPort"    ,    // -> Network_Set_Custom_PropProxyPort
    "UnityEngine.Network::get_useProxy"     ,    // -> Network_Get_Custom_PropUseProxy
    "UnityEngine.Network::set_useProxy"     ,    // -> Network_Set_Custom_PropUseProxy
    "UnityEngine.Network::get_proxyPassword",    // -> Network_Get_Custom_PropProxyPassword
    "UnityEngine.Network::set_proxyPassword",    // -> Network_Set_Custom_PropProxyPassword
    "UnityEngine.BitStream::Serializeb"     ,    // -> BitStream_CUSTOM_Serializeb
    "UnityEngine.BitStream::Serializec"     ,    // -> BitStream_CUSTOM_Serializec
    "UnityEngine.BitStream::Serializes"     ,    // -> BitStream_CUSTOM_Serializes
    "UnityEngine.BitStream::Serializei"     ,    // -> BitStream_CUSTOM_Serializei
    "UnityEngine.BitStream::Serializef"     ,    // -> BitStream_CUSTOM_Serializef
    "UnityEngine.BitStream::INTERNAL_CALL_Serializeq",    // -> BitStream_CUSTOM_INTERNAL_CALL_Serializeq
    "UnityEngine.BitStream::INTERNAL_CALL_Serializev",    // -> BitStream_CUSTOM_INTERNAL_CALL_Serializev
    "UnityEngine.BitStream::INTERNAL_CALL_Serializen",    // -> BitStream_CUSTOM_INTERNAL_CALL_Serializen
    "UnityEngine.BitStream::get_isReading"  ,    // -> BitStream_Get_Custom_PropIsReading
    "UnityEngine.BitStream::get_isWriting"  ,    // -> BitStream_Get_Custom_PropIsWriting
    "UnityEngine.BitStream::Serialize"      ,    // -> BitStream_CUSTOM_Serialize
    "UnityEngine.MasterServer::get_ipAddress",    // -> MasterServer_Get_Custom_PropIpAddress
    "UnityEngine.MasterServer::set_ipAddress",    // -> MasterServer_Set_Custom_PropIpAddress
    "UnityEngine.MasterServer::get_port"    ,    // -> MasterServer_Get_Custom_PropPort
    "UnityEngine.MasterServer::set_port"    ,    // -> MasterServer_Set_Custom_PropPort
    "UnityEngine.MasterServer::RequestHostList",    // -> MasterServer_CUSTOM_RequestHostList
    "UnityEngine.MasterServer::PollHostList",    // -> MasterServer_CUSTOM_PollHostList
    "UnityEngine.MasterServer::RegisterHost",    // -> MasterServer_CUSTOM_RegisterHost
    "UnityEngine.MasterServer::UnregisterHost",    // -> MasterServer_CUSTOM_UnregisterHost
    "UnityEngine.MasterServer::ClearHostList",    // -> MasterServer_CUSTOM_ClearHostList
    "UnityEngine.MasterServer::get_updateRate",    // -> MasterServer_Get_Custom_PropUpdateRate
    "UnityEngine.MasterServer::set_updateRate",    // -> MasterServer_Set_Custom_PropUpdateRate
    "UnityEngine.MasterServer::get_dedicatedServer",    // -> MasterServer_Get_Custom_PropDedicatedServer
    "UnityEngine.MasterServer::set_dedicatedServer",    // -> MasterServer_Set_Custom_PropDedicatedServer
    "UnityEngine.NetworkMessageInfo::NullNetworkView",    // -> NetworkMessageInfo_CUSTOM_NullNetworkView
#endif
    NULL
};

static const void* s_Networking_IcallFuncs [] =
{
#if ENABLE_NETWORK
    (const void*)&NetworkPlayer_CUSTOM_Internal_GetIPAddress,  //  <- UnityEngine.NetworkPlayer::Internal_GetIPAddress
    (const void*)&NetworkPlayer_CUSTOM_Internal_GetPort   ,  //  <- UnityEngine.NetworkPlayer::Internal_GetPort
    (const void*)&NetworkPlayer_CUSTOM_Internal_GetExternalIP,  //  <- UnityEngine.NetworkPlayer::Internal_GetExternalIP
    (const void*)&NetworkPlayer_CUSTOM_Internal_GetExternalPort,  //  <- UnityEngine.NetworkPlayer::Internal_GetExternalPort
    (const void*)&NetworkPlayer_CUSTOM_Internal_GetLocalIP,  //  <- UnityEngine.NetworkPlayer::Internal_GetLocalIP
    (const void*)&NetworkPlayer_CUSTOM_Internal_GetLocalPort,  //  <- UnityEngine.NetworkPlayer::Internal_GetLocalPort
    (const void*)&NetworkPlayer_CUSTOM_Internal_GetPlayerIndex,  //  <- UnityEngine.NetworkPlayer::Internal_GetPlayerIndex
    (const void*)&NetworkPlayer_CUSTOM_Internal_GetGUID   ,  //  <- UnityEngine.NetworkPlayer::Internal_GetGUID
    (const void*)&NetworkPlayer_CUSTOM_Internal_GetLocalGUID,  //  <- UnityEngine.NetworkPlayer::Internal_GetLocalGUID
    (const void*)&NetworkViewID_CUSTOM_INTERNAL_get_unassigned,  //  <- UnityEngine.NetworkViewID::INTERNAL_get_unassigned
    (const void*)&NetworkViewID_CUSTOM_INTERNAL_CALL_Internal_IsMine,  //  <- UnityEngine.NetworkViewID::INTERNAL_CALL_Internal_IsMine
    (const void*)&NetworkViewID_CUSTOM_INTERNAL_CALL_Internal_GetOwner,  //  <- UnityEngine.NetworkViewID::INTERNAL_CALL_Internal_GetOwner
    (const void*)&NetworkViewID_CUSTOM_INTERNAL_CALL_Internal_GetString,  //  <- UnityEngine.NetworkViewID::INTERNAL_CALL_Internal_GetString
    (const void*)&NetworkViewID_CUSTOM_INTERNAL_CALL_Internal_Compare,  //  <- UnityEngine.NetworkViewID::INTERNAL_CALL_Internal_Compare
    (const void*)&Ping_CUSTOM_Ping                        ,  //  <- UnityEngine.Ping::.ctor
    (const void*)&Ping_CUSTOM_DestroyPing                 ,  //  <- UnityEngine.Ping::DestroyPing
    (const void*)&Ping_Get_Custom_PropIsDone              ,  //  <- UnityEngine.Ping::get_isDone
    (const void*)&Ping_Get_Custom_PropTime                ,  //  <- UnityEngine.Ping::get_time
    (const void*)&Ping_Get_Custom_PropIp                  ,  //  <- UnityEngine.Ping::get_ip
    (const void*)&NetworkView_CUSTOM_Internal_RPC         ,  //  <- UnityEngine.NetworkView::Internal_RPC
    (const void*)&NetworkView_CUSTOM_INTERNAL_CALL_Internal_RPC_Target,  //  <- UnityEngine.NetworkView::INTERNAL_CALL_Internal_RPC_Target
    (const void*)&NetworkView_Get_Custom_PropObserved     ,  //  <- UnityEngine.NetworkView::get_observed
    (const void*)&NetworkView_Set_Custom_PropObserved     ,  //  <- UnityEngine.NetworkView::set_observed
    (const void*)&NetworkView_Get_Custom_PropStateSynchronization,  //  <- UnityEngine.NetworkView::get_stateSynchronization
    (const void*)&NetworkView_Set_Custom_PropStateSynchronization,  //  <- UnityEngine.NetworkView::set_stateSynchronization
    (const void*)&NetworkView_CUSTOM_Internal_GetViewID   ,  //  <- UnityEngine.NetworkView::Internal_GetViewID
    (const void*)&NetworkView_CUSTOM_INTERNAL_CALL_Internal_SetViewID,  //  <- UnityEngine.NetworkView::INTERNAL_CALL_Internal_SetViewID
    (const void*)&NetworkView_Get_Custom_PropGroup        ,  //  <- UnityEngine.NetworkView::get_group
    (const void*)&NetworkView_Set_Custom_PropGroup        ,  //  <- UnityEngine.NetworkView::set_group
    (const void*)&NetworkView_CUSTOM_INTERNAL_CALL_SetScope,  //  <- UnityEngine.NetworkView::INTERNAL_CALL_SetScope
    (const void*)&NetworkView_CUSTOM_INTERNAL_CALL_Find   ,  //  <- UnityEngine.NetworkView::INTERNAL_CALL_Find
    (const void*)&Network_CUSTOM_InitializeServer         ,  //  <- UnityEngine.Network::InitializeServer
    (const void*)&Network_CUSTOM_Internal_InitializeServerDeprecated,  //  <- UnityEngine.Network::Internal_InitializeServerDeprecated
    (const void*)&Network_Get_Custom_PropIncomingPassword ,  //  <- UnityEngine.Network::get_incomingPassword
    (const void*)&Network_Set_Custom_PropIncomingPassword ,  //  <- UnityEngine.Network::set_incomingPassword
    (const void*)&Network_Get_Custom_PropLogLevel         ,  //  <- UnityEngine.Network::get_logLevel
    (const void*)&Network_Set_Custom_PropLogLevel         ,  //  <- UnityEngine.Network::set_logLevel
    (const void*)&Network_CUSTOM_InitializeSecurity       ,  //  <- UnityEngine.Network::InitializeSecurity
    (const void*)&Network_CUSTOM_Internal_ConnectToSingleIP,  //  <- UnityEngine.Network::Internal_ConnectToSingleIP
    (const void*)&Network_CUSTOM_Internal_ConnectToGuid   ,  //  <- UnityEngine.Network::Internal_ConnectToGuid
    (const void*)&Network_CUSTOM_Internal_ConnectToIPs    ,  //  <- UnityEngine.Network::Internal_ConnectToIPs
    (const void*)&Network_CUSTOM_Disconnect               ,  //  <- UnityEngine.Network::Disconnect
    (const void*)&Network_CUSTOM_INTERNAL_CALL_CloseConnection,  //  <- UnityEngine.Network::INTERNAL_CALL_CloseConnection
    (const void*)&Network_Get_Custom_PropConnections      ,  //  <- UnityEngine.Network::get_connections
    (const void*)&Network_CUSTOM_Internal_GetPlayer       ,  //  <- UnityEngine.Network::Internal_GetPlayer
    (const void*)&Network_CUSTOM_Internal_AllocateViewID  ,  //  <- UnityEngine.Network::Internal_AllocateViewID
    (const void*)&Network_CUSTOM_INTERNAL_CALL_Instantiate,  //  <- UnityEngine.Network::INTERNAL_CALL_Instantiate
    (const void*)&Network_CUSTOM_INTERNAL_CALL_Destroy    ,  //  <- UnityEngine.Network::INTERNAL_CALL_Destroy
    (const void*)&Network_CUSTOM_INTERNAL_CALL_DestroyPlayerObjects,  //  <- UnityEngine.Network::INTERNAL_CALL_DestroyPlayerObjects
    (const void*)&Network_CUSTOM_INTERNAL_CALL_Internal_RemoveRPCs,  //  <- UnityEngine.Network::INTERNAL_CALL_Internal_RemoveRPCs
    (const void*)&Network_Get_Custom_PropIsClient         ,  //  <- UnityEngine.Network::get_isClient
    (const void*)&Network_Get_Custom_PropIsServer         ,  //  <- UnityEngine.Network::get_isServer
    (const void*)&Network_Get_Custom_PropPeerType         ,  //  <- UnityEngine.Network::get_peerType
    (const void*)&Network_CUSTOM_SetLevelPrefix           ,  //  <- UnityEngine.Network::SetLevelPrefix
    (const void*)&Network_CUSTOM_INTERNAL_CALL_GetLastPing,  //  <- UnityEngine.Network::INTERNAL_CALL_GetLastPing
    (const void*)&Network_CUSTOM_INTERNAL_CALL_GetAveragePing,  //  <- UnityEngine.Network::INTERNAL_CALL_GetAveragePing
    (const void*)&Network_Get_Custom_PropSendRate         ,  //  <- UnityEngine.Network::get_sendRate
    (const void*)&Network_Set_Custom_PropSendRate         ,  //  <- UnityEngine.Network::set_sendRate
    (const void*)&Network_Get_Custom_PropIsMessageQueueRunning,  //  <- UnityEngine.Network::get_isMessageQueueRunning
    (const void*)&Network_Set_Custom_PropIsMessageQueueRunning,  //  <- UnityEngine.Network::set_isMessageQueueRunning
    (const void*)&Network_CUSTOM_INTERNAL_CALL_SetReceivingEnabled,  //  <- UnityEngine.Network::INTERNAL_CALL_SetReceivingEnabled
    (const void*)&Network_CUSTOM_Internal_SetSendingGlobal,  //  <- UnityEngine.Network::Internal_SetSendingGlobal
    (const void*)&Network_CUSTOM_INTERNAL_CALL_Internal_SetSendingSpecific,  //  <- UnityEngine.Network::INTERNAL_CALL_Internal_SetSendingSpecific
    (const void*)&Network_CUSTOM_Internal_GetTime         ,  //  <- UnityEngine.Network::Internal_GetTime
    (const void*)&Network_Get_Custom_PropMinimumAllocatableViewIDs,  //  <- UnityEngine.Network::get_minimumAllocatableViewIDs
    (const void*)&Network_Set_Custom_PropMinimumAllocatableViewIDs,  //  <- UnityEngine.Network::set_minimumAllocatableViewIDs
    (const void*)&Network_Get_Custom_PropUseNat           ,  //  <- UnityEngine.Network::get_useNat
    (const void*)&Network_Set_Custom_PropUseNat           ,  //  <- UnityEngine.Network::set_useNat
    (const void*)&Network_Get_Custom_PropNatFacilitatorIP ,  //  <- UnityEngine.Network::get_natFacilitatorIP
    (const void*)&Network_Set_Custom_PropNatFacilitatorIP ,  //  <- UnityEngine.Network::set_natFacilitatorIP
    (const void*)&Network_Get_Custom_PropNatFacilitatorPort,  //  <- UnityEngine.Network::get_natFacilitatorPort
    (const void*)&Network_Set_Custom_PropNatFacilitatorPort,  //  <- UnityEngine.Network::set_natFacilitatorPort
    (const void*)&Network_CUSTOM_TestConnection           ,  //  <- UnityEngine.Network::TestConnection
    (const void*)&Network_CUSTOM_TestConnectionNAT        ,  //  <- UnityEngine.Network::TestConnectionNAT
    (const void*)&Network_Get_Custom_PropConnectionTesterIP,  //  <- UnityEngine.Network::get_connectionTesterIP
    (const void*)&Network_Set_Custom_PropConnectionTesterIP,  //  <- UnityEngine.Network::set_connectionTesterIP
    (const void*)&Network_Get_Custom_PropConnectionTesterPort,  //  <- UnityEngine.Network::get_connectionTesterPort
    (const void*)&Network_Set_Custom_PropConnectionTesterPort,  //  <- UnityEngine.Network::set_connectionTesterPort
    (const void*)&Network_CUSTOM_HavePublicAddress        ,  //  <- UnityEngine.Network::HavePublicAddress
    (const void*)&Network_Get_Custom_PropMaxConnections   ,  //  <- UnityEngine.Network::get_maxConnections
    (const void*)&Network_Set_Custom_PropMaxConnections   ,  //  <- UnityEngine.Network::set_maxConnections
    (const void*)&Network_Get_Custom_PropProxyIP          ,  //  <- UnityEngine.Network::get_proxyIP
    (const void*)&Network_Set_Custom_PropProxyIP          ,  //  <- UnityEngine.Network::set_proxyIP
    (const void*)&Network_Get_Custom_PropProxyPort        ,  //  <- UnityEngine.Network::get_proxyPort
    (const void*)&Network_Set_Custom_PropProxyPort        ,  //  <- UnityEngine.Network::set_proxyPort
    (const void*)&Network_Get_Custom_PropUseProxy         ,  //  <- UnityEngine.Network::get_useProxy
    (const void*)&Network_Set_Custom_PropUseProxy         ,  //  <- UnityEngine.Network::set_useProxy
    (const void*)&Network_Get_Custom_PropProxyPassword    ,  //  <- UnityEngine.Network::get_proxyPassword
    (const void*)&Network_Set_Custom_PropProxyPassword    ,  //  <- UnityEngine.Network::set_proxyPassword
    (const void*)&BitStream_CUSTOM_Serializeb             ,  //  <- UnityEngine.BitStream::Serializeb
    (const void*)&BitStream_CUSTOM_Serializec             ,  //  <- UnityEngine.BitStream::Serializec
    (const void*)&BitStream_CUSTOM_Serializes             ,  //  <- UnityEngine.BitStream::Serializes
    (const void*)&BitStream_CUSTOM_Serializei             ,  //  <- UnityEngine.BitStream::Serializei
    (const void*)&BitStream_CUSTOM_Serializef             ,  //  <- UnityEngine.BitStream::Serializef
    (const void*)&BitStream_CUSTOM_INTERNAL_CALL_Serializeq,  //  <- UnityEngine.BitStream::INTERNAL_CALL_Serializeq
    (const void*)&BitStream_CUSTOM_INTERNAL_CALL_Serializev,  //  <- UnityEngine.BitStream::INTERNAL_CALL_Serializev
    (const void*)&BitStream_CUSTOM_INTERNAL_CALL_Serializen,  //  <- UnityEngine.BitStream::INTERNAL_CALL_Serializen
    (const void*)&BitStream_Get_Custom_PropIsReading      ,  //  <- UnityEngine.BitStream::get_isReading
    (const void*)&BitStream_Get_Custom_PropIsWriting      ,  //  <- UnityEngine.BitStream::get_isWriting
    (const void*)&BitStream_CUSTOM_Serialize              ,  //  <- UnityEngine.BitStream::Serialize
    (const void*)&MasterServer_Get_Custom_PropIpAddress   ,  //  <- UnityEngine.MasterServer::get_ipAddress
    (const void*)&MasterServer_Set_Custom_PropIpAddress   ,  //  <- UnityEngine.MasterServer::set_ipAddress
    (const void*)&MasterServer_Get_Custom_PropPort        ,  //  <- UnityEngine.MasterServer::get_port
    (const void*)&MasterServer_Set_Custom_PropPort        ,  //  <- UnityEngine.MasterServer::set_port
    (const void*)&MasterServer_CUSTOM_RequestHostList     ,  //  <- UnityEngine.MasterServer::RequestHostList
    (const void*)&MasterServer_CUSTOM_PollHostList        ,  //  <- UnityEngine.MasterServer::PollHostList
    (const void*)&MasterServer_CUSTOM_RegisterHost        ,  //  <- UnityEngine.MasterServer::RegisterHost
    (const void*)&MasterServer_CUSTOM_UnregisterHost      ,  //  <- UnityEngine.MasterServer::UnregisterHost
    (const void*)&MasterServer_CUSTOM_ClearHostList       ,  //  <- UnityEngine.MasterServer::ClearHostList
    (const void*)&MasterServer_Get_Custom_PropUpdateRate  ,  //  <- UnityEngine.MasterServer::get_updateRate
    (const void*)&MasterServer_Set_Custom_PropUpdateRate  ,  //  <- UnityEngine.MasterServer::set_updateRate
    (const void*)&MasterServer_Get_Custom_PropDedicatedServer,  //  <- UnityEngine.MasterServer::get_dedicatedServer
    (const void*)&MasterServer_Set_Custom_PropDedicatedServer,  //  <- UnityEngine.MasterServer::set_dedicatedServer
    (const void*)&NetworkMessageInfo_CUSTOM_NullNetworkView,  //  <- UnityEngine.NetworkMessageInfo::NullNetworkView
#endif
    NULL
};

void ExportNetworkingBindings();
void ExportNetworkingBindings()
{
    for (int i = 0; s_Networking_IcallNames [i] != NULL; ++i )
        scripting_add_internal_call( s_Networking_IcallNames [i], s_Networking_IcallFuncs [i] );
}

#elif ENABLE_DOTNET
#include "Runtime/Scripting/WinRTHelper.h"
void ExportNetworkingBindings()
{
    #if UNITY_WP8
    extern intptr_t g_WinRTFuncPtrs[];
    #define SET_METRO_BINDING(Name) g_WinRTFuncPtrs[k##Name##FuncDef] = reinterpret_cast<intptr_t>(Name);
    #else
    long long* p = GetWinRTFuncDefsPointers();
    #define SET_METRO_BINDING(Name) p[k##Name##Func] = (long long)Name;
    #endif
#if ENABLE_NETWORK
    SET_METRO_BINDING(NetworkPlayer_CUSTOM_Internal_GetIPAddress); //  <- UnityEngine.NetworkPlayer::Internal_GetIPAddress
    SET_METRO_BINDING(NetworkPlayer_CUSTOM_Internal_GetPort); //  <- UnityEngine.NetworkPlayer::Internal_GetPort
    SET_METRO_BINDING(NetworkPlayer_CUSTOM_Internal_GetExternalIP); //  <- UnityEngine.NetworkPlayer::Internal_GetExternalIP
    SET_METRO_BINDING(NetworkPlayer_CUSTOM_Internal_GetExternalPort); //  <- UnityEngine.NetworkPlayer::Internal_GetExternalPort
    SET_METRO_BINDING(NetworkPlayer_CUSTOM_Internal_GetLocalIP); //  <- UnityEngine.NetworkPlayer::Internal_GetLocalIP
    SET_METRO_BINDING(NetworkPlayer_CUSTOM_Internal_GetLocalPort); //  <- UnityEngine.NetworkPlayer::Internal_GetLocalPort
    SET_METRO_BINDING(NetworkPlayer_CUSTOM_Internal_GetPlayerIndex); //  <- UnityEngine.NetworkPlayer::Internal_GetPlayerIndex
    SET_METRO_BINDING(NetworkPlayer_CUSTOM_Internal_GetGUID); //  <- UnityEngine.NetworkPlayer::Internal_GetGUID
    SET_METRO_BINDING(NetworkPlayer_CUSTOM_Internal_GetLocalGUID); //  <- UnityEngine.NetworkPlayer::Internal_GetLocalGUID
    SET_METRO_BINDING(NetworkViewID_CUSTOM_INTERNAL_get_unassigned); //  <- UnityEngine.NetworkViewID::INTERNAL_get_unassigned
    SET_METRO_BINDING(NetworkViewID_CUSTOM_INTERNAL_CALL_Internal_IsMine); //  <- UnityEngine.NetworkViewID::INTERNAL_CALL_Internal_IsMine
    SET_METRO_BINDING(NetworkViewID_CUSTOM_INTERNAL_CALL_Internal_GetOwner); //  <- UnityEngine.NetworkViewID::INTERNAL_CALL_Internal_GetOwner
    SET_METRO_BINDING(NetworkViewID_CUSTOM_INTERNAL_CALL_Internal_GetString); //  <- UnityEngine.NetworkViewID::INTERNAL_CALL_Internal_GetString
    SET_METRO_BINDING(NetworkViewID_CUSTOM_INTERNAL_CALL_Internal_Compare); //  <- UnityEngine.NetworkViewID::INTERNAL_CALL_Internal_Compare
    SET_METRO_BINDING(Ping_CUSTOM_Ping); //  <- UnityEngine.Ping::.ctor
    SET_METRO_BINDING(Ping_CUSTOM_DestroyPing); //  <- UnityEngine.Ping::DestroyPing
    SET_METRO_BINDING(Ping_Get_Custom_PropIsDone); //  <- UnityEngine.Ping::get_isDone
    SET_METRO_BINDING(Ping_Get_Custom_PropTime); //  <- UnityEngine.Ping::get_time
    SET_METRO_BINDING(Ping_Get_Custom_PropIp); //  <- UnityEngine.Ping::get_ip
    SET_METRO_BINDING(NetworkView_CUSTOM_Internal_RPC); //  <- UnityEngine.NetworkView::Internal_RPC
    SET_METRO_BINDING(NetworkView_CUSTOM_INTERNAL_CALL_Internal_RPC_Target); //  <- UnityEngine.NetworkView::INTERNAL_CALL_Internal_RPC_Target
    SET_METRO_BINDING(NetworkView_Get_Custom_PropObserved); //  <- UnityEngine.NetworkView::get_observed
    SET_METRO_BINDING(NetworkView_Set_Custom_PropObserved); //  <- UnityEngine.NetworkView::set_observed
    SET_METRO_BINDING(NetworkView_Get_Custom_PropStateSynchronization); //  <- UnityEngine.NetworkView::get_stateSynchronization
    SET_METRO_BINDING(NetworkView_Set_Custom_PropStateSynchronization); //  <- UnityEngine.NetworkView::set_stateSynchronization
    SET_METRO_BINDING(NetworkView_CUSTOM_Internal_GetViewID); //  <- UnityEngine.NetworkView::Internal_GetViewID
    SET_METRO_BINDING(NetworkView_CUSTOM_INTERNAL_CALL_Internal_SetViewID); //  <- UnityEngine.NetworkView::INTERNAL_CALL_Internal_SetViewID
    SET_METRO_BINDING(NetworkView_Get_Custom_PropGroup); //  <- UnityEngine.NetworkView::get_group
    SET_METRO_BINDING(NetworkView_Set_Custom_PropGroup); //  <- UnityEngine.NetworkView::set_group
    SET_METRO_BINDING(NetworkView_CUSTOM_INTERNAL_CALL_SetScope); //  <- UnityEngine.NetworkView::INTERNAL_CALL_SetScope
    SET_METRO_BINDING(NetworkView_CUSTOM_INTERNAL_CALL_Find); //  <- UnityEngine.NetworkView::INTERNAL_CALL_Find
    SET_METRO_BINDING(Network_CUSTOM_InitializeServer); //  <- UnityEngine.Network::InitializeServer
    SET_METRO_BINDING(Network_CUSTOM_Internal_InitializeServerDeprecated); //  <- UnityEngine.Network::Internal_InitializeServerDeprecated
    SET_METRO_BINDING(Network_Get_Custom_PropIncomingPassword); //  <- UnityEngine.Network::get_incomingPassword
    SET_METRO_BINDING(Network_Set_Custom_PropIncomingPassword); //  <- UnityEngine.Network::set_incomingPassword
    SET_METRO_BINDING(Network_Get_Custom_PropLogLevel); //  <- UnityEngine.Network::get_logLevel
    SET_METRO_BINDING(Network_Set_Custom_PropLogLevel); //  <- UnityEngine.Network::set_logLevel
    SET_METRO_BINDING(Network_CUSTOM_InitializeSecurity); //  <- UnityEngine.Network::InitializeSecurity
    SET_METRO_BINDING(Network_CUSTOM_Internal_ConnectToSingleIP); //  <- UnityEngine.Network::Internal_ConnectToSingleIP
    SET_METRO_BINDING(Network_CUSTOM_Internal_ConnectToGuid); //  <- UnityEngine.Network::Internal_ConnectToGuid
    SET_METRO_BINDING(Network_CUSTOM_Internal_ConnectToIPs); //  <- UnityEngine.Network::Internal_ConnectToIPs
    SET_METRO_BINDING(Network_CUSTOM_Disconnect); //  <- UnityEngine.Network::Disconnect
    SET_METRO_BINDING(Network_CUSTOM_INTERNAL_CALL_CloseConnection); //  <- UnityEngine.Network::INTERNAL_CALL_CloseConnection
    SET_METRO_BINDING(Network_Get_Custom_PropConnections); //  <- UnityEngine.Network::get_connections
    SET_METRO_BINDING(Network_CUSTOM_Internal_GetPlayer); //  <- UnityEngine.Network::Internal_GetPlayer
    SET_METRO_BINDING(Network_CUSTOM_Internal_AllocateViewID); //  <- UnityEngine.Network::Internal_AllocateViewID
    SET_METRO_BINDING(Network_CUSTOM_INTERNAL_CALL_Instantiate); //  <- UnityEngine.Network::INTERNAL_CALL_Instantiate
    SET_METRO_BINDING(Network_CUSTOM_INTERNAL_CALL_Destroy); //  <- UnityEngine.Network::INTERNAL_CALL_Destroy
    SET_METRO_BINDING(Network_CUSTOM_INTERNAL_CALL_DestroyPlayerObjects); //  <- UnityEngine.Network::INTERNAL_CALL_DestroyPlayerObjects
    SET_METRO_BINDING(Network_CUSTOM_INTERNAL_CALL_Internal_RemoveRPCs); //  <- UnityEngine.Network::INTERNAL_CALL_Internal_RemoveRPCs
    SET_METRO_BINDING(Network_Get_Custom_PropIsClient); //  <- UnityEngine.Network::get_isClient
    SET_METRO_BINDING(Network_Get_Custom_PropIsServer); //  <- UnityEngine.Network::get_isServer
    SET_METRO_BINDING(Network_Get_Custom_PropPeerType); //  <- UnityEngine.Network::get_peerType
    SET_METRO_BINDING(Network_CUSTOM_SetLevelPrefix); //  <- UnityEngine.Network::SetLevelPrefix
    SET_METRO_BINDING(Network_CUSTOM_INTERNAL_CALL_GetLastPing); //  <- UnityEngine.Network::INTERNAL_CALL_GetLastPing
    SET_METRO_BINDING(Network_CUSTOM_INTERNAL_CALL_GetAveragePing); //  <- UnityEngine.Network::INTERNAL_CALL_GetAveragePing
    SET_METRO_BINDING(Network_Get_Custom_PropSendRate); //  <- UnityEngine.Network::get_sendRate
    SET_METRO_BINDING(Network_Set_Custom_PropSendRate); //  <- UnityEngine.Network::set_sendRate
    SET_METRO_BINDING(Network_Get_Custom_PropIsMessageQueueRunning); //  <- UnityEngine.Network::get_isMessageQueueRunning
    SET_METRO_BINDING(Network_Set_Custom_PropIsMessageQueueRunning); //  <- UnityEngine.Network::set_isMessageQueueRunning
    SET_METRO_BINDING(Network_CUSTOM_INTERNAL_CALL_SetReceivingEnabled); //  <- UnityEngine.Network::INTERNAL_CALL_SetReceivingEnabled
    SET_METRO_BINDING(Network_CUSTOM_Internal_SetSendingGlobal); //  <- UnityEngine.Network::Internal_SetSendingGlobal
    SET_METRO_BINDING(Network_CUSTOM_INTERNAL_CALL_Internal_SetSendingSpecific); //  <- UnityEngine.Network::INTERNAL_CALL_Internal_SetSendingSpecific
    SET_METRO_BINDING(Network_CUSTOM_Internal_GetTime); //  <- UnityEngine.Network::Internal_GetTime
    SET_METRO_BINDING(Network_Get_Custom_PropMinimumAllocatableViewIDs); //  <- UnityEngine.Network::get_minimumAllocatableViewIDs
    SET_METRO_BINDING(Network_Set_Custom_PropMinimumAllocatableViewIDs); //  <- UnityEngine.Network::set_minimumAllocatableViewIDs
    SET_METRO_BINDING(Network_Get_Custom_PropUseNat); //  <- UnityEngine.Network::get_useNat
    SET_METRO_BINDING(Network_Set_Custom_PropUseNat); //  <- UnityEngine.Network::set_useNat
    SET_METRO_BINDING(Network_Get_Custom_PropNatFacilitatorIP); //  <- UnityEngine.Network::get_natFacilitatorIP
    SET_METRO_BINDING(Network_Set_Custom_PropNatFacilitatorIP); //  <- UnityEngine.Network::set_natFacilitatorIP
    SET_METRO_BINDING(Network_Get_Custom_PropNatFacilitatorPort); //  <- UnityEngine.Network::get_natFacilitatorPort
    SET_METRO_BINDING(Network_Set_Custom_PropNatFacilitatorPort); //  <- UnityEngine.Network::set_natFacilitatorPort
    SET_METRO_BINDING(Network_CUSTOM_TestConnection); //  <- UnityEngine.Network::TestConnection
    SET_METRO_BINDING(Network_CUSTOM_TestConnectionNAT); //  <- UnityEngine.Network::TestConnectionNAT
    SET_METRO_BINDING(Network_Get_Custom_PropConnectionTesterIP); //  <- UnityEngine.Network::get_connectionTesterIP
    SET_METRO_BINDING(Network_Set_Custom_PropConnectionTesterIP); //  <- UnityEngine.Network::set_connectionTesterIP
    SET_METRO_BINDING(Network_Get_Custom_PropConnectionTesterPort); //  <- UnityEngine.Network::get_connectionTesterPort
    SET_METRO_BINDING(Network_Set_Custom_PropConnectionTesterPort); //  <- UnityEngine.Network::set_connectionTesterPort
    SET_METRO_BINDING(Network_CUSTOM_HavePublicAddress); //  <- UnityEngine.Network::HavePublicAddress
    SET_METRO_BINDING(Network_Get_Custom_PropMaxConnections); //  <- UnityEngine.Network::get_maxConnections
    SET_METRO_BINDING(Network_Set_Custom_PropMaxConnections); //  <- UnityEngine.Network::set_maxConnections
    SET_METRO_BINDING(Network_Get_Custom_PropProxyIP); //  <- UnityEngine.Network::get_proxyIP
    SET_METRO_BINDING(Network_Set_Custom_PropProxyIP); //  <- UnityEngine.Network::set_proxyIP
    SET_METRO_BINDING(Network_Get_Custom_PropProxyPort); //  <- UnityEngine.Network::get_proxyPort
    SET_METRO_BINDING(Network_Set_Custom_PropProxyPort); //  <- UnityEngine.Network::set_proxyPort
    SET_METRO_BINDING(Network_Get_Custom_PropUseProxy); //  <- UnityEngine.Network::get_useProxy
    SET_METRO_BINDING(Network_Set_Custom_PropUseProxy); //  <- UnityEngine.Network::set_useProxy
    SET_METRO_BINDING(Network_Get_Custom_PropProxyPassword); //  <- UnityEngine.Network::get_proxyPassword
    SET_METRO_BINDING(Network_Set_Custom_PropProxyPassword); //  <- UnityEngine.Network::set_proxyPassword
    SET_METRO_BINDING(BitStream_CUSTOM_Serializeb); //  <- UnityEngine.BitStream::Serializeb
    SET_METRO_BINDING(BitStream_CUSTOM_Serializec); //  <- UnityEngine.BitStream::Serializec
    SET_METRO_BINDING(BitStream_CUSTOM_Serializes); //  <- UnityEngine.BitStream::Serializes
    SET_METRO_BINDING(BitStream_CUSTOM_Serializei); //  <- UnityEngine.BitStream::Serializei
    SET_METRO_BINDING(BitStream_CUSTOM_Serializef); //  <- UnityEngine.BitStream::Serializef
    SET_METRO_BINDING(BitStream_CUSTOM_INTERNAL_CALL_Serializeq); //  <- UnityEngine.BitStream::INTERNAL_CALL_Serializeq
    SET_METRO_BINDING(BitStream_CUSTOM_INTERNAL_CALL_Serializev); //  <- UnityEngine.BitStream::INTERNAL_CALL_Serializev
    SET_METRO_BINDING(BitStream_CUSTOM_INTERNAL_CALL_Serializen); //  <- UnityEngine.BitStream::INTERNAL_CALL_Serializen
    SET_METRO_BINDING(BitStream_Get_Custom_PropIsReading); //  <- UnityEngine.BitStream::get_isReading
    SET_METRO_BINDING(BitStream_Get_Custom_PropIsWriting); //  <- UnityEngine.BitStream::get_isWriting
    SET_METRO_BINDING(BitStream_CUSTOM_Serialize); //  <- UnityEngine.BitStream::Serialize
    SET_METRO_BINDING(MasterServer_Get_Custom_PropIpAddress); //  <- UnityEngine.MasterServer::get_ipAddress
    SET_METRO_BINDING(MasterServer_Set_Custom_PropIpAddress); //  <- UnityEngine.MasterServer::set_ipAddress
    SET_METRO_BINDING(MasterServer_Get_Custom_PropPort); //  <- UnityEngine.MasterServer::get_port
    SET_METRO_BINDING(MasterServer_Set_Custom_PropPort); //  <- UnityEngine.MasterServer::set_port
    SET_METRO_BINDING(MasterServer_CUSTOM_RequestHostList); //  <- UnityEngine.MasterServer::RequestHostList
    SET_METRO_BINDING(MasterServer_CUSTOM_PollHostList); //  <- UnityEngine.MasterServer::PollHostList
    SET_METRO_BINDING(MasterServer_CUSTOM_RegisterHost); //  <- UnityEngine.MasterServer::RegisterHost
    SET_METRO_BINDING(MasterServer_CUSTOM_UnregisterHost); //  <- UnityEngine.MasterServer::UnregisterHost
    SET_METRO_BINDING(MasterServer_CUSTOM_ClearHostList); //  <- UnityEngine.MasterServer::ClearHostList
    SET_METRO_BINDING(MasterServer_Get_Custom_PropUpdateRate); //  <- UnityEngine.MasterServer::get_updateRate
    SET_METRO_BINDING(MasterServer_Set_Custom_PropUpdateRate); //  <- UnityEngine.MasterServer::set_updateRate
    SET_METRO_BINDING(MasterServer_Get_Custom_PropDedicatedServer); //  <- UnityEngine.MasterServer::get_dedicatedServer
    SET_METRO_BINDING(MasterServer_Set_Custom_PropDedicatedServer); //  <- UnityEngine.MasterServer::set_dedicatedServer
    SET_METRO_BINDING(NetworkMessageInfo_CUSTOM_NullNetworkView); //  <- UnityEngine.NetworkMessageInfo::NullNetworkView
#endif
}

#endif
