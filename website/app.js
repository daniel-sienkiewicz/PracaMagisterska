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
var GPSlatitude = NaN;
var GPSlongitude = NaN;
var lights = NaN;
var doors = NaN;

app.post('/updateData', function (req, res) {
  var car = req.body;
	tempIn = car.tempIn
	tempOut = car.tempOut
	tempEngine =car.tempEngine
	GPSlongitude = car.GPSlongitude;
	GPSlatitude = car.GPSlatitude;
	lights = car.lights
	doors = car.doors;

	io.sockets.emit('tempIn', tempIn);
	io.sockets.emit('tempOut', tempOut);
	io.sockets.emit('tempEngine', tempEngine);
	io.sockets.emit('GPSlongitude', GPSlongitude);
	io.sockets.emit('GPSlatitude', GPSlatitude);
	io.sockets.emit('lights', lights);
	io.sockets.emit('doors', doors);
	res.send("ok");
});

/**
 * Wlasciwa logika
 */
io.sockets.on('connection', function(socket) {
    io.sockets.emit('tempIn', tempIn);
	io.sockets.emit('tempOut', tempOut);
	io.sockets.emit('tempEngine', tempEngine);
	io.sockets.emit('GPSlongitude', GPSlongitude);
	io.sockets.emit('GPSlatitude', GPSlatitude);
	io.sockets.emit('lights', lights);
	io.sockets.emit('doors', doors);
});

/**
 * Nasluch - port 3000
 */
httpServer.listen(3000, function() {
    console.log('Serwer HTTP działa na pocie 3000');
});
