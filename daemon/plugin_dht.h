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
#ifndef FILE_pugg_DHT_H
#define FILE_pugg_DHT_H

#include <puggDriver.h>

#define HAT_SERVER_NAME "HatServer"
#define HAT_CLASS_VERSION 1

class DHTplugin
{
public:
	virtual std::string getName() {return "generic DHTplugin";}
};
class DHTpluginDriver : public pugg::Driver
{
public:
	virtual DHTplugin* createPlugin() = 0;
};

#endif