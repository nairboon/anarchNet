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
			result.push_back( hexDigits[(inp[i]&0xf0) >>4]);
			result.push_back( hexDigits[inp[i]&0x0f]);
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