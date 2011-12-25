/*
 *	setup for parannus
 *	creates admin user and imports pkgs
 */

var common = require('./models/common.js'),
	config = require('./config.js'),
	db = require('./lib/db.js'),
	os = require('os'),
	setup = require("./lib/setup.js");


var system_id = "SYSID" + os.type() + os.release() + os.totalmem() + os.hostname();
console.log(system_id);

var an = require("./lib/an.js");

db.store_ht("lamekey","mycontnet",function(err,data){
  console.log("calling get for:",data);
  db.get_ht("lamekey", function(err,data) {
    console.log("got:",data);
  });
});


/*db.store("lamekey","mycontnet",function(err,data){
  console.log("calling get for:",data.id);
  db.get(data.id, function(err,data) {
    console.log("got:",data);
  });
});*/


setup.UserCreation(function(err,user){
		if(err)
			throw new Error(err);
		console.log("user created");
		userid = user._id;
		setup.ScanLocalPackages(function(err,mlid){
			console.log("packages scaned");
			db.get(system_id+"masterlist", function(err,res) {
				
					db.store(system_id+"masterlist",mlid, function(err){
						if(err)
							throw new Error("could not save settings");

							setup.createDefaultRepo(mlid,function(err,res){
								if(err)
									throw new Error("could not create a repo");

							});
					});

				}
			);
		});
	});


