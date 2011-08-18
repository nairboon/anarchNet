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

#include "singleton.h"
#include "puggKernel.h"
#include "puggServer.h"

#ifndef DAEMON_PLUGIN_MANAGER_H_
#define DAEMON_PLUGIN_MANAGER_H_
namespace an {
	
	class PluginManager : public Singleton<PluginManager>
	{
		friend class Singleton<PluginManager>;
	public:
		bool init();
		bool loadPlugin(const std::string&,const std::string&,const std::string&);
		pugg::Kernel& get_kernel() { return _plugin_kernel; }
	private:
		pugg::Kernel _plugin_kernel;
	};
}
#endif