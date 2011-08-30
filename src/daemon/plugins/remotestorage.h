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

#ifndef SRC_DAEMON_PLUGIN_REMOTESTORAGE_H
#define SRC_DAEMON_PLUGIN_REMOTESTORAGE_H
namespace an {
	namespace plg {
#define PLG_REMOTESTORAGE_SERVER_NAME "RemoteStorageServer"
#define PLG_REMOTESTORAGE_SERVER_VERSION 1
		
class RemoteStorage : public Plugin {
	public:
		enum StorageType { NORMAL, ENCRYPTED, ANONYM };

		virtual const std::string getName() {return "generic RemoteStorage plugin";}
		virtual const StorageType getType() { return RemoteStorage::NORMAL; }
};
}
	namespace plgdrv {
		class RemoteStorage : public pugg::Driver
		{
		public:
			virtual plg::RemoteStorage* createPlugin() = 0;
		};
	}
	
}
#endif
