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
            this.data = new Hash();
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