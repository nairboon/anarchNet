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

#include <cryptopp/sha.h>
#include <cryptopp/files.h>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "db.h"


using std::string;

namespace an
{
	namespace db {
	const ObjID create_ObjID(const String& input)
		{
			boost::posix_time::ptime t(boost::posix_time::microsec_clock::local_time());
			std::string time = boost::posix_time::to_iso_string(t);
			CryptoPP::SHA512 hash;
			std::string result;
			CryptoPP::StringSource(input+time, true,
														 new CryptoPP::HashFilter(hash, new CryptoPP::StringSink(result)));
			return ObjID(result);
		}
	}
}