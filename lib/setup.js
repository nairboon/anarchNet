
var mongoose = require('mongoose'),
	config = require('../config.js'),
	async = require("async"),
	db = require('./db.js'),
	ppm = require('./ppm.js'),
	util = require('./util.js'),
	filestore = require('./filestore.js'),
	fs = require('fs');

var userid = {};

Settings  = mongoose.model('settings');

var walk = function(dir, done) {
  var results = [];
  fs.readdir(dir, function(err, list) {
    if (err) return done(err);
    var pending = list.length;
    list.forEach(function(file) {
      var rname = file;
      file = dir + '/' + file;
      fs.stat(file, function(err, stat) {
        if (stat && stat.isDirectory()) {
          walk(file, function(res) {
            results = results.concat(res);
            if (!--pending) done(null, results);
          });
        } else {
			if(rname.substr(0,1)!==".")
				results.push(file);
          if (!--pending) done(null, results);
        }
      });
    });
  });
};

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
				db.store(JSON.stringify(content),function(err,res){
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

			var loadResources = function(info,rcallback) {
				if(info.resources) {
					var nres = [];
					console.log("loading resources");
					var rq = async.queue(function (rtask, rqcallback) {
						var rfilename = rtask[0].replace("./repo/"+mtask+"/","");
						var suffix = rfilename.split('.').pop();
						if(util.mime.binary.has(suffix)) {
							console.log("storing binary file: ", rfilename);
							filestore.storeFile(rtask[0],rfilename,function(err){
								if(err == 1) // same file
									console.log("file already stored");
								if(err > 1)
									throw err;

								nres.push({key:rfilename,value:rfilename,filestore:true});
								rqcallback();
							});
						}
						else {

							var content = fs.readFileSync(rtask[0],'utf8');
							db.store(content, function(err,res){
								if(err)
									throw err;

									nres.push({key:rfilename,value:res.id});
									rqcallback();
								});
							}
					},2);
					info.resources.forEach(function(ritem) {
						walk('./repo/'+mtask + '/'+ritem,function(err,res){
							if(err)
								throw err  + +mtask + ritem;
							rq.push(res);
						});
					});
					rq.drain = function() {
						console.log(nres);
						info.resources = nres;
						db.store(JSON.stringify(info), function(err,res) {
							console.log("new info file saved",res.id);
							masterlist.push({name:info.name, info:res.id});
							rcallback();
						});
					}

				}
				else {
					db.store(JSON.stringify(info), function(err,res) {
						console.log("new info file saved",res.id);
						masterlist.push({name:info.name, info:res.id});
						rcallback();
					});
				}

			}
			eq.drain = function() {
				info.code = newcode;
				if(info.html) {
					html = fs.readFileSync('./repo/'+mtask + '/'+ info.html,'utf8');
					db.store(html,function(err,res){
						console.log("html file added");
						info.html = res.id;
						loadResources(info,mcallback);
					});
				}
				else
					loadResources(info,mcallback);
			};

		},1);

		mq.drain = function() {
	//		console.log(masterlist,"ms finished");
			db.store(JSON.stringify(masterlist),function(err,res) {
				console.log("your masterfile: ", res.id);
				cb(err,res.id);
			});
		};

		dir.forEach(function(item) {
			if(item.substr(0,1)==".")
				return;
			console.log(item);
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
