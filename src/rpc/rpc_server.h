#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include "jsonrpc_handler.h"
#include "logger.h"

#ifndef RPC_SERVER_H
#define RPC_SERVER_H

namespace an
{

	using boost::asio::ip::tcp;
	
	class session
		{
		public:
			session(boost::asio::io_service& io_service,Json::Rpc::Handler* jsh)
			: socket_(io_service), _jsonHandler(jsh)
			{
			}
			
			tcp::socket& socket()
			{
				return socket_;
			}
			
			void start()
			{
			  async_read_until(socket_, request_buf_, "\r\n" ,boost::bind(&session::handle_read, this,
													boost::asio::placeholders::error,
													boost::asio::placeholders::bytes_transferred));
			}
			
		private:
			void handle_read(const boost::system::error_code& error,
											 size_t bytes_transferred);
			
			void handle_write(const boost::system::error_code& error)
			{
				if (!error)
				{
					socket_.async_read_some(boost::asio::buffer(data_, max_length),
																	boost::bind(&session::handle_read, this,
																							boost::asio::placeholders::error,
																							boost::asio::placeholders::bytes_transferred));
				}
				else
				{
					delete this;
				}
			}
			
			tcp::socket socket_;
			enum { max_length = 1024 };
			char data_[max_length];
			boost::asio::streambuf request_buf_;
			Json::Rpc::Handler* _jsonHandler;
		};
	
	class RpcServer
		{
		public:
			RpcServer(boost::asio::io_service& io_service, short port)
			: io_service_(io_service), acceptor_(io_service, tcp::endpoint(tcp::v4(), port))
			{
				start_accept();
			}
			~RpcServer();

			void AddMethod(Json::Rpc::CallbackMethod* method);
			void DeleteMethod(const std::string& method);
			
		protected:
			Json::Rpc::Handler _jsonHandler;	
			
		private:
			void start_accept()
			{
				session* new_session = new session(io_service_,&_jsonHandler);
				acceptor_.async_accept(new_session->socket(),
															 boost::bind(&RpcServer::handle_accept, this, new_session,
																					 boost::asio::placeholders::error));
			}
			
			void handle_accept(session* new_session,
												 const boost::system::error_code& error)
			{
				if (!error)
				{
					new_session->start();
				}
				else
					delete new_session;
				
				start_accept();
			}
			boost::asio::io_service& io_service_;
			tcp::acceptor acceptor_;
		};
} 

#endif 

