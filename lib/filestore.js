var db = require('./db.js'),
	hashlib = require('hashlib'),
	filesrv = require('node-static'),
	config = require('../config.js'),
	fs = require('fs'),
	fs_util = require('util'),
	util = require('./util.js');


var FileStore = function() {
		srv = new(filesrv.Server)(config.filestore, { cache: 7200 });
};

FileStore.prototype = {
	storeFile: function(phypath,virtpath,cb) {
	var abpath = process.cwd() + phypath.replace("./","/");
	console.log("storing file: ", phypath, virtpath, abpath);
		if(!virtpath)
			virtpath = phypath;
			
		var hash = hashlib.md5_file(phypath);
		db.get_ht(hash, function(err,res){
			var f = { filename: virtpath, hash: hash, localpath: hash };
			
			
			var save = function(f,cb) {
			console.log("save: ", f);
				db.store_ht(hash,JSON.stringify(f),function(err){
					if(err)
						throw err;
						
					db.store_ht(util.system_id+"FS"+f.filename,hash,function(err){
						if(err)
							throw err;
						cb(null);
					});
				});
			}
			var newfile = function() {
			console.log("new file",hash);
			db.put_file(abpath,function(err,res) {
				if(err)
					throw err;
				f.id=res.id;
				save(f,cb);
			});
			/*	var is = fs.createReadStream(phypath)
				var os = fs.createWriteStream(config.filestore+"/"+f.localpath);

				fs_util.pump(is, os, function(err) {
					if(err)
						throw err;
					   save(f,cb);
				});*/
			
			}
			if(err || !res) {
				newfile();
			}
			else {
			console.log(res);
			try {
				res = JSON.parse(res);
				}
				catch(err)
				{
					console.log("not a meta file: ", err);
					newfile();
					return;
				}
				if(res.localpath === f.localpath) {
					console.log("same file with same path exists!");
					cb(1);
				}
				console.log("file already exists");
				save(f,cb);
			}
		});
	},
	getFile: function(filename,req,res) {
		console.log("get file: ", filename);
		db.get_ht(util.system_id+"FS"+f.filename,function(err,fhash){
			if(err || !fres)
				throw err;
			console.log("file found!",filename,fhash);
		db.get_ht(fhash,function(err,fres){
			if(err || !fres)
				throw err;
			req.url = "http://localhost/"+fres.localpath;
				srv.serve(req, res, function (err, srvres) {
				            if (err) { // An error as occured
				                console.log("> Error serving " + req.url + " - " + err.message);
				                res.send("");
				            } else { // The file was served successfully
				                console.log("> " + req.url + " - " + srvres.message);
				            }
				 });
		});
		});
	}
};

module.exports = new FileStore();
