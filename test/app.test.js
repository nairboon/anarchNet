
// Run $ expresso -I lib (-c)
var TEST = true;
var assert = require('assert'),
	mongoose = require('mongoose'),
	config = require('../config.js'),
	setup = require('../setup.js'),
	DatabaseCleaner = require('database-cleaner'),
	ppm = require("ppm.js");
var databaseCleaner = new DatabaseCleaner('mongodb');
var _db;
exports = module.exports;

exports.setup = function() {
	// test setup
	_db = mongoose.connect(config.testingdburl);
	
};

exports.test = function() {
	// tests here
	setup.UserCreation(function(err){
		assert.isNull(err);
	});
	
	/*
	TODO: db.get,update,store
		ppm.appcacheload, updaterepo, cacheapp
	*/
}


////////////////////
exports.teardown = function(){
	databaseCleaner.clean(db);
	mongoose.disconnect();
};

