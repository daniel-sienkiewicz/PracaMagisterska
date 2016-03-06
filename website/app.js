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

var tempIn = NaN;
var tempOut = NaN;
var tempEngine = NaN;
var GPS = NaN;

app.post('/updateData', function (req, res) {
    var car = req.body;
    console.log('New car Data\n TempIn: ' + car.tempIn + "\n TempOut: " + car.tempOut + "\n TempEngine: " + car.tempEngine);
	tempIn = car.tempIn
	tempOut = car.tempOut
	tempEngine =car.tempEngine
	GPS = car.GPS;
	
	io.sockets.emit('tempIn', tempIn);
	io.sockets.emit('tempOut', tempOut);
	io.sockets.emit('tempEngine', tempEngine);
	io.sockets.emit('GPS', GPS);
	res.send("ok");
});



/**
 * Wlasciwa logika
 */
io.sockets.on('connection', function(socket) {
    io.sockets.emit('tempIn', tempIn);
	io.sockets.emit('tempOut', tempOut);
	io.sockets.emit('tempEngine', tempEngine);
	io.sockets.emit('GPS', GPS);
});

/**
 * Nasluch - port 3000
 */
httpServer.listen(3000, function() {	
    console.log('Serwer HTTP działa na pocie 3000');
});