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

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/noncopyable.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "jsonrpc_handler.h"
#include "logger.h"

#ifndef RPC_SERVER_H
#define RPC_SERVER_H

namespace an
{

	using boost::asio::ip::tcp;
	
	class connection
	 : public boost::enable_shared_from_this<connection>,
    private boost::noncopyable
		{
		public:
			connection(boost::asio::io_service& io_service,Json::Rpc::Handler& jsh)
			: socket_(io_service), strand_(io_service), jsonHandler_(jsh)
			{

			}
			
			tcp::socket& socket()
			{
				return socket_;
			}
			
			void start()
			{
			  async_read_until(socket_, request_buf_, "\r\n" ,strand_.wrap(bind(&connection::handle_read, shared_from_this(),
													boost::asio::placeholders::error,
													boost::asio::placeholders::bytes_transferred)));
			}
			
		private:
			void handle_read(const boost::system::error_code& error,
											 size_t bytes_transferred);
			
			void handle_write(const boost::system::error_code& error)
			{
				if (!error)
				{
				  async_read_until(socket_, request_buf_, "\r\n" ,strand_.wrap(boost::bind(&connection::handle_read, shared_from_this(),
													boost::asio::placeholders::error,
													boost::asio::placeholders::bytes_transferred)));

				  
				}
			}
			
			tcp::socket socket_;
			boost::asio::streambuf request_buf_;
			Json::Rpc::Handler& jsonHandler_;
			  /// Strand to ensure the connection's handlers are not called concurrently.
			boost::asio::io_service::strand strand_;
			boost::asio::streambuf response_;
		};
	typedef boost::shared_ptr<connection> connection_ptr;
	
	class RpcServer
		{
		public:
			RpcServer(boost::asio::io_service& io_service, short port,std::size_t thread_pool_size);
			~RpcServer();

			void AddMethod(Json::Rpc::CallbackMethod* method);
			void DeleteMethod(const std::string& method);
			void run();
			
		protected:
			Json::Rpc::Handler jsonHandler_;	
			
		private:
			void handle_accept(const boost::system::error_code& error)
			{
				if (!error)
				{
				  new_connection_->start();
				  new_connection_.reset(new connection(io_service_, jsonHandler_));
				  acceptor_.async_accept(new_connection_->socket(),
							 boost::bind(&RpcServer::handle_accept, this,
								     boost::asio::placeholders::error));
				}
			}
			boost::asio::io_service& io_service_;
			tcp::acceptor acceptor_;
			 /// The number of threads that will call io_service::run().
			std::size_t thread_pool_size_;
			  /// The next connection to be accepted.
			connection_ptr new_connection_;
		};
} 

#endif 

