using UnityEngine.Networking;

namespace Unity.SerializationWeaver.IntegrationTests.Tests.Fixtures.CanRunUNetWeaver
{
	struct CubeState
	{
		public int x;
		public int y;
	}
	class CanRunUNetWeaver : NetworkBehaviour
	{
		[SyncVar]
		CubeState serverState;
	}
}
