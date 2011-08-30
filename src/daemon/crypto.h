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

#include <iostream>
#include <boost/filesystem/path.hpp>

#ifndef SRC_DAEMON_CRYPTO_H_
#define SRC_DAEMON_CRYPTO_H_

namespace an {
namespace crypto
{
	
	/** convert a string to Hex
	 *  @param inp string that is converted
	 *  @return the hex string */
	std::string toHex(const std::string& inp);
	
	/** Hash a string
	 *  @param inp string that is to be hashed.
	 *  @return the hash */
	std::string Hash(const std::string& inp);
	
	/** Hash of a file.
	 *  @param file_path path to file that is to be hashed.
	 *  @return the result of the hash function, or empty string if the file could
	 *  not be read. */
	std::string HashFile(const boost::filesystem::path &file_path);
} 
}
#endif  // SRC_DAEMON_CRYPTO_H_
