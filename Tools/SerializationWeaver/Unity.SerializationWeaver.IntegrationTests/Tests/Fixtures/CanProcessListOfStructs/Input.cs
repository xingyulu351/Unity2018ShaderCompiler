using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;

public class CanProcessListOfStructs : MonoBehaviour
{
	[Serializable]
	public struct TestStruct
	{
		public Transform t;
	}

	public List<TestStruct> list;
}
