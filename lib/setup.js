
var    config = require('../config.js'),
	async = require("async"),
	db = require('./db.js'),
	ppm = require('./ppm.js'),
	util = require('./util.js'),
	filestore = require('./filestore.js'),
	fs = require('fs');

var userid = {};


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
			if( !(rname.substr(0,1)=="." && rname.length ==1)){
				results.push(file);
				console.log("push:", file);}
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
				console.log(info);
			var newcode = {};

			var eq = async.queue(function (etask, ecallback) {
				var name = etask.split('.')[0];
				var path = info.code[etask];
				console.log("code: ",name,path);
				content = fs.readFileSync('./repo/'+mtask + '/'+path,'utf8');
				db.store(content,function(err,res){
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
					console.log("loading resources: ", info.resources);
					var rq = async.queue(function (rtask, rqcallback) {
					console.log(rtask);
						var rfilename = rtask.replace("./repo/"+mtask+"/","");
					//var rfilename = rtask;
						console.log("check:",rfilename);
						var suffix = rfilename.split('.').pop();
						if(util.mime.binary.has(suffix)) {
							console.log("storing binary file: ", rfilename);
							filestore.storeFile(rtask,rfilename,function(err,res){
								if(err == 1) // same file
									console.log("file already stored");
								if(err > 1)
									throw err;

								nres.push({key:rfilename,value:res,filestore:true});
								rqcallback();
							});
						}
						else {
			console.log("store text file" , rtask);
							var content = fs.readFileSync(rtask,'utf8');
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
								throw err  + mtask + ritem;
							
								rq.push(res);
						});
					});
					rq.drain = function() {
						console.log(nres);
						info.resources = nres;
						db.store(info, function(err,res) {
							console.log("new info file saved",res.id, info);
							masterlist.push({name:info.name, info:res.id});
							rcallback();
						});
					}

				}
				else {
					db.store(info, function(err,res) {
						console.log("new info file saved",res.id);
						masterlist.push({name:info.name, info:res.id});
						rcallback();
					});
				}

			}
			eq.drain = function() {
				info.code = newcode;
				console.log("code processed: ",info.code);
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
			console.log(masterlist,"ms finished");
			db.store_with_id(util.system_id + 'masterlist',masterlist,function(err,res) {
				console.log("your masterfile: ", res.id, masterlist);
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
		var repoid = util.system_id + 'defaultRepo';
		db.store_with_id(repoid,{sources: [ masterid ],packages: [] }, function(err,data){
			console.log("created repo: ", repoid, data.id);
				ppm.updateRepo(data.id, function(err,res) { cb(err,res);});
			});
	}
};


module.exports = new Setup();
