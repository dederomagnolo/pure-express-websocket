module.exports = (wss) => {
  return (req, res, next) => {
    req.wss = wss
    return next();
  }
}
