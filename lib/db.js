
var mongoose = require('mongoose'),
	hashlib = require('hashlib'),
	config = require('../config.js'),
	dmp = require('./diff_match_patch.js'),
	common = require('../models/common.js');

var Schema = mongoose.Schema, ObjectId = mongoose.Types.ObjectId;


 /*  db */
var Snapshot = new Schema({
	dataid: Schema.ObjectId,
	name: String,
	content: String,
	date: {type: Date, default: Date.now},
	prev: {type: Schema.ObjectId, default: null}
});

var Diff = new Schema({
	dataid: Schema.ObjectId,
	snapshot: Schema.ObjectId,
	content: String,
	date: {type: Date, default: Date.now},
	prev: {type: Schema.ObjectId, default: null}
});

var Branch = new Schema({
	name: {type:String, default: "trunk"},
	head: Schema.ObjectId,
	snapshots: [Snapshot],
	diffs: [Diff]
});

var Data = new Schema({
	id: String,
	type: String,
	owner: String,
	access: {type:String, default: 'public'},
	head: Schema.ObjectId,
	branches: [Branch]
});


mongoose.model('data',Data);
mongoose.model('snapshot',Snapshot);
mongoose.model('diff',Diff);
mongoose.model('branch',Branch);

var LocalDatabase = function() {
		User = mongoose.model('user');
		Data  = mongoose.model('data');	 
		Diff  = mongoose.model('diff');	 
		Branch  = mongoose.model('branch');	 
		Snapshot  = mongoose.model('snapshot');	 
};

LocalDatabase.prototype = {
	registerUser: function(param,cb) {
		var user = new User(param);		
		user.save();
		//console.log(user);
		if(cb)
			cb();
	},
	authorizeUser: function(param,cb) {
		User.findOne({'username': param.username, 'password': hashlib.sha256(param.password)},function(err,res){
			console.log("login:",param.username,param.password);
			if(err)
				console.log(err);
				
			cb(err,res);
		});
	},
	getAll: function(cb) {
		Data.find({'access':'public'}, function(err,res) {
			cb(res);
		});
	},
	getHead: function(id,cb) { return this.get(id,null,null,false,cb); },
	get: function(id,branch,rev,completeDocument,cb) {
		Data.findOne({'id': id},function(err,res) {
			if(err || !res) {
				return cb(err,res);
			}
			if(completeDocument)
				return	cb(null,res);
			
				if(branch)
					branch_id = branch;
				else
					branch_id = res.head;
					
			res.branch = res.branches.id(branch_id);
			res.snapshot = res.branch.snapshots.id(res.branch.head);

			var diffs = new Array();
			var snap = res.snapshot.content;
			var newc = snap;
			var patch;
			//console.log("applying patches: ",res.branch.diffs.length);
			for (var i = 0, l =res.branch.diffs.length; i < l; i++) {
			    if (ObjectId.toString(res.branch.diffs[i].snapshot) == ObjectId.toString(res.snapshot._id)) {
			      patch = dmp.patch_fromText(res.branch.diffs[i].content);
					newc = dmp.patch_apply(patch,newc)[0];
				}				
			  }
				
			res.content = newc;
			return cb(null,res);
		});
	},
	update: function(id,newcontent,branch,session,cb) {
		if(!branch)
			throw new Error("Branch cannot be null!");
		//FIXME: check ownership
		this.get(id,branch,null,false,function(err,res) {
				if(err || !res) 
					return cb(err,null);
			
			var patch = dmp.patch_make(res.content,newcontent);
			var diff = dmp.patch_toText(patch);
			console.log(diff);
			
			dobj = new Diff();
			dobj.snapshot = res.snapshot._id;
			dobj.content = diff;
			res.content = newcontent;
			res.branches.id(branch).diffs.push(dobj);
			res.save(function(err){
				if(err)
					cb(err,null);
				else
					cb(null,res);
			});
			
		});
		
	},
	store: function(param,cb) {
		var data = new Snapshot();
		data.content = param.content;
		data.save();
		
		var branch = new Branch();
		branch.snapshots.push(data);
		branch.head = data._id;
		branch.save();
		
		var ins = new Data();
		//ins.type = param.type;
		ins.id = hashlib.sha256(new Date()+param.owner+Math.random()); // TODO: + username, +usercount+ servername
		ins.owner = param.owner;
		ins.branches.push(branch);
		ins.head = branch._id;
		ins.save(function(err){
			if(err)
				cb(err,null);
			else
				cb(null,ins);
		});
		console.log("new data: "+ins._id);
	},
	delete: function(id,userid,cb) {
			Data.findOne({'id': id, owner: userid},function(err,res) {
				if(err || !res) 
					return cb(err,null);
				
				res.remove(function(err){
					if(err)
						cb(err,null);
					else
						cb(null,res.id);
				});
			});
	}
};


module.exports = new LocalDatabase();
