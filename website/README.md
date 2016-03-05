# Strona www obrazująca aktualny stan samochodu"

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
curl -i -X POST -H 'Content-Type: application/json' -d '{"tempIn": "40", "tempOut" : "20", "tempEngine" : "16", "GPS" : "152"}' http://localhost:3000/updateData
~~~