

var mongoose = require('mongoose');
var hashlib = require('hashlib');


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
					
			branch = res.branches.id(branch_id);
			snapshot = branch.snapshots.id(branch.head);

			var diffs;
			for (var i = 0, l = branch.diffs.length; i < l; i++) {
			    if (branch.diffs[i].snapshot == snapshot._id)
			      diffs.push_back(branch.diffs[i]);
			  }
				
							
				return cb(snapshot.content);
					

			
			}
		});
	},
	update: function(id,newcontent,session,cb) {
		console.log(session);
		//check ownership
		Data.findOne({'id': id},function(err,res) {
			if(res.owner == session.userid) {
				var data = new RawData();
				data.content = newcontent.content;
				data.save();
				
				res.type = newcontent.type;
				res.revisions.push({dataid: data._id,prev:res.head});
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
