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

#include <sstream>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/foreach.hpp>
#include "config.h"
#include "crypto.h"
#include "logger.h"
#include "anarchNet.h"
#include "db_manager.h"
#include "module_manager.h"
#include "diff_match_patch.h"
//#include "config_manager.h"

using std::string;

namespace an
{
	bool DBManager::init(const string& db_path)
	{

		return true;
	}


	bool DBManager::create_object(db::ObjPtr obj)
	{
		if(ModuleManager::instance().db_store_obj(obj))
			return true;

		return false;
	}
	bool DBManager::update_object(const db::ObjID& id,const String& diff)
	{
	  db::ObjPtr obj;
		if(!ModuleManager::instance().db_get_obj(id,obj))
			return false;
		
		if(!update_object(obj,diff))
			return false;
		
	  return true;
	}
	bool DBManager::update_object(db::ObjPtr obj,const String& diff)
	{
		// create diff
		diff_match_patch<String> dmp;
		  std::string patch = dmp.patch_toText(dmp.patch_make(obj->get(), diff));
   
		LOG(INFO) << "patch: " << patch;
	  
		db::DiffPtr ddiff(new db::Diff(obj->snapshots[0]->id,patch));

		if(!ModuleManager::instance().db_store_diff(ddiff))
			return false;

		obj->diffs.push_back(ddiff);

		if(!ModuleManager::instance().db_store_obj(obj))
			return false;

		return true;
	}
	bool DBManager::delete_object(const db::ObjID& id)
	{
		if(ModuleManager::instance().db_remove(id))
			return true;

		return false;
	}
	bool DBManager::get_object_head(const db::ObjID& id,db::ObjPtr obj)
	{
		db::ObjID headid = db::ObjID(crypto::toHex(crypto::Hash( "HEAD" + id )));
		if(ModuleManager::instance().kv_get_unique(headid,obj->head))
		{	// head snapshot exists
			return true;
		}
		// no haed yet
		KV_ResPtr diffs;
		obj->id = id;
		
		if(ModuleManager::instance().kv_get(id,diffs)) {
		  BOOST_FOREACH(const KV_ResMap::value_type& entry, *diffs) {
		    try {
		    db::DiffPtr diff;

		    std::istringstream inputStream(entry.second);
		  boost::archive::text_iarchive inputArchive(inputStream);
		  inputArchive >> *diff;

		    obj->diffs.push_back(diff);
		    }
		    catch(boost::archive::archive_exception& e)
		    {
		      LOG(ERROR) << "not a diff: " << entry.second;
		    }
		  }
		  
		  if(obj->create_from_diffs()) {
		    return true;
		  }
		}
		return false;
	}
	bool DBManager::get_object_rev(const db::ObjPtr obj,db::ObjPtr rev)
	{
	  // lastRev = obj->get();
	  LOG(ERROR) << "unimplemented";
	  return false;
	}


}
