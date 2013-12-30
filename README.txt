
This project is continued as AnEvoNet

===
see doc/API.txt, TODO.txt

== How to build == 
cd build/<your os>/<Debug|Release>
cmake ../../..

Dependencies:
 * boost >= 1.47
 * cryptopp >= 5.6
 * cmake

for Debian/Ubuntu:
 sudo apt-get install libcrypto++-dev cmake libboost1.48-dev libboost-system1.48-dev libboost-program-options1.48-dev libboost-thread1.48-dev libboost-serialization1.48-dev libboost-filesystem1.48-dev

on Mac:
 sudo fink install libcryptopp

cd third-party
sh install-dependencies.sh

This will install all the dependencies


Structure:
	*an			anarchnet packages
		*core 		plugins
		*universe	web*extensions
	*browser		browser app
		*http		http layer
	*daemon			daemon app
		*protocol	protobuf files
	*library		library interface
	*third-party		ext. software, dependencies
