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
#include <boost/property_tree/ptree.hpp>

#ifndef SRC_DAEMON_PLUGIN_H
#define SRC_DAEMON_PLUGIN_H
namespace an {
	class Plugin {
		public:
			Plugin() {}
			virtual ~Plugin() {}
			virtual const std::string getName() = 0;
			virtual const unsigned int getVersion() = 0;
			virtual bool initialise() = 0;
			virtual void shutdown() = 0;
		protected:
		  bool get_config() {
		    try {
		      _config = an::ConfigManager::instance().modules_config(getName());
		    }
		    catch(boost::property_tree::ptree_error& e)
		    {
		      return false;
		    }
		    return true;
		  }
		  boost::property_tree::ptree _config;
		};
}
#endif