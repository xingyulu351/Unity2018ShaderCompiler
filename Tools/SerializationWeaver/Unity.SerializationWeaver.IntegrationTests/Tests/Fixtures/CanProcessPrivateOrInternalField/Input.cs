using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;

public class CanProcessPrivateOrInternalField : MonoBehaviour
{
	[SerializeField]
	private int privField;
	[SerializeField]
	internal int internField;

	public CanProcessPrivateOrInternalField(int x, int y)
	{
		privField = x;
		internField = y;
	}
}
