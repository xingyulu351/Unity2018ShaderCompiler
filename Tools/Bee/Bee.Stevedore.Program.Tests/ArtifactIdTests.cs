using NUnit.Framework;

namespace Bee.Stevedore.Program.Tests
{
    public class ArtifactNameTests
    {
        [TestCase("totally-legit---name")]
        [TestCase("may_contain_underscores")]
        [TestCase("_foobar")]
        public void StringRepresentation(string name)
        {
            var artifactName = new ArtifactName(name);
            Assert.AreEqual(name, artifactName.Value);
            Assert.AreEqual(name, artifactName.ToString());
            Assert.AreEqual(name, ((string)artifactName));
        }

        [TestCase("name with spaces")]
        [TestCase("namewith/slash")]
        [TestCase("1.2.3")]
        [TestCase("-cannot_start_with_dash")]
        [TestCase("..")]
        [TestCase("/")]
        [TestCase("lpt1")] // invalid on Windows
        [TestCase("foo.")] // invalid on Windows (but also not valid)
        public void ThrowsWhenInvalid(string name)
        {
            var e = Assert.Throws<InvalidArtifactStringException>(() => new ArtifactName(name));
            Assert.That(e.Message, Contains.Substring("not a valid artifact name"));
        }
    }

    public class ArtifactVersionTests
    {
        [TestCase("with-a-40-characters-long-version-string_0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef.zip")]
        [TestCase("totally-legit---version_0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef.zip")]
        [TestCase("may_contain_underscores_0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef.7z")]
        [TestCase("_foobar_0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef.zip")]
        [TestCase("1.2.3_f49c8d8b0a771f26cb64abdcd36f15347fdd7dabc4c297e5de8d999e05384876.zip")]
        public void StringRepresentation(string version)
        {
            var artifactVersion = new ArtifactVersion(version);
            Assert.AreEqual(version, artifactVersion.Value);
            Assert.AreEqual(version, artifactVersion.ToString());
            Assert.AreEqual(version, ((string)artifactVersion));
        }

        [TestCase("this_is_a_valid.7z-version", "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef", ".zip")]
        [TestCase("0123456789abcdef0123456789abcdef01234567", "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef", ".7z")]
        public void PartsParseCorrectly(string vs, string hash, string ext)
        {
            var artifactVersion = new ArtifactVersion(vs + "_" + hash + ext);
            Assert.AreEqual(vs, artifactVersion.VersionString);
            Assert.AreEqual(ext, artifactVersion.Extension);
            Assert.AreEqual(hash, artifactVersion.Hash);
        }

        [TestCase("foo.zip")] // no hash
        [TestCase("foo_0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef")] // no extension
        [TestCase("..")]
        [TestCase("/")]
        [TestCase("foo.")] // invalid on Windows (but also not valid)
        public void BadStructure_Throws(string version)
        {
            var e = Assert.Throws<InvalidArtifactStringException>(() => new ArtifactVersion(version));
            Assert.That(e.Message, Contains.Substring("not a valid artifact version"));
        }

        [TestCase("with-a--41-characters-long-version-string")]
        [TestCase("version-string-with-trailing-period.")]
        [TestCase(".version-string-with-leading-period")]
        [TestCase("version with spaces")]
        [TestCase("versionwith/slash")]
        [TestCase("lpt1.foo")] // invalid on Windows
        public void BadVersionString_Throws(string versionString)
        {
            var e = Assert.Throws<InvalidArtifactStringException>(() => new ArtifactVersion(versionString + "_0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef.zip"));
            Assert.That(e.Message, Contains.Substring("not a valid artifact version string"));
        }

        [TestCase("thisisnotavalidhashaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa")]
        [TestCase("00123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef")] // too long
        [TestCase("123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef")] // too short
        public void BadHash_Throws(string hash)
        {
            var e = Assert.Throws<InvalidArtifactStringException>(() => new ArtifactVersion($"foo_{hash}.zip"));
            Assert.That(e.Message, Contains.Substring("not a valid artifact hash"));
        }

        [TestCase(".tar.gz")]
        [TestCase(".rar")]
        public void BadExtension_Throws(string extension)
        {
            var e = Assert.Throws<InvalidArtifactStringException>(() => new ArtifactVersion($"foo_0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef{extension}"));
            Assert.That(e.Message, Contains.Substring("not a valid artifact extension"));
        }
    }

    public class ArtifactIdTests
    {
        [TestCase("AVeryCrazyLongDependencyName/with-a-40-characters-long-version-string_f49c8d8b0a771f26cb64abdcd36f15347fdd7dabc4c297e5de8d999e05384876.zip")]
        [TestCase("my-artifact/totally-legit---version_0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef.zip")]
        public void StringRepresentation(string id)
        {
            var artifactId = new ArtifactId(id);
            Assert.AreEqual(id, artifactId.ToString());
        }

        [TestCase("my-artifact", "this_is_a_valid.7z-version", "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef", ".zip")]
        [TestCase("AVeryCrazyLongDependencyName", "0123456789abcdef0123456789abcdef01234567", "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef", ".7z")]
        public void PartsParseCorrectly(string an, string vs, string hash, string ext)
        {
            var artifactId = new ArtifactId(an + "/" + vs + "_" + hash + ext);
            Assert.AreEqual(an, artifactId.Name.Value);
            Assert.AreEqual(vs, artifactId.Version.VersionString);
            Assert.AreEqual(ext, artifactId.Version.Extension);
            Assert.AreEqual(hash, artifactId.Version.Hash);
        }

        [TestCase("no-slash")]
        [TestCase("too/many/slashes")]
        public void ThrowsWhenInvalid(string id)
        {
            var e = Assert.Throws<InvalidArtifactStringException>(() => new ArtifactId(id));
            Assert.That(e.Message, Contains.Substring("not a valid artifact ID"));
        }
    }
}
