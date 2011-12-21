see doc/API.txt, TODO.txt

== How to build == 
cd build/<your os>/<Debug|Release>
cmake ../../..

Dependencies:
 * boost >= 1.47
 * litesql: https://sourceforge.net/apps/trac/litesql/
	* http://sourceforge.net/projects/litesql/files/litesql/0.3.13/litesql-src-0.3.13b2.tar.gz/download
 * cryptopp >= 5.6
 * boost.log

on Ubuntu:
 sudo apt-get install libcrypto++-dev

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
