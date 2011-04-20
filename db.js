

var mongoose = require('mongoose');
var Schema = mongoose.Schema, ObjectId = Schema.ObjectId;

var UserSchema = new Schema({
	id: ObjectId,
	username: String,
	password: String,
	registered: Date
});

var DataSchema = new Schema({
	type: String,
	owner: String,
	content: String
});
mongoose.model('users',UserSchema);
mongoose.model('data',DataSchema);

var DataProvider = function(){
		User = mongoose.model('users');
		Data  = mongoose.model('data');
		
		this.authorizeUser = function(user) {}
		this.getData = function(id,rev) { return {msg: "no data for" + id}; }
		this.setData = function(id,newcontent) {}
		this.addData = function(param) {
			var ins = new Data();
			ins.type = param.type;
			ins.owner = param.owner;
			ins.content = param.content;
			ins.save();
		}
		 
};


exports.DataProvider = DataProvider;
