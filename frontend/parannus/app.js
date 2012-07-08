/**
 * Parannus
 * http://rbose.org/wiki/Parannus
 * Copyright 2011, 2012 Remo Hertig - Licensed under: GNU Public License 3 or later
 */

var express = require('express'),
	crypto = require('crypto'),
	fs = require("fs"),
	resource = require('express-resource'),
	namespace = require('express-namespace'),
	db = require('./lib/db.js'),
	editor = require('./editor.js'),
	api = require('./lib/api.js'),
	auth = require('./auth.js'),
	util = require('./lib/util.js'),
	apploader = require('./lib/apploader.js'),
//	sharejs = require('share').server,
	config = require('./config.js');


if(config.use_ssl) {
  var options = {
  key: fs.readFileSync(config.ssl_key),
  cert: fs.readFileSync(config.ssl_cert)
};
	var app = module.exports = express.createServer(options);
}
else
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
  app.use(express.session({ secret: config.sessionsecret }));
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

app.get('/app/:id/js/:package.js',apploader.loadCode);

app.get('/app/*', function(req, res,next){
	if(req.params[0].search("index.html") > 0)
		next();
	else	// resource request
		apploader.loadResource(req,res);
});

app.get('/app/:id/index.html', apploader.loadApp);
app.get('/lib/:id', apploader.loadLib);

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
  console.log(err.message);
  res.send(err.message);
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

app.namespace('/api', function() {
	app.get('emergency_shutdown',api.emergency_shutdown);
	app.post('register',api.create_identity_form,api.create_identity);
});

if (!module.parent) {
	db.get(util.system_id + 'masterlist',function(err,res){
		if(err || !res)
			throw new Error("No masterlist, run setup.js!");
	});

  app.listen(config.port);
  console.log("Express server listening on port %d", app.address().port);

	var options = {db: {type: 'none'}};
	//sharejs.attach(app, options);
}
