using System;
using UnityEngine;

[Serializable]
public abstract class Base
{
	public int x;
}

[Serializable]
public class Derived : Base
{
	public int y;
}

public class ArraysToObjectsSerializedProperly : MonoBehaviour
{
	public Base[] bases;
	public Derived[] derivees;
}
