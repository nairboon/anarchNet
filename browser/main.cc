/*
 * Copyright (C) 2010 Remo Hertig (nairboon)
 * https://launchpad.net/anarchNet
 *
 * This file is part of anarchNet.
 *
 * anarchNet is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * anarchNet is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with anarchNet.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <pthread.h>
#include <signal.h>
#include <iostream>
#include <sstream>
#include <glog/logging.h>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include "http/server.hpp"
#include "http/reply.hpp"
#include "http/request.hpp"
#include "http/file_handler.hpp"
#include "boost/filesystem.hpp"

#include "Util.pb.h"
#include "control_service_messages.pb.h"
#include "library/client.h"
#include "library/pb_json.h"
#include "third-party/json_spirit/json_spirit.h"
#include "third-party/json_spirit/json_spirit_stream_reader.h"


using namespace std;
namespace fs = boost::filesystem;

/**
 * @brief list the active apps from apps/active
 */
void list_apps(const http::server4::request&, http::server4::reply& rep)
{
	std::string fpath = "www/apps/active";
	if( !fs::exists( fpath)  && !fs::is_directory(fpath)) {
		rep.content = "Error: no app dir";
		return;
	}
	stringstream out;
	out << "[";
	fs::directory_iterator end_i;
	for( fs::directory_iterator i(fpath); i!= end_i; ++i) {
		if( fs::is_directory(i->status())) 
					out << "\"" << i->path().filename() << "\", ";
	}
	string b = out.str();
	rep.content = b.erase(b.size()-2) + "]";
}

/**
 * @brief converts json calls to pb:rpc -> daemon -> pb:rpc-> json response
 */
void rpc_gateway(const http::server4::request& req, http::server4::reply& rep)
{
	try
	{
		LOG(INFO) << req.content;
		json_spirit::Value value;
		json_spirit::Object object;
		google::protobuf::Message* m;

		json_spirit::read_or_throw(req.content,value);
			object = value.get_obj();
			string mname = json_spirit::find_str(object,"f");
			
			if(mname == "get") {
				GetRequest* gr = new GetRequest();
				gr->set_key(json_spirit::find_str(object,"key"));
				m = static_cast<google::protobuf::Message*>(gr);
			}	
			else if(mname == "put") {
				PutRequest *pr = new PutRequest();
				pr->set_key(json_spirit::find_str(object,"key"));
				pr->set_value(json_spirit::find_str(object,"value"));
				pr->set_ttl(json_spirit::find_int(object,"ttl"));
				m = static_cast<google::protobuf::Message*>(pr);
			}
			else if(mname == "deleteObject") {
				DeleteObjectRequest *pr = new DeleteObjectRequest();
				pr->set_key(json_spirit::find_str(object,"key"));
				m = static_cast<google::protobuf::Message*>(pr);
			}
			else if(mname == "storeObject") {
				StoreObjectRequest *pr = new StoreObjectRequest();
				pr->set_key(json_spirit::find_str(object,"key"));
				pr->set_value(json_spirit::find_str(object,"value"));
				pr->set_time(time(0));
				pr->set_app(json_spirit::find_str(object,"app"));
				pr->set_protocol(json_spirit::find_str(object,"protocol"));
				pr->set_public_(json_spirit::find_bool(object,"public"));
				m = static_cast<google::protobuf::Message*>(pr);
			}
			else if(mname == "getInfo") {
				m = new Void();
			}
			else {
				rep.content = "rpc malformed, unknown method: "+mname;
				return;
			} 
		
		
		LOG(INFO) << "rpc_gateway: " << mname;
		google::protobuf::Message* rpc_res = an::Query(mname,m);
		if(rpc_res != NULL)
			rep.content = an::JSONInterface::write( *rpc_res);
		else
			rep.content = "undefined method";
		}
		catch(std::runtime_error& e) {
			LOG(ERROR) << e.what();
			rep.content = string("parse Error: ")+ e.what() ;
		}
	catch(json_spirit::Error_position& e) {
		LOG(ERROR) << "json :" << e.reason_;
	}
		catch(...) {
				rep.content = "rpc malformed";
		}
/*	json_spirit::Stream_reader_thrower<istringstream,json_spirit::Value> reader(in);
	reader.read_next(value);
		string mname = value.get_array()[0].get_str();

		//if(value.type() != json_spirit::Value::array_type)
		reader.read_next(pl);
		if(pl.get_array().size() > 0) {
			rep.content = "no rpc request sent";
			return;
		}
		google::protobuf::Message* m;
		if(pl.get_array().size() == 0)
				LOG(INFO) << "nul";
		else
			LOG(INFO) << pl.get_array().size();
		string inp = json_spirit::write(pl.get_array()[0]);
		
		if(inp == "\"\"")
			m = new Void();
		else  {
			json_spirit::Object args = pl.get_array()[0].get_obj();

			if(mname == "get") {
				GetRequest* gr = new GetRequest();
				gr->set_key(args[0].value_.get_str());
				m = static_cast<google::protobuf::Message*>(gr);
			}	else {
				rep.content = "rpc malformed, method: "+mname;
			}
			return;
		}
		
	LOG(INFO) << "rpc_gateway: " << mname;
		google::protobuf::Message* rpc_res = an::Query(mname,m);
		if(rpc_res != NULL)
			rep.content = an::JSONInterface::write( *rpc_res);
		else
			rep.content = "undefined method";
	}
	catch(std::runtime_error& e) {
		LOG(ERROR) << e.what();
		rep.content = "parse Error";
	}*/
}
int main (int argc, char **argv) {
	google::InitGoogleLogging(argv[0]);
	
  // setting output to be stderr
  bool FLAGS_logtostderr;
	std::string FLAGS_log_dir;
	
	FLAGS_logtostderr = true;
	FLAGS_stderrthreshold = 0;

	
	an::anClient::instance().init();
		
	try
  {
		boost::asio::io_service io_service;
		http::server4::file_handler fh("www");
		fh.add_hook("list_apps",list_apps,"json");
		fh.add_hook("rpc",rpc_gateway,"json");

    // Launch the initial server coroutine.
    http::server4::server(io_service, "0.0.0.0", "8082",fh)();
		
    // Block all signals for background thread.
    sigset_t new_mask;
    sigfillset(&new_mask);
    sigset_t old_mask;
    pthread_sigmask(SIG_BLOCK, &new_mask, &old_mask);
		
    // Run server in background thread.
		LOG(INFO) << "start webserver";
    boost::thread t(boost::bind(&boost::asio::io_service::run, &io_service));
		
    // Restore previous signals.
    pthread_sigmask(SIG_SETMASK, &old_mask, 0);
		
    // Wait for signal indicating time to shut down.
    sigset_t wait_mask;
    sigemptyset(&wait_mask);
    sigaddset(&wait_mask, SIGINT);
    sigaddset(&wait_mask, SIGQUIT);
    sigaddset(&wait_mask, SIGTERM);
    pthread_sigmask(SIG_BLOCK, &wait_mask, 0);
    int sig = 0;
    sigwait(&wait_mask, &sig);
		
    // Stop the server and wait for the background thread to exit.
    io_service.stop();
    t.join();
		
  }
  catch (std::exception& e)
  {
    std::cerr << "exception: " << e.what() << "\n";
  }
	return EXIT_SUCCESS;
}