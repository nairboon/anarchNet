var mongoose = require('mongoose'),
	hashlib = require('hashlib'),
	filesrv = require('node-static'),
	config = require('../config.js'),
	fs = require('fs'),
	util = require('util'),
	Schema = mongoose.Schema, 
	ObjectId = mongoose.Types.ObjectId;


var File = new Schema({
	filename: String, //fullpath
	hash: String,
	localpath: String
});

mongoose.model('file',File);

var FileStore = function() {
		File = mongoose.model('file');
		srv = new(filesrv.Server)('.', { cache: 7200 });
};

FileStore.prototype = {
	storeFile: function(phypath,virtpath,cb) {
		if(!virtpath)
			virtpath = phypath;
			
		var hash = hashlib.md5_file(phypath);
		File.findOne({hash:hash},function(err,res){
			var f = new File();
			f.filename = virtpath;
			f.hash = hash;
			f.localpath = config.filestore + "/" + hash;
			
			var save = function(f,cb) {
				f.save(function(err){
					if(err)
						throw err;
					cb(null);
				});
			}
			if(err || !res) {
				console.log("new file",hash);
				var is = fs.createReadStream(phypath)
				var os = fs.createWriteStream(f.localpath);

				util.pump(is, os, function() {
					   save(f,cb);
				});
			}
			else {
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
		File.findOne({filename:filename},function(err,fres){
			if(err || !fres)
				throw err;
			console.log("file found!",filename);
			
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
	}
};

module.exports = new FileStore();
