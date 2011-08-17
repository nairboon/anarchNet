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

#include <boost/shared_ptr.hpp>

#ifndef DAEMON_DB_H_
#define DAEMON_DB_H_

namespace an {
	namespace db {
		typedef std::string ObjID;

		class Snapshot;
		class Diff {
		public:
			ObjID shapshot;
			std::vector<ObjID> prev;
		};
		
		class Snapshot {
		public:
			std::vector<boost::shared_ptr<Diff> > diffs;
			ObjID based;
			// timestamp
		};
		
		class Object {
		public:
			ObjID id;
			std::vector<boost::shared_ptr<Snapshot> > snapshots;
			std::vector<boost::shared_ptr<Diff> > diffs;
		};
	}
}
#endif  // DAEMON_DB_H_
