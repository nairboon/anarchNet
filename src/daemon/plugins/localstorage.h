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

#include "puggDriver.h"
#include "plugins/plugin.h"
#include "db.h"

#ifndef DAEMON_PLUGIN_LOCALSTORAGE_H
#define DAEMON_PLUGIN_LOCALSTORAGE_H
namespace an {

#define PLG_LOCALSTORAGE_SERVER_NAME "LocalStorageServer"
#define PLG_LOCALSTORAGE_SERVER_VERSION 1
namespace plg {
class LocalStorage : public Plugin
{
public:
	enum StorageType { PLAIN, BINARY };

	virtual const std::string getName() {return "generic LOCALSTORAGEplugin";}
	virtual const StorageType getType() { return LocalStorage::PLAIN; }
	
	virtual const bool db_store_snapshot(const db::Snapshot& sn) { return false; }
	virtual const bool db_store_diff(const db::Diff& diff) { return false; }

	/**
	 * \brief KV Store
	 * \param key id
	 * \param value value to store
	 * \return true if successfuly stored
	 */
	virtual const bool kv_put(const std::string& key, const std::string& value) { return false; }
	virtual const std::string kv_get(const std::string& key) { return ""; }

};
}
	namespace plgdrv {
class LocalStorage : public pugg::Driver
{
public:
	virtual plg::LocalStorage* createPlugin() = 0;
};
	}
}
#endif
