var	config = require('../config.js'),
	net = require("net");
var rpc_calls = {};
		old_buffer = "";
		waiting = false;
function occurrences(string, substring){

    var n=0;
    var pos=0;

    while(true){
        pos=string.indexOf(substring,pos);
        if(pos!=-1){ n++; pos+=substring.length;}
        else{break;}
    }
    return(n);
}

function isCompletePackage(inp) {
	var co = occurrences(inp,"{");
	var ce = occurrences(inp,"}");
	console.log(co,ce);
	if(co != ce || co == 0 || ce == 0)
		return false;
	else 
		return true;
}

	function _cb(res) {
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

		}

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
		client.addListener("end",function() {
			console.log("END");
		});
		client.addListener("data", function(data){
			//console.log("rpcres: ",data);
			console.log("size: ", data.length);

				if(waiting) {
					var data = old_buffer + data;
					old_buffer ="";
					waiting = false;
				}


			if(data.indexOf("{$}") != -1) // complete package
			{
				var newset = data.split("{$}");
				console.log(newset[0]);
				res = JSON.parse(newset[0]);
			if(newset.length > 1) // begin of next msg already here
				{
				if(occurrences(data,"{$}") == newset.length) {
					console.loge("complete multi msg");
					for(var i = 0;i<newset.length;i++)
						_cb(JSON.parse(newset[i]));

					return;
				}
				else
				{
					for(var i = 0;i<newset.length-1;i++)
						_cb(JSON.parse(newset[i]));
					
					console.log("incomplete multi",occurrences(data,"{$}"),newset.length );
				old_buffer = newset[newset.length-1];
				waiting = true;
				console.log("waiting for more data");	
				return;			
				}
				

			}
}
			else {
				console.log("waiting...");
				waiting = true;
				old_buffer = data;
				return;
				//res = JSON.parse(data);
			}
			_cb(res);

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
