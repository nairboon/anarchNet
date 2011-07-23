var mongoose = require('mongoose'),
	hashlib = require('hashlib');

var Schema = mongoose.Schema, ObjectId = mongoose.Types.ObjectId;

/*   pkg   */
var Dependency = new Schema({
	name: String,
	version: String
});

var Map = new Schema({
	key: String,
	value: String
});

var Package = new Schema({
	name: String,
	version: String,
	repo: String, // to which repo it belongs
	revid: String,
	code: String,
	html: String,
	resources: [Map],
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
	revid: String,
	content: String
});

mongoose.model('dependency', Dependency);
mongoose.model('map', Map);
mongoose.model('package', Package);
mongoose.model('repository', Repository);
mongoose.model('appcache', Appcache);
