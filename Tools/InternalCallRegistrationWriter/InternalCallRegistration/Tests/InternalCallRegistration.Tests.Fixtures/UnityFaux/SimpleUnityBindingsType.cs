namespace UnityEngine
{
	class SimpleUnityBindingsType
	{
		public void InstanceMethodVoidReturn()
		{
			
		}

		public void StaticMethodVoidReturn()
		{
			
		}

		[System.Runtime.CompilerServices.MethodImplAttribute(System.Runtime.CompilerServices.MethodImplOptions.InternalCall)]
		extern private void CInternalCall();
	
		[System.Runtime.CompilerServices.MethodImplAttribute(System.Runtime.CompilerServices.MethodImplOptions.InternalCall)]
		extern private void InternalCallVoidReturnNoArguments();

		[System.Runtime.CompilerServices.MethodImplAttribute(System.Runtime.CompilerServices.MethodImplOptions.InternalCall)]
		extern private static void StaticInternalCallVoidReturnNoArguments();

		[System.Runtime.CompilerServices.MethodImplAttribute(System.Runtime.CompilerServices.MethodImplOptions.InternalCall)]
		extern private void AInternalCall();

		[System.Runtime.CompilerServices.MethodImplAttribute(System.Runtime.CompilerServices.MethodImplOptions.InternalCall)]
		extern private void BInternalCall();

	}
}
