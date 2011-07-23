var mongoose = require('mongoose'),
	hashlib = require('hashlib'),
	config = require('../config.js'),
	dmp = require('./diff_match_patch.js'),
	db = require('./db.js'),
	async = require("async"),
	jade = require("jade"),
	ObjectId = mongoose.Types.ObjectId;
	require('../models/common.js');
	require('../models/ppm.js');

var PPM = function() {
		Repository = mongoose.model('repository');
		Map = mongoose.model('map');
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
			db.get(repo.sources[0],null,null,false,function(err,ml) {
				if(err || !ml) 
					return cb(new Error("Master list not found"));
					
				var masterlist = JSON.parse(ml.content);
				
				var q = async.queue(function (task, callback) {
				    console.log('loading ' + task.name);
					db.get(task.info,null,null,false,function(err,res) {
						if(err || !res) 
							return cb(err);
						
						var appinfo = JSON.parse(res.content);
						var pack = new Package();
						pack.name = appinfo.name;
						pack.version = appinfo.version;
						if(appinfo.html)
							pack.html = appinfo.html;
						pack.revid = repo.revid;
						pack.repo = repo._id;
						pack.code = "";
						/* load code */
						var cq = async.queue(function (ctask, ccallback) {
							// loading all code entries
							console.log("loading code: ",ctask);
							db.get(appinfo.code[ctask],null,null,false,function(err,res) {
								if(err || !res) {
									console.log(ctask.data, " not found!");
								return	ccallback();
								}
								//console.log("add ", res.content);
								pack.code += res.content;
								ccallback();
							});
						},1);
						
						Object.keys(appinfo.code).forEach(function(item) {
							console.log("(apps) add",item);
							cq.push(item,function (err) {
							    if(err) 
									throw err;
							});
						});
						
						var loadResources = function(callback){
							if(!appinfo.resources)
								return callback();
							console.log("load resources...");
							
							var lrq = async.queue(function (lrtask, lrcallback) {
								var map = new Map();
								map.key = lrtask.key;
								map.value = lrtask.value;
								pack.resources.push(map);
								console.log(map);
								lrcallback();
							},2);
							
							appinfo.resources.forEach(function(item){lrq.push(item);});
							
							lrq.drain = function() {
								callback();
							}
							
						}
						var savePack = function(callback){
								pack.save(function(err) {
									if(err)
										throw err;

									console.log("added package to repo: ",pack.name);
									repo.packages.push(pack);
									callback();
								});
						};
						cq.drain = function() {
						    console.log('programm code processed');
							
							if(appinfo.dependencies) {
								var dq = async.queue(function (item, dcallback) {
									console.log("new dependency: ",item);
									var dep = new Dependency();
									dep.name = item;
									dep.version = appinfo.dependencies[item];
									dep.save(function(err) {
										if(err)
											throw err;
										pack.depends.push(dep);
										dcallback();
									});
								
								},1);
							Object.keys(appinfo.dependencies).forEach(function(item) {
								dq.push(item);
							});
							dq.drain = function(){
								loadResources(function(){savePack(callback)});
							};
							}
							else
								loadResources(function(){savePack(callback)});
						};
					});
				}, 1);
				
				// assign a callback
				q.drain = function() {
				    console.log('all items have been processed');
					repo.save(function(err) {
						if(err) {
							console.log(repo);
							throw err;
						}
						cb(err,repo);
					});
				};
				
				console.log(masterlist);
				masterlist.forEach(function(item) {
					console.log("(masterlist) add",item);
					q.push(item,function (err) { if(err) throw err;});
					});	
			});	
		});
};
//clean the repo
Repository.findOne({'_id':id}, function(err,repo) {
	if(err || !repo) 
		return cb(err);
		// clean packages!
		var kq = async.queue(function(task,callback) {
			task.remove(function(err) {
				if(err)
					throw err.message;
				Package.findOne({'_id':task._id},function(err,res){
					if(err || !res){
					//	console.log("no package to kil");
						return callback();
					}
					console.log("removed: ", res.name);
					res.remove(function(err){
						if(err)
							throw err;
						callback();
					});
					
				});
			}); 
		},1);
		
		if(repo.packages.length === 0)
			update(id,cb);
			
		repo.packages.forEach(function(item){kq.push(item);});

		kq.drain = function() {
			repo.save(function(err) {
				if(err)
					throw err;
				console.log("existing entries removed",repo.packages.length);
				update(id,cb);
			});
		};
});
		
	},
	cacheApp: function(id,repo,cb) {
		console.log("caching... ", id);
		Package.findOne({name:id,repo:repo},function(err,item) {
			if(err || !item)
				cb(new Error("package not found"+err.message));
				console.log("app found!");
			//	console.log("code: ",item.code);
				console.log("depends",item.depends);
				jade.renderFile('./views/app.jade',{locals:{name:id, code:item.code,deps:item.depends}},function(err, html) {
				      if(err)
						throw new Error("could not render app");
						
					var ac = new Appcache();
					ac.name = id;
					ac.revid = item.revid;
					ac.content = html;
					ac.save(function(err){
						if(err)
							throw err;
					console.log("cached:",ac._id);
					//	console.log(html);
						return cb(err,html);
					});
				});
			
		});
	},
	getPackCode: function(id,repoid,cb){
		Package.findOne({name:id,repo:repoid},function(err,res){
			if(err || !res)
				cb(err);
			else
				cb(err,res.code);
		});
	}
};

module.exports = new PPM();
