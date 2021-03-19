using System;
using NUnit.Framework;

namespace Unity.BuildSystem.CompilerTests.Attributes
{
    /// <summary>
    /// Used by the compiler test report generator. It should be added to tests that require a more detailed description
    /// of what the compiler feature under test is.
    /// </summary>
    [AttributeUsage(AttributeTargets.Method, AllowMultiple = false)]
    public class CompilerFeatureDescriptionAttribute : PropertyAttribute
    {
        public CompilerFeatureDescriptionAttribute(string name)
            : base(name)
        {}
    }
}
