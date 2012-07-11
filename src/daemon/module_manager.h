/*
 * Copyright (C) 2010, 2011, 2012 Remo Hertig
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
#include <boost/shared_ptr.hpp>
#include <boost/signals2/signal.hpp>
#include "plugins/plugin.h"
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
		class Generic;

	}
	typedef std::string fid_t;
	class block;
			
	  struct KV_Stat {
	    std::string key;
	    boost::posix_time::ptime last_check;
	    int n_successful_checks;
	  };
	  typedef boost::shared_ptr<std::vector<KV_Stat> > KV_StatsPtr;
	
	  typedef std::map<std::string,std::string> KV_ResMap;
	  typedef boost::shared_ptr<KV_ResMap> KV_ResPtr;
	  #define newKVRes new KV_ResMap

// combiner which returns if one is true
template<typename T>
struct res_check
{
  typedef T result_type;

  template<typename InputIterator>
  T operator()(InputIterator first, InputIterator last) const
  {
    if(first == last ) return false;
    T state = false;
    while (first != last) {
      if (*first)
        state = true;
      ++first;
    }
    return state;
  }
};

class ModuleManager : public Singleton<ModuleManager> {
		friend class Singleton<ModuleManager>;
public:
		bool init();
		
			~ModuleManager() {
			kv_put.disconnect_all_slots();
			}

		//signals
		boost::signals2::signal<bool (const std::string& key, const std::string& value),
              res_check<bool> > kv_put;
	      	boost::signals2::signal<bool (const std::string& key, KV_ResPtr& value),
              res_check<bool> > kv_get;
	      		boost::signals2::signal<bool (const std::string& key),
              res_check<bool> > kv_remove;
	      		// binary file storage
	      
	      		boost::signals2::signal<bool (const fid_t& id, boost::shared_ptr<block>&),
              res_check<bool> > get_block;
	      
		boost::signals2::signal<bool (const std::string& path, std::string& res),
              res_check<bool> > store_file;
		boost::signals2::signal<bool (const std::string& id,std::string& res),
              res_check<bool> > get_file;
		boost::signals2::signal<bool (const std::string& id),
              res_check<bool> > remove_file;
	      
		//boost::signals2::signal<bool (const an::log::severity_level&, const std::string&),
           //   res_check<bool> > log;
		
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
		//bool kv_put(const std::string& key, const std::string& value);
		//bool kv_get(const std::string& key,KV_ResPtr& res);
		bool kv_get_unique(const std::string& key, std::string& res);
		//bool kv_remove(const std::string& key);
		bool kv_get_stats(const std::string& key, KV_Stat& res);
		bool kv_get_unsuccessful(int n, KV_StatsPtr& res);
		bool kv_get_unchecked_since(boost::posix_time::time_duration t , KV_StatsPtr& res);


		
		
		plg::LocalStorage * get_ls_plugin(std::string name);		
		
protected:
		ModuleManager() {}
private:
		std::vector<plg::Bootstrap*> _bootstrapers;
		std::vector<plg::LocalStorage*> _localstorages;
		std::vector<plg::RemoteStorage*> _remotestorages;
		std::vector<plg::Session*> _sessions;
		std::vector<plg::Generic*> _utils;
	};
}
#endif  // SRC_DAEMON_MODULE_MANAGER_H_
