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

#include <iostream>
#include <boost/property_tree/info_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>
#include "anarchNet.h"
#include "config_manager.h"
#include "logger.h"

namespace fs = boost::filesystem;

namespace an {

	bool ConfigManager::init(const std::string& directory) {
	
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
	_dir = _pt.get("daemon.dir", "~/.anarchNet");
	_data_dir = _pt.get("daemon.datadir", "~/.anarchNet/data");
	_log_dir = _pt.get("daemon.logdir", "~/.anarchNet/log");

	if (_dir[0] == '~') 
		_dir = std::string(getenv("HOME")) + _dir.substr(1);

	if (_data_dir[0] == '~') 
		_data_dir = std::string(getenv("HOME")) + _data_dir.substr(1);

	if (_log_dir[0] == '~') 
	_log_dir = std::string(getenv("HOME")) + _log_dir.substr(1);

	if(!fs::exists(_dir)) {
		 LOG(INFO)<< "creating anarchNet dir: " << _dir;
		 if (!fs::create_directory(_dir)) {
			LOG(ERROR) << "Could not create: " << _dir;
			return false;
		 }
	} else if (!fs::is_directory(_dir)) {
		 LOG(ERROR) << _dir << " is not a directory";
		return false;
	}
	if(!fs::exists(_data_dir)) {
		 LOG(INFO)<< "creating anarchNet data dir: " << _data_dir;
		 if (!fs::create_directory(_data_dir)) {
			LOG(ERROR) << "Could not create: " << _data_dir;
			return false;
		 }		
	} else if (!fs::is_directory(_data_dir)) {
	 LOG(ERROR) << _data_dir << " is not a directory";
	 return false;
	 }
	 
	if(!fs::exists(_log_dir)) {
		 LOG(INFO)<< "creating anarchNet log dir: " << _log_dir;
		 if (!fs::create_directory(_log_dir)) {
			LOG(ERROR) << "Could not create: " << _log_dir;
			return false;
		 }		
	} else if (!fs::is_directory(_log_dir)) {
	 LOG(ERROR) << _log_dir << " is not a directory";
	 return false;
	 }		 
	return true;
}
	

}
