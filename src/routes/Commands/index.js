const router = require('express').Router();
const SendCommandToClient= require('./sendCommand');

SendCommandToClient(router);

module.exports = router;
