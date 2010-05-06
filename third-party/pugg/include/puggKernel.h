/**
* @file
* @author  Tunc Bahcecioglu <http://www.tuncbahcecioglu.com>
* @version 1.0
*
* @section LICENSE
*
* GNU LESSER GENERAL PUBLIC LICENSE
* Version 3, 29 June 2007
*
* <http://www.gnu.org/copyleft/lesser.html>
*/

/*! \mainpage Pugg version Documentation
*
* \section intro_sec Introduction
*
* Pugg is a c++ framework for plugin management.
* 
* - Features
*   - Header only library, no installation
*   - Auto loading of plugins from files
*   - Auto Version control
*   - Object oriented design

* \section download_sec Downloads
* Go to sourceforge for the latest version of Pugg 
*
* https://sourceforge.net/projects/pugg/
* \section more_sec More...
* Pugg sourforge web site https://sourceforge.net/projects/pugg/
*
* My persenal web site http://www.tuncbahcecioglu.com
*/

#ifndef FILE_pugg_KERNEL_H
#define FILE_pugg_KERNEL_H

#include "puggPlugin.h"
#include "puggServer.h"

#include <map>
#include <string>

#define PUGG_VERSION 0.42



namespace pugg
{
	/**
		Kernel, Administrator class for all servers and plugins. 
	 Kernel loads plug-ins from files,stores and manages all plugins and servers.
	 @see loadPlugin(const std::string &fileName )
	 @see addServer( ServerBase* server )
	 */	
	class Kernel
	{
	public:
		Kernel() {}
		/**
		 * Destructor.
		 Deletes all stored plugins.
		 */
		~Kernel(void)
		{
			for (PluginMap::iterator it = loadedPlugins.begin();it != loadedPlugins.end();++it)
				delete it->second;
		}
		/**
		 * Returns version of the pugg library                     
		 */
		double getpuggVersion() {return PUGG_VERSION;}
		/**
			Tries to load a plugin from the given file name.
		@param fileName a string holding the full path and name to the plugin file
		@return true if the plugin is successfully loaded.
		Checks the following:
		  -If the plugin from the file is already loaded
		  -If the file has a registerPlugin(pugg::Kernel&) function
		If above coditions are plug-in and all of its drivers that passes the version control are auto loaded.  

		 */
		bool loadPlugin(const PuggFileStringType& fileName )
		{
			bool isOK = false;
			if (loadedPlugins.find(fileName) == loadedPlugins.end())
			{
				Plugin* plugin = new Plugin(fileName,isOK);
				if (isOK)
				{
					loadedPlugins[fileName] = plugin;
					plugin->registerPlugin(*this); 
				} else delete plugin;
			}
			return isOK;
		}
		/**
		 * Adds the given server to the system.
		 * Returns the given Server pointer
		 */
		ServerBase* addServer( ServerBase* server )
		{
			std::string serverName = server->getEngineName();
			if(loadedServers.find(serverName) == loadedServers.end())
			{
				loadedServers[serverName] = server;
				return server;
			}
			else return NULL;
		}
		/**
		 * Gets the pointer for the server matching the given name
		 * returns null, if no match is found.
		 */
		ServerBase* getServer( const std::string& serverName )
		{
			ServerMapIterator it;
			it = loadedServers.find(serverName);
			if (it != loadedServers.end()) return it->second;
			else return NULL;
		}
	private:
		typedef std::map<PuggFileStringType,Plugin*> PluginMap;
		typedef PluginMap::iterator PluginMapIterator;

		typedef std::map<std::string,ServerBase*> ServerMap;
		typedef ServerMap::iterator ServerMapIterator;

		PluginMap loadedPlugins;
		ServerMap loadedServers;
	};


}

#endif