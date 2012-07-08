
/* Run $ expresso -I lib (-c) -s
 * current coverage 72.2%
 */
var assert = require('assert'),
	mongoose = require('mongoose'),
	config = require('../config.js'),
	db = require('db.js'),
	setup = require('setup.js'),
	ppm = require("ppm.js");
	
var _db = mongoose.connect(config.testingdburl);

var userid = "000000000000";
var workingrepo = "";

exports.test_setup = function(next) {

	setup.UserCreation(function(err,res){
		assert.isNull(err);
		assert.isNotNull(res._id);
		//userid = res._id;		
			
	setup.ScanLocalPackages(function(err,mlid){
		assert.isNull(err);
		setup.createDefaultRepo(mlid,function(err,res){
			assert.isNull(err);
			
			db.getSettings('defaultRepo',function(err,repoid){
				assert.isNull(err);
				assert.equal(res,repoid);
				workingrepo = repoid;

				ppm.appcacheLoad("nonexistingAPP",repoid,function(err,ac){
					assert.isNotNull(err);
				});
				
				ppm.updateRepo(repoid,function(err,res){
					assert.isNull(err);
					
					ppm.cacheApp("todoapp",res,function(err,res){
						assert.isNull(err);
						next();
					});
				});
			});
		});
	});
});
	/*
	TODO: db.get,update,store
		ppm.appcacheload, updaterepo, cacheapp
	*/
};

exports.test_db = function(next) {
	var toStore = "Hello World!";
	var updated = "Abllo Worldo!";
	db.store({content: toStore, owner:userid}, function(err,res){
		assert.isNull(err);

		db.getHead(res.id,function(err,res){
			assert.isNull(err);
			assert.equal(toStore,res.content);
			
			db.update(res.id,updated,res.head,null,function(err,res){
				assert.isNull(err);
				assert.equal(updated,res.content);
				
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
	});
};

exports.last = function() {
	//databaseCleaner.clean(_db);
	mongoose.disconnect();
};
