
#include "rpc_server.h"
#include "logger.h"


 
namespace an
{

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


	void session::handle_read(const boost::system::error_code& error,
									 size_t bytes_transferred)
	{
		if (!error)
		{
			std::istream request_stream(&request_buf_);
			std::stringstream os;
			std::copy(std::istream_iterator<char>(request_stream), std::istream_iterator<char>(), std::ostream_iterator<char>(os));
		      

		     
			std::string data = os.str();
			LOG(INFO) << "got: " << data.size() << ": " << data;

			if(!data.size())
			{
			  delete this;
			  return;
			}
    
			//try {
			boost::json::Value response;
				_jsonHandler->Process(std::string(data), response);
			//}
			//catch(boost::json::

			if(!response.is_null())
			{
				std::string rep = _jsonHandler->GetString(response);
				LOG(INFO) << "send: " << rep;
			boost::asio::async_write(socket_,
															 boost::asio::buffer(rep, rep.size()),
															 boost::bind(&session::handle_write, this,
																					 boost::asio::placeholders::error));
			}
		}
		else
		{
			delete this;
		}
	}
}
