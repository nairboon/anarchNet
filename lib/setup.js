var mongoose = require('mongoose'),
	config = require('../config.js'),
	async = require("async"),
	db = require('./db.js'),
	ppm = require('./ppm.js'),
	fs = require('fs');	
	
var userid = {};

var Setup = function () {
	
}

Setup.prototype = {
	UserCreation: function(cb) {
		db.registerUser({username:'admin',password:'admin',email:'admin@localhost'},function(err,res){
			if(err)
				return	cb(err,null);
			db.authorizeUser({username:'admin',password:'admin'},function(err,res){
				userid = res._id;
				cb(err,res);
			});
		});
	},
	ScanLocalPackages: function(cb) {
		var dir = fs.readdirSync('./repo');
		console.log(dir);
		var masterlist = new Array();
		
		var mq = async.queue(function (mtask, mcallback) {
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
			
			eq.drain = function(){
				info.code = newcode;
				console.log(newcode);
				console.log(info);
				db.store({content: JSON.stringify(info),owner:userid}, function(err,res){
					console.log("new info file saved",res.id);
					masterlist.push(info.name+':'+res.id);
					mcallback();
				});
				console.log("processing dir finished");
			}
			
		},1);
		
		mq.drain = function() {
			console.log(masterlist,"ms finished");
			
			db.store({content:JSON.stringify(masterlist),owner:userid},function(err,res){
				console.log("your masterfile: ", res.id);
				cb(err,res.id);
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

module.exports = new Setup();
