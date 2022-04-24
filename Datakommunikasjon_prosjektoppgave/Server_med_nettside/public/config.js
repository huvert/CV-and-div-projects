/**
  This file makes it possible to change some parameters that are used in
  different parts of the project by just editing this one file.
*/
const PORT = 4000;
const HOST = "http://192.168.1.14:4000";         // Huverts ip: 192.168.1.15 Tomas IP: "http://82.164.75.155:4000"
const maxDataTableSize = 20; // How many sensor-readings the website should hold.



// If statement avoids ugly error-messages on website
if (typeof(module) !== 'undefined') {
  module.exports = {
    PORT,
    HOST,
    maxDataTableSize
  };
}
