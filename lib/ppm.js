var hashlib = require('hashlib'),
	config = require('../config.js'),
	dmp = require('./diff_match_patch.js'),
	db = require('./db.js'),
	async = require("async"),
	jade = require("jade");
	require('../models/common.js');
	
	var util = require('./util.js');

var PPM = function() {
		
};

PPM.prototype = {
	appcacheLoad: function(id,repoid,cb) {
		console.log("appcache: ", id, repoid);
	
		/*db.get(repoid, function(err,res) {
				if(err || !res) {
					console.log("user repo not found! creating one...");
					/*
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
					});*/
					/*return cb(new Error("creating user repo??"));
				} 
				else {*/
					db.get(util.ids.appcache(id),  function(err,acres) {
						if(err || !acres) {
							console.log("App not in cache or outdated");

						return cb(new Error("repo.update"),res);
								
							
						}
						console.log("cache hit");
						 return cb(err,acres.content);
					});
				/*}
		});*/
		
	},
updateRepo: function(repoid,cb) {
		// load "master list"
		//console.log("update ", id);

//var update = function(id,cb) {
	console.log("actual update");
		db.get(repoid, function(err,repo) {
			if(err || !repo) 
				return cb(err);
										
		repo.sources.forEach(function(source) {
			console.log("fetching", source);
			// load sources
			db.get(source,function(err,masterlist) {
				if(err || !masterlist) 
					return cb(new Error("Master list not found"));
				
				var q = async.queue(function (task, callback) {
				    console.log('loading ' + task.name);
					db.get(task.info,function(err,appinfo) {
						if(err || !appinfo) 
							return cb(err);
						console.log(appinfo);
						var pack = {};
						pack.name = appinfo.name;
						pack.version = appinfo.version;
						if(appinfo.html)
							pack.html = appinfo.html;
						if(appinfo.resources)
						 pack.resources = appinfo.resources;
						pack.revid = repo.revid;
						pack.repo = repoid;
						pack.code = "";
						/* load code */
						var cq = async.queue(function (ctask, ccallback) {
							// loading all code entries
							console.log("loading code: ",ctask);
							db.get(appinfo.code[ctask],function(err,res) {
								if(err || !res) {
									console.log(ctask.data, " not found!");
								return	ccallback();
								}
								//console.log("add ", res);
								pack.code += res;
								ccallback();
							});
						},1);
						console.log(appinfo.code);
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
								var map = {};
								map.key = lrtask.key;
								map.value = lrtask.value;
								map.packid = pack.name;
								map.filestore = lrtask.filestore;
								db.store(map,function(err,res){
									if(err)
										throw err;
									//pack.resources.push(res.id);
									console.log(map);
									lrcallback();
								});		
							},2);
							
							appinfo.resources.forEach(function(item){lrq.push(item);});
							
							lrq.drain = function() {
								callback();
							}
							
						}
						var savePack = function(callback){
								db.store(pack,function(err,res) {
									if(err)
										throw err;

									console.log("added package to repo: ",pack.name,res.id, pack);
									repo.packages.push({name: pack.name, id:res.id});
									callback();
								});
						};
						cq.drain = function() {
						    console.log('programm code processed',pack);
							
							if(appinfo.dependencies) {
								/*var dq = async.queue(function (item, dcallback) {
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
							savePack(callback);
								//loadResources(function(){savePack(callback)});
							};*/
							pack.depends = appinfo.dependencies;
							loadResources(function(){savePack(callback)});
							}
							else
								//savePack(callback);
								loadResources(function(){savePack(callback)});
						};
					});
				}, 1);
				
				q.drain = function() {
				    console.log('all items have been processed',repo);
					db.store_with_id(repoid,repo,function(err,res) {
						if(err)
							throw err;
					console.log("stored repo under: ", repoid);
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
		});
//};

//clean the repo
/*db.get(id, function(err,repo) {
	if(err || !repo) 
		return cb(err);
		
		
		
		//var repo = JSON.parse(new Buffer(repoh,'base64').toString('utf-8'));
		//console.log(repo);

		// clean packages!
		var kq = async.queue(function(task,callback) {
			task.remove(function(err) {
				if(err)
					throw err.message;
				db.get(task._id,function(err,res){
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
});*/
		
	},
	cacheApp: function(id,repo,cb) {
		console.log("caching... ", id,repo);
		var saveCache = function(revid,content) {
			console.log("save cache: ",content);
			var ac = {};
			ac.name = id;
			ac.revid = revid;
			ac.content = content;//JSON.stringify(content);
			
			db.store_with_id(util.ids.appcache(id),ac,function(err,res){
				if(err)
					throw err;
					
			console.log("cached:",res.id);
				return cb(err,content);
			});
		}

		var found = false;
		var pack = {};
		for(var i=0;i<repo.packages.length;i++)
			if(repo.packages[i].name == id) {
				pack = repo.packages[i];
				found = true;
				}
		if(!found) {
			console.log(repo.packages);
				cb(new Error("package not found: "+id));
				return;
			}
				console.log("app found!");
		db.get(pack.id, function(err,item) {
			if(err || !item) {
				cb(new Error("Package not found!"+pack.id));
				return
				}
				console.log("package: ",item);
			//	console.log("code: ",item.code);
				console.log("depends",item.dependcies);
				if(!item.html) {
				jade.renderFile('./views/app.jade',{locals:{name:id, code:item.code,deps:item.dependcies}},function(err, html) {
				      if(err)
						throw new Error("could not render app");
					saveCache(item.revid,html);
					});
				}
			else {
				db.get(item.html,function(err,res){
					if(err || !res)
						throw new Error("html file not found!");
						
					saveCache(item.revid,res);
					
				});
			}
			});
		
	},
	getPackage: function(id,repoid,cb){
	console.log("getPackage:",id,repoid);
		db.get(repoid,function(err,repo){
		if(err || !repo) {
				cb(new Error("Repo not found!"+repoid));
				return
		}
		
		console.log(repo);
		var found = false;
		var pack = {};
		for(var i=0;i<repo.packages.length;i++)
			if(repo.packages[i].name == id) {
				pack = repo.packages[i];
				found = true;
				}
		if(!found) {
			console.log(repo.packages);
				cb(new Error("package not found in repo: "+id));
				return;
			}
			
			db.get(pack.id,cb);

		});
		
	},
	getMap: function(key,packid,cb){
		db.get(key,function(err,res){
			console.log("search for:",key,packid);
				if(err || !res)
					return cb(err,null);
				console.log("found mapping:",res);
				
				cb(null,res);
		});
	}
};

module.exports = new PPM();
