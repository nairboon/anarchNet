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

#include <cstdlib>
#include <sstream>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "db.h"
#include "crypto.h"
#include "db_manager.h"


using std::string;

namespace an
{

	namespace db {
		bool needs_ObjID::create_random_id() {
			std::string stime = boost::posix_time::to_iso_string(time);
			std::stringstream rndinp;
			rndinp << stime << content << random();
			id = ObjID(crypto::sha512(rndinp.str()));
			return true;
		}
		bool needs_ObjID::create_content_id() {
			id = ObjID(crypto::sha512(content));		
			return true;
		}
		
		Object::Object(std::string inp)
		{
			create(inp);
		}

		bool Object::create(std::string inp)
		{
			return DBManager::instance().store_object(inp,shared_from_this());
		}

		bool Object::save() {
			return DBManager::instance().update_object(id,shared_from_this());
		}
		
		bool Object::remove() {
			return DBManager::instance().delete_entry(id);
		}
		
		bool Object::load(const ObjID& id)
		{
			return DBManager::instance().get_object(id,shared_from_this());
		}
	}
}