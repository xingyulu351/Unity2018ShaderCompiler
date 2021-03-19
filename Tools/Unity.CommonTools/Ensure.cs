using System;

namespace Unity.CommonTools
{
    public static class Ensure
    {
        public static void IsNotNull<T>(T value, string parameterName)
            where T : class
        {
            if (value == null)
            {
                throw new ArgumentNullException(parameterName);
            }
        }

        public static void IsNotNullOrEmpty(string value, string parameterName)
        {
            if (value == null)
            {
                throw new ArgumentNullException(parameterName);
            }
            if (value == string.Empty)
            {
                throw new ArgumentException("", parameterName);
            }
        }
    }
}
