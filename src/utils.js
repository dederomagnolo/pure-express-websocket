const Device = require("./models/device");

function generateToken(params = {}) {
	return jwt.sign(params, authConfig.secret, {
		expiresIn: 86400 // token to expire in one day
	});
}

async function checkNewClientConnection(info, done) {
  const url = info.req.url;
  const splittedUrl = url.split('/');
  console.log(url, splittedUrl)
  const deviceSerialKey = splittedUrl[1]
  console.log('------verify connected client------');

  if(!deviceSerialKey) {
    console.log("Serial key not provided from client.");
    return false;
  }
  const isDeviceSerialKeyValid = await Device.findOne({ deviceSerialKey });
  if(!isDeviceSerialKeyValid) {
    console.log("Serial key not authorized.");
    return false
  };
  console.log("Serial key authorized. Connection Opened.");
  done(info.req);
}

module.exports = { 
  generateToken,
  checkNewClientConnection
}