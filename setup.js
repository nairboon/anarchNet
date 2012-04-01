/*
 *	setup for parannus
 *	creates admin user and imports pkgs
 */

var util = require('./lib/util.js'),
	config = require('./config.js'),
	db = require('./lib/db.js'),
	ppm = require('./lib/ppm.js'),
	setup = require("./lib/setup.js");



console.log(util.system_id);

var an = require("./lib/an.js");

/*db.store_ht("lamekey","mycontnet",function(err,data){
  console.log("calling get for:",data);
  db.get_ht("lamekey", function(err,data) {
    console.log("got:",data);
  });
});
*
db.store_with_id("lamekey",{a:"mycontnet"},function(err,data){
  console.log("calling get for:",data.id);
  db.get(data.id, function(err,data) {
    console.log("got:",data);
  });
});
*/
/*
var x = new db.obj("","hahah");
var obj = db.odb.create("abcdefg");
obj.save(function(err,res) {
	console.log("res: ",res);
});
console.log(x);
x.save(function(e,r){
console.log("x:",x,"!=0");
	var y = new db.obj();
	console.log("new x " ,r.id);
	y.load(r.id);
	console.log(r.obj,y.obj);
	r.obj = "lalalalal";
	r.save(function(e,p) {
		console.log(p);
	});
}
);
*/

console.log("scan packages...");
setup.ScanLocalPackages(function(err,mlid){
	console.log("packages scaned");
		console.log("storing master list...",mlid);
		/*db.store_ht(util.system_id+"masterlist",mlid, function(err){
			if(err)
				throw new Error("could not save masterlist");
*/
			setup.createDefaultRepo(mlid,function(err,res){
				if(err)
					throw err;
				console.log("repo created!",res);
				
				/*ppm.updateRepo(res,function(err,repores){
				if(err)
					return next(new Error('Could not update repo: ' + err));
				*/ppm.cacheApp("user",res,function(err,appres){
					if(err || !appres)
						return	next( new Error('Could not cache that app: '+err.message));

					console.log("sending",appres);
					//db.close();
					var abpath = process.cwd() + "/repo/user/img/bg.jpg";
db.put_file(abpath,function(err,res) {
	console.log("file:" , res);
	db.get_file(res.id,function(err,res) {
	console.log("file2:" , res);
	});
});
				});/*
				});*/
				//db.close();
			});
		//});
});





