	dp = require('./db.js'),
	p = require('./ppm.js');
var db = new dp.db();
var ppm = new p();

exports.load = function (req,res,next) {
	console.log("loading cache:",req.params.id);

	ppm.appcacheLoad(req.params.id,req.session,function(err,ac){
		if(err) {
			console.log('appcacheLoad fails');
			
			ppm.updateRepo(ac._id,function(err,repores){
					if(err)
						return next(new Error('Could not update repo: ' + err.message));
						
					ppm.cacheApp(req.params.id,repores,function(err,appres){
						if(err || !appres)
							return	next( new Error('Could not cache that app: '+err.message));
							
							console.log("sending",appres);
						res.send(appres); // content of appcache
					});
				});
		}
		else {
		console.log("send2");
		res.send(ac); // cache hit
		}
	});	
};
