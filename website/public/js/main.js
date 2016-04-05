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
		$scope.GPSlongitude = NaN;
		$scope.GPSlatitude = NaN;
		$scope.lights = 0;
		$scope.doors = 0;
		$scope.seatbelt = 0;

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
            return $scope.GPS;
    };

    socket.on('seatbelt', function(data) {
      $scope.seatbelt = data;
      $scope.$digest();

      if($scope.seatbelt != 0){
        $("#p").css("visibility", "visible");
      } else{
        $("#p").css("visibility", "hidden");
      }
    });

		socket.on('tempIn', function(data) {
      $scope.tempIn = data;
			$scope.$digest();
    });

		socket.on('tempOut', function(data) {
      $scope.tempOut = data;
			$scope.$digest();
    });

		socket.on('tempEngine', function(data) {
      $scope.tempEngine = data;
			$scope.$digest();
    });

		socket.on('GPSlatitude', function(data) {
      $scope.GPSlatitude = data;
			$scope.$digest();
			init();
    });

		socket.on('GPSlongitude', function(data) {
      $scope.GPSlongitude = data;
			$scope.$digest();
			init();
    });

		socket.on('lights', function(data) {
      $scope.lights = data;
			$scope.$digest();

			if($scope.lights == 1){
				$("#s").css("visibility", "visible");
			} else if($scope.lights == 0)
				$("#s").css("visibility", "hidden");
      });

		socket.on('doors', function(data) {
      $scope.doors = data;
			$scope.$digest();

      if($scope.doors & 16){
        $("#1").css("visibility", "visible");
      } else{
        $("#1").css("visibility", "hidden");
      }

      if($scope.doors & 8){
        $("#2").css("visibility", "visible");
      } else{
        $("#2").css("visibility", "hidden");
      }

      if($scope.doors & 4){
        $("#3").css("visibility", "visible");
      } else{
        $("#3").css("visibility", "hidden");
      }

      if($scope.doors & 2){
        $("#4").css("visibility", "visible");
      } else{
        $("#4").css("visibility", "hidden");
      }

      if($scope.doors & 1){
        $("#5").css("visibility", "visible");
      } else{
        $("#5").css("visibility", "hidden");
      }});

		google.maps.event.addDomListener(window, 'load', init);

		function init() {
			var mapOptions = {
			zoom: 11,
			scrollwheel: false,
			center: new google.maps.LatLng($scope.GPSlongitude, $scope.GPSlatitude),
			styles: [
            {
                "featureType": "water",
                "elementType": "geometry.fill",
                "stylers": [
                    {
                        "color": "#d3d3d3"
                    }
                ]
            },
            {
                "featureType": "transit",
                "stylers": [
                    {
                        "color": "#808080"
                    },
                    {
                        "visibility": "off"
                    }
                ]
            },
            {
                "featureType": "road.highway",
                "elementType": "geometry.stroke",
                "stylers": [
                    {
                        "visibility": "on"
                    },
                    {
                        "color": "#b3b3b3"
                    }
                ]
            },
            {
                "featureType": "road.highway",
                "elementType": "geometry.fill",
                "stylers": [
                    {
                        "color": "#ffffff"
                    }
                ]
            },
            {
                "featureType": "road.local",
                "elementType": "geometry.fill",
                "stylers": [
                    {
                        "visibility": "on"
                    },
                    {
                        "color": "#ffffff"
                    },
                    {
                        "weight": 1.8
                    }
                ]
            },
            {
                "featureType": "road.local",
                "elementType": "geometry.stroke",
                "stylers": [
                    {
                        "color": "#d7d7d7"
                    }
                ]
            },
            {
                "featureType": "poi",
                "elementType": "geometry.fill",
                "stylers": [
                    {
                        "visibility": "on"
                    },
                    {
                        "color": "#ebebeb"
                    }
                ]
            },
            {
                "featureType": "administrative",
                "elementType": "geometry",
                "stylers": [
                    {
                        "color": "#a7a7a7"
                    }
                ]
            },
            {
                "featureType": "road.arterial",
                "elementType": "geometry.fill",
                "stylers": [
                    {
                        "color": "#ffffff"
                    }
                ]
            },
            {
                "featureType": "road.arterial",
                "elementType": "geometry.fill",
                "stylers": [
                    {
                        "color": "#ffffff"
                    }
                ]
            },
            {
                "featureType": "landscape",
                "elementType": "geometry.fill",
                "stylers": [
                    {
                        "visibility": "on"
                    },
                    {
                        "color": "#efefef"
                    }
                ]
            },
            {
                "featureType": "road",
                "elementType": "labels.text.fill",
                "stylers": [
                    {
                        "color": "#696969"
                    }
                ]
            },
            {
                "featureType": "administrative",
                "elementType": "labels.text.fill",
                "stylers": [
                    {
                        "visibility": "on"
                    },
                    {
                        "color": "#737373"
                    }
                ]
            },
            {
                "featureType": "poi",
                "elementType": "labels.icon",
                "stylers": [
                    {
                        "visibility": "off"
                    }
                ]
            },
            {
                "featureType": "poi",
                "elementType": "labels",
                "stylers": [
                    {
                        "visibility": "off"
                    }
                ]
            },
            {
                "featureType": "road.arterial",
                "elementType": "geometry.stroke",
                "stylers": [
                    {
                        "color": "#d6d6d6"
                    }
                ]
            },
            {
                "featureType": "road",
                "elementType": "labels.icon",
                "stylers": [
                    {
                        "visibility": "off"
                    }
                ]
            },
            {},
            {
                "featureType": "poi",
                "elementType": "geometry.fill",
                "stylers": [
                    {
                        "color": "#dadada"
                    }
                ]
            }
        ]
    };

	var mapElement = document.getElementById('map');
	var map = new google.maps.Map(mapElement, mapOptions);
	var marker = new google.maps.Marker({
        position: new google.maps.LatLng($scope.GPSlongitude, $scope.GPSlatitude),
        map: map,
        title: 'Nileforest',
        icon: 'img/map-marker.png'
    });
	};
	}
]);
