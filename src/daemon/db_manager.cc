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
	
	
	db::ObjID DBManager::store_object(const std::string& content)
	{
		db::ObjID id = db::create_ObjID(content);
		db::Snapshot sn;
		sn.content = content;
		sn.based = "";
		sn.id = id;
		
	/*	std::stringstream ss;
    {
			boost::archive::text_oarchive oa(ss);
			oa << sn;
    }*/
		
		if(ModuleManager::instance().db_store_snapshot(sn))
			return id;
		else
			return "";
	}
	bool DBManager::update_object(db::Object* obj) 
	{
		return false;
	}
	bool DBManager::delete_object(db::Object* obj)
	{
		return false;
	}
	bool DBManager::get_object(db::Object* obj)
	{
		return false;
	}
	

}
