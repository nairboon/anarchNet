#include <gcrypt.h>

#include "crypto.h"
namespace an {
namespace crypto
{

	const char hexDigits[] = "0123456789abcdef";
	
	
	std::string toHex(char *inp,size_t length)
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
	
	std::string sha512(const std::string& inp) {
		char res[64];
		gcry_md_hash_buffer(GCRY_MD_SHA512, res, inp.c_str(), inp.length());
		return toHex(res,64);
	}

} // namespace crypto
}