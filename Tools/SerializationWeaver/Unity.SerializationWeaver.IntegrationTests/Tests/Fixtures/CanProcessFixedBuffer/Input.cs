using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

[Serializable]
public unsafe struct FixedBuffer
{
    public fixed int buffer[7];
}

public class CanSerializeFixedBuffer : UnityEngine.MonoBehaviour
{
    public FixedBuffer fixedBuffer;
}