using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using NUnit.Framework;

namespace Unity.BuildSystem.Tests
{
    class JamStyleCommandLineParserTests
    {
        [Test]
        public void ParsesVariableValuesWithEqualsIn()
        {
            var cmdLine = new JamStyleCommandLineParser("-sVAR=value=with=equals");
            Assert.That(cmdLine.Variables["VAR"], Is.EqualTo("value=with=equals"));
        }

        [Test]
        public void ParsesMultipleTargets()
        {
            var cmdLine = new JamStyleCommandLineParser("TargetOne", "TargetTwo", "-sSOMEVAR=BLAH");
            Assert.That(cmdLine.TargetsToBuild, Is.EquivalentTo(new[] {"TargetOne", "TargetTwo"}));
        }

        [Test]
        public void VariableWithNoValueIsIgnored()
        {
            var cmdLine = new JamStyleCommandLineParser("-sVAR");
            Assert.That(cmdLine.Variables.Keys, Does.Not.Contain("VAR"));
        }

        [Test]
        public void OmittedDashSVariablesAreStillLocated()
        {
            var cmdLine = new JamStyleCommandLineParser("VAR=VALUE");
            Assert.That(cmdLine.Variables["VAR"], Is.EqualTo("VALUE"));
        }

        [Test]
        public void SpaceAfterDashSStillSetsVariable()
        {
            var cmdLine = new JamStyleCommandLineParser("-s", "VAR=VALUE");
            Assert.That(cmdLine.Variables["VAR"], Is.EqualTo("VALUE"));
        }

        [Test]
        public void NoTargetsSpecifiedImpliesAllTargets()
        {
            var cmdLine = new JamStyleCommandLineParser();
            Assert.That(cmdLine.TargetsToBuild, Does.Contain("all"));
        }
    }
}
