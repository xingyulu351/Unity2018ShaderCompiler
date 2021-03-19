using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using NUnit.Framework;

namespace Bee.Why.Tests
{
    class TokenDifferTests
    {
        [Test]
        public void AddedTokensArePaintedGreen()
        {
            TokenDiffer.Diff("token1", "token1 token2", out TokenDiffer.Token[] oldTokens, out TokenDiffer.Token[] newTokens);

            Assert.That(oldTokens, Has.Length.EqualTo(1));
            Assert.That(oldTokens[0].Value, Is.EqualTo("token1"));
            Assert.That(oldTokens[0].Color, Is.EqualTo(ConsoleColor.Gray));

            Assert.That(newTokens, Has.Length.EqualTo(2));
            Assert.That(newTokens[0].Value, Is.EqualTo("token1"));
            Assert.That(newTokens[0].Color, Is.EqualTo(ConsoleColor.Gray));
            Assert.That(newTokens[1].Value, Is.EqualTo("token2"));
            Assert.That(newTokens[1].Color, Is.EqualTo(ConsoleColor.Green));
        }

        [Test]
        public void RemovedTokensArePaintedGreen()
        {
            TokenDiffer.Diff("token1 token2", "token1", out TokenDiffer.Token[] oldTokens, out TokenDiffer.Token[] newTokens);

            Assert.That(oldTokens, Has.Length.EqualTo(2));
            Assert.That(oldTokens[0].Value, Is.EqualTo("token1"));
            Assert.That(oldTokens[0].Color, Is.EqualTo(ConsoleColor.Gray));
            Assert.That(oldTokens[1].Value, Is.EqualTo("token2"));
            Assert.That(oldTokens[1].Color, Is.EqualTo(ConsoleColor.Red));

            Assert.That(newTokens, Has.Length.EqualTo(1));
            Assert.That(newTokens[0].Value, Is.EqualTo("token1"));
            Assert.That(newTokens[0].Color, Is.EqualTo(ConsoleColor.Gray));
        }

        [Test]
        public void ReorderedTokensArePaintedYellow()
        {
            TokenDiffer.Diff("st token1 token2 ed", "st token2 token1 ed", out TokenDiffer.Token[] oldTokens, out TokenDiffer.Token[] newTokens);

            Assert.That(oldTokens, Has.Length.EqualTo(4));
            Assert.That(oldTokens[0].Value, Is.EqualTo("st"));
            Assert.That(oldTokens[0].Color, Is.EqualTo(ConsoleColor.Gray));
            Assert.That(oldTokens[1].Value, Is.EqualTo("token1"));
            Assert.That(oldTokens[1].Color, Is.EqualTo(ConsoleColor.Yellow));
            Assert.That(oldTokens[2].Value, Is.EqualTo("token2"));
            Assert.That(oldTokens[2].Color, Is.EqualTo(ConsoleColor.Yellow));
            Assert.That(oldTokens[3].Value, Is.EqualTo("ed"));
            Assert.That(oldTokens[3].Color, Is.EqualTo(ConsoleColor.Gray));

            Assert.That(newTokens, Has.Length.EqualTo(4));
            Assert.That(newTokens[0].Value, Is.EqualTo("st"));
            Assert.That(newTokens[0].Color, Is.EqualTo(ConsoleColor.Gray));
            Assert.That(newTokens[1].Value, Is.EqualTo("token2"));
            Assert.That(newTokens[1].Color, Is.EqualTo(ConsoleColor.Yellow));
            Assert.That(newTokens[2].Value, Is.EqualTo("token1"));
            Assert.That(newTokens[2].Color, Is.EqualTo(ConsoleColor.Yellow));
            Assert.That(newTokens[3].Value, Is.EqualTo("ed"));
            Assert.That(newTokens[3].Color, Is.EqualTo(ConsoleColor.Gray));
        }

        [Test]
        public void QuotedStringsAreTreatedAsSingleTokens()
        {
            TokenDiffer.Diff("a \"quoted string\" is not separated", "", out TokenDiffer.Token[] oldTokens, out TokenDiffer.Token[] newTokens);

            Assert.That(oldTokens.Select(t => t.Value), Is.EqualTo(new[] {"a", "\"quoted string\"", "is", "not", "separated"}).AsCollection);
        }

        [Test]
        public void EscapedQuoteCharactersInQuotedStringsAreRespected()
        {
            TokenDiffer.Diff("a \"quoted string with \\\" in the middle\" is not separated", "", out TokenDiffer.Token[] oldTokens, out TokenDiffer.Token[] newTokens);

            Assert.That(oldTokens.Select(t => t.Value), Is.EqualTo(new[] { "a", "\"quoted string with \\\" in the middle\"", "is", "not", "separated" }).AsCollection);
        }

        [Test]
        public void IdenticalStringsThrowArgumentException()
        {
            Assert.That(() => TokenDiffer.Diff("identical string", "identical string", out TokenDiffer.Token[] oldTokens, out TokenDiffer.Token[] newTokens), Throws.ArgumentException);
        }

        [Test]
        public void StringsThatDifferOnlyInWhitespaceThrowInvalidDataException()
        {
            Assert.That(() => TokenDiffer.Diff("whitespaced string", "whitespaced     string", out TokenDiffer.Token[] oldTokens, out TokenDiffer.Token[] newTokens), Throws.InstanceOf<InvalidDataException>());
        }
    }
}
