#include "UnityPrefix.h"
#include "Runtime/Scripting/ScriptingExportUtility.h"


#include "UnityPrefix.h"
#include "Runtime/Math/Quaternion.h"
#include "Runtime/Utilities/Utility.h"
#include "Runtime/Geometry/AABB.h"
#include "Runtime/Geometry/Ray.h"
#include "Runtime/Geometry/Intersection.h"
#include <vector>
#include "Runtime/Utilities/BitUtility.h"
#include "Runtime/Terrain/PerlinNoise.h"
#include "Runtime/Camera/CameraUtil.h"
#include "Runtime/Math/Color.h"
#include "Runtime/Math/ColorSpaceConversion.h"
SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Vector3_CUSTOM_INTERNAL_CALL_Slerp(const Vector3f& from, const Vector3f& to, float t, Vector3f& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Vector3_CUSTOM_INTERNAL_CALL_Slerp)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_Slerp)
     { returnValue =(Slerp (from, to, clamp01 (t))); return; } 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Vector3_CUSTOM_INTERNAL_CALL_Internal_OrthoNormalize2(Vector3f& a, Vector3f& b)
{
    SCRIPTINGAPI_ETW_ENTRY(Vector3_CUSTOM_INTERNAL_CALL_Internal_OrthoNormalize2)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_Internal_OrthoNormalize2)
     OrthoNormalize (&a, &b); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Vector3_CUSTOM_INTERNAL_CALL_Internal_OrthoNormalize3(Vector3f& a, Vector3f& b, Vector3f& c)
{
    SCRIPTINGAPI_ETW_ENTRY(Vector3_CUSTOM_INTERNAL_CALL_Internal_OrthoNormalize3)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_Internal_OrthoNormalize3)
     OrthoNormalize (&a, &b, &c); 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Vector3_CUSTOM_INTERNAL_CALL_RotateTowards(const Vector3f& current, const Vector3f& target, float maxRadiansDelta, float maxMagnitudeDelta, Vector3f& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Vector3_CUSTOM_INTERNAL_CALL_RotateTowards)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_RotateTowards)
     { returnValue =(RotateTowards (current, target, maxRadiansDelta, maxMagnitudeDelta)); return; } 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Quaternion_CUSTOM_INTERNAL_CALL_AngleAxis(float angle, const Vector3f& axis, Quaternionf& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Quaternion_CUSTOM_INTERNAL_CALL_AngleAxis)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_AngleAxis)
     { returnValue =(AxisAngleToQuaternionSafe (axis, Deg2Rad(angle))); return; } 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Quaternion_CUSTOM_INTERNAL_CALL_FromToRotation(const Vector3f& fromDirection, const Vector3f& toDirection, Quaternionf& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Quaternion_CUSTOM_INTERNAL_CALL_FromToRotation)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_FromToRotation)
     { returnValue =(FromToQuaternionSafe (fromDirection, toDirection)); return; } 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Quaternion_CUSTOM_INTERNAL_CALL_LookRotation(const Vector3f& forward, const Vector3f& upwards, Quaternionf& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Quaternion_CUSTOM_INTERNAL_CALL_LookRotation)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_LookRotation)
    
    		Quaternionf q = Quaternionf::identity ();
    		if (!LookRotationToQuaternion (forward, upwards, &q))
    		{
    			float mag = Magnitude (forward);
    			if (mag > Vector3f::epsilon)
    			{
    				Matrix3x3f m;
    				m.SetFromToRotation (Vector3f::zAxis, forward / mag);
    				MatrixToQuaternion (m, q);
    			}
    			else
    			{
    				LogString ("Look rotation viewing vector is zero");
    			}
    		}
    		{ returnValue =(q); return; }
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Quaternion_CUSTOM_INTERNAL_CALL_Slerp(const Quaternionf& from, const Quaternionf& to, float t, Quaternionf& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Quaternion_CUSTOM_INTERNAL_CALL_Slerp)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_Slerp)
     { returnValue =(Slerp (from, to, clamp01 (t))); return; } 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Quaternion_CUSTOM_INTERNAL_CALL_Lerp(const Quaternionf& from, const Quaternionf& to, float t, Quaternionf& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Quaternion_CUSTOM_INTERNAL_CALL_Lerp)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_Lerp)
     { returnValue =(Lerp (from, to, clamp01 (t))); return; } 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Quaternion_CUSTOM_INTERNAL_CALL_UnclampedSlerp(const Quaternionf& from, const Quaternionf& to, float t, Quaternionf& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Quaternion_CUSTOM_INTERNAL_CALL_UnclampedSlerp)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_UnclampedSlerp)
     { returnValue =(Slerp (from, to, t)); return; } 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Quaternion_CUSTOM_INTERNAL_CALL_Inverse(const Quaternionf& rotation, Quaternionf& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Quaternion_CUSTOM_INTERNAL_CALL_Inverse)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_Inverse)
     { returnValue =(Inverse (rotation)); return; } 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Quaternion_CUSTOM_INTERNAL_CALL_Internal_ToEulerRad(const Quaternionf& rotation, Vector3f& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Quaternion_CUSTOM_INTERNAL_CALL_Internal_ToEulerRad)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_Internal_ToEulerRad)
    
    		Quaternionf outRotation = NormalizeSafe (rotation);
    		{ returnValue =(QuaternionToEuler (outRotation)); return; }
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Quaternion_CUSTOM_INTERNAL_CALL_Internal_FromEulerRad(const Vector3f& euler, Quaternionf& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Quaternion_CUSTOM_INTERNAL_CALL_Internal_FromEulerRad)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_Internal_FromEulerRad)
    
    		{ returnValue =(EulerToQuaternion (euler)); return; } 
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Quaternion_CUSTOM_INTERNAL_CALL_Internal_ToAxisAngleRad(const Quaternionf& q, Vector3f* axis, float* angle)
{
    SCRIPTINGAPI_ETW_ENTRY(Quaternion_CUSTOM_INTERNAL_CALL_Internal_ToAxisAngleRad)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_Internal_ToAxisAngleRad)
    
    		QuaternionToAxisAngle (NormalizeSafe (q), axis, angle); 
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Quaternion_CUSTOM_INTERNAL_CALL_AxisAngle(const Vector3f& axis, float angle, Quaternionf& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Quaternion_CUSTOM_INTERNAL_CALL_AxisAngle)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_AxisAngle)
     { returnValue =(AxisAngleToQuaternionSafe (axis, angle)); return; } 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Matrix4x4_CUSTOM_INTERNAL_CALL_Inverse(const Matrix4x4f& m, Matrix4x4f& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Matrix4x4_CUSTOM_INTERNAL_CALL_Inverse)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_Inverse)
     Matrix4x4f output (m); output.Invert_Full(); { returnValue =(output); return; } 
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Matrix4x4_CUSTOM_INTERNAL_CALL_Transpose(const Matrix4x4f& m, Matrix4x4f& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Matrix4x4_CUSTOM_INTERNAL_CALL_Transpose)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_Transpose)
     Matrix4x4f output (m); output.Transpose(); { returnValue =(output); return; } 
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Matrix4x4_CUSTOM_INTERNAL_CALL_Invert(const Matrix4x4f& inMatrix, Matrix4x4f* dest)
{
    SCRIPTINGAPI_ETW_ENTRY(Matrix4x4_CUSTOM_INTERNAL_CALL_Invert)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_Invert)
     return Matrix4x4f::Invert_Full(inMatrix, *dest); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Matrix4x4_Get_Custom_PropIsIdentity(const Matrix4x4f& self)
{
    SCRIPTINGAPI_ETW_ENTRY(Matrix4x4_Get_Custom_PropIsIdentity)
    SCRIPTINGAPI_STACK_CHECK(get_isIdentity)
    return self.IsIdentity();
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Matrix4x4_CUSTOM_INTERNAL_CALL_TRS(const Vector3f& pos, const Quaternionf& q, const Vector3f& s, Matrix4x4f& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Matrix4x4_CUSTOM_INTERNAL_CALL_TRS)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_TRS)
    
    		Matrix4x4f temp;
    		temp.SetTRS(pos,q,s);
    		{ returnValue =(temp); return; }
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Matrix4x4_CUSTOM_INTERNAL_CALL_Ortho(float left, float right, float bottom, float top, float zNear, float zFar, Matrix4x4f& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Matrix4x4_CUSTOM_INTERNAL_CALL_Ortho)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_Ortho)
    
    		Matrix4x4f m;
    		m.SetOrtho(left, right, bottom, top, zNear, zFar);
    		{ returnValue =(m); return; }
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
void SCRIPT_CALL_CONVENTION Matrix4x4_CUSTOM_INTERNAL_CALL_Perspective(float fov, float aspect, float zNear, float zFar, Matrix4x4f& returnValue)
{
    SCRIPTINGAPI_ETW_ENTRY(Matrix4x4_CUSTOM_INTERNAL_CALL_Perspective)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_Perspective)
    
    		Matrix4x4f m;
    		m.SetPerspective( fov, aspect, zNear, zFar );
    		{ returnValue =(m); return; }
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Bounds_CUSTOM_INTERNAL_CALL_Internal_Contains(const AABB& m, const Vector3f& point)
{
    SCRIPTINGAPI_ETW_ENTRY(Bounds_CUSTOM_INTERNAL_CALL_Internal_Contains)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_Internal_Contains)
     return m.IsInside (point); 
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Bounds_CUSTOM_INTERNAL_CALL_Internal_SqrDistance(const AABB& m, const Vector3f& point)
{
    SCRIPTINGAPI_ETW_ENTRY(Bounds_CUSTOM_INTERNAL_CALL_Internal_SqrDistance)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_Internal_SqrDistance)
     return CalculateSqrDistance(point, m); 
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Bounds_CUSTOM_INTERNAL_CALL_Internal_IntersectRay(Ray& ray, AABB& bounds, float* distance)
{
    SCRIPTINGAPI_ETW_ENTRY(Bounds_CUSTOM_INTERNAL_CALL_Internal_IntersectRay)
    SCRIPTINGAPI_STACK_CHECK(INTERNAL_CALL_Internal_IntersectRay)
     return IntersectRayAABB(ray, bounds, distance); 
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Mathf_CUSTOM_ClosestPowerOfTwo(int value)
{
    SCRIPTINGAPI_ETW_ENTRY(Mathf_CUSTOM_ClosestPowerOfTwo)
    SCRIPTINGAPI_STACK_CHECK(ClosestPowerOfTwo)
    
    		return ClosestPowerOfTwo(value);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Mathf_CUSTOM_GammaToLinearSpace(float value)
{
    SCRIPTINGAPI_ETW_ENTRY(Mathf_CUSTOM_GammaToLinearSpace)
    SCRIPTINGAPI_STACK_CHECK(GammaToLinearSpace)
    
    		return GammaToLinearSpace (value);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Mathf_CUSTOM_LinearToGammaSpace(float value)
{
    SCRIPTINGAPI_ETW_ENTRY(Mathf_CUSTOM_LinearToGammaSpace)
    SCRIPTINGAPI_STACK_CHECK(LinearToGammaSpace)
    
    		return LinearToGammaSpace (value);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
ScriptingBool SCRIPT_CALL_CONVENTION Mathf_CUSTOM_IsPowerOfTwo(int value)
{
    SCRIPTINGAPI_ETW_ENTRY(Mathf_CUSTOM_IsPowerOfTwo)
    SCRIPTINGAPI_STACK_CHECK(IsPowerOfTwo)
    
    		return IsPowerOfTwo(value);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
int SCRIPT_CALL_CONVENTION Mathf_CUSTOM_NextPowerOfTwo(int value)
{
    SCRIPTINGAPI_ETW_ENTRY(Mathf_CUSTOM_NextPowerOfTwo)
    SCRIPTINGAPI_STACK_CHECK(NextPowerOfTwo)
    
    		return NextPowerOfTwo(value);
    	
}

SCRIPT_BINDINGS_EXPORT_DECL
float SCRIPT_CALL_CONVENTION Mathf_CUSTOM_PerlinNoise(float x, float y)
{
    SCRIPTINGAPI_ETW_ENTRY(Mathf_CUSTOM_PerlinNoise)
    SCRIPTINGAPI_STACK_CHECK(PerlinNoise)
    
    		return PerlinNoise::NoiseNormalized (x,y);
    	
}

#if !defined(INTERNAL_CALL_STRIPPING)
#   error must include unityconfigure.h
#endif
#if INTERNAL_CALL_STRIPPING
void Register_UnityEngine_Vector3_INTERNAL_CALL_Slerp()
{
    scripting_add_internal_call( "UnityEngine.Vector3::INTERNAL_CALL_Slerp" , (gpointer)& Vector3_CUSTOM_INTERNAL_CALL_Slerp );
}

void Register_UnityEngine_Vector3_INTERNAL_CALL_Internal_OrthoNormalize2()
{
    scripting_add_internal_call( "UnityEngine.Vector3::INTERNAL_CALL_Internal_OrthoNormalize2" , (gpointer)& Vector3_CUSTOM_INTERNAL_CALL_Internal_OrthoNormalize2 );
}

void Register_UnityEngine_Vector3_INTERNAL_CALL_Internal_OrthoNormalize3()
{
    scripting_add_internal_call( "UnityEngine.Vector3::INTERNAL_CALL_Internal_OrthoNormalize3" , (gpointer)& Vector3_CUSTOM_INTERNAL_CALL_Internal_OrthoNormalize3 );
}

void Register_UnityEngine_Vector3_INTERNAL_CALL_RotateTowards()
{
    scripting_add_internal_call( "UnityEngine.Vector3::INTERNAL_CALL_RotateTowards" , (gpointer)& Vector3_CUSTOM_INTERNAL_CALL_RotateTowards );
}

void Register_UnityEngine_Quaternion_INTERNAL_CALL_AngleAxis()
{
    scripting_add_internal_call( "UnityEngine.Quaternion::INTERNAL_CALL_AngleAxis" , (gpointer)& Quaternion_CUSTOM_INTERNAL_CALL_AngleAxis );
}

void Register_UnityEngine_Quaternion_INTERNAL_CALL_FromToRotation()
{
    scripting_add_internal_call( "UnityEngine.Quaternion::INTERNAL_CALL_FromToRotation" , (gpointer)& Quaternion_CUSTOM_INTERNAL_CALL_FromToRotation );
}

void Register_UnityEngine_Quaternion_INTERNAL_CALL_LookRotation()
{
    scripting_add_internal_call( "UnityEngine.Quaternion::INTERNAL_CALL_LookRotation" , (gpointer)& Quaternion_CUSTOM_INTERNAL_CALL_LookRotation );
}

void Register_UnityEngine_Quaternion_INTERNAL_CALL_Slerp()
{
    scripting_add_internal_call( "UnityEngine.Quaternion::INTERNAL_CALL_Slerp" , (gpointer)& Quaternion_CUSTOM_INTERNAL_CALL_Slerp );
}

void Register_UnityEngine_Quaternion_INTERNAL_CALL_Lerp()
{
    scripting_add_internal_call( "UnityEngine.Quaternion::INTERNAL_CALL_Lerp" , (gpointer)& Quaternion_CUSTOM_INTERNAL_CALL_Lerp );
}

void Register_UnityEngine_Quaternion_INTERNAL_CALL_UnclampedSlerp()
{
    scripting_add_internal_call( "UnityEngine.Quaternion::INTERNAL_CALL_UnclampedSlerp" , (gpointer)& Quaternion_CUSTOM_INTERNAL_CALL_UnclampedSlerp );
}

void Register_UnityEngine_Quaternion_INTERNAL_CALL_Inverse()
{
    scripting_add_internal_call( "UnityEngine.Quaternion::INTERNAL_CALL_Inverse" , (gpointer)& Quaternion_CUSTOM_INTERNAL_CALL_Inverse );
}

void Register_UnityEngine_Quaternion_INTERNAL_CALL_Internal_ToEulerRad()
{
    scripting_add_internal_call( "UnityEngine.Quaternion::INTERNAL_CALL_Internal_ToEulerRad" , (gpointer)& Quaternion_CUSTOM_INTERNAL_CALL_Internal_ToEulerRad );
}

void Register_UnityEngine_Quaternion_INTERNAL_CALL_Internal_FromEulerRad()
{
    scripting_add_internal_call( "UnityEngine.Quaternion::INTERNAL_CALL_Internal_FromEulerRad" , (gpointer)& Quaternion_CUSTOM_INTERNAL_CALL_Internal_FromEulerRad );
}

void Register_UnityEngine_Quaternion_INTERNAL_CALL_Internal_ToAxisAngleRad()
{
    scripting_add_internal_call( "UnityEngine.Quaternion::INTERNAL_CALL_Internal_ToAxisAngleRad" , (gpointer)& Quaternion_CUSTOM_INTERNAL_CALL_Internal_ToAxisAngleRad );
}

void Register_UnityEngine_Quaternion_INTERNAL_CALL_AxisAngle()
{
    scripting_add_internal_call( "UnityEngine.Quaternion::INTERNAL_CALL_AxisAngle" , (gpointer)& Quaternion_CUSTOM_INTERNAL_CALL_AxisAngle );
}

void Register_UnityEngine_Matrix4x4_INTERNAL_CALL_Inverse()
{
    scripting_add_internal_call( "UnityEngine.Matrix4x4::INTERNAL_CALL_Inverse" , (gpointer)& Matrix4x4_CUSTOM_INTERNAL_CALL_Inverse );
}

void Register_UnityEngine_Matrix4x4_INTERNAL_CALL_Transpose()
{
    scripting_add_internal_call( "UnityEngine.Matrix4x4::INTERNAL_CALL_Transpose" , (gpointer)& Matrix4x4_CUSTOM_INTERNAL_CALL_Transpose );
}

void Register_UnityEngine_Matrix4x4_INTERNAL_CALL_Invert()
{
    scripting_add_internal_call( "UnityEngine.Matrix4x4::INTERNAL_CALL_Invert" , (gpointer)& Matrix4x4_CUSTOM_INTERNAL_CALL_Invert );
}

void Register_UnityEngine_Matrix4x4_get_isIdentity()
{
    scripting_add_internal_call( "UnityEngine.Matrix4x4::get_isIdentity" , (gpointer)& Matrix4x4_Get_Custom_PropIsIdentity );
}

void Register_UnityEngine_Matrix4x4_INTERNAL_CALL_TRS()
{
    scripting_add_internal_call( "UnityEngine.Matrix4x4::INTERNAL_CALL_TRS" , (gpointer)& Matrix4x4_CUSTOM_INTERNAL_CALL_TRS );
}

void Register_UnityEngine_Matrix4x4_INTERNAL_CALL_Ortho()
{
    scripting_add_internal_call( "UnityEngine.Matrix4x4::INTERNAL_CALL_Ortho" , (gpointer)& Matrix4x4_CUSTOM_INTERNAL_CALL_Ortho );
}

void Register_UnityEngine_Matrix4x4_INTERNAL_CALL_Perspective()
{
    scripting_add_internal_call( "UnityEngine.Matrix4x4::INTERNAL_CALL_Perspective" , (gpointer)& Matrix4x4_CUSTOM_INTERNAL_CALL_Perspective );
}

void Register_UnityEngine_Bounds_INTERNAL_CALL_Internal_Contains()
{
    scripting_add_internal_call( "UnityEngine.Bounds::INTERNAL_CALL_Internal_Contains" , (gpointer)& Bounds_CUSTOM_INTERNAL_CALL_Internal_Contains );
}

void Register_UnityEngine_Bounds_INTERNAL_CALL_Internal_SqrDistance()
{
    scripting_add_internal_call( "UnityEngine.Bounds::INTERNAL_CALL_Internal_SqrDistance" , (gpointer)& Bounds_CUSTOM_INTERNAL_CALL_Internal_SqrDistance );
}

void Register_UnityEngine_Bounds_INTERNAL_CALL_Internal_IntersectRay()
{
    scripting_add_internal_call( "UnityEngine.Bounds::INTERNAL_CALL_Internal_IntersectRay" , (gpointer)& Bounds_CUSTOM_INTERNAL_CALL_Internal_IntersectRay );
}

void Register_UnityEngine_Mathf_ClosestPowerOfTwo()
{
    scripting_add_internal_call( "UnityEngine.Mathf::ClosestPowerOfTwo" , (gpointer)& Mathf_CUSTOM_ClosestPowerOfTwo );
}

void Register_UnityEngine_Mathf_GammaToLinearSpace()
{
    scripting_add_internal_call( "UnityEngine.Mathf::GammaToLinearSpace" , (gpointer)& Mathf_CUSTOM_GammaToLinearSpace );
}

void Register_UnityEngine_Mathf_LinearToGammaSpace()
{
    scripting_add_internal_call( "UnityEngine.Mathf::LinearToGammaSpace" , (gpointer)& Mathf_CUSTOM_LinearToGammaSpace );
}

void Register_UnityEngine_Mathf_IsPowerOfTwo()
{
    scripting_add_internal_call( "UnityEngine.Mathf::IsPowerOfTwo" , (gpointer)& Mathf_CUSTOM_IsPowerOfTwo );
}

void Register_UnityEngine_Mathf_NextPowerOfTwo()
{
    scripting_add_internal_call( "UnityEngine.Mathf::NextPowerOfTwo" , (gpointer)& Mathf_CUSTOM_NextPowerOfTwo );
}

void Register_UnityEngine_Mathf_PerlinNoise()
{
    scripting_add_internal_call( "UnityEngine.Mathf::PerlinNoise" , (gpointer)& Mathf_CUSTOM_PerlinNoise );
}

#elif ENABLE_MONO || ENABLE_IL2CPP
static const char* s_Math_IcallNames [] =
{
    "UnityEngine.Vector3::INTERNAL_CALL_Slerp",    // -> Vector3_CUSTOM_INTERNAL_CALL_Slerp
    "UnityEngine.Vector3::INTERNAL_CALL_Internal_OrthoNormalize2",    // -> Vector3_CUSTOM_INTERNAL_CALL_Internal_OrthoNormalize2
    "UnityEngine.Vector3::INTERNAL_CALL_Internal_OrthoNormalize3",    // -> Vector3_CUSTOM_INTERNAL_CALL_Internal_OrthoNormalize3
    "UnityEngine.Vector3::INTERNAL_CALL_RotateTowards",    // -> Vector3_CUSTOM_INTERNAL_CALL_RotateTowards
    "UnityEngine.Quaternion::INTERNAL_CALL_AngleAxis",    // -> Quaternion_CUSTOM_INTERNAL_CALL_AngleAxis
    "UnityEngine.Quaternion::INTERNAL_CALL_FromToRotation",    // -> Quaternion_CUSTOM_INTERNAL_CALL_FromToRotation
    "UnityEngine.Quaternion::INTERNAL_CALL_LookRotation",    // -> Quaternion_CUSTOM_INTERNAL_CALL_LookRotation
    "UnityEngine.Quaternion::INTERNAL_CALL_Slerp",    // -> Quaternion_CUSTOM_INTERNAL_CALL_Slerp
    "UnityEngine.Quaternion::INTERNAL_CALL_Lerp",    // -> Quaternion_CUSTOM_INTERNAL_CALL_Lerp
    "UnityEngine.Quaternion::INTERNAL_CALL_UnclampedSlerp",    // -> Quaternion_CUSTOM_INTERNAL_CALL_UnclampedSlerp
    "UnityEngine.Quaternion::INTERNAL_CALL_Inverse",    // -> Quaternion_CUSTOM_INTERNAL_CALL_Inverse
    "UnityEngine.Quaternion::INTERNAL_CALL_Internal_ToEulerRad",    // -> Quaternion_CUSTOM_INTERNAL_CALL_Internal_ToEulerRad
    "UnityEngine.Quaternion::INTERNAL_CALL_Internal_FromEulerRad",    // -> Quaternion_CUSTOM_INTERNAL_CALL_Internal_FromEulerRad
    "UnityEngine.Quaternion::INTERNAL_CALL_Internal_ToAxisAngleRad",    // -> Quaternion_CUSTOM_INTERNAL_CALL_Internal_ToAxisAngleRad
    "UnityEngine.Quaternion::INTERNAL_CALL_AxisAngle",    // -> Quaternion_CUSTOM_INTERNAL_CALL_AxisAngle
    "UnityEngine.Matrix4x4::INTERNAL_CALL_Inverse",    // -> Matrix4x4_CUSTOM_INTERNAL_CALL_Inverse
    "UnityEngine.Matrix4x4::INTERNAL_CALL_Transpose",    // -> Matrix4x4_CUSTOM_INTERNAL_CALL_Transpose
    "UnityEngine.Matrix4x4::INTERNAL_CALL_Invert",    // -> Matrix4x4_CUSTOM_INTERNAL_CALL_Invert
    "UnityEngine.Matrix4x4::get_isIdentity" ,    // -> Matrix4x4_Get_Custom_PropIsIdentity
    "UnityEngine.Matrix4x4::INTERNAL_CALL_TRS",    // -> Matrix4x4_CUSTOM_INTERNAL_CALL_TRS
    "UnityEngine.Matrix4x4::INTERNAL_CALL_Ortho",    // -> Matrix4x4_CUSTOM_INTERNAL_CALL_Ortho
    "UnityEngine.Matrix4x4::INTERNAL_CALL_Perspective",    // -> Matrix4x4_CUSTOM_INTERNAL_CALL_Perspective
    "UnityEngine.Bounds::INTERNAL_CALL_Internal_Contains",    // -> Bounds_CUSTOM_INTERNAL_CALL_Internal_Contains
    "UnityEngine.Bounds::INTERNAL_CALL_Internal_SqrDistance",    // -> Bounds_CUSTOM_INTERNAL_CALL_Internal_SqrDistance
    "UnityEngine.Bounds::INTERNAL_CALL_Internal_IntersectRay",    // -> Bounds_CUSTOM_INTERNAL_CALL_Internal_IntersectRay
    "UnityEngine.Mathf::ClosestPowerOfTwo"  ,    // -> Mathf_CUSTOM_ClosestPowerOfTwo
    "UnityEngine.Mathf::GammaToLinearSpace" ,    // -> Mathf_CUSTOM_GammaToLinearSpace
    "UnityEngine.Mathf::LinearToGammaSpace" ,    // -> Mathf_CUSTOM_LinearToGammaSpace
    "UnityEngine.Mathf::IsPowerOfTwo"       ,    // -> Mathf_CUSTOM_IsPowerOfTwo
    "UnityEngine.Mathf::NextPowerOfTwo"     ,    // -> Mathf_CUSTOM_NextPowerOfTwo
    "UnityEngine.Mathf::PerlinNoise"        ,    // -> Mathf_CUSTOM_PerlinNoise
    NULL
};

static const void* s_Math_IcallFuncs [] =
{
    (const void*)&Vector3_CUSTOM_INTERNAL_CALL_Slerp      ,  //  <- UnityEngine.Vector3::INTERNAL_CALL_Slerp
    (const void*)&Vector3_CUSTOM_INTERNAL_CALL_Internal_OrthoNormalize2,  //  <- UnityEngine.Vector3::INTERNAL_CALL_Internal_OrthoNormalize2
    (const void*)&Vector3_CUSTOM_INTERNAL_CALL_Internal_OrthoNormalize3,  //  <- UnityEngine.Vector3::INTERNAL_CALL_Internal_OrthoNormalize3
    (const void*)&Vector3_CUSTOM_INTERNAL_CALL_RotateTowards,  //  <- UnityEngine.Vector3::INTERNAL_CALL_RotateTowards
    (const void*)&Quaternion_CUSTOM_INTERNAL_CALL_AngleAxis,  //  <- UnityEngine.Quaternion::INTERNAL_CALL_AngleAxis
    (const void*)&Quaternion_CUSTOM_INTERNAL_CALL_FromToRotation,  //  <- UnityEngine.Quaternion::INTERNAL_CALL_FromToRotation
    (const void*)&Quaternion_CUSTOM_INTERNAL_CALL_LookRotation,  //  <- UnityEngine.Quaternion::INTERNAL_CALL_LookRotation
    (const void*)&Quaternion_CUSTOM_INTERNAL_CALL_Slerp   ,  //  <- UnityEngine.Quaternion::INTERNAL_CALL_Slerp
    (const void*)&Quaternion_CUSTOM_INTERNAL_CALL_Lerp    ,  //  <- UnityEngine.Quaternion::INTERNAL_CALL_Lerp
    (const void*)&Quaternion_CUSTOM_INTERNAL_CALL_UnclampedSlerp,  //  <- UnityEngine.Quaternion::INTERNAL_CALL_UnclampedSlerp
    (const void*)&Quaternion_CUSTOM_INTERNAL_CALL_Inverse ,  //  <- UnityEngine.Quaternion::INTERNAL_CALL_Inverse
    (const void*)&Quaternion_CUSTOM_INTERNAL_CALL_Internal_ToEulerRad,  //  <- UnityEngine.Quaternion::INTERNAL_CALL_Internal_ToEulerRad
    (const void*)&Quaternion_CUSTOM_INTERNAL_CALL_Internal_FromEulerRad,  //  <- UnityEngine.Quaternion::INTERNAL_CALL_Internal_FromEulerRad
    (const void*)&Quaternion_CUSTOM_INTERNAL_CALL_Internal_ToAxisAngleRad,  //  <- UnityEngine.Quaternion::INTERNAL_CALL_Internal_ToAxisAngleRad
    (const void*)&Quaternion_CUSTOM_INTERNAL_CALL_AxisAngle,  //  <- UnityEngine.Quaternion::INTERNAL_CALL_AxisAngle
    (const void*)&Matrix4x4_CUSTOM_INTERNAL_CALL_Inverse  ,  //  <- UnityEngine.Matrix4x4::INTERNAL_CALL_Inverse
    (const void*)&Matrix4x4_CUSTOM_INTERNAL_CALL_Transpose,  //  <- UnityEngine.Matrix4x4::INTERNAL_CALL_Transpose
    (const void*)&Matrix4x4_CUSTOM_INTERNAL_CALL_Invert   ,  //  <- UnityEngine.Matrix4x4::INTERNAL_CALL_Invert
    (const void*)&Matrix4x4_Get_Custom_PropIsIdentity     ,  //  <- UnityEngine.Matrix4x4::get_isIdentity
    (const void*)&Matrix4x4_CUSTOM_INTERNAL_CALL_TRS      ,  //  <- UnityEngine.Matrix4x4::INTERNAL_CALL_TRS
    (const void*)&Matrix4x4_CUSTOM_INTERNAL_CALL_Ortho    ,  //  <- UnityEngine.Matrix4x4::INTERNAL_CALL_Ortho
    (const void*)&Matrix4x4_CUSTOM_INTERNAL_CALL_Perspective,  //  <- UnityEngine.Matrix4x4::INTERNAL_CALL_Perspective
    (const void*)&Bounds_CUSTOM_INTERNAL_CALL_Internal_Contains,  //  <- UnityEngine.Bounds::INTERNAL_CALL_Internal_Contains
    (const void*)&Bounds_CUSTOM_INTERNAL_CALL_Internal_SqrDistance,  //  <- UnityEngine.Bounds::INTERNAL_CALL_Internal_SqrDistance
    (const void*)&Bounds_CUSTOM_INTERNAL_CALL_Internal_IntersectRay,  //  <- UnityEngine.Bounds::INTERNAL_CALL_Internal_IntersectRay
    (const void*)&Mathf_CUSTOM_ClosestPowerOfTwo          ,  //  <- UnityEngine.Mathf::ClosestPowerOfTwo
    (const void*)&Mathf_CUSTOM_GammaToLinearSpace         ,  //  <- UnityEngine.Mathf::GammaToLinearSpace
    (const void*)&Mathf_CUSTOM_LinearToGammaSpace         ,  //  <- UnityEngine.Mathf::LinearToGammaSpace
    (const void*)&Mathf_CUSTOM_IsPowerOfTwo               ,  //  <- UnityEngine.Mathf::IsPowerOfTwo
    (const void*)&Mathf_CUSTOM_NextPowerOfTwo             ,  //  <- UnityEngine.Mathf::NextPowerOfTwo
    (const void*)&Mathf_CUSTOM_PerlinNoise                ,  //  <- UnityEngine.Mathf::PerlinNoise
    NULL
};

void ExportMathBindings();
void ExportMathBindings()
{
    for (int i = 0; s_Math_IcallNames [i] != NULL; ++i )
        scripting_add_internal_call( s_Math_IcallNames [i], s_Math_IcallFuncs [i] );
}

#elif ENABLE_DOTNET
// This comment is here on purpose, so Jam won't pick WinRTHelper.h as dependency for non WinRT targets, thus not doing unneeded recompilation.
//#include "Runtime/Scripting/WinRTHelper.h"
void ExportMathBindings()
{
    #if UNITY_WP8
    extern intptr_t g_WinRTFuncPtrs[];
    #define SET_METRO_BINDING(Name) g_WinRTFuncPtrs[k##Name##FuncDef] = reinterpret_cast<intptr_t>(Name);
    #else
    long long* p = GetWinRTFuncDefsPointers();
    #define SET_METRO_BINDING(Name) p[k##Name##Func] = (long long)Name;
    #endif
    SET_METRO_BINDING(Vector3_CUSTOM_INTERNAL_CALL_Slerp); //  <- UnityEngine.Vector3::INTERNAL_CALL_Slerp
    SET_METRO_BINDING(Vector3_CUSTOM_INTERNAL_CALL_Internal_OrthoNormalize2); //  <- UnityEngine.Vector3::INTERNAL_CALL_Internal_OrthoNormalize2
    SET_METRO_BINDING(Vector3_CUSTOM_INTERNAL_CALL_Internal_OrthoNormalize3); //  <- UnityEngine.Vector3::INTERNAL_CALL_Internal_OrthoNormalize3
    SET_METRO_BINDING(Vector3_CUSTOM_INTERNAL_CALL_RotateTowards); //  <- UnityEngine.Vector3::INTERNAL_CALL_RotateTowards
    SET_METRO_BINDING(Quaternion_CUSTOM_INTERNAL_CALL_AngleAxis); //  <- UnityEngine.Quaternion::INTERNAL_CALL_AngleAxis
    SET_METRO_BINDING(Quaternion_CUSTOM_INTERNAL_CALL_FromToRotation); //  <- UnityEngine.Quaternion::INTERNAL_CALL_FromToRotation
    SET_METRO_BINDING(Quaternion_CUSTOM_INTERNAL_CALL_LookRotation); //  <- UnityEngine.Quaternion::INTERNAL_CALL_LookRotation
    SET_METRO_BINDING(Quaternion_CUSTOM_INTERNAL_CALL_Slerp); //  <- UnityEngine.Quaternion::INTERNAL_CALL_Slerp
    SET_METRO_BINDING(Quaternion_CUSTOM_INTERNAL_CALL_Lerp); //  <- UnityEngine.Quaternion::INTERNAL_CALL_Lerp
    SET_METRO_BINDING(Quaternion_CUSTOM_INTERNAL_CALL_UnclampedSlerp); //  <- UnityEngine.Quaternion::INTERNAL_CALL_UnclampedSlerp
    SET_METRO_BINDING(Quaternion_CUSTOM_INTERNAL_CALL_Inverse); //  <- UnityEngine.Quaternion::INTERNAL_CALL_Inverse
    SET_METRO_BINDING(Quaternion_CUSTOM_INTERNAL_CALL_Internal_ToEulerRad); //  <- UnityEngine.Quaternion::INTERNAL_CALL_Internal_ToEulerRad
    SET_METRO_BINDING(Quaternion_CUSTOM_INTERNAL_CALL_Internal_FromEulerRad); //  <- UnityEngine.Quaternion::INTERNAL_CALL_Internal_FromEulerRad
    SET_METRO_BINDING(Quaternion_CUSTOM_INTERNAL_CALL_Internal_ToAxisAngleRad); //  <- UnityEngine.Quaternion::INTERNAL_CALL_Internal_ToAxisAngleRad
    SET_METRO_BINDING(Quaternion_CUSTOM_INTERNAL_CALL_AxisAngle); //  <- UnityEngine.Quaternion::INTERNAL_CALL_AxisAngle
    SET_METRO_BINDING(Matrix4x4_CUSTOM_INTERNAL_CALL_Inverse); //  <- UnityEngine.Matrix4x4::INTERNAL_CALL_Inverse
    SET_METRO_BINDING(Matrix4x4_CUSTOM_INTERNAL_CALL_Transpose); //  <- UnityEngine.Matrix4x4::INTERNAL_CALL_Transpose
    SET_METRO_BINDING(Matrix4x4_CUSTOM_INTERNAL_CALL_Invert); //  <- UnityEngine.Matrix4x4::INTERNAL_CALL_Invert
    SET_METRO_BINDING(Matrix4x4_Get_Custom_PropIsIdentity); //  <- UnityEngine.Matrix4x4::get_isIdentity
    SET_METRO_BINDING(Matrix4x4_CUSTOM_INTERNAL_CALL_TRS); //  <- UnityEngine.Matrix4x4::INTERNAL_CALL_TRS
    SET_METRO_BINDING(Matrix4x4_CUSTOM_INTERNAL_CALL_Ortho); //  <- UnityEngine.Matrix4x4::INTERNAL_CALL_Ortho
    SET_METRO_BINDING(Matrix4x4_CUSTOM_INTERNAL_CALL_Perspective); //  <- UnityEngine.Matrix4x4::INTERNAL_CALL_Perspective
    SET_METRO_BINDING(Bounds_CUSTOM_INTERNAL_CALL_Internal_Contains); //  <- UnityEngine.Bounds::INTERNAL_CALL_Internal_Contains
    SET_METRO_BINDING(Bounds_CUSTOM_INTERNAL_CALL_Internal_SqrDistance); //  <- UnityEngine.Bounds::INTERNAL_CALL_Internal_SqrDistance
    SET_METRO_BINDING(Bounds_CUSTOM_INTERNAL_CALL_Internal_IntersectRay); //  <- UnityEngine.Bounds::INTERNAL_CALL_Internal_IntersectRay
    SET_METRO_BINDING(Mathf_CUSTOM_ClosestPowerOfTwo); //  <- UnityEngine.Mathf::ClosestPowerOfTwo
    SET_METRO_BINDING(Mathf_CUSTOM_GammaToLinearSpace); //  <- UnityEngine.Mathf::GammaToLinearSpace
    SET_METRO_BINDING(Mathf_CUSTOM_LinearToGammaSpace); //  <- UnityEngine.Mathf::LinearToGammaSpace
    SET_METRO_BINDING(Mathf_CUSTOM_IsPowerOfTwo); //  <- UnityEngine.Mathf::IsPowerOfTwo
    SET_METRO_BINDING(Mathf_CUSTOM_NextPowerOfTwo); //  <- UnityEngine.Mathf::NextPowerOfTwo
    SET_METRO_BINDING(Mathf_CUSTOM_PerlinNoise); //  <- UnityEngine.Mathf::PerlinNoise
}

#endif
