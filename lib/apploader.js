var	db = require('./db.js'),
	filestore = require('./filestore.js'),
	util = require('./util.js'),
	 fs = require('fs'),
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
		console.log("gp: ",gpres);
		
		if(!gpres.resources) {
			res.send(new Error("no resources for that app"));
			return;
			}

		//ppm.getMap(resource,app,function(err,mcres) {
		var found = false;
		for(var i=0;i<gpres.resources.length;i++)
		{
			if(gpres.resources[i].key != resource)
				continue;
			else
				found = true;
				
			
			if(gpres.resources[i].filestore) {
			db.get_file(gpres.resources[i].value,function(err,fres) {
				console.log("gf: " , err,fres);
				rs = fs.createReadStream(fres.path);
				res.contentType(util.mime.type[suffix]);
				console.log("contenttype:",util.mime.type[suffix]);
				rs.addListener('data', function (b) {
        				res.write(b, 'binary');
    				});
   				 rs.addListener('end', function () { res.end(); })
				
			});
				//filestore.getFile(gpres.resources[i].value,req,res);
			}
			else {
			db.get(gpres.resources[i].value,function(err,nres){
				if(err || !nres)
					res.send(new Error("file not found"));

				res.contentType(util.mime.type[suffix]);
				console.log("contenttype:",util.mime.type[suffix]);
				res.send(nres);
			});
		}
		}
		if(!found)
			return res.send(new Error("resource not found"));
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
			console.log("send: ",app,package);
			res.contentType("js");
			res.send(pres.code);
		});
	}
	else {
		ppm.getPackage(package,exports.getRepo(req),function(err,pres){
			if(err || !pres) {
			console.log(err);
				res.send(err);
				return;
				}
			res.send(pres);
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

