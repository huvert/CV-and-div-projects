const io = require("socket.io-client");
const { port, HOST } = require("./../public/config.js");

const name = "Torgeir";

let socket = io.connect(HOST);     // Creates websocket connection to server

socket.emit("join-room", "esp#"+name);  // Request to join room: website

socket.on("data->website", (data) => {
  console.log("[data S->C] "+data);
});
socket.on("website", (data) => {
  console.log("data: ", data);
});
socket.on("maintenance", (data) => {
  console.log(data);
})

// TEST
socket.on('req-data', (data) => {
  // GET DATA FROM ESP HERE
  console.log("get-data --> data from esp")
  socket.emit("res-data", "4");
});

// My local ip address:   "http://192.168.1.12:4000"
