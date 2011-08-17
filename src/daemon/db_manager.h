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

#include <map>
#include <vector>
#include "db.h"
#include "singleton.h"

#ifndef DAEMON_DB_MANAGER_H_
#define DAEMON_DB_MANAGER_H_
namespace an {

class DBManager : public Singleton<DBManager>
	{
		friend class Singleton<DBManager>;
public:
		bool init(const std::string&);
		
	//	bool store_object(const StoreObjectRequest* req);
//		bool delete_object(const DeleteObjectRequest* req);
		
		bool store_object(db::Object* obj);
		bool update_object(db::Object* obj);
		bool delete_object(db::Object* obj);
		bool get_object(db::Object* obj);

		std::vector<std::string> get_unchecked_keys_since(int);
private:
	
	};
}
#endif  // DAEMON_DB_MANAGER_H_
