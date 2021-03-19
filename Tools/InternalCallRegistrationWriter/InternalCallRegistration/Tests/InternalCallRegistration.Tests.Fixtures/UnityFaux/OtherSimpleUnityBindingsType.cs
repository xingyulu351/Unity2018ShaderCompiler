namespace UnityEngine
{
	class OtherSimpleUnityBindingsType
	{
		public void OtherInstanceMethodVoidReturn()
		{
			
		}

		public void OtherStaticMethodVoidReturn()
		{
			
		}

		[System.Runtime.CompilerServices.MethodImplAttribute(System.Runtime.CompilerServices.MethodImplOptions.InternalCall)]
		extern private void OtherInternalCallVoidReturnNoArguments();

		[System.Runtime.CompilerServices.MethodImplAttribute(System.Runtime.CompilerServices.MethodImplOptions.InternalCall)]
		extern private static void OtherStaticInternalCallVoidReturnNoArguments();
	}
}
