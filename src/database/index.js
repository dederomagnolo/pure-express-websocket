const mongoose = require('mongoose');
mongoose.Promise = global.Promise;

const env = process.env.HEROKU_ENV === "production"
// Database connection
mongoose.connect(
  env
    ? process.env.DB_CONNECTION_PROD
    : process.env.DB_CONNECTION_DEV,
  { 
    useUnifiedTopology: true, 
    useNewUrlParser: true 
  },
  (err) => {
    return err 
      ? console.log(`cannot connect to db: ${err}`) 
      : console.log(`Connected to ${env ? env : 'dev'} db`);
  }
);

module.exports = mongoose;