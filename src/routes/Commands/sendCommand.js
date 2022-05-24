function sendCommandToClient (router) {
  router.post("/send", async function (req, res) {

    console.log("aq")
    console.log(req.wss)
    const { userId, deviceId } = req.body;
  
    const user = await User.findOne({ _id: userId });
    if (!user)
      return res.send({ error: true, message: "Usuário não encontrado" });
  
    // check if the deviceId requested to send command exists
    const device = await Device.findOne({ _id: deviceId });
  
    if (!device)
      return res.send({ error: true, message: "Dispositivo não encontrado" });
    
    // array of ids assigned to that user
    const userDevices = _.get(user, "devices"); 
  
    // check if the device is under user devices collection
    let isDeviceFromThisUser;
    if (userDevices.length > 0) {
      isDeviceFromThisUser = userDevices.indexOf(deviceId);
    }
  
    if (isDeviceFromThisUser === -1)
      return res.send({ true: true, message: "Operação não autorizada" });
  
    // serial key from the localize the device in this context on ws clients
    const deviceSerialKey = _.get(device, "deviceSerialKey");
    const command = await Command.create(req.body);
  
    // wss.clients.forEach((client) => {
    //   if (client.readyState === WebSocket.OPEN) {
    //     // send command only for the client requested
    //     if (deviceSerialKey === client.id) {
    //       client.send(req.body.commandName);
    //       if (req.body.commandName === "SETTINGS") {
    //         client.send(req.body.value);
    //       }
  
    //       if (req.body.commandName === "MP0") {
    //         client.send("WR_PUMP_OFF");
    //       }
    //     }
    //   }
    // });
    res.send(command);
  });  
}

module.exports = sendCommandToClient;