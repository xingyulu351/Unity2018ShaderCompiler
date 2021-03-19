const ipc=require('../../../External/nodejs/node_modules/node-ipc');

ipc.config.rawBuffer = false;

var socketPath;
if (process.platform == "win32")
    socketPath = 'Unity-nodeJSTest';
else
    socketPath = "/tmp/Unity-nodeJSTest.sock";

ipc.serve(
    socketPath,
    function(){
        ipc.server.on(
            'message',
            function(data, socket){
                ipc.log('Recieved message: ', data);
                ipc.server.emit(socket, 'message', data);
            }
        );
        console.log("Ready\n");
    }
);

ipc.server.start();

