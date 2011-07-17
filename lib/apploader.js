var	db = require('./db.js'),
	ppm = require('./ppm.js');

exports.load = function (req,res,next) {
	
	console.log("loading cache:",req.params.id);
	var repoid = req.session.user.defaultRepo;
	var appid = req.params.id;

	if(req.session.user.userRepo) {
		console.log("using private repo");
		repoid = req.session.user.userRepo;
	}
	ppm.appcacheLoad(appid,repoid,function(err,ac){
		if(err) {
			console.log('appcacheLoad fails');
		
			ppm.updateRepo(repoid,function(err,repores){
				if(err)
					return next(new Error('Could not update repo: ' + err));

				ppm.cacheApp(appid,repores,function(err,appres){
					if(err || !appres)
						return	next( new Error('Could not cache that app: '+err.message));
						
					console.log("sending",appres);
					res.send(appres); // content of appcache
				});
			});
		}
		else {
			console.log("send cache");
			res.send(ac); // cache hit
		}
	});	
};
