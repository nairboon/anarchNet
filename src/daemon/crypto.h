#include <iostream>
#include <boost/filesystem/path.hpp>

#ifndef CRYPTO_H
#define CRYPTO_H

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
#endif // CRYPTO_H
