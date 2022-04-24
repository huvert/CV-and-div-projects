"use strict";
/*
We first import http. socket.io (which will be our websocket,) is built upon http.
The server will do as follows:
1.  Use http to send .HTML, .css and .js to Webserver.
2.  Request to open a websocket connection between browser and server.
    Later communication between server and brower will be sent using websocket (socket.io)
*/
const express = require('express');
const app     = express();
const http    = require('http').createServer(app);
const io      = require('socket.io')(http);

// require my custom-made libraries
// We split the project into several files to make it easier to read
const time    = require('./libraries/server-libs/timelib.js');
const fb      = require('./libraries/server-libs/firebase-database.js');

// import constants from config.js
const { maxDataTableSize, PORT } = require('./public/config.js');

var available_rooms = ["website", "esp"];
var c = 0;
/*
  clients stores all clients connected to rooms "esp" and "website"
    structure:
      clients {esp: [{clientId: "", clientName: ""}, {...}], website: []};
      Note that esp has both ID and Name. Website has only ID.
*/
var clients = {esp: [], website: []};

function pushAndShift(list) {
  list.push(list[0]);
  list.shift();
  return list
}

// ==   Functions for keeping track of clients   ==
function createClientName(name) {
  let new_name;
  if (getClientId(name) != 0) { // Name already taken (getClientId will return 0 if no Id were found)
    new_name = createClientName(`${name}*`);    // Same name but with a * to identify its a copy (this will loop untill a name is found.)
    return new_name
  }
  else if (name != null) { // Name specified and not taken
    new_name = name;
    return new_name
  }
  else {  // no name specified
    new_name = createClientName("Unnamed Client");
    return new_name;
  }
}

function removeClientName(name, socketId) {
  // Check room: "website"
  if (clients.website.includes(socketId)) {
    let index = clients.website.indexOf(socketId);
    clients.website.splice(index, 1);
  }
  // Check room: "esp"
  let len = clients.esp.length;
  for (let i=0; i<len; ++i) {
    var client = clients.esp[i];
    if (client.clientId == socketId) {
      clients.esp.splice(i,1);
      break;
    }
  }
}

// The following 2 functions are only used on room: "esp"
function getClientId(client_name) {
  let len = clients.esp.length;
  for (let i=0; i<len; ++i) {
    var client = clients.esp[i];
    if (client.clientName == client_name) {
      return client.clientId
    }
  }
  return 0
}

function getClientName(clientId) {
  let len = clients.esp.length;
  for (let i=0; i<len; ++i) {
    var client = clients.esp[i];
    if (client.clientId == clientId) {
      return client.clientName
    }
  }
  return 0
}



// == Setting up server ==
app.use(express.static(__dirname + '/public/'));                   // Tells express where to look for files (such as stylesheets)
app.use('/scripts', express.static(__dirname + '/node_modules/')); // Tells index.html where to look for libraries such as chart.js
app.get('/', (req, res) => {
  res.sendFile(__dirname+"/public/index.html");
});
app.get('*', (req, res) => {
  res.sendFile(__dirname+"/public/404.html");
});

http.listen(PORT, () => {
  console.log('Listening on port: ' + PORT);
});


// ==========================================
// =======         Websockets        ========
io.sockets.on("connection", (socket) => {


  socket.on('disconnect', () => {
    let clientName = getClientName(socket.id);
    removeClientName(clientName, socket.id);
    console.log(`[DISCONNECT] client ${clientName} disconnected`);
    socket.to('website').emit("res-client-list", JSON.stringify(clients.esp)); // update client-tables on websites.
  });



  // -- Join room and give client name --
  socket.on("join-room", (data) => {  // data in format: "room_name#client_name"
    data = data.split("#");
    let client_name = createClientName(data[1]);
    let room = data[0];

    // Join Room
    if (available_rooms.includes(room)) {
      socket.join(room);
      console.log(`[JOIN-ROOM] client ${client_name} joined room: ${room}`);

      if (room === "website") {
        clients.website.push(socket.id);
        socket.emit("res-client-list", JSON.stringify(clients.esp));
      }
      // give client name
      else {
        var clientInfo = new Object();
        clientInfo.clientId = socket.id;
        clientInfo.clientName = client_name;
        clients.esp.push(clientInfo);
        // Ask website to update list of CLIENTS
        socket.in('website').emit("res-client-list", JSON.stringify(clients.esp)); // update client-tables on websites.
      }
    }
  });



  // Sends list of all esp clients to requester
  socket.on("req-client-list", (_) => {
    socket.emit("res-client-list", JSON.stringify(clients.esp));
  });

  // Tell specific ESP-client to start/stop Servo
  socket.on("maintenance", (state) => {
    state = state.split("#");  // [name, state]
    let id = getClientId(state[0]);
    io.to(id).emit("maintenance", state[1]);
    console.log(`[MAINTENANCE] set ${state[0]}'s servo to: ${state[1]}`);
  });

  // A client asks for data from specific client
  socket.on('req-data-full', (client_name) => {
    console.log(`[REQ-DATA-FULL] a website requested data from client ${client_name}`)
    let client_id = getClientId(client_name);

    fb.getLogDataFromFirebase(client_name)
      .then(container => {
        socket.emit('res-data-log', JSON.stringify(container));
      })
      .catch(error => {
        console.log('[ERROR] error when getting log data from firebase');
        console.log(error);
      });

    fb.getBarchartDataFromFirebase(client_name)
      .then(y_axis => {
        let x_axis = time.getListOfWeekdays();
        let barchartData = {x_axis: x_axis, y_axis: y_axis}
        socket.emit('res-data-barchart', JSON.stringify(barchartData));
      })
      .catch((error) => {
        console.log('[ERROR] error when getting barchart data from firebase');
        console.log(error);
      })

    fb.getLinechartDataFromFirebase(client_name)
      .then((linechartData) => {
        fb.fillLinechartData(linechartData);
        socket.emit('res-data-linechart', JSON.stringify(linechartData));
      })
      .catch((error) => {
        console.log('[ERROR] error when getting linechart data from firebase');
        console.log(error);
      })

  });

  // Got data from ESP
  socket.on('res-data', (data) => {             // Takes the data from esp and broadcasts
    let client_name = getClientName(socket.id);
    data = time.wrapDataWithClockAndDate(data);
    console.log(`[NEW DATA] new data from ${client_name}: ${data}`);
    fb.writeEspData(client_name, data);
    socket.in('website').emit('data->website', `${data}#${client_name}`);
  });
});
