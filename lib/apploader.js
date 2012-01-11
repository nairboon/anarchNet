var	db = require('./db.js'),
	filestore = require('./filestore.js'),
	util = require('./util.js'),
	ppm = require('./ppm.js');

var defaultRepo = 0;



exports._loadApp = function (appid,repoid,res,next) {
	console.log("loading cache:",appid);

	ppm.appcacheLoad(appid,repoid,function(err,ac){
		if(err) {
			console.log('appcacheLoad fails');

			ppm.updateRepo(repoid,function(err,repores){
				if(err)
					return next(new Error('Could not update repo: ' + err));

				ppm.cacheApp(appid,repores,function(err,appres){
					if(err || !appres)
						return	next( new Error('Could not cache that app: '+err.message));

				//	console.log("sending",appres);
					res.send(appres); // content of appcache
				});
			});
		}
		else {
			console.log("send cache");
			res.send(ac); // cache hit
		}
	});
};

exports.getRepo = function(req){

	var repoid = util.ids.defaultRepo;

	if(req.session.user != undefined && req.session.user.userRepo != undefined) {
		console.log("using private repo");
		repoid = req.session.user.userRepo;
	}
	return repoid;
};

exports.loadApp = function (req,res,next) {
	var appid = req.params.id.split('.')[0];

	exports._loadApp(appid,exports.getRepo(req),res,next);
};
exports.loadLib = function (req,res) {
	var name = req.params.id.split('.')[0];
	console.log("load lib",name);
	res.contentType('application/javascript');
	var repoid = req.session.user.defaultRepo;
	ppm.getPackage(name,repoid,function(err,code){
		if(err || !code)
			res.send(new Error("lib not found!"));
		else
			res.send(code.code);
	});
};

exports.loadResource = function(req,res) {
	var app = req.params[0].split('/')[0];
	var resource = req.params[0].replace(app+"/","");
	var suffix = req.params[0].split('.').pop();

	console.log("app:",app);
	console.log("res:",resource);
	var repoid = exports.getRepo(req);
	ppm.getPackage(app,repoid,function(err,gpres){
		if(err || gpres === undefined) {
			res.send(new Error("app not found!:"+err ));
			return;
		}
		console.log(gpres);
		
		if(!gpres.resources)
			res.send(new Error("no resources for that app"));

		ppm.getMap(resource,app,function(err,mcres) {
			if(err || !mcres)
				return res.send(new Error("invalid url mapping"));
			if(mcres.filestore) {
				filestore.getFile(mcres.value,req,res);
			}
			else {
			db.getHead(mcres.value,function(err,nres){
				if(err || !nres)
					res.send(new Error("file not found"));

				res.contentType(util.mime.type[suffix]);
				console.log("contenttype:",util.mime.type[suffix]);
				res.send(nres.content);
			});
		}
		});
	});
};

exports.loadCode = function(req,res) {
	var app = req.params.id;
	var package = req.params.package;
	res.contentType(util.mime.type["js"]);
console.log("requesting code for",app,package);
	if(package === "main") { // wants not deps, but main exec
		ppm.getPackage(app,exports.getRepo(req),function(err,pres){
			if(err || !pres)
				res.send(new Error("could not load code"));
			res.send(pres.code);
		});
	}
	else {
		ppm.getPackage(package,exports.getRepo(req),function(err,pres){
			if(err || !pres)
				res.send(new Error("could not load code"));
			res.send(pres.code);
		});
	}
};


	if (!defaultRepo) {
	  db.get(util.system_id + 'defaultRepo', function(err,res) {
				if(!res) {
				  console.log("no defaultRepo!!");
				} else {// update
					defaultRepo = res;
					console.log("default repo: ", res);
				}
	  });
	}

