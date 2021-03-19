using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;

public class SerializingEnumListWorksScript : MonoBehaviour
{
	public enum testEnum
	{
		NONE,
		Something
	}

	// List should be allocated by serialization system on the fly
	public List<testEnum> enumList;

	public void Start()
	{
	}
}
