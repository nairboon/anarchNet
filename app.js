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
	config = require('./config.js').conf;
	
var db = new dp();
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

app.get('/rawdata/:id/:rev?', function(req, res){
	db.getData(req.params.id,req.params.rev,function(data){
		res.send(data.content);
	});
});

app.get('/data/:id?/:rev?', function(req, res,next){
	var id = req.params.id;
	console.log(db);
	if(id)
		db.getData(id,req.params.rev,function(data){
			res.send(data);
		});
	else
		next();
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
