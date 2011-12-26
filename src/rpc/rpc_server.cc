/*
 * Copyright (C) 2010, 2011 Remo Hertig (nairboon)
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

#include <boost/thread.hpp>
#include "rpc_server.h"
#include "logger.h"


 
namespace an
{

RpcServer::RpcServer(boost::asio::io_service& io_service, short int port, std::size_t thread_pool_size)
: io_service_(io_service), acceptor_(io_service),new_connection_(new connection(io_service_, _jsonHandler)), thread_pool_size_(thread_pool_size)
{
	  std::stringstream ports;
	  ports << port;
	  
  	  // Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
	  boost::asio::ip::tcp::resolver resolver(io_service_);
	  boost::asio::ip::tcp::resolver::query query("0.0.0.0", ports.str());
	  boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);
	  acceptor_.open(endpoint.protocol());
	  acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
	  acceptor_.bind(endpoint);
	  acceptor_.listen();
	  acceptor_.async_accept(new_connection_->socket(),
				 boost::bind(&RpcServer::handle_accept, this,
					     boost::asio::placeholders::error));
}

 void RpcServer::AddMethod(Json::Rpc::CallbackMethod* method)
 {
      _jsonHandler.AddMethod(method);
 }

    void RpcServer::DeleteMethod(const std::string& method)
    {
      _jsonHandler.DeleteMethod(method);
    }
	RpcServer::~RpcServer()
	{

	}
void RpcServer::run()
{
// Create a pool of threads to run all of the io_services.
  std::vector<boost::shared_ptr<boost::thread> > threads;
  for (std::size_t i = 0; i < thread_pool_size_; ++i)
  {
    boost::shared_ptr<boost::thread> thread(new boost::thread(
          boost::bind(&boost::asio::io_service::run, &io_service_)));
    threads.push_back(thread);
  }
LOG(INFO) << thread_pool_size_ << " threads launchd: " << boost::this_thread::get_id();

  // Wait for all threads in the pool to exit.
  for (std::size_t i = 0; i < threads.size(); ++i)
    threads[i]->join();
}


	void connection::handle_read(const boost::system::error_code& error,
									 size_t bytes_transferred)
	{
	  LOG(INFO) << __FUNCTION__;
		if (!error)
		{
			std::istream request_stream(&request_buf_);
			std::stringstream os;
			std::copy(std::istream_iterator<char>(request_stream), std::istream_iterator<char>(), std::ostream_iterator<char>(os));
		      

		     
			std::string data = os.str();
			LOG(INFO) << "got: " << bytes_transferred;// << ": " << data;

			if(!data.size())
			{
			  LOG(INFO) << "data is null";	 
			  return;
			}
    
			//try {
			boost::json::Value response;
				_jsonHandler.Process(std::string(data), response);
			//}
			//catch(boost::json::

			if(!response.is_null())
			{
				std::string rep = _jsonHandler.GetString(response);
				LOG(INFO) << "send: " << rep;
			boost::asio::async_write(socket_,
															 boost::asio::buffer(rep, rep.size()),
															 strand_.wrap(boost::bind(&connection::handle_write, shared_from_this(),
																					 boost::asio::placeholders::error)));
			}
		}
		else
		  LOG(INFO) << "error: " << error.default_error_condition().value() << " " << error.category().name();
	}
}
