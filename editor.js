	dp = require('./db.js');
var db = new dp();

exports.index = function(req, res){
  db.getAll(function(r){
	res.render('editor/list',{data:r,title:'List',session:req.session});
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
  res.send('edit forum ' + req.params.id);
};

exports.update = function(req, res){
  res.send('update forum ' + req.params.id);
};

exports.destroy = function(req, res){
  res.send('destroy forum ' + req.params.id);
};

exports.id = 'id';