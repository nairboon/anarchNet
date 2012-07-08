var form = require('express-form'),
	filter = form.filter,
	validate = form.validate,
	db = require('./db.js');

exports.emergency_shutdown = function(req,res,next){
	console.log("system is going down...");
	/*
	clearing all identities from memory, closing encrypted files, etc... 
	*/
}
exports.create_identity_form = form(
		filter("username").trim(),
		validate("username").required(),
		validate("password").required(),
		validate("email").required().isEmail()
	);

exports.create_identity = function(req,res,next){
		if(!req.form.isValid) {
			console.log(req.form.errors);
		}
		else {
			
		}
};
