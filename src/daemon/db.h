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

#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/date_time/posix_time/time_serialize.hpp>
#include <boost/serialization/string.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "config.h"

#ifndef SRC_DAEMON_DB_H_
#define SRC_DAEMON_DB_H_

namespace an {
	namespace db {
		typedef std::string ObjID;

		class Snapshot;
		class needs_ObjID {
			public:
				ObjID id;
				int _db_id;
				std::string _db_type;
				String content;
				boost::posix_time::ptime time;
				bool create_random_id();
				bool create_content_id();
				needs_ObjID() : _db_id(0), _db_type("") {}
				needs_ObjID(const String& c) : _db_id(0), _db_type(""), content(c),
					time(boost::posix_time::second_clock::local_time()) {
					create_content_id();
				}
		};
		class Diff : public needs_ObjID {
		 public:
			ObjID snapshot;
			std::vector<ObjID> prev;
			Diff() {}
			Diff(const ObjID& s, const String& c) : needs_ObjID(c), snapshot(s) {}
		};
		typedef boost::shared_ptr<Diff> DiffPtr;

		class Snapshot : public needs_ObjID{
		 public:
			std::vector<DiffPtr> diffs;
			ObjID based;
			Snapshot() {}
			Snapshot(const ObjID& b, const String& c) : needs_ObjID(c), based(b) {}

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
			int _db_id;
			std::string _db_type;
			std::vector<SnapshotPtr> snapshots;
			std::vector<DiffPtr> diffs;
			bool save();
			bool create(String inp);
			bool create(String inp,const ObjID& custom_id);
			bool load(const ObjID& id);
			bool remove();
			Object() : _db_id(0), _db_type("") {}
		 private:
		};
		typedef boost::shared_ptr<Object> ObjPtr;
	}
	std::string sha512(const std::string& input);
}
#endif  // SRC_DAEMON_DB_H_
