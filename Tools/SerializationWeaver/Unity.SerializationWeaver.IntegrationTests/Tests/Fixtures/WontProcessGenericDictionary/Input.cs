using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;

public class WontProcessGenericDictionaryScript : MonoBehaviour
{
	public class SubDict : Dictionary<int, int> { }
	
	public SubDict subDict;
	public Dictionary<int, int> dict;
	
	[SerializeField] SubDict subDict2;
	[SerializeField] Dictionary<int, int> dict2;

	public void Start()
	{
	}
}
