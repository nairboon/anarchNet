/*
Namespace: Backend
*/
/* 
    Class: Object
        root object, "interface" to the dht    
*/
an.Object = new Class({
        Implements: Events,
        initialize: function() {
            this.data = new Hash();
        }
});
/* section Object
*/
// holds objects (dht entry)
an.Index = new Class({
        initialize: function() {
            this.objects = new Array();
        }
});

/*
    Class: Tags
            generic tag class
        
    Arguments:
        protocol
        
    Members:
            search
    
*/
an.Tags = new Class({
        initialize: function(p) {
            this.protocol = p;
        },
        /*
        Function: search
            Searchs in the DHT
            
        Arguments:
            tag - (string) tag to search for
            cb  - (function) callback with result
            
        Return:
            (array) a list with the values linked from the tag
            
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

an.rpc = function(fname,args,callback) {

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

an.put = function(k,data,t,cb) {
    an.rpc("put",{key: k, value: data, ttl: t},cb);
}
an.get = function(key,cb) {
    this.c = cb;
    an.rpc("get",{key: key},function(data) {
        if(data == null)
            alert("probably not connected, is anDaemon running? try to restart anBrowser");
        if(data.status.ok === false)
            throw "Error";
    this.c(data);
    }.bind(this));
}

an.getInfo = function(cb) {
    an.rpc("getInfo",{},cb);
}