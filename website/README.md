# Strona www obrazująca aktualny stan samochodu

### Instalacja
~~~
npm install
bower install
~~~

### Start aplikacji
~~~
node app.js
~~~

### Opis
Użyte technoligie:
* SocketIO
* Bower
* Grunt
* jQuery
* AngularJS
* Express
* Bootstrap

### Przykladowy CURL
~~~
curl -i -X POST -H 'Content-Type: application/json' -d '{"tempIn": "40", "tempOut" : "20", "tempEngine" : "6", "GPSlongitude" : "52", "GPSlatitude" : "18", "doors" : "5", "seatbelt" : "6", "lights" : "0" }' http://localhost:3000/updateData
~~~