var	config = require('../config.js'),
	net = require("net");
var rpc_calls = {};
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
			//console.log("rpcres: ",data);
			res = JSON.parse(data);
			if(res.error === undefined || res.result === undefined) {
			    console.log("malformed response: ", res);
			}
			else if (res.id === undefined)
			{
			  console.log("no call id");
			  return;
			}
			else if(rpc_calls[res.id] !== undefined) {
				//console.log(rpc_calls[res.id]);
				rpc_calls[res.id](res.error,res.result);
				//delete rpc_calls[res.id];
			}
			else
			  console.log("call not found:", res.id,rpc_calls);
		});
		id_counter = 1;
}


an.prototype = {
	call: function(name,params,cb) {
		call_id = id_counter++;
		rpc_call = {jsonrpc:"2.0", id: call_id, method: name, params: params};
		rpc_calls[call_id] = cb;
		//console.log("rpc call:",call_id);
		client.write(JSON.stringify(rpc_call)+"\r\n");
	},
	close: function() {
		client.end();
		client.destroy();
	}
};

module.exports = new an();
