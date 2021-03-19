using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;


public class CanSerializeStruct : UnityEngine.MonoBehaviour
{
	[System.Serializable]
	public struct MyStruct
	{
		public byte myByte;
		public int t;
		public UInt16 myUInt16;
		public char myChar;


		public MyStruct(int i)
		{
			t = 0;
			myChar = 'a';
			myByte = 127;
			myUInt16 = 320;
		}
	}

	public MyStruct myStruct;
}
