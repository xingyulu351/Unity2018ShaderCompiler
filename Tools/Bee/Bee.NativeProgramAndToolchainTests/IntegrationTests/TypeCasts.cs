using Bee.Core.Tests;
using Bee.NativeProgramAndToolchainTests.IntegrationTests;
using NUnit.Framework;

namespace Unity.BuildSystem.NativeProgramSupport.Tests.IntegrationTests
{
    public class TypeCasts : NativeProgramAndToolchainTestBase
    {
        [TestCaseSource(nameof(RunnableToolchains_FastestBackend))]
        public void TypeCast_RTTI(ToolChain toolchain, BeeAPITestBackend<ToolChain> testBackend)
        {
            ManualFixtureSetup(toolchain, testBackend);

            MainCpp.WriteAllText(@"
#include <stdio.h>

class Base { virtual void dummy() {} };
class Derived: public Base { int a; };

int main () {
    Base* pba = new Derived;
    Base* pbb = new Base;
    Derived* pd;

    pd = dynamic_cast<Derived*>(pba);
    if (pd == nullptr) printf(""Cast to actual Derived failed. Should've succeeded."");

    pd = dynamic_cast<Derived*>(pbb);
    if (pd != nullptr) printf(""Cast to actual Base succeeded. Should've failed."");

    printf(""1"");
    return 0;
}
");
            BuildAndCheckProgramOutput(toolChain => SetupNativeProgramAndRun(toolChain,
                np => np._compilerCustomizations.Add<CLikeCompilerSettings>(c => c.WithRTTI(true))), "1");
        }
    }
}
