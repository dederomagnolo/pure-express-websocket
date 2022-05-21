const dotenv = require("dotenv");
const express = require("express");
const bodyParser = require("body-parser");
const { Server } = require("ws");
const WebSocket = require("ws");
const moment = require("moment");
const http = require("http");
const cors = require("cors");
const tz = require("moment-timezone");
const _ = require("lodash");

dotenv.config();

const app = express();
app.use(express.json());
app.use(bodyParser.urlencoded({ extended: false }));
app.use(cors());

app.get("/", (req, res) => {
  res.send("BeThere - Pure express websocket");
});

const server = http.createServer(app);

const wss = new Server({ server });

const heartbeat = (client) => {
	console.log("got pong from client")
  client.isAlive = true;
}

wss.on("connection", async (ws, req) => {
	console.log("client received")
	ws.isAlive = true;
  ws.on('pong', heartbeat);
})

const sendPing = () => {
	wss.clients.forEach((client) => {
		if (client.isAlive === false) 
			return client.terminate();
		client.isAlive = false;
		client.ping();
	});
}

const interval = setInterval(() => sendPing(), 30000);

wss.on('close', function close() {
  clearInterval(interval);
});

const port = 8080;

server.listen(process.env.PORT || port, () => {
  console.log(`Server running in the port ${port}`);
});


