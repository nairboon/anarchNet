
// Run $ expresso -I lib (-c) -s
var assert = require('assert'),
	mongoose = require('mongoose'),
	config = require('../config.js'),
	db = require('db.js'),
	setup = require('setup.js'),
	DatabaseCleaner = require('database-cleaner'),
	ppm = require("ppm.js");
	
var databaseCleaner = new DatabaseCleaner('mongodb');
var _db = mongoose.connect(config.testingdburl);

var userid = "000000000000";

exports.test_setup = function(next) {
	// tests here

	setup.UserCreation(function(err,res){
		assert.isNull(err);
		assert.isNotNull(res._id);
		//userid = res._id;
		next();
	});
	
	setup.ScanLocalPackages(function(err){
		assert.isNull(err);
	});

	/*
	TODO: db.get,update,store
		ppm.appcacheload, updaterepo, cacheapp
	*/
};

exports.test_db = function(next) {
	var toStore = "Hello World!";
	db.store({content: toStore, owner:userid}, function(err,res){
		assert.isNull(err);

		db.getHead(res.id,function(err,res){
			assert.isNull(err);
			assert.equal(toStore,res.content);

			db.delete(res.id,userid,function(err,id){
				assert.isNull(err);
				assert.equal(id,res.id);
				db.getHead(id,function(err,res){
					assert.isNull(res);
					next();
				});
			});
		});
		
	});
};

exports.last = function() {
	//databaseCleaner.clean(_db);
	mongoose.disconnect();
};
