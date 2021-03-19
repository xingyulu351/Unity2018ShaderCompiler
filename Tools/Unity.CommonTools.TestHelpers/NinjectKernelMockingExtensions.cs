using Moq;
using Ninject.Syntax;

namespace Unity.CommonTools.TestHelpers
{
    public static class NinjectKernelMockingExtensions
    {
        public static Mock<T> Mock<T>(this IBindingToSyntax<T> binding)
            where T : class
        {
            var mock = new Mock<T>();
            binding.ToMethod(x => mock.Object).InSingletonScope();
            return mock;
        }
    }
}
