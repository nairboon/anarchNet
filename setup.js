/*
 *	setup for parannus
 *	creates admin user and imports pkgs
 */

var mongoose = require('mongoose'),
	config = require('./config.js'),
	setup = require("./lib/setup.js");	

	
mongoose.connect(config.dburl);
	setup.UserCreation(function(cb,user){
		console.log("user created");
		userid = user._id;
		setup.ScanLocalPackages(function(cb){
			console.log("packages scaned");
			mongoose.disconnect();	
		});
	});

