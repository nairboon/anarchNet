/*
 *	setup for parannus
 *	creates admin user and imports pkgs
 */

var mongoose = require('mongoose'),
	config = require('./config.js'),
	async = require("async"),
	db = require('./lib/db.js'),
	ppm = require('./lib/ppm.js'),
	fs = require('fs');	
	
var userid = {};

var Setup = function () {
	
}

Setup.prototype = {
	UserCreation: function(cb) {
		db.registerUser({username:'admin',password:'admin',email:'admin@localhost'},function(){
			db.authorizeUser({username:'admin',password:'admin'},function(err,res){
				cb(err,res);
			});
		});
	},
	ScanLocalPackages: function(cb) {
		var dir = fs.readdirSync('./repo');
		console.log(dir);
		var masterlist = new Array();
		
		var mq = async.queue(function (mtask, mcallback) {
			console.log("mqueue");
			var rawinfo = fs.readFileSync('./repo/'+mtask + '/info.json','utf8');
			var info = JSON.parse(rawinfo);
				console.log(info);
			var newcode = new Array();
			
			var eq = async.queue(function (etask, ecallback) {
				var name = etask.split('.')[0];
				content = fs.readFileSync('./repo/'+mtask + '/'+etask,'utf8');
				db.store({content:content, owner:userid},function(err,res){
					if(err) {
						console.log(err.message);
						throw err;
					}
					newcode.push(name+':'+res.id);
					ecallback();
				});
			},1);
			
			info.code.forEach(function(codeitem){
				eq.push(codeitem);
			});
			
			info.code = newcode;
			db.store({content: info,owner:userid}, function(err,res){
				console.log("new info file saved");
				masterlist.push(info.name+':'+res.id);
				mcallback();
			});
			
			eq.drain = function(){
				console.log("processing dir finished");
			}
			
		},1);
		
		mq.drain = function() {
			console.log(masterlist,"ms finished");
			
			db.store({content:masterlist,owner:userid},function(err,res){
				console.log("your masterfile: ", res.id);
				cb();
			});
		};
		
		
		dir.forEach(function(item){
			console.log(item);
			mq.push(item);
		});
	}
	/*
	TODO: scan repo/*, add to db, create masterfile, create repo
	*/	
};

if(typeof(TEST) == "undefined") { // we're not in the test suite
	var setup = new Setup();
	
mongoose.connect(config.dburl);
	setup.UserCreation(function(cb,user){
		console.log("user created");
		userid = user._id;
		setup.ScanLocalPackages(function(cb){
			console.log("packages scaned");
			mongoose.disconnect();	
		});
	});

}

module.exports = new Setup();
