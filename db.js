
var mongoose = require('mongoose'),
	hashlib = require('hashlib'),
	config = require('./config.js').conf,
	dmp = require('./lib/diff_match_patch.js'),
	common = require('models/common.js');

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

var LocalDatabase = function(){
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
		console.log(user);
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
			if(err || !res) 
				return cb(new Error('Could not load Document'));
			
			if(completeDocument)
				cb(null,res);
			else
			{
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
			console.log("applying patches: ",res.branch.diffs.length);
			for (var i = 0, l =res.branch.diffs.length; i < l; i++) {
			    if (ObjectId.toString(res.branch.diffs[i].snapshot) == ObjectId.toString(res.snapshot._id)) {
			      patch = dmp.patch_fromText(res.branch.diffs[i].content);
				//	console.log("apply",i,res.branch.diffs[i].content);
					newc = dmp.patch_apply(patch,newc)[0];
				//	console.log(newc,patch);
					
				}				
			  }
				
			res.content = newc;
			return cb(null,res);
			}
		});
	},
	update: function(id,newcontent,branch,session,cb) {
		//FIXME: check ownership
		this.get(id,branch,null,false,function(err,res) {
				if(err || !res) 
					return cb(new Error('Could not update Document'));
			
			var patch = dmp.patch_make(res.content,newcontent);
			var diff = dmp.patch_toText(patch);
			console.log(diff);
			
			dobj = new Diff();
			dobj.snapshot = res.snapshot._id;
			dobj.content = diff;
			
			res.branches.id(branch).diffs.push(dobj);
			res.save();
			cb("success");
			return;
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
		ins.type = param.type;
		ins.id = hashlib.sha256(new Date()+param.owner); // TODO: + username, +usercount+ servername
		ins.owner = param.owner;
		ins.branches.push(branch);
		ins.head = branch._id;
		ins.save();
		console.log("new data: "+ins._id);
		cb(ins);
	},
	delete: function(id,session,cb) {
			Data.findOne({'id': id, owner: session.user.id},function(err,res) {
				if(err || !res) 
					return cb(new Error('Could not remove Document: non existent or not owner'));
					
				res.remove();
				cb("success");
			});
	}
};


module.exports = {db: LocalDatabase};
