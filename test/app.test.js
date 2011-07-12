
// Run $ expresso -I lib (-c)

var assert = require('assert'),
	mongoose = require('mongoose'),
	DatabaseCleaner = require('database-cleaner'); 
var databaseCleaner = new DatabaseCleaner('mongodb');
var p = require("ppm.js");
var _db;
exports = module.exports;
exports.setup = function() {
	// test setup
	_db = mongoose.connect(config.testingdburl);
	
};

exports.test = function() {
	// tests here
}



////////////////////
exports.teardown = function(){
	databaseCleaner.clean(db);
	mongoose.disconnect();
};

