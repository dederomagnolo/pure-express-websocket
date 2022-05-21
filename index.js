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

// to renew client state
const heartbeat = (client) => {
	console.log(`time#${moment().tz("America/Sao_Paulo").format("HH:mm")}$message#PONG FROM SERVER`)
  client.isAlive = true;
}

wss.on("connection", async (ws, req) => {
	const headers = req.headers;
	console.log(headers)
	console.log("new client connected");
	ws.isAlive = true;
  ws.on('pong', heartbeat);
	ws.on('ping', () => console.log("ping"));
	ws.on('close', () => {
		ws.isAlive = false;
		console.log("client disconnected");
	})

	ws.on('open', () => ws.send('hey you'));
})

wss.on("close", () => {
	console.log("disconnected")
})

const sendPing = () => {
	wss.clients.forEach((client) => {
		if (client.isAlive === false) {
			console.log("found a dead connection")
			return client.terminate();
		}
		client.ping(`time#${moment().tz("America/Sao_Paulo").format("HH:mm")}$message#PING FROM SERVER`);
	});
}

const interval = setInterval(
	() => sendPing()
, 30000);

wss.on('close', function close() {
  clearInterval(interval);
});

const port = 8080;

server.listen(process.env.PORT || port, () => {
  console.log(`Server running in the port ${port}`);
});


