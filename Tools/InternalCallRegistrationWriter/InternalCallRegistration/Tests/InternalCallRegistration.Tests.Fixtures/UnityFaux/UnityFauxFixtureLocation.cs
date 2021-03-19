namespace InternalCallRegistration.Tests.Fixtures.UnityFaux
{
	public class UnityFauxFixtureLocation
	{
		public static string Location()
		{
			return new UnityFauxFixtureLocation().GetType().Assembly.Location;
		}
	}
}
