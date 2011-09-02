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
	
	
	bool DBManager::create_object(const std::string& content, db::ObjPtr obj)
	{		
		if(ModuleManager::instance().db_store_obj(obj))
			return true;
	
		return false;
	}
	bool DBManager::save_object(const db::ObjID& id,const std::string& diff) 
	{
		db::ObjPtr obj;
		if(!ModuleManager::instance().db_get(id,obj))
			return false;
		
		db::DiffPtr ddiff(new db::Diff(obj->snapshot[0],diff));

		if(!ModuleManager::instance().db_store_diff(ddiff))
			return false;
		
		obj->diffs.push_back(
		return false;
	}
	bool DBManager::delete_entry(const db::ObjID& id)
	{
		if(ModuleManager::instance().db_remove(obj))
			return true;
		
		return false;
	}
	bool DBManager::get_object(const db::ObjID& id,db::ObjPtr obj)
	{
		if(ModuleManager::instance().db_get_obj(id,obj))
			return true;
		
		return false;
	}
	

}
