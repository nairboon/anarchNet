/**
 * The backend module provides methods for communication with the network
 * @module backend
 */

/** 
 * root object, "interface" to the dht    
 * @class Object
 * @namespace an
 */
an.Object = new Class({
        Implements: Events,
        initialize: function() {
            this._an_meta = { data: new Hash(), public: true, key : "",
                            app: "genApp", protocol: "genP"};
        },
        addProperty: function(name) {
            if (typeof(this[name]) === "function") 
                throw "Can't add property: "+name+" function already exist!";
                this["set"+name] = function(value) {
                    this._an_meta.data.set(name,value);
                }.bind(this);
                this[name] = function() {
                    return this._an_meta.data.get(name);
                }.bind(this);
        },
        save: function() {
            an.storeObject(this._an_meta);
        },
        remove: function() {
            an.deleteObject(this._an_meta);
        }
});

/**
 * holds objects (dht entry)
 * @class Index
 * @namespace an
 */
an.Index = new Class({
        initialize: function() {
            this.objects = new Array();
        }
});


/**
 * dht-db interface
 * @class DHTInterface
 * @namespace an
 * @constructor
 * @param app {string} the used application identifier
 * @param p {string} the used protocol identifier
 */
an.DHTInterface = new Class({
        initialize: function(app,p) {
            this.application = app;
            this.protocol = p;
        },
        /**
         * Searchs the tag in the DHT
         * @method searchTag
         * @param tag {string} tagtype defined by the protocol
         * @param tag {string} tag to search for
         * @param cb {function} callback with result
         * @return {null} cb is called with [{string}tagname,{array}keys]
        */
        searchTag: function(tagtype, tag,cb) {
                an.get("T$_"+this.protocol+"_"+tagtype+"_"+tag+"%",function(res) {
                    for(var key in res.values) {
                        tmp = key.split(":");
                        an.get(tmp[1],function(res) {
                            arguments[2](arguments[1],res.values);
                        }.pass([tmp[0],this.cb]));
                    }
                }.bind(this));
        }
});
/**
 * generic tag class
 * @class Tags
 * @namespace an
 * @constructor
 * @param p {string} the used protocol identifier
 */
an.Tags = new Class({
        initialize: function(p) {
            this.protocol = p;
        },
        /**
         * Searchs in the DHT
         * @method search
         * @param tag {string} tag to search fo
         * @param cb {function} callback with result
         * @return {array} a list with the values linked from the tag
        */
        search: function(tag,cb) {
            this.tags = [];
            an.get("T$"+this.protocol+tag,function(res) {
                for(var key in res.values)
                    an.get(key,function(res) {
                        this.tags.push(res.values[0]);
                    }.bind(this));
                }.bind(this));
        }
}); 

/**
 * rpc functions
 * @class rpc
 * @static
 * @namespace an
 */
an.rpc = { };

/**
 * low-level daemon query, executes a RPC
 * @method call
 * @namespace an.rpc
 * @param fname {string} the function to execute
 * @param args {object} the parameters for the function
 * @param callback {function} the callback when the rpc is executed
 */
an.rpc.call = function(fname,args,callback) {

    args.f = fname;
    var req = JSON.encode(args);
 
    new Request.JSON({url: "rpc", data: req, onSuccess: callback }).get();
}

an.printJSON = function(el,data) {
    var out = "";
  
    for(var key in data) {
        if($type(data[key]) == "array") {
            out += "<p><strong>"+key+": </strong><ul>";
                data[key].forEach(function(item) { 
                        out += "<li>" + item + "</li>";
                        },this);
            out += "</ul></p>";
        }
        else {
            out += "<p><strong>" + key + ": </strong>"+data[key]+"</p>";
        }
    }
    el.append(out);
}

/**
 * DHT put/store
 * @method put
 * @namespace an.rpc
 * @param k {string} dht key
 * @param data {object} the data which will be stored
 * @param t {int} time to live of the store
 * @param cb {function} the callback when the rpc is executed
 */
an.rpc.put = function(k,data,t,cb) {
    an.rpc.call("put",{key: k, value: data, ttl: t},cb);
}

/**
 * DHT get
 * @method get
 * @namespace an.rpc
 * @param key {string} dht key
 * @param callback {function} the callback when the rpc is executed
 */
an.rpc.get = function(key,cb) {
    this.c = cb;
    an.rpc.call("get",{key: key},function(data) {
        if(data == null)
            alert("probably not connected, is anDaemon running? try to restart anBrowser");
        if(data.status.ok === false)
            throw "Error";
    this.c(data);
    }.bind(this));
}

/**
 * stats/general info about the network/host
 * @method getInfo
 * @namespace an.rpc
 * @param callback {function} the callback when the rpc is executed
 */
an.rpc.getInfo = function(cb) {
    an.rpc.call("getInfo",{},cb);
}

/**
 * store in the object in the db
 * @method storeObject
 * @namespace an.rpc
 * @param object {Object} the object to store
 * @param callback {function} the callback when the rpc is executed
 */
an.rpc.storeObject = function(object,cb) {
    var o = object._an_meta;
    an.rpc.call("storeObject",{key: o.key, value: o.data, app: o.app, protocol: o.protocol, public: o.public},cb);
}

/**
 * delete the object from the db
 * @method deleteObject
 * @namespace an.rpc
 * @param o {Object} the object to store
 * @param callback {function} the callback when the rpc is executed
 */
an.rpc.deleteObject = function(o,cb) {
    an.rpc.call("deleteObject",{key: o._an_meta.key},cb);
}