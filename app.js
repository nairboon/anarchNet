/**
 * Parannus
 * http://rbose.org/wiki/Parannus
 */

var express = require('express'),
	mongoose = require('mongoose'),
	MongoStore = require('connect-mongo'),
	resource = require('express-resource'),
	namespace = require('express-namespace'),
	dp = require('./db.js'),
	editor = require('./editor.js'),
	auth = require('./auth.js'),
	apploader = require('./pkg/apploader.js'),
	config = require('./config.js').conf;
	
var db = new dp.db();
mongoose.connect(config.dburl);
var app = module.exports = express.createServer();

app.configure('development', function(){
  app.use(express.errorHandler({ dumpExceptions: true, showStack: true })); 
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
  app.use(express.session({ secret: config.sessionsecret, store: new MongoStore({db: config.db}) }));
  app.use(express.compiler({ src: __dirname + '/public', enable: ['less'] }));
  app.use(app.router);
  app.use(express.static(__dirname + '/public'));
});


app.get('/', function(req, res){  
  res.render('index', {
    title: 'Parannus',
    session: req.session
  });
});

app.get('/app/:id', apploader.load);

app.get('/rawdata/:id/:branch?/:rev?', function(req, res){
	db.get(req.params.id,req.params.branch,req.params.rev,true,function(err,data){
		res.send(data);
	});
});

app.get('/data/:id?/:branch?/:rev?', function(req, res,next){
	var id = req.params.id;
	if(id)
		db.get(id,req.params.branch,req.params.rev,false,function(err,data){
			res.send(data.content);
		});
	else
		next();
});

app.error(function(err, req, res){
  /*res.render('500.jade', {
     error: err
  });*/
  res.send(err);
});

app.namespace('/edit',function(){
	app.get('list',editor.list);
	app.get(':id/show', editor.show);
	app.get(':id/edit', editor.edit);
	app.get('new', editor.new);
	app.post('create', editor.create);
	app.post(':id/update', editor.update);
	app.get(':id/del', editor.destroy);
});

app.namespace('/auth', function() {	
	app.get('login',auth.login);
	app.get('register',auth.register);
	app.post('register',auth.register_form,auth.register_post);
	app.post('login',auth.login_form,auth.login_post);
	app.get('logout',auth.logout);
});

if (!module.parent) {
  app.listen(config.port);
  console.log("Express server listening on port %d", app.address().port);
}
