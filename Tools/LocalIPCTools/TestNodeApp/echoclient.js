const ipc=require('../../../External/nodejs/node_modules/node-ipc');

var socketPath = process.argv[2];
var msg = process.argv[3];

ipc.config.rawBuffer = true;

ipc.connectTo(
    'test',
	socketPath,
	function(){
		ipc.of.test.emit(msg);
	}
);

