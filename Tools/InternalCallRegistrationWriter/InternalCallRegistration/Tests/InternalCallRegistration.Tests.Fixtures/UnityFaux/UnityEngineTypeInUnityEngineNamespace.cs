using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace UnityEngine.Serialization
{
	class UnityEngineClassInNamespace
	{
		[System.Runtime.CompilerServices.MethodImplAttribute(System.Runtime.CompilerServices.MethodImplOptions.InternalCall)]
		extern private void CInternalCall	();
		
		[System.Runtime.CompilerServices.MethodImplAttribute(System.Runtime.CompilerServices.MethodImplOptions.InternalCall)]
		extern public UnityEngineClassInNamespace();
	}
}
