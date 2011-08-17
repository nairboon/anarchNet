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

#include <vector>
#include "singleton.h"
#include "plugins/bootstrap.h"
#include "plugins/localstorage.h"
#include "plugins/remotestorage.h"
#include "db.h"


#ifndef DAEMON_MODULE_MANAGER_H_
#define DAEMON_MODULE_MANAGER_H_
namespace an {
	
class ModuleManager : public Singleton<ModuleManager>
	{
		friend class Singleton<ModuleManager>;
public:
		bool init();
		
			~ModuleManager() {}
		
		bool bootstrapFromPeer(const std::string&ip,int port);
		bool bootstrapFromHostlist(const std::string&url);

		// db types
		bool db_store_snapshot(const db::Snapshot& ss);
		bool db_store_diff(const db::Diff& diff);

		
		// key value store
		bool kv_put(const std::string& key, const std::string& value);
		const std::string kv_get(const std::string& key);

		
		static bool http_request(const std::string& url,std::string* response);
protected:
		ModuleManager() {}
private:
		std::vector<plg::Bootstrap*> _bootstrapers;
		std::vector<plg::LocalStorage*> _localstorages;
		std::vector<plg::RemoteStorage*> _remotestorages;
	};
}
#endif  // DAEMON_MODULE_MANAGER_H_
