using System;
using System.Linq;

namespace BindingsToCsAndCpp
{
    public enum ValidationType
    {
        Visibility
    }

    public static class ValidationHelpers
    {
        public static readonly string[] ValidVisibility = {"public", "private", "protected", "internal"};

        public static void ValidateVisibility(string value)
        {
            if (!ValidVisibility.Contains(value))
                throw new ValidationException(ValidationType.Visibility, value);
        }

        public static bool TryValidateVisibility(string value)
        {
            return ValidVisibility.Contains(value);
        }
    }

    public class ValidationException : Exception
    {
        public ValidationType ValidationType;

        public ValidationException(ValidationType type, string value) : base(Enum.GetName(type.GetType(), type) + " validation failed: " + value)
        {
            ValidationType = type;
        }
    }
}
