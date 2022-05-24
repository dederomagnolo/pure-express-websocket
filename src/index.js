const dotenv = require('dotenv');
const express = require('express');
const bodyParser = require('body-parser');
const { Server } = require('ws');
const WebSocket = require('ws');
const moment = require('moment');
const http = require('http');
const cors = require('cors');
const tz = require('moment-timezone');
const _ = require('lodash');

dotenv.config();

// import models
const User = require("./models/user");
const Device = require("./models/device");

const { generateToken, checkNewClientConnection} = require('./utils');
const wssMiddleware = require('./middlewares/wss');
const commandsRoute = require('./routes/Commands');

const app = express();
app.use(express.json());
app.use(bodyParser.urlencoded({ extended: false }));
app.use(cors());

const server = http.createServer(app);

const wss = new Server({ 
	verifyClient: async (info, done) => checkNewClientConnection(info, done),
	server 
});

// clients middleware to pass clients to router context
app.use(wssMiddleware(wss));
app.use('/api/commands', commandsRoute);


// to renew client state
const heartbeat = (client) => {
	console.log(`time#${moment().tz('America/Sao_Paulo').format('HH:mm')}$message#PONG FROM SERVER`)
  client.isAlive = true;
}

const sendPing = () => {
	wss.clients.forEach((client) => {
		if (client.isAlive === false) {
			console.log('found a dead connection')
			return client.terminate();
		}
		client.ping(`time#${moment().tz('America/Sao_Paulo').format('HH:mm')}$message#PING FROM SERVER`);
	});
}

const interval = setInterval(
	() => sendPing()
, 20000);

// websocket connection handler
wss.on('connection', async (ws, req) => {
	console.log(`Connected clients: ${wss.clients.size}`);
	// const headers = req.headers;
	// console.log(headers)
	ws.isAlive = true;
	sendPing();
  ws.on('pong', heartbeat);
	ws.on('close', () => {
		ws.isAlive = false;
		console.log(`Connected clients: ${wss.clients.size}`)
		console.log('client disconnected');
	})
})

wss.on('close', function close() {
  clearInterval(interval);
});


const port = 8080;

server.listen(process.env.PORT || port, () => {
  console.log(`Server running in the port ${port}`);
});

app.get('/', (req, res) => {
  res.send('BeThere - Pure express websocket');
});
