const fs = require('fs');
const jsonServer = require('json-server')
const app = jsonServer.create()
const router = jsonServer.router('db.json')
const https = require('https')

var options = {
  key: fs.readFileSync('./key.pem'),
  cert: fs.readFileSync('./cert.pem'),
  passphrase: 'asteroidsbsidesfun'
};

// /!\ Bind the router db to the app
app.db = router.db

const middlewares = jsonServer.defaults()

// Set default middlewares (logger, static, cors and no-cache)
app.use(middlewares)

// You must apply the auth middleware before the router
const auth = require('json-server-auth')

const rules = auth.rewriter({
  // Permission rules
  users: 666,
  asteroids: 666
})

// You must apply the middlewares in the following order
app.use(rules)
app.use(auth)
app.use(router)
app.use(function(req, res, next) {

  res.header("Access-Control-Allow-Origin", "*");
  res.header("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");

  next();

});

const PORT = process.env.PORT || 3000
app.listen(PORT, () => {
  console.log(`Run Auth API Server on http://localhost:${PORT}/`)
})

https.createServer(options, app).listen(3002, function() {
  console.log("json-server started on port " + 3002);
});
