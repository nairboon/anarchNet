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

#ifndef DAEMON_VERSION_H
#define DAEMON_VERSION_H
//  X_VERSION % 100 is the patch level
//  X_VERSION / 100 % 1000 is the minor version
//  X_VERSION / 100000 is the major version

#define ANARCHNET_VERSION_MAJOR 0
#define ANARCHNET_VERSION_MINOR 0
#define ANARCHNET_VERSION_PATCH 1
#define ANARCHNET_VERSION (ANARCHNET_VERSION_MAJOR * 100000 \
+ ANARCHNET_VERSION_MINOR * 1000 \
+ ANARCHNET_VERSION_PATCH)

#define ANARCHNET_PROTOCOL_VERSION_MAJOR 0
#define ANARCHNET_PROTOCOL_VERSION_MINOR 0
#define ANARCHNET_PROTOCOL_VERSION_PATCH 1

#define ANARCHNET_PROTOCOL_VERSION (ANARCHNET_PROTOCOL_VERSION_MAJOR * 100000 \
+ ANARCHNET_PROTOCOL_VERSION_MINOR * 1000 \
+ ANARCHNET_PROTOCOL_VERSION_PATCH)
#endif  // DAEMON_VERSION_H_
