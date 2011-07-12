var mongoose = require('mongoose'),
	hashlib = require('hashlib');

var Schema = mongoose.Schema, ObjectId = mongoose.Types.ObjectId;

/*   pkg   */
var Dependency = new Schema({
	name: String,
	version: String
});

var Package = new Schema({
	name: String,
	version: String,
	code: String,
	resources: String,
	depends: [Dependency]
});

var Repository = new Schema({
	sources: [String],
	branch: {type:String, default: "trunk"},
	revid: {type:String, default: ""},
	packages: [Package]
});

var Appcache = new Schema({
	name: String,
	revid: Schema.ObjectId,
	content: String
});

mongoose.model('package',Package);
mongoose.model('dependency',Dependency);
mongoose.model('repository',Repository);
mongoose.model('appcache',Appcache);
