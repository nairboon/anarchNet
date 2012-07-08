var form = require('express-form'),
	filter = form.filter,
	validate = form.validate,
	db = require('./lib/db.js');


exports.login = function(req,res){
	if(req.session.auth)
		res.redirect("/");
	res.render('auth/login', {
	    title: 'Login',
		session: req.session
	  });
};
exports.register = function(req,res){
	res.render('auth/register', {
	    title: 'Register',
		session: req.session
	  });
};
exports.logout = function(req,res){
	req.session.destroy();
	res.render('index', {
	    title: 'logedout',
		session: req.session
	  });
}

exports.login_form = form(
	validate("username").required(),
	validate("password").required()
	);
exports.register_form = form(
		filter("username").trim(),
		validate("username").required(),
		validate("password").required(),
		validate("email").required().isEmail()
	);

exports.register_post = function(req,res,next){
		if(!req.form.isValid) {
			console.log(req.form.errors);
		}
		else {
			db.registerUser(req.form, function(error) {
				if(error)
					console.log(error);
					
				res.send("reg successful");
			});
		}
};

exports.login_post = function(req,res){
	db.authorizeUser(req.form,function(error,r){
		if(!r)
			res.send("login failed");
		else {
			req.session.auth = true;
			db.getSettings('defaultRepo',function(err,res){
				if(err)
					res.send(err.message);
				req.session.user = {id: r._id, userRepo: r.repo, defaultRepo:res};
				console.log(req.session.user);
				req.session.save(function(err){});
			});
		res.redirect("/");
	}
	});

};
