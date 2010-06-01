/*
 * Copyright (C) 2010 Remo Hertig (nairboon)
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
#include <boost/program_options.hpp>
#include "singleton.h"

#ifndef DAEMON_CONFIG_MANAGER_H_
#define DAEMON_CONFIG_MANAGER_H_
namespace an {
	
class ConfigManager : public Singleton<ConfigManager>
	{
		friend class Singleton<ConfigManager>;
public:
		bool init(const std::string&);
		std::string kad_config() { return kad_config_; }
		const boost::program_options::variables_map& config() { return vm_; }
private:
		boost::program_options::variables_map vm_;
		std::string kad_config_;
	};
}
#endif  // DAEMON_CONFIG_MANAGER_H_
