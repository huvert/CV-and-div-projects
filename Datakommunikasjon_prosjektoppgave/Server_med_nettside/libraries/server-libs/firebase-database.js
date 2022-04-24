// ============   FIREBASE  =============
var firebase = require('firebase');
var { getDate, getClock } = require('./timelib.js');

// Your web app's Firebase configuration
var firebaseConfig = {
   apiKey: "AIzaSyDPv6NqXMvrr-mRGHIvKU9XbgzimMu_PLg",
   authDomain: "raspbarry-pi---gruppe-6.firebaseapp.com",
   databaseURL: "https://raspbarry-pi---gruppe-6.firebaseio.com",
   projectId: "raspbarry-pi---gruppe-6",
   storageBucket: "raspbarry-pi---gruppe-6.appspot.com",
   messagingSenderId: "988917312975",
   appId: "1:988917312975:web:858f19591cd6732d2bb6ba"
};

// Initialize Firebase
firebase.initializeApp(firebaseConfig);
var db = firebase.database();


function writeEspData(client_name, espData) {
  var data = espData;
  var parse =data.split('#');

  db.ref('ESP32-Data/' + client_name).push({
    date: parse[3]+'-'+parse[2]+'-'+parse[1],
    time: parse[4],
    nivå: Number(parse[0]),
  });
}



// ===    Fetching data from firebase     ===
// ---                AND                 ---
// ===   Process data into right format   ===

function getLogDataFromFirebase(client_name) {
  return new Promise((res, rej) => {
    let endDate = getDate();
    let startDate = getDate(2);
    var container = [];
    var ref = db.ref('ESP32-Data/' + client_name);

    ref
      .orderByChild("date")
      .startAt(startDate)
      .endAt(endDate)
      .on("child_added", function(snapshot) {
        container.unshift({date: snapshot.val().date, time: snapshot.val().time, reading: snapshot.val().nivå});
        res(container);
      });
  });
}

function getBarchartDataFromFirebase(client_name) {
  return new Promise((res, rej) => {
    var y_axis = [0,0,0,0,0,0,0,0];
    var ref = db.ref('ESP32-Data/' + client_name);

    // Loops over 8 last days
    // 1. (in loop) Reads number of data readings per day
    // 2. (in loop) Sorts this data into y_axis list
    // 3. Place x_axis and y_axis into object and send to website.
    for (let i=0; i<8; i++) {
      let day = getDate(i);
      var promise = new Promise((res, rej) => {
        ref
          .orderByChild("date")
          .startAt(day)
          .endAt(day)
          .on("value", function(snapshot) {
            let number_of_readings = snapshot.numChildren();
            res(number_of_readings);
          });
      })

      promise
        .then((n) => {
          y_axis[7-i] = n;
          if (i === 7) {
            res(y_axis);
          }
        })
        .catch((error) => {
          console.log('[ERROR]');
          console.log(error);
        })
      }
  });
}


function getLinechartDataFromFirebase(client_name) {
  return new Promise((res, rej) => {
    let startDate = getDate(1);
    let endDate = getDate();
    let clock = getClock();
    let x_axis = _formatXaxis(clock);
    var y_axis = [NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,
                  NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,
                  NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,
                  NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,];

    var ref = db.ref('ESP32-Data/' + client_name);
    ref
      .orderByChild("date")
      .startAt(startDate)
      .endAt(endDate)
      .on("child_added", function(snapshot) {

        // Its yesterday after start time OR today
        if ((snapshot.val().date == startDate && snapshot.val().time > clock) || snapshot.val().date === endDate) {
          let index = _getIndex(snapshot.val().time, clock);
          y_axis[index] = snapshot.val().nivå;
        }
        res({x_axis: x_axis, y_axis: y_axis});
      });
  });
}

// Finds the proper index for the data of linechart.
// x_axis has a steplength of 15minutes.
// Equation:
//    Definitions: yh = yesterday hours, ymin = yesterday minutes
//                 h  = todays hours,    min  = todays minutes
//    If yesterday: Index = 95 - 4*(24-yh+h) + ( (ymin-(ymin%15))/15 - (min-(min%15))/15 )
//    If today:     Index = 95 - 4*(h-yh)    + ( (ymin-(ymin%15))/15 - (min-(min%15))/15 )
function _getIndex(time, current_time) {
  time = time.split(":");                   // [hours, minutes]
  current_time = current_time.split(":");   // [hours, minutes]
  time[0] = parseInt(time[0]);
  time[1] = parseInt(time[1]);
  current_time[0] = parseInt(current_time[0]);
  current_time[1] = parseInt(current_time[1]);

  // Check if data is from yesterday or today
  if (time[0] > current_time[0]) {
    time[0] = 24 - time[0] + current_time[0];
  }
  else {
    time[0] = current_time[0] - time[0];
  }
  time[1] = (time[1]-(time[1]%15))/15 - (current_time[1]-(current_time[1]%15))/15;
  return 95 - (time[0]*4) + time[1]
}

function _formatXaxis(current_clock) {   // in format: "h:min:sek"
  let clock = current_clock.split(":"); // in format: [h, min, sek]
  let n = Math.floor(clock[1]/15);
  let hour = `${clock[0]}:00`
  let x_axis = ['00:00','','','','01:00','','','','02:00','','','','03:00','','','','04:00',
                '','','','05:00','','','','06:00','','','','07:00','','','','08:00','','','',
                '09:00','','','','10:00','','','','11:00','','','','12:00','','','','13:00',
                '','','','14:00','','','','15:00','','','','16:00','','','','17:00','','','',
                '18:00','','','','19:00','','','','20:00','','','','21:00','','','','22:00',
                '','','','23:00','','',''];
  // Uses for loops instead of while to prevent it from getting stuck due to
  // unforseen bugs.
  for (let i=0; i<100; i++) {
    _pushAndShift(x_axis);
    if (x_axis[0] === hour) {
      for (let ii=0; ii<n+1; ii++) {
        _pushAndShift(x_axis);
      }
      break;
    }
  }
  return x_axis
}

function fillLinechartData(linechartData) {
  // Find first reading
  let prev_val;
  linechartData.y_axis.forEach(data => {
    if (!Number.isNaN(data) && prev_val === undefined) {
      prev_val = data;
    }
  });

  for (let i=0; i<linechartData.y_axis.length; i++) {
    if (Number.isNaN(linechartData.y_axis[i])) {
      linechartData.y_axis[i] = prev_val;
    }
    else {
      prev_val = linechartData.y_axis[i];
    }
  }
}

function _pushAndShift(list) {
  list.push(list[0]);
  list.shift();
  return list
}

module.exports = {
  writeEspData,
  getLogDataFromFirebase,
  getBarchartDataFromFirebase,
  getLinechartDataFromFirebase,
  fillLinechartData
};
