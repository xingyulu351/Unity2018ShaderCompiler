using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using UnityEngine.Serialization;

public class ExplicitSerializationCallbackInBaseClassBase : MonoBehaviour, ISerializationCallbackReceiver
{
	public string mystring;
	public Component c;

	void ISerializationCallbackReceiver.OnBeforeSerialize()
	{
		mystring = "Two";
	}

	void ISerializationCallbackReceiver.OnAfterDeserialize()
	{

	}
}

public class ExplictiSerializationCallbackInBaseClassScript : ExplicitSerializationCallbackInBaseClassBase
{
	public bool firstPass = true;

	void Awake()
	{
	}

	void Test()
	{
		if (UnityEngine.Serialization.PPtrRemapper.Instance.CanExecuteSerializationCallbacks())
			((ISerializationCallbackReceiver)this).OnBeforeSerialize();
	}
}
