"use strict";

var prev_data_reading;       // Stores last read value from sensor
var selected_client;         // Stores selected client from client list

var clients = [];            // Stores clients by names (not ID)
var dataTable = [];          // Stores data log


// ===    Global Functions    ===
/* data should come in format: "int#str#str#str ..." */
function handleData(data) {
  let cnt = data.split("#");
  cnt[0] = parseInt(cnt[0]);
  prev_data_reading = cnt[0];
  return cnt;
}


// ===    Functions for Log (logg tab on navbar)   ===
var log = {
  indexCounter: 0,                 // Keeps track on # of indexes

  loadTable: function(dataTable) {
    prev_data_reading = dataTable[0].reading; // Update global variable
    let dataHtml = '';
    for(let data of dataTable) {
      log.indexCounter = log.indexCounter+1;
      dataHtml += log.formatData(data);
    }
    $("#data-log-table").html(dataHtml);
  },

  formatData: function(data) {
    let formated_data = (`<tr id="index${log.indexCounter}">
                          <td>${data.date}</td>
                          <td>${data.time}</td>
                          <td>${data.reading}%</td></tr>`);
    return formated_data
  },

  update: function(data) {
    // input format:      [int, "day", "month", "year", "clock"]
    // Convert to format: {date: "", time: "", reading: int}
    //    1. Formats data
    //    2. Updates logg
    //    3. Updates linechart
    let formatedData = {date: `${data[3]}/${data[2]}/${data[1]}`, time: `${data[4]}`, reading: data[0]};
    let dataHtml = log.formatData(formatedData);
    $("#data-log-table").prepend(dataHtml);
    dataTable.push(formatedData);
    log.indexCounter = log.indexCounter+1;
    if(maxDataTableSize < dataTable.length+1) {   // Remove first element of object and table.
      $("#data-log-table tr:last-child").remove();
      $(`#index${log.indexCounter-maxDataTableSize}`).remove();
    }
  }
};




// ====================================
// ===          Socket IO           ===
$(function() {          // Waits for document to fully load before executing this block
  var socket = io.connect();
  socket.emit("join-room", "website");  // Request to join room: website

  // Server has given us list of ESP clients
  socket.on("res-client-list", (clients_list) => {
    console.log("[RES-CLIENT-LIST] received list of clients");
    clients = JSON.parse(clients_list);
    updateClientLogTable();
  });

  // Start/stop servo motor for the selected client
  $('#stop-button').click(() =>  {
    socket.emit("maintenance", `${selected_client}#0`);
  });

  $('#start-button').click(() => {
    socket.emit("maintenance", `${selected_client}#1`);
  });

  /* Nest the new data into existing data
     This inclues:
        1. Adding new element to logg
        2. Add new count to barchart ("Today")
        3. Update last value on linechart. (This will only update y-axis)
  */
  socket.on("data->website", (data) => {  //in format: "måling#day#month#year#clock#client_name"
    let cnt = handleData(data);           //in format: ["måling","day","month","year","clock","client_name"]
    // Only handle data if from selceted client
    if (cnt[cnt.length-1] === selected_client) {
      console.log(`[RECEIVED-DATA] from ${selected_client}: ${data}`);
      cnt.pop();
      log.update(cnt);
      updateBarChart();
      updateLineChartData(prev_data_reading);     // Update linechart
    }
  });

  // Server has given us a new datalog for the selected client
  socket.on('res-data-log', (data) => {
    console.log("[RES-DATA-LOG] recieved data log");
    data = JSON.parse(data);
    log.loadTable(data);
  });

  // Server has given us a new barchart for the selected client
  socket.on('res-data-barchart', (data) => {
    data = JSON.parse(data);
    loadBarChart(data);
  });

  // Server has given us a new linechart for the selected client
  socket.on('res-data-linechart', (data) => {
    data = JSON.parse(data);
    loadLineChart(data);
  });


  // ===   Functions for KLIENTER table on front page   ===

  // Full update of client-log-table on front page
  function updateClientLogTable() {
    $("#client-log-table").html("");
    let dataHtml = '';
    let len = clients.length;
    for (let i=0; i<len; ++i) {
      let client = clients[i];
      dataHtml = `${dataHtml}<tr><td>${client.clientName}</td></tr>`;
    }
    $("#client-log-table").html(dataHtml);
    makeTableElementsClickable();
  }

  // Each new element added to KLIENTER list needs to be clickable
  function makeTableElementsClickable() {
    var tds = document.querySelectorAll('#client-log-table tr td');
    tds.forEach(function(td){
      td.addEventListener('click', handleTDClick);
    });
  };

  /*
    Clicking on a client inside KLIENTER table, sends request to server
    that all charts + log has to be updated according to clients ID (== socket.id on server)
  */
  function handleTDClick() {
    let client_name = $(this).html();
    if (client_name !== selected_client) {
      selected_client = client_name;
      // Visuals
      $("#client-log-table>tr>td.selected").removeClass("selected");
      $(this).addClass("selected");
      $("#servo-btn-panel").show();
      // Request data from server
      socket.emit("req-data-full", client_name);
      // Store selected client to cookie (This will remember which client you had selected)
      document.cookie = `selected_client=${client_name}`;
    }
  };
});
