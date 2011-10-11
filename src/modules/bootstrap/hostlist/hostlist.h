#ifndef PLG_HOSTLIST_H
#define PLG_HOSTLIST_H

#include "plugins/bootstrap.h"

#define PLG_HOSTLIST_VERSION 1

class Hostlist : public an::plg::Bootstrap
{
public:
	const std::string getName() {return "hostlist";}
	const unsigned int getVersion() { return PLG_HOSTLIST_VERSION; }
	bool initialise();
	void shutdown();
	bool bootstrapFromPeer(std::string ip, int port);
	bool bootstrapFromHostlist(std::string args);

	static bool http_request(const std::string& url,std::string& response);
};

class HostlistDriver : public an::plgdrv::Bootstrap
{
public: 
	const std::string getName() { return "hostlist_driver"; }
	Hostlist* createPlugin() {return new Hostlist();}
};

#endif // PLG_HOSTLIST_H