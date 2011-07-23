Array.prototype.indexOf = function(obj) {
  for (var i = 0; i < this.length; i++) {
    if (this[i] == obj)
      return i;
  }
  return -1;
}

Array.prototype.has = function(obj) {
  return this.indexOf(obj) >= 0;
}

module.exports = {
	mime: {
		type: {
			"gif": "image/gif",
			"gtar": "application/x-gtar",
			"tar": "application/x-tar",
			"zip": "application/zip",
			"gz": "application/x-gzip",
			"html": "text/html",
			"jpeg": "image/jpeg",
			"jpg": "image/jpeg",
			"json": "application/json",
			"css": "text/css",
			"pdf": "application/pdf",
			"png": "image/png",
			"js": "application/javascript",
		},
		binary:
			[
				"jpeg",
				"jpg",
				"png",
				"png",
				"gz",
				"zip",
				"tar",
				"gtar",
				"gif"
			]
		}
};