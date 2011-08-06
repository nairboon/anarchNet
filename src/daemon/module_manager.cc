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

#include <iostream>
#include <string>
#include <glog/logging.h>
#include <boost/filesystem/fstream.hpp>
#include <boost/asio.hpp>
#include <boost/tokenizer.hpp>

#include "anarchNet.h"
#include "module_manager.h"
#include "config_manager.h"

using std::string;

using boost::asio::ip::tcp;

namespace an
{

bool ModuleManager::init()
{

	
	return true;
}
	
/*	bool ModuleManager::bootstrapp_from_cache() {
		JoinCallback cb;

		LOG(INFO) << "trying to bootstrap from cache";
		node_->Join(ConfigManager::instance().kad_config(), boost::bind(&JoinCallback::Callback, &cb, _1));
		while (!cb.result_arrived())
      boost::this_thread::sleep(boost::posix_time::milliseconds(500));
		
    if (cb.success()) 
			return true;
		
			return false;
	}
	bool ModuleManager::bootstrapp_from_server() {
		JoinCallback cb;
		std::string res;
		foreach (string server_url, ConfigManager::instance().config()["bs-list"].as< std::vector<string> >()) {
			try {
				if(!http_request(server_url,&res))
					continue;
			
			
			std::stringstream in(res);
			
			typedef boost::tokenizer< boost::escaped_list_separator<char> > Tokenizer;
			std::vector< string > vec;
			string line;
			
			while (getline(in,line))
			{
        Tokenizer tok(line);
        vec.assign(tok.begin(),tok.end());
				
				write_to_kadconfig(ConfigManager::instance().kad_config(),
													 vec[0], vec[1],
													 atoi(vec[2].c_str()), "127.0.0.1", ANARCHNET_PORT);
				
				LOG(INFO) << "trying to bootstrap from "<< vec[1] <<":"<<vec[2];
				node_->Join(ConfigManager::instance().kad_config(), boost::bind(&JoinCallback::Callback, &cb, _1));
				while (!cb.result_arrived())
					boost::this_thread::sleep(boost::posix_time::milliseconds(500));
				
				if (cb.success()) {
						LOG(INFO) << "connected!";
					return true;
				}
			}
			}
			catch(...) {
				LOG(WARNING) << "error occured while loading: " << server_url;
			}
		}
	
		return false;
	}
	
	bool ModuleManager::kad_init()
	{
		trans_handler_.Register(new transport::TransportUDT, &trans_id_);
		chmanager_ = new rpcprotocol::ChannelManager(&trans_handler_);
		
    node_ = new kad::KNode(chmanager_, &trans_handler_, kad::VAULT, kad::K,
													 kad::kAlpha, kad::kBeta, kad::kRefreshTime, "", "", false,false);
    node_->set_transport_id(trans_id_);
    if (!chmanager_->RegisterNotifiersToTransport() ||
        !trans_handler_.RegisterOnServerDown(boost::bind(
																												 &kad::KNode::HandleDeadRendezvousServer, node_, _1))) {
      return false;
    }
    if (0 != trans_handler_.Start(ANARCHNET_PORT, trans_id_) || 0!= chmanager_->Start()) {
      LOG(ERROR) << "Unable to start node on port " << ANARCHNET_PORT;
      return false;
    }
		
    JoinCallback cb;
				
		bool online = false;
		// something is in cache
		if (!kadconfig_empty(ConfigManager::instance().kad_config())) 
			if(bootstrapp_from_cache())
				online = true;
		
		// no cache or chache bootstrapp failed
		if(!online)
			if(!bootstrapp_from_server()) { // bootstrapp failed
				LOG(INFO) << "No bootstraping Info, start on " << ANARCHNET_PORT;
				node_->Join(ConfigManager::instance().kad_config(), "127.0.0.1",
										ANARCHNET_PORT, boost::bind(&JoinCallback::Callback, cb, _1));
				while (!cb.result_arrived())
					boost::this_thread::sleep(boost::posix_time::milliseconds(500));
				
				if (!cb.success()) {
					LOG(ERROR) <<"Node failed to create a network.";
					trans_handler_.Stop(trans_id_);
					chmanager_->Stop();
					return false;
				}
			}
				
		LOG(INFO) << "Nodeid: " << base::EncodeToHex(node_->contact_info().node_id());
		
		write_to_kadconfig(ConfigManager::instance().kad_config(),
											 node_->node_id().ToStringEncoded(), node_->host_ip(),
											 node_->host_port(), node_->local_host_ip(), node_->local_host_port());
		
		return true;
	}
	void ModuleManager::create_network(JoinCallback* cb)
	{
		
		
	}
	
	bool ModuleManager::kad_shutdown()
	{
		trans_handler_.StopPingRendezvous();
    node_->Leave();
		trans_handler_.Stop(trans_id_);
    chmanager_->Stop();
		return true;
	}
	bool ModuleManager::kadconfig_empty(const std::string &path) {
		base::KadConfig kadconfig;
		try {
			boost::filesystem::ifstream input(path.c_str(),
																				std::ios::in | std::ios::binary);
			if (!kadconfig.ParseFromIstream(&input)) {;
				return true;
			}
			input.close();
			if (kadconfig.contact_size() == 0)
				return true;
		}
		catch(const std::exception &) {
			return true;
		}
		return false;
	}
	
	bool ModuleManager::write_to_kadconfig(const std::string &path, const std::string &node_id,
																		const std::string &ip, const boost::uint16_t &port,
																		const std::string &local_ip, const boost::uint16_t &local_port) {
		base::KadConfig kadconfig;
		try {
			base::KadConfig::Contact *ctc = kadconfig.add_contact();
			ctc->set_ip(ip);
			ctc->set_node_id(node_id);
			ctc->set_port(port);
			ctc->set_local_ip(local_ip);
			ctc->set_local_port(local_port);
			boost::filesystem::fstream output(path.c_str(), std::ios::out |
																				std::ios::trunc | std::ios::binary);
			if (!kadconfig.SerializeToOstream(&output)) {
				output.close();
				return false;
			}
			output.close();
		}
    catch(const std::exception &) {
			return false;
		}
		return boost::filesystem::exists(path);
	}
*/	
	bool ModuleManager::http_request(const std::string& _url,std::string *_response)
	{
    std::size_t index = _url.find_first_of('/');
    std::string url_server = _url.substr(0,index);
		std::string url_file = _url.substr(index);
		
		LOG(INFO) << "bootstrapping " << url_file << " from " << url_server;
		
		std::stringstream out;
		try {
		boost::asio::io_service io_service;
		
    // Get a list of endpoints corresponding to the server name.
    tcp::resolver resolver(io_service);
    tcp::resolver::query query(url_server, "http");
    tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
    tcp::resolver::iterator end;
		
    // Try each endpoint until we successfully establish a connection.
    tcp::socket socket(io_service);
    boost::system::error_code error = boost::asio::error::host_not_found;
    while (error && endpoint_iterator != end)
    {
      socket.close();
      socket.connect(*endpoint_iterator++, error);
    }
    if (error)
      throw boost::system::system_error(error);
		
    // Form the request. We specify the "Connection: close" header so that the
    // server will close the socket after transmitting the response. This will
    // allow us to treat all data up until the EOF as the content.
    boost::asio::streambuf request;
    std::ostream request_stream(&request);
    request_stream << "GET " << url_file << " HTTP/1.0\r\n";
    request_stream << "Host: " << url_server << "\r\n";
    request_stream << "Accept: */*\r\n";
    request_stream << "Connection: close\r\n\r\n";
		
    // Send the request.
    boost::asio::write(socket, request);
		
    // Read the response status line.
    boost::asio::streambuf response;
    boost::asio::read_until(socket, response, "\r\n");
		
    // Check that response is OK.
    std::istream response_stream(&response);
    std::string http_version;
    response_stream >> http_version;
    unsigned int status_code;
    response_stream >> status_code;
    std::string status_message;
    std::getline(response_stream, status_message);
    if (!response_stream || http_version.substr(0, 5) != "HTTP/")
    {
      LOG(WARNING) << "http: Invalid response\n";
      return false;
    }
    if (status_code != 200)
    {
      LOG(WARNING) << "http: Response returned with status code " << status_code << "\n";
      return false;
    }
		
    // Read the response headers, which are terminated by a blank line.
    boost::asio::read_until(socket, response, "\r\n\r\n");
		
    // Write whatever content we already have to output.
			std::stringstream buffer;
    if (response.size() > 0)
      buffer << &response;
		
    // Read until EOF, writing data to output as we go.
    while (boost::asio::read(socket, response,
														 boost::asio::transfer_at_least(1), error))
      out << &response;
    if (error != boost::asio::error::eof)
      throw boost::system::system_error(error);
  }
  catch (const std::exception& e)
  {
    LOG(ERROR) << "http: Exception: " << e.what() << "\n";
		return false;
		}

	*_response = out.str();
return true;
}
}
