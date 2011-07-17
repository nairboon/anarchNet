/*
 *	setup for parannus
 *	creates admin user and imports pkgs
 */

var mongoose = require('mongoose'),
	common = require('./models/common.js'),
	config = require('./config.js'),
	setup = require("./lib/setup.js");	

	
mongoose.connect(config.dburl);
Settings  = mongoose.model('settings');	 

	setup.UserCreation(function(err,user){
		if(err)
			throw new Error(err);
		console.log("user created");
		userid = user._id;
		setup.ScanLocalPackages(function(err,mlid){
			console.log("packages scaned");
			var s = new Settings();
			s.key = "masterlist";
			s.value = mlid;
			s.save(function(err){
				if(err)
					throw new Error("could not save settings");
				mongoose.disconnect();	
			});
		});
	});

