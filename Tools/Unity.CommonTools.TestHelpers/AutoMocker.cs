using System;
using System.Collections.Generic;
using Moq;
using NUnit.Framework;

namespace Unity.CommonTools.TestHelpers
{
    public abstract class AutoMocker
    {
        private IDictionary<Type, Mock> m_mocks;

        [SetUp]
        public void AutoMockerSetUp()
        {
            m_mocks = new Dictionary<Type, Mock>();
        }

        protected T GetMocked<T>()
            where T : class
        {
            return GetMock<T>().Object;
        }

        protected Mock<T> GetMock<T>() where T : class
        {
            Mock mock;
            if (!m_mocks.TryGetValue(typeof(T), out mock))
            {
                mock = new Mock<T>();
                m_mocks[typeof(T)] = mock;
            }
            return (Mock<T>)mock;
        }
    }
}
