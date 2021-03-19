using System;
using System.Collections.Generic;


[Flags]
public enum UnityProfiles
{
    None = 0,
    Base = 1,
    Unity = 2,
    UnityWeb = 4,
    Micro = 8,
    All = 15,
    Last = 16
}

public static class UnityProfilesUtils
{
    public static string DirectoryNameFromProfile(this UnityProfiles profile)
    {
        switch (profile)
        {
            case UnityProfiles.None:
                return "2.0";
            case UnityProfiles.Base:
                return "2.0";
            case UnityProfiles.Unity:
                return "unity";
            case UnityProfiles.UnityWeb:
                return "unity_web";
            case UnityProfiles.Micro:
                return "micro";
            default:
                throw new ArgumentException(String.Format("Invalid profle {0}", profile));
        }
    }

    public static string DocumentationNameFromProfile(this UnityProfiles profile)
    {
        switch (profile)
        {
            case UnityProfiles.None:
                return "Base";
            case UnityProfiles.Base:
                return ".NET 2.0";
            case UnityProfiles.Unity:
                return "2.0 subset";
            case UnityProfiles.UnityWeb:
                return "Web player";
            case UnityProfiles.Micro:
                return "Micro";
            default:
                throw new ArgumentException(String.Format("Invalid profle {0}", profile));
        }
    }

    public static IEnumerable<UnityProfiles> ListProfiles()
    {
        for (int bit = 0; (1 << bit) < ((int)UnityProfiles.Last); bit++)
        {
            UnityProfiles r = (UnityProfiles)(1 << bit);
            //if (r == UnityProfiles.Micro)
            //  continue;
            //dont support micro for now, it creates duplicates entries for some types: Activation
            //if (r == UnityProfiles.Base)
            //  continue;
            //dont support 2.0 for now, it creates duplicates entries for some types: Socket
            yield return r;
        }
        yield break;
    }

    public static bool Contains(this UnityProfiles profiles, UnityProfiles profile)
    {
        return (profiles & profile) != 0;
    }

    public static void AddTo(this UnityProfiles profile, ref UnityProfiles profiles)
    {
        profiles |= profile;
    }

    #if false
    public static bool HasSecurityCriticalAttribute(this ICompAttributeContainer attributeContainer)
    {
        List<CompNamed> attributes = attributeContainer.GetAttributes();
        foreach (CompNamed cn in attributes)
        {
            if (cn.Name == "System.Security.SecurityCriticalAttribute")
            {
                return true;
            }
        }
        return false;
    }

    #endif
}
