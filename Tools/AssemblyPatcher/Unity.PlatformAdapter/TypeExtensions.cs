using System;

namespace Unity.PlatformAdapter
{
    public class TypeExtensions
    {
        public static Type[] EmptyTypes = new Type[0];

        public static string StringEmpty = "";

        public static bool IsSubClassOf(Type toBeChecked, Type type)
        {
            try
            {
                return Convert.ChangeType(Activator.CreateInstance(toBeChecked), type) != null;
            }
            catch (InvalidCastException)
            {
            }
            catch (FormatException)
            {
            }

            return false;
        }

        public static bool IsValueType(Type type)
        {
            return false;
        }

        public static bool EnumToBoolean(Enum @enum, object formatProvider)
        {
            return Convert.ToInt32(@enum) != 0;
        }
    }
}
