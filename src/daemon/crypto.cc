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

#include "cryptopp/sha.h"
#include "cryptopp/files.h"
#include "cryptopp/filters.h"
#include "crypto.h"

#if CRYPTOPP_VERSION < 560
#error CryptoPP is older than 560
#endif

namespace an {
namespace crypto
{

	const char hexDigits[] = "0123456789abcdef";

	std::string toHex(const char *inp,size_t length)
	{
		std::string result;
		result.reserve(2*length);
		for (size_t i = 0; i < length;i++)
		{
			result.push_back(hexDigits[(inp[i]&0xf0) >>4]);
			result.push_back(hexDigits[inp[i]&0x0f]);
		}
		return result;
	}
	
	
	std::string toHex(const std::string& inp) 
	{ 
		return toHex(inp.c_str(), inp.length());
	}
	

	
	std::string Hash(const std::string& inp) {
		std::string result;
		CryptoPP::SHA512 hash;
		CryptoPP::StringSource(inp, true,
													 new CryptoPP::HashFilter(hash, new CryptoPP::StringSink(result)));
		return result;
	}

	std::string HashFile(const boost::filesystem::path &file_path) {
		std::string result;
		CryptoPP::SHA512 hash;
		try {
			CryptoPP::FileSource(file_path.c_str(), true,
													 new CryptoPP::HashFilter(hash, new CryptoPP::StringSink(result)));
		}
		catch(...) {
			result.clear();
		}
		return result;
	}
	
} // namespace crypto
}
