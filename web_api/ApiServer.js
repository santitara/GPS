// Cargar modulos y crear nueva aplicacion
var express = require("express"); 
var cors = require('cors')
var app = express();
var session = require('express-session');


const mariadb = require('mariadb/callback');
const conn = mariadb.createConnection({
      host: 'localhost', 
      user:'root',
      database:'myIotdb',
      password: 'lanjaron',
    });
conn.connect(err => {
  if (err) {
    console.log("not connected due to error: " + err);
  } else {
    console.log("connected ! connection id is " + conn.threadId);
  }
});

app.use(cors())

app.use(session({
  secret: 'secret',
  resave: true,
  saveUninitialized: true
}));

var bodyParser = require('body-parser');

app.use(bodyParser.urlencoded({ extended: false })); // support encoded bodies
app.use(bodyParser.json()); // support json encoded bodies

//GetAll
//Ejemplo: GET http://localhost:8080/item
app.get('/item/', function(req, res, next) {
  if(req.query.filter) {
   next();
   return;
  }
  //res.send('Get all');
  console.log('Get all');
  conn.query(
  "SELECT * FROM prueba WHERE 1",
  (err, result) => {
	if (err) throw err;
	console.log(result);
	res.send(JSON.stringify(result));
	//log : { affectedRows: 1, insertId: 1, warningStatus: 0 }
  }
);
});

//GetDabta by mail
//Ejemplo: GET http://localhost:8080/item
app.get('/user/', function(req, res, next) {
  /*if(req.query.filter) {
   next();
   return;
  }*/
  //res.send('Get all');
  console.log('Get all');
  var mail = req.body.mail;
  var username;
  var device_name;
  console.log(mail);
  conn.query(
  "SELECT Username FROM tbUsers WHERE Mail = (?)",
  [mail],
  (err, result) => {
  if (err) throw err;
  console.log(result);
  //res.send(JSON.stringify(result));
  username  = result[0].Username;
  console.log(username);
  //log : { affectedRows: 1, insertId: 1, warningStatus: 0 }
  }
);
});

//GetDabta by mail
//Ejemplo: GET http://localhost:8080/item
app.get('/devices/', function(req, res, next) {
  /*if(req.query.filter) {
   next();
   return;
  }*/
  //res.send('Get all');
  console.log(req.body);
  var device_name = req.body.devicename;
  console.log('Devices from user'+device_name);
  conn.query(
  //SELECT * FROM `tbData` WHERE `DeviceName`LIKE 'node' AND `Time` BETWEEN '2020-05-16 00:00:00.000000' AND '2020-05-16 23:00:00.000000
  "SELECT * FROM tbData WHERE DeviceName LIKE (?) AND Time BETWEEN '2020-05-16 00:00:00.000000' AND '2020-05-16 23:59:59.000000' ORDER BY Time DESC",
  [device_name],
  (err, result) => {
  if (err) throw err;
  console.log(result);
  res.send(JSON.stringify(result));
  //username  = result[0].Username;
  //console.log(username);
  //log : { affectedRows: 1, insertId: 1, warningStatus: 0 }
  }
);
});
 
//GetById
//Ejemplo: GET http://localhost:8080/item/10
app.get('/item/:id', function(req, res, next) {
  var itemId = req.params.id;
  res.send('Get ' + req.params.id);
  console.log('Get ' + req.params.id);
});
 
//GetFiltered
//Ejemplo: GET http://localhost:8080/item?filter=ABC
app.get('/item', function(req, res) {
  var filter = req.query.filter;
  res.send('Get filter ' + filter);
  console.log('Get filter ' + filter);
});
 
//Create
//Ejemplo: POST http://localhost:8080/item
app.post('/item', function(req, res) {
   var data = req.body;
   res.send('ok log');
   console.log('Add pos');
});

//Create
//Ejemplo: POST http://localhost:8080/item
app.post('/data', function(req, res) {
  var data = req.body;
  console.log (data);
  var device_name = req.body.DeviceName;
  var time = req.body.Time;
  var ip = req.body.Ip;
  var mac = req.body.Mac;
  var temp = req.body.Temperature;
  var hum = req.body.Humidity;
  var humfloor = req.body.HumFloor;
  var rain = req.body.Rain;
  var wind = req.body.Wind;
  var batt = req.body.Battery;
  console.log (device_name);
  console.log (time);
  console.log (ip);
  console.log (mac);
  console.log (temp);
  console.log (hum);
  console.log (humfloor);
  console.log (rain);
  console.log (wind);
  console.log (batt);

   //res.send('ok log');
   console.log('Add pos');


   
  conn.query(
  "INSERT INTO tbData (DeviceName, Ip, Mac, Temperature, Humidity, HumFloor, Rain, Wind, Battery) VALUES (?,?,?,?,?,?,?,?,?)",
  [device_name,ip,mac,temp,hum,humfloor,rain,wind,batt],
  (err, result) => {
  if (err) throw err;
  console.log(err);
  console.log(result[2]);
  console.log(result[0]);
  console.log(result[1]);
  console.log(result.warningStatus);
  //console.log(result[0].Username);
  if(result.warningStatus == 0)
  {
    console.log("insertado");
    res.send("ok");
  }
  else
  {
    res.send("log fail");
  }
  //res.send(JSON.stringify(result));
  //log : { affectedRows: 1, insertId: 1, warningStatus: 0 }
  }
  );

});

//Ejemplo: POST http://localhost:8080login
/**************LOGIN*******************/

app.post('/log', function(req, res) {
   var user = req.body.username;
   var pass = req.body.password;
   //console.log(req);
   console.log(req.body);
   console.log(pass);

  conn.query(
  "SELECT Username FROM tbUsers WHERE Mail = (?) AND Password =(?)",
  [user,pass],
  (err, result) => {
  if (err) throw err;
  console.log(result);
  console.log(result[0]);
  //console.log(result[0].Username);
  if(result[0])
  {
    console.log("coincidencia");
    res.send("ok");
  }
  else
  {
    console.log("no coincide");
    res.send("log fail");
  }
  //res.send(JSON.stringify(result));
  //log : { affectedRows: 1, insertId: 1, warningStatus: 0 }
  }
  
);
  console.log("envio respuesta");


   //res.send('logg');
   //console.log('Add login');
});

 
//Replace
//Ejemplo: PUT http://localhost:8080/item/10
app.put('/item/:id', function(req, res) {
   var itemId = req.params.id;
   var data = req.body.data;
   res.send('Replace ' + itemId + ' with ' + data);
   console.log('Replace ' + itemId + ' with ' + data);
});
 
//Update
//Ejemplo: PATCH http://localhost:8080/item/10
app.patch('/item/:id', function(req, res) {
   var itemId = req.params.id;
   var data = req.body.data;
   res.send('Update ' + itemId + ' with ' + data);
   console.log('Update ' + itemId + ' with ' + data);
});
 
//Delete
//Ejemplo: DEL http://localhost:8080/items
app.delete('/item/:id', function(req, res) {
   var itemId = req.params.id;
   res.send('Delete ' + itemId);
   console.log('Delete ' + itemId);
});


/*app.get('/', function(request, response) {
  response.sendFile(path.join(__dirname + '/login.html'));
});*/

app.post('/login', function(request, response) {
  var username = request.body.username;
  var password = request.body.password;
  console.log(username);
  console.log(password);
  if (username && password) {
    conn.query('SELECT * FROM tbUsers WHERE Mail = ? AND Password = ?', 
    [username, password], 
    function(error, results, fields) {
      console.log(results);
      if (results.length > 0) {
        request.session.loggedin = true;
        request.session.username = username;
        //response.redirect('/home');
        response.send('Correct');
      } else {
        response.send('Incorrect Username and/or Password!');
      }     
      response.end();
    });
  } else {
    response.send('Please enter Username and Password!');
    response.end();
  }
});

var server = app.listen(8080, function () {
    console.log('Server is running..'); 
});


let posicion = {
	latitude:'',
        longitude:'',
	speed:'',
	timestamp:''
};

//Misana iot recogida de datos
//Ejemplo: POST http://localhost:8080/
app.post('/metrics', function(req, res) {

	  var data = req.body;
	  console.log (data);
	  var count = Object.keys(data).length;
	  console.log("number of registers " + count);
	  //bucle for segun cantidad de elementos que se envien en json
	  var iter = 0; 
	  var latitude = 0;
	  var longitude = 0;
	  var speed = 0;
	  var timestamp = 0;
	  var pdop = 0;
	  var resultado;
	  var inserts = [];
	  var imei = req.query.imei;
	  imei = parseInt(imei, 10);
	  //console.log('Get imei ' + imei);
	  var url = req.query;
	  //console.log(url);
	  if(count != 0)
	  {
		  for(iter = 0; iter<count;iter++)
		  {
			latitude = req.body[iter].latitude;
			longitude = req.body[iter].longitude;
			speed = req.body[iter].speed;
			timestamp = req.body[iter].timestamp;
			pdop = req.body[iter].PDOP;
			if(timestamp == null )
      {
				timestamp = 0;
      }
			if(pdop == null )
      {
				pdop = 0;
      }
      if(latitude == null)
      {
        latitude = 0;
      }
      if(longitude == null)
      {
        longitude =0;
      }
      if(speed == null )
      {
        speed = 0;
      }
			//console.log (latitude);
			//console.log (longitude);
			//console.log (speed);
			//console.log (timestamp);
			//console.log("iter");
			//console.log(pdop);
			inserts.push([imei,latitude,longitude,speed,timestamp,pdop]);
		  }
			
			var sql = "INSERT INTO tbMisana (imei,latitude, longitude, speed, timestamp,pdop) VALUES (?,?,?,?,?,?) ";
			conn.batch(
				sql,
				inserts,
				(err, result) => {
					if (err) throw err;
					//console.log(result.warningStatus);
					resultado = result.warningStatus;
					if(resultado == 0)
					{
						console.log("insertados");
						res.send("ok");
					}
					else if(resultado != 0)
					{	
						console.log("error");
						res.send("log fail");
					}
					
				}
			);
	
  	}
    else
    {
      console.log("error mensaje vacio")
      res.statusCode = 400;
      res.send("log empty");

    }
});

//GetDabta by imei and timpestamp
//Ejemplo: GET http://79.109.207.104/data_gps?imei=xxxxx?timestamp=xxxxx
app.get('/data_gps/', function(req, res, next) {
  
  var imei = req.query.imei;
  imei = parseInt(imei, 10);
  var timestamp = req.query.timestamp;
  timestamp = parseInt(timestamp, 10);
  console.log("get data to device " +imei + "and timestamp greater than " +timestamp);
  //console.log(imei);
  //console.log(timestamp);
  conn.query(
  "SELECT * FROM tbMisana WHERE imei = ? AND timestamp >= ? ORDER BY timestamp ASC",
  [imei,timestamp],
  (err, result) => {
  if (err) throw err;
  //console.log(result);
  res.send(JSON.stringify(result));
  }
)});

app.get('/imeis', function(req, res, next ) {

  conn.query( "SELECT imei FROM tbMisana GROUP BY imei", (err, result) => {
    if (err) throw err;
    //console.log(result);
    res.send(JSON.stringify(result));
    }
  );
  });

  app.get('/coordinates', function(req, res, next ) {
    var imei = req.query.imei;
    imei = parseInt(imei, 10);
    conn.query( "SELECT latitude, longitude, timestamp FROM tbMisana  WHERE imei = ? GROUP BY timestamp",
    [imei], (err, result) => {
      if (err) throw err;
      //console.log(result);
      res.send(JSON.stringify(result));
      }
    );
    });

//GetDabta by imei and date_ini and date_end
//Ejemplo: GET http://79.109.207.104/data_gps?imei=xxxxx?date_ini=xxxxx?date_end=xxxxxxx
app.get('/coordinates/', function(req, res, next) {
  
  var imei = req.query.imei;
  imei = parseInt(imei, 10);

  var date_ini = req.query.date_ini;
  date_ini = parseInt(date_ini, 10);

  var date_end = req.query.date_end;
  date_end = parseInt(date_end, 10);

  //console.log("get data to device " +imei + "and timestamp greater than " +timestamp);
  //console.log(imei);
  //console.log(timestamp);
  conn.query(
  "SELECT latitude, longitude, timestamp FROM tbMisana WHERE imei = ? AND timestamp >= ? AND timestamp <= ? ORDER BY timestamp ASC",
  [imei,date_ini,date_end],
  (err, result) => {
  if (err) throw err;
  //console.log(result);
  res.send(JSON.stringify(result));
  }
)});
