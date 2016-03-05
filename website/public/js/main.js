var app = angular.module('Autko', []);

app.factory('socket', function() {
    var socket = io.connect('http://' + location.host);
    return socket;
});

app.controller('ctrlr', ['$scope', 'socket',
    function($scope, socket) {
		$scope.tempIn = NaN;
		$scope.tempOut = NaN;
		$scope.tempEngine = NaN;
		$scope.GPS = NaN;
		
		$scope.getTempIn = function() {
            return $scope.tempIn;
        };
        
		$scope.getTempOut = function() {
            return $scope.tempOut;
        };
		
		$scope.getTempEngine = function() {
            return $scope.tempEngine;
        };
		
		$scope.getGPS = function() {
            return $scope.GPS;;
        };
		
		socket.on('tempIn', function(data) {
			console.log(data);
            $scope.tempIn = data;
			$scope.$digest();
        });
		
		socket.on('tempOut', function(data) {
			console.log(data);
            $scope.tempOut = data;
			$scope.$digest();
        });
		
		socket.on('tempEngine', function(data) {
			console.log(data);
            $scope.tempEngine = data;
			$scope.$digest();
        });
		
		socket.on('GPS', function(data) {
			console.log(data);
            $scope.GPS = data;
			$scope.$digest();
        });
	}
]);