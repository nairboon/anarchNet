
var mongoose = require('mongoose'),
	hashlib = require('hashlib'),
	config = require('../config.js').conf,
	dmp = require('./diff_match_patch.js'),
	dp = require('./db.js'),
	async = require("async"),
	jade = require("jade");
	require('../models/common.js');
	require('../models/ppm.js');
var db = new dp.db();


var ObjectId = mongoose.Types.ObjectId;

var PPM = function() {
		Repository = mongoose.model('repository');
		Package = mongoose.model('package');
		Dependency = mongoose.model('dependency');
		Appcache = mongoose.model('appcache');
		User = mongoose.model('user');
};

PPM.prototype = {
	appcacheLoad: function(id,session,cb) {
		// FIXME for non-registered users
		var rid = config.defaultrepo;
			if(!session.user) 
				console.log("not logged in");
			else
				rid = session.user.repo;
				
		Repository.findOne({'_id':ObjectId(rid)}, function(err,res) {
				if(err || !res) {
					console.log("user repo not found! creating one...");
					
					var r = new Repository();
					r.sources.push(config.defaultmaster);
					r.save();
					session.user.repo = r._id;
					console.log("new repo id: "+ r._id);
					User.findOne({'_id': ObjectId(session.user.id)}, function(err,res) {
						if(err || !res) 
							throw new Error("user not found: "+ session.user.id);
						res.repo = r._id;
						res.save();
						return cb(new Error('created new repo'),r);
					});
				} 
				else {
					Appcache.findOne({'name': id, revid: res.revid},function(err,acres){
						if(err || !acres) {
							console.log("App not in cache or outdated");
							return cb(new Error('Nothing cached'),res);
						}
						console.log("cache hit");
						 return cb(err,acres.content);
					});
				}
		});
		
	},
	updateRepo: function(id,cb) {
		// load "master list"
		console.log("update ", id);
		Repository.findOne({'_id':id}, function(err,repo) {
			if(err || !repo) 
				return cb(new Error("repo not found, cannot update!"));
				
			// clean packages!
			// how to clear that array?
			repo.packages.forEach(function(x){x.remove();});
			
			//FIXME? multiple sources
			console.log("fetching", repo.sources[0]);
			// load sources
			db.get(repo.sources[0],null,null,false,function(err,ml){
				if(err || !ml) 
					return cb(new Error("Master list not found"));
					
				var masterlist = JSON.parse(ml.content);
				console.log(masterlist[0]);
				
				var q = async.queue(function (task, callback) {
				    console.log('loading ' + task.name);
					db.get(task.info,null,null,false,function(err,res){
						if(err || !res) 
							return cb(new Error("Infofile not found"));
						
						var appinfo = JSON.parse(res.content);
						var pack = new Package();
						pack.name = appinfo.name;
						pack.version = appinfo.version;
						pack.code = "";
						/* load code */
						var cq = async.queue(function (ctask, ccallback) {
							// loading all code entries
							console.log("loading: ",ctask.name);
							db.get(ctask.data,null,null,false,function(err,res){
								if(err || !res) {
									console.log(ctask.data, " not found!");
								return	ccallback();
								}
								console.log("add ", res.content);
								pack.code += res.content;
								ccallback();
							});
						},1);
						
						appinfo.code.forEach(function(item){
							//console.log("(apps) add",item);
							cq.push(item,function (err) {
							    //console.log('finished processing:',item);
							});
						});
						
						cq.drain = function() {
						    console.log('programm code processed');
							
							appinfo.depends.forEach(function(item){
								var dep = new Dependency();
								dep.name = item[0];
								dep.version = item[1];
								dep.save();
								pack.depends.push(dep);
							});
							
							pack.save();
							console.log("added package to repo");
							repo.packages.push(pack);
							callback();

						}
						
						/* */
						
					});
				}, 1);
				
				// assign a callback
				q.drain = function() {
				    console.log('all items have been processed');
					repo.save();
					return cb(null,repo);
				}
				
				masterlist.forEach(function(item){
					console.log("(masterlist) add",item);
					q.push(item,function (err) {
					    console.log('finished processing masterlist entry');
					});
					});
			
				
			});
		});
		
	},
	cacheApp: function(id,repo,cb) {
				console.log("repo: ", repo);
				console.log("caching...");
				repo.packages.forEach(function(item) {
					if(item.name = id) { // search the app
						console.log("app found!");
						console.log("code: ",item.code);
						jade.renderFile('./views/app.jade',{locals:{name:id, code:item.code}},function(err, html){
						      if(err)
								throw new Error("could not render app");
								
							var ac = new Appcache();
							ac.name = id;
							ac.revid = repo.revid;
							ac.code = html;
							ac.save();
							console.log(html);
								cb(err,html);
							
						    });					
						
						//FIXME dependencies
					}
				});
	}
};

module.exports = PPM;
