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

#include <iostream>
#include <fstream>
#include <vector>
#include <glog/logging.h>
#include "boost/filesystem.hpp"
#include "anarchNet.h"
#include "config_manager.h"
#include "connection_manager.h"

using std::string;
namespace po = boost::program_options;
namespace fs = boost::filesystem;

namespace an
{

bool ConfigManager::init(const string& directory_path)
{
	string directory;
	if (directory_path[0] == '~') 
		directory = string(getenv("HOME")) + directory_path.substr(1);
	else
		directory = directory_path;
	
	string config_file = directory + "/config.cfg";

	po::options_description config_file_options("Configuration");
	config_file_options.add_options()
	("control-rpc-port", po::value<int>()->default_value(RPC_CONTROL_PORT))
	("anarchnet-port", po::value<int>()->default_value(ANARCHNET_PORT))
	("plugin",po::value< std::vector<string> >()->composing());
	
	std::ifstream ifs(config_file.c_str());
	if (!ifs) {
		LOG(ERROR) << "can not open config file: " << config_file << "\n";
		return false;
	}
	
	store(parse_config_file(ifs, config_file_options), vm_);
	notify(vm_);
	
	fs::path plugin_path;
	fs::path plugin_data_path;

	foreach (string plugin_name, vm_["plugin"].as< std::vector<string> >()) {
		plugin_path = fs::path(directory) / "plugins" / plugin_name;
		if (!fs::exists( plugin_path ) ) {
			LOG(ERROR) << "plugin not found: " << plugin_path;
			return false;
		}
		plugin_data_path = fs::path(directory) / "plugin_data" / plugin_name;
		if (!fs::is_directory( plugin_data_path )) 
			if (!fs::create_directory( plugin_data_path )) {
				LOG(ERROR) << "could not create directory: " << plugin_data_path;
				return false;
			}
		if( !ConnectionManager::instance().loadPlugin(plugin_name,plugin_path.file_string(),plugin_data_path.directory_string())) {
			LOG(ERROR) << "could not load plugin: " << plugin_name;
			return false;
		}
	}
	return true;
}
}
