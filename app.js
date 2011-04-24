/**
 * Parannus
 * http://rbose.org/wiki/Parannus
 */

var express = require('express'),
	mongoose = require('mongoose'),
	MongoStore = require('connect-mongo'),
	resource = require('express-resource'),
	form = require('express-form'),
	filter = form.filter,
	validate = form.validate,
	namespace = require('express-namespace'),
	DataProvider = require('./db.js').DataProvider;
	
var app = module.exports = express.createServer();

// Configuration
app.configure('development', function(){
  app.use(express.errorHandler({ dumpExceptions: true, showStack: true })); 
  app.set('db-url', 'mongodb://localhost/test');
  app.set('servername','rbose.org');
});

app.configure('production', function(){
  app.use(express.errorHandler()); 
});

app.configure(function(){
  app.set('views', __dirname + '/views');
  app.set('view engine', 'jade');
  app.use(express.bodyParser());
  app.use(express.methodOverride());
  app.use(express.cookieParser());
  app.use(express.session({ secret: 'your secret here', store: new MongoStore({db: 'test'}) }));
  app.use(express.compiler({ src: __dirname + '/public', enable: ['less'] }));
  app.use(app.router);
  app.use(express.static(__dirname + '/public'));
});

var db = mongoose.connect(app.set('db-url'));
var dp = new DataProvider();

app.get('/', function(req, res){
  res.render('index', {
    title: 'Parannus',
    session: req.session
  });
});


app.get('/users', function(req, res){
    // do something else
res.send("hallo");
});

app.get('/app/:id', function(req, res){
  res.render('index', {
    title: 'Express'
  });
});

app.get('/rawdata/:id/:rev', function(req, res){
  res.send(dp.getData(req.params.id,req.params.rev).content)
});

app.get('/data/:id?/:rev?', function(req, res,next){
	var id = req.params.id;
	if(id)
		dp.getData(id,req.params.rev,function(data){
			res.send(data);
		});
	else
		next();
});

app.get('/adddata',function(req,res) {
	dp.addData({type:"xx", owner: "ll",content:"hallo world",id:"p"});
	res.send("ok");
});

//app.resource("edit",require("./editor"));

app.namespace('/auth', function() {
	app.get('register',function(req,res){
		res.render('auth/register', {
		    title: 'Register',
			session: req.session
		  });
	});
	app.post('register',
		form(
			filter("username").trim(),
			validate("username").required(),
			validate("password").required(),
			validate("email").required().isEmail()
		),
		function(req,res,next){
			if(!req.form.isValid) {
				console.log(req.form.errors);
			}
			else {
				dp.registerUser(req.form, function(error) {
					if(error)
						console.log(error);
						
					res.send("reg successful");
				});
			}
	});
	app.get('login',function(req,res){
		if(req.session.auth)
			res.redirect("/");
		res.render('auth/login', {
		    title: 'Login',
			session: req.session
		  });
	});
	app.post('login',form(
		validate("username").required(),
		validate("password").required()
		),
		function(req,res){
			dp.authorizeUser(req.form,function(error,r){
				if(!r)
					res.send("login failed");
				else {
					req.session.auth = true;
					req.session.user = r;
				res.send("authenticated");
			}
			});
		
	});
	app.get('logout',function(req,res){
		req.session.dextroy();
		res.render('index', {
		    title: 'logedout'
		  });
	});
});

if (!module.parent) {
  app.listen(3000);
  console.log("Express server listening on port %d", app.address().port);
}
