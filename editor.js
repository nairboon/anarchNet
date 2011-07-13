	db = require('./lib/db.js');

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
		
		req.body.owner = req.session.user.id;
		console.log(req.body);
		db.store(req.body,function(r){
			res.send("your data:"+r._id);
		});
};

exports.show = function(req, res){
	db.get(req.params.id,function(r){
		res.send(r);
	});
};

exports.edit = function(req, res){
	db.getHead(req.params.id,function(err,r){
		console.log("edit doc:",req.params.id);
		res.render('editor/edit',{title:'Edit',session:req.session,item:r});
	});
};

exports.update = function(req, res){
	console.log(req.params,req.body);
	db.update(req.params.id,req.body.content,req.body.branch,req.session,function(r){
		res.send(r);
	});	
};

exports.destroy = function(req, res){
  
	db.delete(req.params.id,req.session,function(r){
		res.send('destroy' + req.params.id);
	});	
};

exports.id = 'id';
