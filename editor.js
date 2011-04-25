	dp = require('./db.js');
var db = new dp();

exports.list = function(req, res){
  db.getAll(function(r){
	res.render('editor/list',{items:r,title:'List',session:req.session});
});
};

exports.new = function(req, res){
	if(!req.session.auth)
		res.send("please log in!");
  res.render('editor/new',{title:'New',session:req.session});
};

exports.create = function(req, res){
	if(!req.session.auth)
		res.send("please log in!");
		
		req.body.owner = req.session.userid;
		console.log(req.body);
		db.addData(req.body,function(r){
			res.send("your data:"+r._id);
		});
};

exports.show = function(req, res){
	db.getData(req.params.id,null,function(r){
		res.send(r);
	});
};

exports.edit = function(req, res){
	db.getData(req.params.id,null,function(r){
		console.log("edit doc:",req.params.id);
		res.render('editor/edit',{title:'Edit',session:req.session,item:r});
	});
};

exports.update = function(req, res){
	console.log(req.params,req.body);
	db.update(req.params.id,req.body,req.session,function(r){
		res.send(r);
	});
	
 // res.redirect('editor/list');
};

exports.destroy = function(req, res){
  res.send('destroy forum ' + req.params.id);
};

exports.id = 'id';
