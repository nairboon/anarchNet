
var mongoose = require('mongoose'),
	hashlib = require('hashlib'),
	config = require('../config.js'),
	dmp = require('./diff_match_patch.js'),
	db = require('./db.js'),
	async = require("async"),
	jade = require("jade");
	require('../models/common.js');
	require('../models/ppm.js');


var ObjectId = mongoose.Types.ObjectId;

var PPM = function() {
		Repository = mongoose.model('repository');
		Package = mongoose.model('package');
		Dependency = mongoose.model('dependency');
		Appcache = mongoose.model('appcache');
		User = mongoose.model('user');
};

PPM.prototype = {
	appcacheLoad: function(id,repoid,cb) {
		console.log("appcache: ", id, repoid);
	
		Repository.findOne({'_id':ObjectId(repoid)}, function(err,res) {
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
					console.log(res);
					Appcache.findOne({'name': id, revid: res.revid}, function(err,acres) {
						if(err || !acres) {
							console.log("App not in cache or outdated");
							return cb(new Error("not cached"),res);
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

var update = function(id,cb) {
	console.log("actual update");
		Repository.findOne({'_id':id}, function(err,repo) {
			if(err || !repo) 
				return cb(err);
										
			//FIXME? multiple sources
			console.log("fetching", repo.sources[0]);
			// load sources
			db.get(repo.sources[0],null,null,false,function(err,ml){
				if(err || !ml) 
					return cb(new Error("Master list not found"));
					
				var masterlist = JSON.parse(ml.content);
				
				var q = async.queue(function (task, callback) {
				    console.log('loading ' + task.name);
					db.get(task.info,null,null,false,function(err,res){
						if(err || !res) 
							return cb(err);
						
						var appinfo = JSON.parse(res.content);
						var pack = new Package();
						pack.name = appinfo.name;
						pack.version = appinfo.version;
						pack.repo = repo._id;
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
							console.log("(apps) add",item);
							cq.push(item,function (err) {
							    if(err) 
									throw err;
							});
						});
						
						cq.drain = function() {
						    console.log('programm code processed');
							
							if(appinfo.depends) {
								//FIXME should be a queue!
							appinfo.depends.forEach(function(item){
								console.log("new dependency: ",item);
								var dep = new Dependency();
								dep.name = item[0];
								dep.version = item[1];
								dep.save(function(err){
									if(err)
										throw err;
									pack.depends.push(dep);
								});
							});
							}
							pack.save(function(err) {
								if(err)
									throw err;
									
								console.log("added package to repo: ",pack.name);
								repo.packages.push(pack);
								callback();
							});
						}
					});
				}, 1);
				
				// assign a callback
				q.drain = function() {
				    console.log('all items have been processed');
					repo.save(function(err){
						if(err) {
							console.log(repo);
							throw err;
						}
						cb(err,repo);
					});
				}
				
				console.log(masterlist);
				masterlist.forEach(function(item) {
					console.log("(masterlist) add",item);
					q.push(item,function (err) { if(err) throw err;});
					});	
			});	
		});
}
//clean the repo
Repository.findOne({'_id':id}, function(err,repo) {
	if(err || !repo) 
		return cb(err);
		// clean packages!
		var kq = async.queue(function(task,callback){
			task.remove(function(err){
				if(err)
					throw err.message;
				console.log("removed: ", task.name);
				callback();
			}); 
			callback();
		},1);
		
		if(repo.packages.length == 0)
			update(id,cb);
			
		repo.packages.forEach(function(item){kq.push(item);});

		kq.drain = function(){
			repo.save(function(err){
				console.log("existing entries removed");
				update(id,cb);
			});
		}
});
		
	},
	cacheApp: function(id,repo,cb) {
				console.log("caching...");
				repo.packages.forEach(function(item) {
					if(item.name == id) { // search the app
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
							return cb(err,html);
						});
						//FIXME dependencies
					}
					else
						cb(new Error("App not found!!"));
				});
	}
};

module.exports = new PPM();
