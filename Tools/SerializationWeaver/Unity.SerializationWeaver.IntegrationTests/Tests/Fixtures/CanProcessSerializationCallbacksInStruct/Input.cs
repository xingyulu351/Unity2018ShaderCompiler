using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using UnityEngine.Serialization;

[System.Serializable]
public struct ExplicitSerializationCallbackInStruct: ISerializationCallbackReceiver
{
	public string mystring;

	public void OnBeforeSerialize()
	{
		mystring = "Two";
	}

	public void OnAfterDeserialize()
	{

	}
}
