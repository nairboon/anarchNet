var mongoose = require('mongoose'),
	config = require('../config.js'),
	async = require("async"),
	db = require('./db.js'),
	ppm = require('./ppm.js'),
	fs = require('fs');	
	
var userid = {};

Settings  = mongoose.model('settings');	 

var Setup = function () { };

Setup.prototype = {
	UserCreation: function(cb) {
		db.registerUser({username:'admin',password:'admin',email:'admin@localhost'},function(err,res) {
			if(err)
				return	cb(err,null);
			db.authorizeUser({username:'admin',password:'admin'},function(err,res) {
				userid = res._id;
				cb(err,res);
			});
		});
	},
	ScanLocalPackages: function(cb) {
		var dir = fs.readdirSync('./repo');
		console.log(dir);
		var masterlist = [];
		
		var mq = async.queue(function (mtask, mcallback) {
			var rawinfo = fs.readFileSync('./repo/'+mtask + '/info.json','utf8');
			var info = JSON.parse(rawinfo);
//				console.log(info);
			var newcode = {};
			
			var eq = async.queue(function (etask, ecallback) {
				var name = etask.split('.')[0];
				var path = info.code[etask];
				content = fs.readFileSync('./repo/'+mtask + '/'+path,'utf8');
				db.store({content:content, owner:userid},function(err,res){
					if(err) {
						console.log(err.message);
						throw err;
					}
					newcode[etask] = res.id;
					ecallback();
				});
			},1);
			
			Object.keys(info.code).forEach(function(codeitem) {
				eq.push(codeitem);
			});
			
			eq.drain = function() {
				info.code = newcode;
				db.store({content: JSON.stringify(info),owner:userid}, function(err,res) {
					console.log("new info file saved",res.id);
					masterlist.push({name:info.name, info:res.id});
					mcallback();
				});
			};
			
		},1);
		
		mq.drain = function() {
	//		console.log(masterlist,"ms finished");
			db.store({content:JSON.stringify(masterlist),owner:userid},function(err,res) {
				console.log("your masterfile: ", res.id);
				cb(err,res.id);
			});
		};
		
		dir.forEach(function(item) {
			mq.push(item);
		});
	},
	createDefaultRepo: function(masterid,cb) {
		var repo = new Repository();
		repo.sources.push(masterid);
		repo.save(function(err) {
			
			Settings.findOne({key:'defaultRepo'}, function(err,res) {
				if(!res) {//no settings create some
					res = new Settings();
					res.key = "defaultRepo";
					res.value = repo._id;
				} else {// update
					res.value = repo._id;
				}
				
				res.save(function(err) {
					if(err)
						throw new Error("could not save settings");

						ppm.updateRepo(repo._id, function(err,res) {
							if(err)
								cb(err);
							else
								cb(null,repo._id);
						});
					});
			});
			
		});
	}
};

module.exports = new Setup();
