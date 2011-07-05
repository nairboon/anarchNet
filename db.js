

var mongoose = require('mongoose');
var hashlib = require('hashlib');
var dmp = require('./lib/diff_match_patch.js');


var Schema = mongoose.Schema, ObjectId = mongoose.Types.ObjectId;

var User = new Schema({
	username: String,
	password: {type:String, set: function(inp){ return hashlib.sha256(inp);}},
	email: {type:String, set: function(inp){ return inp.toLowerCase();}},
	registered: {type: Date, default: Date.now},
	count: {type:Number, default: 0}
});

var RawData = new Schema({
	content: String
});

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


mongoose.model('user',User);
mongoose.model('data',Data);
mongoose.model('rawdata',RawData);
mongoose.model('snapshot',Snapshot);
mongoose.model('diff',Diff);
mongoose.model('branch',Branch);


var LocalDatabase = function(){
		User = mongoose.model('user');
		Data  = mongoose.model('data');	 
		Diff  = mongoose.model('diff');	 
		Branch  = mongoose.model('branch');	 
		Snapshot  = mongoose.model('snapshot');	 
		RawData = mongoose.model('rawdata');
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
	getRawData: function(id,rev,cb) { 
		Data.findOne({'id': id},function(err,res) {
				
			if(err || !res) 
				return cb(new Error('Could not load Document'));
				
			if(rev) //specific revision
					cb("not implemented");//RawData.findById();
			else // HEAD 
			{
				console.log("get: " + res.head);
				RawData.findById(res.revisions.id(res.head).dataid,function(err,res){
					cb(res);
				});
			}
		});		
	},
	getData: function(id,rev,cb) {
		Data.findOne({'id': id},function(err,res) {
				
			if(err || !res) 
				return cb(new Error('Could not load Document'));
				
				// get content
				DataProvider.prototype.getRawData(id,rev,function(r){
					console.log(r);
					res.content = r.content;
					cb(res);
				});
		});
	},
	getAll: function(cb) {
		Data.find({'access':'public'}, function(err,res) {
			cb(res);
		});
	},
	getHead: function(id,cb) { return this.get(id,null,null,false,cb); },
	getHeadforDoc: function(doc,branch_id) {
		branch = doc.branches.id(branch_id);
		snapshot = branch.snapshots.id(branch.head);
		return snapshot.content;
	},
	get: function(id,branch,rev,completeDocument,cb) {
		Data.findOne({'id': id},function(err,res) {
			if(err || !res) 
				return cb(new Error('Could not load Document'));
			
			if(completeDocument)
				cb(res);
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
		//	for (var i = res.branch.diffs.length-1, l = 0; i >= l; i--) {
			for (var i = 0, l =res.branch.diffs.length; i < l; i++) {
			    if (ObjectId.toString(res.branch.diffs[i].snapshot) == ObjectId.toString(res.snapshot._id)) {
			      patch = dmp.patch_fromText(res.branch.diffs[i].content);
					console.log("apply",i,res.branch.diffs[i].content);
					newc = dmp.patch_apply(patch,newc)[0];
					console.log(newc,patch);
					
				}				
			  }
				
			res.content = newc;
				return cb(res);
					

			
			}
		});
	},
	update: function(id,newcontent,branch,session,cb) {
		//check ownership
		this.get(id,branch,null,false,function(res) {
				if(!res) 
					return cb(new Error('Could not update Document'));
				
					console.log("res.contentUPDATE");
				
			console.log(branch,res.content);		
			//var diff = dmp.diff_main(res.content,newcontent);
			//dmp.diff_cleanupSemantic(diff);
			var patch = dmp.patch_make(res.content,newcontent);
			var diff = dmp.patch_toText(patch);
			console.log(diff);
			
			dobj = new Diff();
			dobj.snapshot = res.snapshot._id;
			dobj.content = diff;
			
			res.branches.id(branch).diffs.push(dobj);
			res.save();
			console.log(res);
			cb("success");
			return;
		});
		
		return;
		Data.findOne({'id': id},function(err,res) {
			if(res.owner == session.userid) {
				var data = new RawData();
				data.content = newcontent.content;
				data.save();
				
				res.type = newcontent.type;
				
				var diff = diff_main()
				//res.revisions.push({dataid: data._id,prev:res.head});
				res.head = res.revisions[res.revisions.length-1]._id;
				res.save();
				console.log("updated: ",id);
				console.log("new revision: ",res.head);
				cb("success");
			}
			else {
				console.log("access denied",id);
				cb("access denied");
			}
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
	}
};

module.exports = LocalDatabase;
