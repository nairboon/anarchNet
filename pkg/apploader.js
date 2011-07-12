	dp = require('../db.js'),
	p = require('../ppm.js');
var db = new dp.db();
var ppm = new p();

exports.load = function (req,res,next) {

	ppm.appcacheLoad(req.params.id,req.session,function(err,ac){
		if(err)
			console.log(new Error('Nothing cached'));
			
			// update, update cache, serve
			ppm.updateRepo(ac._id,function(err,repores){
					if(err)
						return next(new Error('Could not update repo: ' + err.message));
						
					ppm.cacheApp(req.params.id,repores,function(err,appres){
						if(err || !appres)
							return	next( new Error('Could not cache that app: '+err.message));
							
						res.send(appres); // content of appcache
					});
				});
	});
/*	db.getHead(req.params.id,function(err,appinfodoc){
		
		if(err) 
			return res.send( new Error('Could not load Document'));
			
		console.log(appinfodoc);
		appinfo = JSON.parse(appinfodoc.content);
		console.log("loading: ", appinfo.name);
		
		/* prepare code 
			search cache
				put
			else
				search in repos
					
				else
					err no app!!
		
		*
		for (var code in appinfo.code)
			console.log(code);
		var code ="alert('test');";
		
			
		res.render('app', {
		    name: appinfo.name,
			code: code,
		    session: req.session,
			layout: false
		  });
		
	});

*/	  
	
	
	
};