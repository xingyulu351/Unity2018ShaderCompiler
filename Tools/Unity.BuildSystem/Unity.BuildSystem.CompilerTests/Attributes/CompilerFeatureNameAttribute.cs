using System;
using NUnit.Framework;

namespace Unity.BuildSystem.CompilerTests.Attributes
{
    /// <summary>
    /// Used by the compiler test report generator to add a human readable description to what compiler feature this test
    /// falls under. Without it the test will just be the name of the test method, which won't be very impressive.
    /// </summary>
    [AttributeUsage(AttributeTargets.Method)]
    public class CompilerFeatureNameAttribute : PropertyAttribute
    {
        public CompilerFeatureNameAttribute(string name)
            : base(name)
        {}
    }
}
