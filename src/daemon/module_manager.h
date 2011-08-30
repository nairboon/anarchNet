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

#include <vector>
#include "singleton.h"
#include "db.h"

#ifndef SRC_DAEMON_MODULE_MANAGER_H_
#define SRC_DAEMON_MODULE_MANAGER_H_
namespace an {
	namespace plg { 
		class Bootstrap; 
		class LocalStorage; 
		class RemoteStorage; 
		class Session; 
	}
class ModuleManager : public Singleton<ModuleManager> {
		friend class Singleton<ModuleManager>;
public:
		bool init();
		
			~ModuleManager() {}
		
		bool bootstrapFromPeer(const std::string&ip,int port);
		bool bootstrapFromHostlist(const std::string&url);

		// db types
		bool db_store_snapshot(db::SnapshotPtr ss);
		bool db_store_diff(db::DiffPtr diff);
		bool db_store_obj(db::ObjPtr obj);
		bool db_get_snapshot(const db::ObjID& id,db::SnapshotPtr res);
		bool db_get_diff(const db::ObjID& id,db::DiffPtr res);
		bool db_get_obj(const db::ObjID& id,db::ObjPtr res);
		bool db_remove(const db::ObjID& id);

		// key value store
		bool kv_put(const std::string& key, const std::string& value);
		bool kv_get(const std::string& key,std::string& res);
		bool kv_remove(const std::string& key);

		// binary file storage
		bool store_file(const std::string& path, std::string& res);
		bool get_file_path(const std::string& id,std::string& res);
		bool remove_file(const std::string& id);
		
protected:
		ModuleManager() {}
private:
		std::vector<plg::Bootstrap*> _bootstrapers;
		std::vector<plg::LocalStorage*> _localstorages;
		std::vector<plg::RemoteStorage*> _remotestorages;
		std::vector<plg::Session*> _sessions;
	};
}
#endif  // SRC_DAEMON_MODULE_MANAGER_H_
