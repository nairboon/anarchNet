/*
 *	setup for parannus
 *	creates admin user and imports pkgs
 */

var mongoose = require('mongoose'),
	config = require('./config.js').conf,
	common = 
	db = require('lib/db.js'),
	ppm = require('lib/ppm.js');

var Setup = function () {
	
}

Setup.prototype = {
	UserCreation: function(cb) {
		db.registerUser({username:'admin',password:'admin',email:'admin@localhost'},function(){
			db.authorizeUser({username:'admin',password:'admin'},function(err){
				cb(err);
			});
		});
	}
	/*
	TODO: scan repo/*, add to db, create masterfile, create repo
	*/	
};

if(typeof(TEST) == "undefined") { // we're not in the test suite
	var setup = new Setup();
mongoose.connect(config.dburl);
	setup.UserCreation(function(cb){
		console.log("user created");
		mongoose.disconnect();
	});

}

module.exports = new Setup();
