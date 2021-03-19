public class ContainerClass
{
    public class NestedClass
    {
    }

    public enum NestedEnum
    {
        ValueA,
        ValueB
    }
}

namespace UnityEngine
{
    public enum RuntimePlatform
    {
        OSXEditor,
        LinuxPlayer
    }

    public class Object
    {
    }
}

namespace UnityEditor
{
    //docs present at this level
    public class PlayerSettings : UnityEngine.Object
    {
        //NO docs at this level
        public sealed class PS4
        {
            //docs present at this level
            public enum PS4RemotePlayKeyAssignment
            {
                None,
                PatternA,
                PatternB
            }
        }

        public sealed class PSVita
        {
            public enum PSVitaAppCategory
            {
            }
        }
    }
}

namespace UnityEngine.UI
{
    public class AspectRatioFitter
    {
        public enum AspectMode
        {
        }
    }
}
