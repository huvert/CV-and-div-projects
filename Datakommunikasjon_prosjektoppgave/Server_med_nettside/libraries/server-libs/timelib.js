/*
  This library is made specific for managing time and
  wrapping data with timestamps etc.
  NOTE: this is not a library that you can import and use in any project.
        We split the project into libraries to easier keep track on files and
        functions.
*/

var date = new Date();

function _updateTime() {
  date = new Date();
}

function getClock() {
  _updateTime();
  let hours = date.getHours();
  let minutes = date.getMinutes();
  let seconds = date.getSeconds();
  hours = hours < 10 ? '0'+hours : hours;
  minutes = minutes < 10 ? '0'+minutes : minutes;
  seconds = seconds < 10 ? '0'+seconds : seconds;
  return `${hours}:${minutes}:${seconds}`  // format: 07:02:01
}

function getDate(days_from_today=0) {
  _updateTime();
  return `${date.getYear()+1900}-${date.getMonth()+1}-${date.getDate()-days_from_today}`
}

function wrapDataWithClock(data) {
  data = data.toString();
  return `${data}#${getClock()}`
}

function wrapDataWithDate(data) {
  data = data.toString();
  let dato = date.getDate();
  let month = date.getMonth()+1;
  dato = dato < 10 ? '0'+dato : dato;
  month = month < 10 ? '0'+month : month;
  return `${data}#${dato}#${month}#${date.getYear()+1900}`
}

function wrapDataWithClockAndDate(data) {   // returns: data#day#month#year#05:20:40
  return `${wrapDataWithDate(data)}#${getClock()}`
}

function getListOfWeekdays() {
  _updateTime();
  let weekdays = ['M','Ti','O','T','F','L','S'];    // 'Today'
  let weekday = date.getDay();
  for (let i=1; i<weekday; i++) {
    _pushAndShift(weekdays);
  }
  weekdays.push('Today');
  return weekdays
}

function _pushAndShift(list) {
  list.push(list[0]);
  list.shift();
  return list
}


module.exports = {
  getClock,
  getDate,
  wrapDataWithClockAndDate,
  getListOfWeekdays,
};
