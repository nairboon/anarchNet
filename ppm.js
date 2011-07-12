	//require('./models/ppm.js');
var jade = require("jade");
	dp = require('./db.js');
var db = new dp.db();
var async = require("async");

var mongoose = require('mongoose'),
	hashlib = require('hashlib'),
	config = require('./config.js').conf,
	dmp = require('./lib/diff_match_patch.js');

	var Schema = mongoose.Schema, ObjectId = mongoose.Types.ObjectId;

	var User = new Schema({
		username: String,
		password: {type:String, set: function(inp){ return hashlib.sha256(inp);}},
		email: {type:String, set: function(inp){ return inp.toLowerCase();}},
		registered: {type: Date, default: Date.now},
		count: {type:Number, default: 0},
		repo: Schema.ObjectId
	});

	/*   pkg   */
	var Dependency = new Schema({
		name: String,
		version: String
	});

	var Package = new Schema({
		name: String,
		version: String,
		code: String,
		resources: String,
		depends: [Dependency]
	});

	var Repository = new Schema({
		sources: [String],
		branch: {type:String, default: "trunk"},
		revid: {type:String, default: ""},
		packages: [Package]
	});

	var Appcache = new Schema({
		name: String,
		revid: Schema.ObjectId,
		content: String
	});

	mongoose.model('user',User);
	mongoose.model('package',Package);
	mongoose.model('dependency',Dependency);
	mongoose.model('repository',Repository);

var PPM = function(){
		Repository = mongoose.model('repository');
		Package = mongoose.model('package');
		Dependency = mongoose.model('dependency');
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
				
		Repository.findOne({'id':rid}, function(err,res) {
				if(err || !res) {
					console.log("user repo not found! creating one...");
					
					var r = new Repository();
					r.sources.push(config.defaultmaster);
					r.save();
					session.user.repo = r._id;
					console.log("new repo id: "+ r._id);
					User.findOne({'_id': ObjectId(session.user.id)}, function(err,res) {
						console.log(err,res);
						if(err || !res) 
							throw new Error("user not found: "+ session.user.id);
						res.repo = r._id;
						res.save();
						return cb(new Error('Nothing cached, created new repo'),r);
					
					});
				} 
				else {
					Appcache.findOne({'name': id, revid: res.revid},function(err,res){
						if(err || !res) {
							console.log("not in cache or outdated");
							return cb(new Error('Nothing cached'));
						}
						 return cb(err,res.content);
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
				
			//FIXME? multiple sources
			console.log("fetching", repo.sources[0]);
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
							console.log("(apps) add",item);
							cq.push(item,function (err) {
							    console.log('finished processing:',item);
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
							console.log(pack);
							pack.save();
							repo.packages.push(pack);
							callback();

						}
						
						/* */
						
					});
				}, 2);
				
				// assign a callback
				q.drain = function() {
				    console.log('all items have been processed');
					repo.save();
					return cb(null,repo);
				
				}
				
				masterlist.forEach(function(item){
					console.log("(masterlist) add",item);
					q.push(item,function (err) {
					    console.log('finished processing bar');
					});
					});
			
				
			});
		});
		
	},
	cacheApp: function(id,repo,cb) {
			/*Repository.findOne({'_id': ObjectId(session.user.repo)}, function(err,res) {
					if(err || !res) 
						cb(new Error("user repo not found!: "+session.user.repo));
						
					console.log("caching...");
					cb(null,"haha");	
					
				});*/
				console.log("repo: ", repo);
				console.log("caching...");
				repo.packages.forEach(function(item) {
					if(item.name = id) {
						console.log("app found!");
						console.log("code: ",item.code);
						jade.renderFile('./views/app.jade',{locals:{name:id, code:item.code}},function(err, html){
						        cb(err,html);
						    });
						
						
						//FIXME dependencies
					}
				});
	}
};

module.exports = PPM;