var	db = require('./db.js'),
	filestore = require('./filestore.js'),
	ppm = require('./ppm.js');

exports._loadApp = function (appid,repoid,res,next) {
	console.log("loading cache:",appid);

	ppm.appcacheLoad(appid,repoid,function(err,ac){
		if(err) {
			console.log('appcacheLoad fails');
		
			ppm.updateRepo(repoid,function(err,repores){
				if(err)
					return next(new Error('Could not update repo: ' + err));

				ppm.cacheApp(appid,repores._id,function(err,appres){
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
	var repoid = req.session.user.defaultRepo;

	if(req.session.user.userRepo) {
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
	var mimes = {
		"css" : "text/css",
		"jpg" : "image/jpeg",
		"png" : "image/png"
	};
		
	console.log("app:",app);
	console.log("res:",resource);
	var repoid = req.session.user.defaultRepo;
	ppm.getPackage(app,repoid,function(err,gpres){
		if(err || !gpres)
			res.send(new Error("app not found!"));
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
					
				res.contentType(mimes[suffix]);
				console.log("contenttype:",mimes[suffix]);
				res.send(nres.content);
			});
		}
		});		
	});
};

exports.loadCode = function(req,res) {
	var app = req.params.id;
	var package = req.params.package;
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

