

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

var Revision = new Schema({
	dataid: Schema.ObjectId,
	date: {type: Date, default: Date.now},
	prev: {type: Schema.ObjectId, default: null}
});

var Data = new Schema({
	id: String,
	type: String,
	owner: String,
	access: {type:String, default: 'public'},
	head: Schema.ObjectId,
	revisions: [Revision],
});


mongoose.model('user',User);
mongoose.model('data',Data);
mongoose.model('rawdata',RawData);
mongoose.model('revision',Revision);


var DataProvider = function(){
		User = mongoose.model('user');
		Data  = mongoose.model('data');	 
		RawData = mongoose.model('rawdata');
};

DataProvider.prototype = {
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
	getData: function(id,rev,cb) { 
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
	getAll: function(cb) {
		Data.find({'access':'public'}, function(err,res) {
			cb(res);
		});
	},
	setData: function(id,newcontent) {},
	addData: function(param,cb) {
		var data = new RawData();
		data.content = param.content;
		data.save();
		
		var ins = new Data();
		ins.type = param.type;
		ins.id = hashlib.sha512(new Date()+param.owner); // TODO: + username, +usercount+ servername
		ins.owner = param.owner;
		ins.revisions.push({dataid: data._id});
		ins.head = ins.revisions[0]._id;
		ins.save();
		console.log("new data: "+ins._id);
		cb(ins);
	}
};

module.exports = DataProvider;
