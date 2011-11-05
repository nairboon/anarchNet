var	config = require('../config.js'),
	net = require("net");

var an = function() {
		client = net.createConnection(config.anarchnet_port, config.anarchnet_url);
		client.setEncoding('ascii');
		client.addListener("connect", function(){
			console.log("connected to anarchnet");
		});
		client.addListener("error", function(err){
			console.log("connecting to anarchnet failed:",err.message);
			console.log("is the daemon running???");
			process.exit();
		});
		client.addListener("data", function(data){
			console.log(data);
			res = JSON.parse(data);
			if(res.error == "undefined" || res.data == "undefined") {
			    console.log("malformed response: ", res);
			}
			if(rpc_calls[res.id] != "undefined") {
				rpc_calls[res.id](res.error,res.data);
				delete rpc_calls[res.id];
			}
			console.log("call not found");
		});

		rpc_calls = {};
		id_counter = 1;
}

an.prototype = {
	call: function(name,params,cb) {
		call_id = id_counter++;
		rpc_call = {jsonrpc:"2.0", id: call_id, method: name, params: params};
		rpc_calls[call_id] = cb;
		console.log(JSON.parse(JSON.stringify(rpc_call)));
		client.write(JSON.stringify(rpc_call));
	}
};

module.exports = new an();