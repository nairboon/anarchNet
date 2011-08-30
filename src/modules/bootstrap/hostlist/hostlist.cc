#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/asio.hpp>
#include <boost/foreach.hpp>

#include "logger.h"
#include "puggKernel.h"
#include "hostlist.h"

using namespace pugg;
using namespace boost::asio;
using boost::asio::ip::tcp;

extern "C" //__declspec(dllexport)
void registerPlugin(Kernel &K) 
{
	Server<an::plgdrv::Bootstrap>* server =CastToServerType<an::plgdrv::Bootstrap>(K.getServer(PLG_BOOTSTRAP_SERVER_NAME));
	assert(server != NULL);
	server->addDriver(new HostlistDriver(),PLG_BOOTSTRAP_SERVER_VERSION);
}


bool Hostlist::initialise()
{
	return true;
}


void Hostlist::shutdown()
{
	
}

bool bootstrapFromPeer(std::string ip, int port) {
	
	return false; 
}

bool Hostlist::bootstrapFromHostlist(std::string args) {

	std::string hlist;
	if(!http_request(args,hlist)) {
		LOG(ERROR) << "could not fetch hostlist: " << args;
		return false;
	}
	std::vector<std::string> plist;
	boost::algorithm::split( plist, hlist, boost::algorithm::is_any_of("\n"), boost::algorithm::token_compress_on );
	bool fail =true;
	BOOST_FOREACH(std::string peer, plist) {
		std::vector<std::string> pinfo;
		boost::algorithm::split( pinfo, peer, boost::algorithm::is_any_of(":"), boost::algorithm::token_compress_on );
		if(bootstrapFromPeer(pinfo[0],atoi(pinfo[1].c_str())))
			fail = false;
	}
	return fail;
}

bool Hostlist::http_request(const std::string& _url,std::string& _response)
 {
 std::size_t index = _url.find_first_of('/');
 std::string url_server = _url.substr(0,index);
 std::string url_file = _url.substr(index);
 
 LOG(INFO) << "HTTP REQ " << url_file << " from " << url_server;
 
 std::stringstream out;
 try {
 boost::asio::io_service io_service;
 
	 tcp::resolver resolver(io_service);
 tcp::resolver::query query(url_server, "http");
 tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
 tcp::resolver::iterator end;
 
 tcp::socket socket(io_service);
 boost::system::error_code error = boost::asio::error::host_not_found;
 while (error && endpoint_iterator != end)
 {
 socket.close();
 socket.connect(*endpoint_iterator++, error);
 }
 if (error)
 throw boost::system::system_error(error);
 
 boost::asio::streambuf request;
 std::ostream request_stream(&request);
 request_stream << "GET " << url_file << " HTTP/1.0\r\n";
 request_stream << "Host: " << url_server << "\r\n";
 request_stream << "Accept: *//*\r\n";
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
	
	 _response = out.str();
	return true;
	}