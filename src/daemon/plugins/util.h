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

#include "puggDriver.h"
#include "plugins/plugin.h"

#ifndef SRC_DAEMON_PLUGIN_UTIL_H
#define SRC_DAEMON_PLUGIN_UTIL_H
namespace an {
	namespace plg {
#define PLG_UTIL_SERVER_NAME "UtilServer"
#define PLG_UTIL_SERVER_VERSION 1


class Util : public Plugin {
public:

	virtual const std::string getName() {return "generic util plugin";}
	
	virtual bool log(log::severity_level level, const std::string message) { return false; }
	virtual bool on_kv_put(const db::ObjID &key) { return false; }
	virtual bool on_kv_remove(const db::ObjID &key) { return false; }
	virtual bool on_db_update(const db::ObjID &key) { return false; }
};
	}
		namespace plgdrv {
class Util : public pugg::Driver {
public:
	virtual plg::Util* createPlugin() = 0;
};
		}

}
#endif
