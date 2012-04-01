/*
 *	setup for parannus
 *	creates admin user and imports pkgs
 */

var util = require('./lib/util.js'),
	config = require('./config.js'),
	db = require('./lib/db.js'),
	ppm = require('./lib/ppm.js'),
	setup = require("./lib/setup.js");

var an = require("./lib/an.js");

console.log("scan packages...");
setup.ScanLocalPackages(function(err,mlid){
	console.log("packages scaned");
		//console.log("storing master list...",mlid);
			setup.createDefaultRepo(mlid,function(err,res){
				if(err)
					throw err;
				console.log("repo created!",res);
				
				/*ppm.updateRepo(res,function(err,repores){
				if(err)
					return next(new Error('Could not update repo: ' + err));
				*/ppm.cacheApp("user",res,function(err,appres){
					if(err || !appres)
						return	next( new Error('Could not cache that app: '+err.message));
					db.close();
				});
			
			});
});





