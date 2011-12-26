var db = require('./db.js'),
	hashlib = require('hashlib'),
	filesrv = require('node-static'),
	config = require('../config.js'),
	fs = require('fs'),
	util = require('util');


var FileStore = function() {
		srv = new(filesrv.Server)(config.filestore, { cache: 7200 });
};

FileStore.prototype = {
	storeFile: function(phypath,virtpath,cb) {
		if(!virtpath)
			virtpath = phypath;
			
		var hash = hashlib.md5_file(phypath);
		db.get_ht(hash, function(err,res){
			var f = { filename: virtpath, hash: hash, localpath: hash };
			
			
			var save = function(f,cb) {
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
			if(err || !res) {
				console.log("new file",hash);
				var is = fs.createReadStream(phypath)
				var os = fs.createWriteStream(f.localpath);

				util.pump(is, os, function(err) {
					if(err)
						throw err;
					   save(f,cb);
				});
			}
			else {
				res = JSON.parse(res);
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
