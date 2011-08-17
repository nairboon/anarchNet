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

#include <glog/logging.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "anarchNet.h"
#include "config_manager.h"
#include <boost/property_tree/info_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/foreach.hpp>



namespace an
{

	bool ConfigManager::init(const std::string& directory)
{
	
	std::string config_file = directory + "/anarchnet.conf";

/*	po::options_description config_file_options("Configuration");
	config_file_options.add_options()
	("rpc-port", po::value<int>()->default_value(ANARCHNET_RPC_PORT))
	("port", po::value<int>()->default_value(ANARCHNET_PORT))
	("modules",po::value< std::vector<std::string> >()->composing())
	("db",po::value<std::string>()->default_value("data.db"))
	("db-recheck", po::value<int>()->default_value(10))
	("bs-list",po::value< std::vector<std::string> >()->composing());
	
	std::ifstream ifs(config_file.c_str());
	if (!ifs) {
		LOG(ERROR) << "can not open config file: " << config_file << "\n";
		return false;
	}
	
	po::store(po::parse_config_file<char>(config_file.c_str(),config_file_options,true), vm_);
	po::notify(vm_);

	if(!vm_.count("modules")){
		LOG(ERROR) << "no modules";
		return false;
	}
*/
	boost::property_tree::info_parser::read_info(config_file, _pt);
	
	BOOST_FOREACH(boost::property_tree::ptree::value_type &v,
								_pt.get_child("daemon.modules"))
	_modules.insert(v.first);
	
	_port = _pt.get("daemon.port", ANARCHNET_PORT);
	_rpc_port = _pt.get("daemon.rpc_port", ANARCHNET_RPC_PORT);

	return true;
}
	

}
