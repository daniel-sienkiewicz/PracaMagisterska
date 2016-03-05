/**
 * Zalaczenie bibliotek oraz utworzenie serwera
 */
var express = require("express");
var app = express();

var httpServer = require("http").createServer(app);

var socketio = require("socket.io");
var io = socketio.listen(httpServer);
var path = require('path');

/**
 * Wykorzystanie wlasciwego kodu oraz zaleznosci
 */
 app.configure(function(){
  app.use(express.bodyParser());
  app.use(app.router);
});
 
app.use(express.static("public"));
app.use(express.static("bower_components"));
app.use(express.static(path.join(__dirname, 'bower_components/jquery/dist')));

app.post('/updateData', function (req, res) {
    var car = req.body;
    console.log('New car Data\n TempIn: ' + car.tempIn + "\n TempOut: " + car.tempOut + "\n TempEngine: " + car.tempEngine);
	io.sockets.emit('tempIn', car.tempIn);
	io.sockets.emit('tempOut', car.tempOut);
	io.sockets.emit('tempEngine', car.tempEngine);
	io.sockets.emit('GPS', car.GPS);
	res.send("ok");
});

/**
 * Wlasciwa logika
 */
io.sockets.on('connection', function(socket) {
    
});

/**
 * Nasluch - port 3000
 */
httpServer.listen(3000, function() {	
    console.log('Serwer HTTP działa na pocie 3000');
});