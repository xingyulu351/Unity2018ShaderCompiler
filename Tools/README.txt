Before adding anything to the Tools/ directory, please take note of the
information on Confluence about adding 3rd-party software to the Unity
codebase:

https://confluence.hq.unity3d.com/display/DEV/How+To+Add+Third+Party+Software+in+Unity+Code+Base

In short, unless we wrote the entire tool ourselves, it probably
belongs in External/.  Or it may belong in Editor/Tools or under a
particular Platform's PlatformDependent folder.  The documentation
referenced above provides much more detail -- please read and
understand it before proceeding.
