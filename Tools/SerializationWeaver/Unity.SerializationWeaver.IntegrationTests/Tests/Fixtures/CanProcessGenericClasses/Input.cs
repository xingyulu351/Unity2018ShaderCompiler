using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;


public class CanSerializeGenericInstanceScript : UnityEngine.MonoBehaviour
{
	[System.Serializable]
	public class GenericBaseClass<T>
	{
		public T t;
	}

	[System.Serializable]
	public class GenericClassOfString : GenericBaseClass<string>
	{
	}

	public GenericClassOfString _genericOfString;
	public GenericBaseClass<int> _genericOfInt;

	public void Start()
	{
	}
}
