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
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/date_time/posix_time/time_serialize.hpp>
#include <boost/serialization/string.hpp>
#include <boost/enable_shared_from_this.hpp>

#ifndef DAEMON_DB_H_
#define DAEMON_DB_H_

namespace an {
	namespace db {
		typedef std::string ObjID;
		typedef std::string String;

		class Snapshot;
		class Diff {
		public:
			ObjID shapshot;
			std::vector<ObjID> prev;
			ObjID id;
		};
		typedef boost::shared_ptr<Diff> DiffPtr;

		class Snapshot {
		public:
			std::vector<boost::shared_ptr<Diff> > diffs;
			std::string content;
			ObjID based;
			ObjID id;
			boost::posix_time::ptime time;
			
			template<class Archive>
			void serialize(Archive & ar, const unsigned int version)
			{
        ar & id;
				ar & based;
				ar & content;
				ar & time;
			}
		};
		typedef boost::shared_ptr<Snapshot> SnapshotPtr;
		
		class Object : public boost::enable_shared_from_this<Object> {
		public:
			ObjID id;
			std::vector<SnapshotPtr> snapshots;
			std::vector<DiffPtr> diffs;
			bool save();
			bool create(std::string inp);
			bool load(const ObjID& id);
			bool remove();
			Object() {}
			Object(std::string inp);
		private:
		};
		typedef boost::shared_ptr<Object> ObjPtr;
		const ObjID create_ObjID(const String& input);
	}
}
#endif  // DAEMON_DB_H_
