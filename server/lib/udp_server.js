"use strict";

var socketio = require('socket.io');
var io;
var dgram = require('dgram');

exports.listen = function (server) {
    io = socketio.listen(server);
    // io.set('log level 1');
    io.sockets.on('connection', function (socket) {
        handleCommand(socket);
    });
};

function handleUDPCommand(socket, command, replyCommand) {
    socket.on(command, function (data) {

        // Info for connecting to the local process via UDP
        var PORT = 12345;
        var HOST = '192.168.7.2';
        var buffer = new Buffer(data);

        var client = dgram.createSocket('udp4');

        // Send command
        client.send(buffer, 0, buffer.length, PORT, HOST, function (err, bytes) {
            if (err)
                throw err;
            console.log('UDP message sent to ' + HOST + ':' + PORT + ", Buffer: " + buffer);
        });

        client.on('message', function (message, remote) {
            console.log("UDP Client: message Rx" + remote.address + ':' + remote.port + ' - ' + message);

            // send back to client
            var reply = message.toString('utf8').replace(/\u0000/g, '');
            var newlineIndex = reply.indexOf('\n');
            if (newlineIndex !== -1) {
                reply = reply.substring(0, newlineIndex);
            }

            console.log("Sending back Completed\n");

            socket.emit(replyCommand, reply);
            console.log(replyCommand);
            console.log(reply);
            client.close();

        });

        client.on("UDP Client: close", function () {
            console.log("closed");
        });

        client.on("UDP Client: error", function (err) {
            console.log("error: ", err);
        });
    });
}

function handleCommand(socket) {
    handleUDPCommand(socket, "history", "Get History");
    handleUDPCommand(socket, "upTime", "Uptime Info");
};
