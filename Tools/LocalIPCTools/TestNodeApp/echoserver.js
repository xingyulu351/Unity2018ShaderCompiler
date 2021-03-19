const ipc=require('../../../External/nodejs/node_modules/node-ipc');

ipc.config.rawBuffer = true;

var socketPath;
if (process.platform == "win32")
	socketPath = 'Unity-nodeJSTest';
else
	socketPath = "/tmp/Unity-nodeJSTest.sock";

ipc.serve(
    socketPath,
	function(){
		ipc.server.on(
			'data',
			function(buffer, socket)
			{
				ipc.log('Recieved ' + buffer.toString() + ', echoing\n');
				ipc.server.emit(socket, buffer);
			}
		);
		console.log("Ready\n");
	}
);

ipc.server.start();

