/**
* @file
* @author  Tunc Bahcecioglu <http://www.tuncbahcecioglu.com>
* @version 1.1
*
* @section LICENSE
*
* GNU GENERAL PUBLIC LICENSE
* Version 3, 29 June 2007
*
* <http://www.gnu.org/licenses/gpl-3.0.html>
*/

#include <string>
#include <cassert>

#ifndef FILE_pugg_PLUGIN_H
#define FILE_pugg_PLUGIN_H

#ifdef __WIN32__
	#include <Windows.h>
	#define DYNLIB_HANDLE hInstance
	#define DYNLIB_LOAD( a ) LoadLibraryEx( a, NULL, LOAD_WITH_ALTERED_SEARCH_PATH )
	#define DYNLIB_GETSYM( a, b ) GetProcAddress( a, b )
	#define DYNLIB_UNLOAD( a ) !FreeLibrary( a )
#else //linux
	#include <dlfcn.h>
	#define DYNLIB_HANDLE void*
	#define DYNLIB_LOAD( a ) dlopen( a, RTLD_LAZY | RTLD_GLOBAL)
	#define DYNLIB_GETSYM( a, b ) dlsym( a, b )
	#define DYNLIB_UNLOAD( a ) dlclose( a )
#endif


#ifdef PUGG_USE_WSTRING_FILE_NAMES
typedef std::wstring PuggFileStringType;
#else
typedef std::string PuggFileStringType;
#endif

#define WIN32_LEAN_AND_MEAN
namespace pugg
{
	class Kernel;
	class Plugin
	{
	public:
		Plugin(PuggFileStringType fileName, bool& isLoaded)
			:m_hDLL(0),m_pDLLRefCount(0),m_pfnRegisterPlugin(0)
		{
			isLoaded = true;
#ifdef __WIN32__
	#ifdef UNICODE
				LPCWSTR fileNameInUnicode;
		#ifndef PUGG_USE_WSTRING_FILE_NAMES
					std::wstring fileNameInWideString = s2ws(fileName);
					fileNameInUnicode = fileNameInWideString.c_str();
		#else
					fileNameInUnicode = fileName.c_str();
		#endif
	#else
		#ifdef PUGG_USE_WSTRING_FILE_NAMES
			#error PUGG_USE_WSTRING_FILE_NAMES is defined without UNICODE defination, incompitable definitions !!!
		#endif
				LPCSTR fileNameInUnicode;
				fileNameInUnicode = (fileName.c_str());
	#endif
		m_hDLL = LoadLibrary(fileNameInUnicode);
#else
			m_hDLL = DYNLIB_LOAD(fileName.c_str());
#endif
			if (m_hDLL)
			{
				try 
				{
					m_pfnRegisterPlugin = reinterpret_cast<fnRegisterPlugin *>(DYNLIB_GETSYM(m_hDLL, "registerPlugin"));

					if(! m_pfnRegisterPlugin)
					{
						isLoaded = false;
						DYNLIB_UNLOAD(m_hDLL);
					}
					else m_pDLLRefCount = new size_t(1);
				}
				catch(...) 
				{
					DYNLIB_UNLOAD(m_hDLL);
					isLoaded = false;
				}
			}else isLoaded = false;
		}


		/** Destroys the plugin, unloading its library when no more references
		to it exist.
		*/
		~Plugin() 
		{
			// Only unload the DLL if there are no more references to it
			if( m_pDLLRefCount && !--*m_pDLLRefCount) 
			{
				delete m_pDLLRefCount;
				DYNLIB_UNLOAD(m_hDLL);
			}
		}



		/** Creates a copy of a plugin that has already been loaded.
		Required to provide correct semantics for storing plugins in
		an STL map container.

		@param  Other  Other plugin instance to copy
		*/
		Plugin(const Plugin &Other) 
			:m_hDLL(Other.m_hDLL),m_pDLLRefCount(Other.m_pDLLRefCount),m_pfnRegisterPlugin(Other.m_pfnRegisterPlugin) 
		{
				++*m_pDLLRefCount;
		}
	
		/// Register the plugin to a kernel
		void registerPlugin(Kernel& K) 
		{
			m_pfnRegisterPlugin(K);
		}

	private:
#ifdef __WIN32__
		// Andrew Revvo MSDN forums
		std::wstring s2ws(const std::string& s)
		{
			int len;
			int slength = (int)s.length() + 1;
			len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
			wchar_t* buf = new wchar_t[len];
			MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
			std::wstring r(buf);
			delete[] buf;
			return r;
		}
#endif
		/// Signature for the plugin's registration function
		typedef void fnRegisterPlugin(Kernel&);

		DYNLIB_HANDLE       m_hDLL;                ///< Win32 DLL handle
		size_t             *m_pDLLRefCount;        ///< Number of references to the DLL
		fnRegisterPlugin   *m_pfnRegisterPlugin;   ///< Plugin registration function
	};
}

#endif
