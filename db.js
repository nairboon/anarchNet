
require('joose');
var mongoose = require('mongoose');
var Schema = mongoose.Schema, ObjectId = Schema.ObjectId;

var user = new Schema({
	id: ObjectId,
	username: String,
	password: String,
	registered: Date
});

var data = new Schema({
	type: String,
	owner: String,
	content: String
});

Class('DataProvider',{
	methods: {
		authorizeUser: function(user) {},
		getData: function(id,rev) {},
		setData: function(id,newcontent) {}
		 }
});


exports.DataProvider = DataProvider;
