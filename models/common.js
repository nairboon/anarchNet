var mongoose = require('mongoose'),
	hashlib = require('hashlib'),
	config = require('../config.js');

var Schema = mongoose.Schema, ObjectId = mongoose.Types.ObjectId;

var User = new Schema({
	username: String,
	password: {type:String, set: function(inp){ return hashlib.sha256(inp);}},
	email: {type:String, set: function(inp){ return inp.toLowerCase();}},
	registered: {type: Date, default: Date.now},
	count: {type:Number, default: 0},
	repo: Schema.ObjectId
});

var Settings = new Schema({
	key: {type:String, unique: true},
	value: String
});

mongoose.model('user',User);
mongoose.model('settings',Settings);
