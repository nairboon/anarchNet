/*
 *	setup for parannus
 *	creates admin user and imports pkgs
 */

var util = require('./lib/util.js'),
	config = require('./config.js'),
	db = require('./lib/db.js'),
	setup = require("./lib/setup.js");



console.log(util.system_id);

var an = require("./lib/an.js");

/*db.store_ht("lamekey","mycontnet",function(err,data){
  console.log("calling get for:",data);
  db.get_ht("lamekey", function(err,data) {
    console.log("got:",data);
  });
});


db.store("lamekey","mycontnet",function(err,data){
  console.log("calling get for:",data.id);
  db.get(data.id, function(err,data) {
    console.log("got:",data);
  });
});
*/

var filestore = require('./lib/filestore.js');
filestore.storeFile("config.js","heheh",function(err){
console.log(err);
});

console.log("scan packages...");
setup.ScanLocalPackages(function(err,mlid){
	console.log("packages scaned");
	db.get(util.system_id+"masterlist", function(err,res) {
		console.log("storing master list...");
		db.store(util.system_id+"masterlist",mlid, function(err){
			if(err)
				throw new Error("could not save settings");

			setup.createDefaultRepo(mlid,function(err,res){
				if(err)
					throw new Error("could not create a repo");

			});
		});

	});
});

