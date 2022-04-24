
====  Setting up Server   ====
1. Download Node.js if you dont have it.
   See https://nodejs.org/en/download/

2. (Optional) download Hyper terminal.
   See: https://hyper.is/

3. Go to to projectfolder and find config.js
   Path: '.../Server_med_nettside/public/config.js'.
   Set the following to paramters to match your system.
   const PORT = port;
   const HOST = "http://your_server_ip:port";

4. (with Hyper):
   Go to folder: '.../Server_med_nettside'
   right click on folder and open a Hyper terminal here.

   (with windows CMD):
   Open windows CMD.
   Manually navigate to folder: '.../Server_med_nettside'

5. Type into terminal: "node server.js" (without the "")
   The server should now be running.
   Your website should now be running on http://your_server_ip:port
   Use a browser to test this.

6. Go to server lib and open firebase-database.js.
   Path: '.../libraries/server-libs/firebase-database.js'
   change the firebase config to your dedicated firebase real-time
   database config.
   (Code snippet found in firebase settings -> webapp -> CDN)


====  Using testclients   ====
You can test communication using testclients.
Same as with server: use terminal to run clients with node.
ex: "node client5.js"
