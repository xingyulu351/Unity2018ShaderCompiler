using System;
using NUnit.Framework;

namespace Unity.BuildSystem.CompilerTests.Attributes
{
    /// <summary>
    /// Used by the compiler test report generator.
    /// This attribute should be used on compiler tests that always should generate compilation errors or execution errors on ALL toolchains.
    /// It is used to generate the compiler test report, so that we know when a compilation failure is what is expected
    /// for all toolchains or if one of them is diverging.
    /// So in essence:
    /// Add this attribute on tests that should "fail" on all toolchains.
    /// </summary>
    [AttributeUsage(AttributeTargets.Method)]
    public class CompilerTestFailureByDefaultAttribute : PropertyAttribute
    {
        public CompilerTestFailureByDefaultAttribute()
            : base(true)
        {}
    }
}
