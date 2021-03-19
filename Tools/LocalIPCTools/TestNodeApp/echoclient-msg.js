const ipc=require('../../../External/nodejs/node_modules/node-ipc');

var socketPath = process.argv[2];
var msg = process.argv[3];

ipc.config.rawBuffer = false;

ipc.connectTo(
    'test',
    socketPath,
    function(){
        ipc.of.test.on(
            'connect',
            function(){
                ipc.of.test.emit('message', msg);
            }
        );
        ipc.of.test.on(
            'message',
            function(data){
                ipc.log('Got message from server:', data);
                ipc.disconnect('test');

                // Validate that message sent == message received
                let ret = (data == msg) ? 0 : 1;
                process.exit(ret);
            }
        );
        ipc.of.test.on(
            'disconnect',
            function(){
                ipc.log('Disconnecting');
            }
        );

    }
);

