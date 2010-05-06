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

#include <vector>
#include <map>
#include "puggDriver.h"

#ifndef FILE_puggServer_h__
#define FILE_puggServer_h__

namespace pugg
{
	/**
	 * Base class for server class
	 * Required for storage of diffrent types of servers
	 */
	class ServerBase
	{
	public:
		/**
		* Constructor, assigns name and version to the server                                                                    
		*/
		ServerBase(const std::string& name,const int& version) : _engineName(name),_engineVersion(version) {}
		/**
		 * Destructor, does nada                                                                     
		 */
		virtual ~ServerBase() {}

		/**
		* Returns the engine version
		*/
		virtual int getEngineVersion() = 0;
		/**
		* Returns the server name
		*/
		virtual std::string getEngineName() = 0;


	protected:
		std::string _engineName;
		int _engineVersion;

	};

	template <typename DriverType>
	class Server : public ServerBase
	{
	public:
		typedef std::map<std::string,DriverType*> DriverMap;
		/**
		 * Constructor, assigns name and version to the server                                                                    
		 */
		Server(const std::string& name,const int& version) : ServerBase(name,version)
		{}
		/**
		 * Destructor, auto deletes all loaded drivers                                                                  
		 */
		~Server(void)
		{
			for (typename DriverMap::iterator it = loadedDrivers.begin(); it != loadedDrivers.end();++it)
			{
				delete it->second;
			}
		}
		/**
		 * Attamps to add a driver to the server.
		 * Server engine versions must match.
		 * Return NULL other wise
		 */
		DriverType* addDriver(DriverType* GD,const int& engineVersion )
		{
			if ((GD != NULL) && (engineVersion == this->getEngineVersion()))
			{
				this->loadedDrivers[GD->getName()] = GD;
				return GD;
			}else return NULL;
		}
		/**
		 * Returns driver matched with driverName
		 * Returns NULL on error
		 */
		DriverType* getDriver(const std::string& driverName)
		{
			typename DriverMap::iterator it = this->loadedDrivers.find(driverName);
			if (it != loadedDrivers.end()) return it->second;
			else return NULL;
		}
		/**
		 * Returns the number of drivers loaded in server                                                                     
		 */
		size_t getDriverCount() {return loadedDrivers.size();}

		/**
		 * Fils the driverVector with all the drivers of the server                                                                     
		 */
		void getAllDrivers(std::vector<DriverType*>& driverVector  )
		{
			for (typename DriverMap::iterator it = loadedDrivers.begin();it != loadedDrivers.end();++it)
			{
				DriverType* dr = it->second;
				driverVector.push_back(dr);
			}
		}
		/**
		* Returns the engine version
		*/
		int getEngineVersion() { return _engineVersion;}
		/**
		* Returns the server name
		*/
		std::string getEngineName() {return _engineName;}
	private:
		DriverMap loadedDrivers;

	};

	template<typename DriverType>
	Server<DriverType>* CastToServerType(ServerBase* base)
	{
		try
		{
			return reinterpret_cast<Server<DriverType>*>(base);
		}catch (...) {return NULL;}
	}
}

#endif // FILE_puggServer_h__