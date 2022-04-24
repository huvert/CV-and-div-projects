"use strict";

var current_time = new Date();

// ====       Time and Sync Functions        ====
// Returns time untill next quarter (15min) in seconds.
/* Ordinær ligning: 60(15-(Minutes%15)) - Sekunder
   This function is used to sync the graph with your computers time
   */
var timeFuncs = {
  getInitTime: function() {
    current_time = new Date();
    return 60*(15-(current_time.getMinutes()%15)) - current_time.getSeconds()
  },

  // Used to sync Bar Chart
  minutesUntilMidnight: function() {
      var midnight = new Date();
      midnight.setHours( 24 );
      midnight.setMinutes( 0 );
      midnight.setSeconds( 0 );
      midnight.setMilliseconds( 0 );
      return ( midnight.getTime() - new Date().getTime() ) / 1000 / 60;
  },
};


// ====       Functions for all charts      ====
// The 2 following functions can be used on any chart
function addData(chart, label, data) {
    chart.data.labels.push(label);
    chart.data.datasets[0].data.push(data);
    chart.update();
}

function removeData(chart) {
    chart.data.labels.shift();
    chart.data.datasets[0].data.shift();
}

function pushAndShift(list) {
  list.push(list[0]);
  list.shift();
  return list
}



// ========================================
// ===           LineGraph              ===
/*
  X-axis is updated every 15minutes
  Y-axis is updated every 15minutes BUT will edit last index in data list
  when receiving new data pack from server.
*/

var ctx = $('#line-chart')[0].getContext('2d');
var linechart = new Chart(ctx, {
    type: 'line',

    // The data for our dataset
    data: {
        labels: [],
        datasets: [{
            label: 'Soap Dispenser Level [%]',
            backgroundColor: '#0d7377',
            borderColor: '#212121',
            data: []   // len: 24
        }]
    },

    options: {
      responsive: true,
      maintainAspectRatio: false,
      yAxes: [{
        ticks: {
          suggestedMin: 0,
          suggestedMax: 100
        }
      }]
    }
});

// ====       Linechart functions       ====
// Used to label linechart
function getTimestamp() {
  let t = current_time.getHours();
  if (t < 10) {
    return `0${t}:00`
  }
  return `${t}:00`
}

function updateLineChart() {
  let new_label = '';
  current_time = new Date();
  if (current_time.getMinutes() == 0) {   // Update label hver time.
    new_label = getTimestamp();
  }
  removeData(linechart);
  addData(linechart, new_label, prev_data_reading);
}

// When receiving new data from client the linechart should update y-axis
// without updating x-axis.
function updateLineChartData(data) {
  linechart.data.datasets[0].data.pop();
  linechart.data.datasets[0].data.push(data);
  linechart.update();
}

// Lenght of x-axis should be 24 * 4 = 96
function loadLineChart(data) {
  linechart.data.datasets[0].data = data.y_axis;
  linechart.data.labels = data.x_axis;
}

// Update labels and data-axis (hvert kvarter)
setTimeout(() => {
  updateLineChart();
  setInterval(() => {
    updateLineChart();
  }, 15*60*1000);               // Hvert kvarter: 15*60*1000
}, timeFuncs.getInitTime()*1000);




// ========================================
// ===           barChart               ===
/*
  X-axis is updated every day 00:00
  Y-axis is updated every time data is received
*/
var ctx_bar = $('#bar-chart')[0].getContext('2d');
var barchart = new Chart(ctx_bar, {
    type: 'bar',
    data: {
        labels: [],
        datasets: [{
            label: '# trykk per dag [Stk]',
            backgroundColor: ["#003f5c", "#2f4b7c","#665191","#a05195","#d45087","#f95d6a","#ff7c43","#ffa600"],
            data: []
        }]
    },

    options: {
      responsive: true,
      maintainAspectRatio: false,
      scales: {
        yAxes: [{
          ticks: {
            beginAtZero: true
          }
        }]
      }
    },

});


// Update barchart when passing midnight (00:00)
function barChartNewDay() {
  // Update colors
  pushAndShift(barchart.data.datasets[0].backgroundColor);
  // Update label "Today" -> ex: "M".
  barchart.data.labels[barchart.data.labels.length - 1] = barchart.data.labels[0];
  // Update data
  addData(barchart, "Today", 0);
  removeData(barchart);
  barchart.update();
}

// add +1 to Todays value
function updateBarChart() {
  let index = barchart.data.datasets[0].data.length - 1;
  let y = barchart.data.datasets[0].data[index];
  barchart.data.datasets[0].data[index] = y+1;
  barchart.update();
}

// data should arrive in format:
// data = {x_axis: ['M',...'Today'], y_axis: [1,2,3...8]}
function loadBarChart(data) {
  barchart.data.datasets[0].data = data.y_axis;
  barchart.data.labels = data.x_axis;
}

// Barchart update at midnight, with interval every 24hours.
setTimeout(() => {
  barChartNewDay();
  setInterval(() => {
    barChartNewDay();
  }, 24*60*60*1000);
}, timeFuncs.minutesUntilMidnight()*60*1000);
