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

#include <string>
#include <set>
#include <boost/property_tree/ptree.hpp>
#include "singleton.h"

#ifndef SRC_DAEMON_CONFIG_MANAGER_H_
#define SRC_DAEMON_CONFIG_MANAGER_H_
namespace an {
	
class ConfigManager : public Singleton<ConfigManager> {
		friend class Singleton<ConfigManager>;
 public:
		bool init(const std::string&);
		const 	boost::property_tree::ptree& config() { return _pt; }
		const std::set<std::string>& modules() { return _modules; }
		int port() { return _port; }
		int rpc_port() { return _rpc_port; }
		std::string datadir() { return _data_dir; }
		std::string logdir() { return _log_dir; }
 private:
		boost::property_tree::ptree _pt;
		std::set<std::string> _modules;
		int _port;
		int _rpc_port;
		std::string _data_dir;
		std::string _log_dir;
		std::string _dir;
	};
}
#endif  // SRC_DAEMON_CONFIG_MANAGER_H_
